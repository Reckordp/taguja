class PabrikFiturBot
  def initialize
    @fitur = []
  end

  def tambahkan_fitur(fitur_bot)
    @fitur.push(fitur_bot)
  end

  def jalankan_fitur
    pesan = TagujaManajer.pesan.dup
    return nil unless pesan.slice!(0, 1) == %q(.)
    return jalankan_fitur_dalam(pesan)
  end

  def jalankan_fitur_dalam(pesan)
    perintah = {}
    pemberhentian = pesan.index(32.chr)
    if pemberhentian.nil?
      perintah[:nama] = pesan.dup
      perintah[:kasus] = String.new
    else
      perintah[:nama] = pesan.slice!(0, pemberhentian)
      pesan.slice!(0, 1)
      perintah[:kasus] = pesan.dup
    end
    return seleksi_fitur(perintah)
  end

  def seleksi_fitur(perintah)
    fitur_terseleksi = @fitur.select do |ftr|
      np = ftr.nama_perintah
      next case np
      when Array
        np.any? { |nama| perintah[:nama] == nama }
      when String
        perintah[:nama] == np
      end
    end
    jawaban = fitur_terseleksi.collect do |ft|
      ft.lakukan(perintah)
      ft.jawaban_dari_bot
    end
    return jawaban.join("\n&\n")
  end
end

module TagujaManajer
  @kendali_fitur = PabrikFiturBot.new
  @seragam = nil

  def self.memulai
    Taguja.ganti_proses_pesan do |psn|
      TagujaManajer.terima_pesan(psn)
    end
  end

  def self.fitur
    return @kendali_fitur
  end

  def self.terima_pesan(sr)
    @seragam = sr
    puts "#{pengirim.nama}@#{pengirim.chat}: #{pesan}"
    respon = @kendali_fitur.jalankan_fitur
    sr.jawab(respon) unless respon.nil?
  end

  def self.pengirim
    return @seragam.pengirim
  end

  def self.pesan
    return @seragam.pesan
  end
end

class IndukBotFitur
  def self.inherited(turunan)
    TagujaManajer.fitur.tambahkan_fitur(turunan.new)
  end

  attr_reader :jawaban_dari_bot, :sasaran

  def initialize
    @jawaban_dari_bot = String.new
    @keterangan_kesempatan = {}
    @sasaran = nil
    memulai
  end

  def memulai
  end

  def nama_perintah
    return self.class.const_get("PERINTAH")
  end

  def jawab(teks)
    @jawaban_dari_bot += teks
  end

  def kesempatan
    jid = TagujaManajer.pengirim.jid
    return @keterangan_kesempatan.fetch(jid) { Array.new }
  end

  def ket_kesempatan=(jid, nilai)
    jid = TagujaManajer.pengirim.jid
    kk = @keterangan_kesempatan[jid] ||= Array.new
    kk.push(nilai)
    return nil
  end

  def lakukan(config)
    if config[:kasus].empty?
      rutinitas
    else
      @sasaran = config[:kasus]
      kasus
    end
  end

  def rutinitas
  end

  def kasus
  end
end

class FiturBiodata < IndukBotFitur
  PERINTAH = %q(my)
  BIODATA = <<RINCIAN
\u232c Nama = *$*
\u232c Limit = *7737*
\u232c Exp = *191086021*
\u232c Level = *1634*
\u232c Money = *10220088575*
\u232c Atm = *78608300*
\u232c Rank = *Mythic II*
\u232c Pasangan= @6287872924376
RINCIAN

  def rutinitas
    jawab(BIODATA.sub(%q($), TagujaManajer.pengirim.nama))
  end

  def kasus
    return rutinitas
  end
end

class MailBoxFitur < IndukBotFitur
  PERINTAH = %w( mailbox mb )
  UCAPAN = <<LEMBARAN
Ada %d pesan
Sebanyak %d belum terbaca
LEMBARAN

  def pesan_belum_terbaca
    kesempatan.select(&:baru?)
  end

  def rutinitas
    jumlah = kesempatan.length
    bt = pesan_belum_terbaca.length
    jawab(UCAPAN % [jumlah, bt])
  end
end

# Taguja.ganti_proses_pesan do |rinci|
#   p rinci.pengirim.id
#   p rinci.pengirim.chat
#   p rinci.pengirim.subjek
#   p rinci.pengirim.jid
#   p rinci.pengirim.nama
#   p rinci.pesan
#   puts "\n\n"
#   rinci.jawab("Nah kan") if rinci.pesan == "Tes"
#   rinci.jawab(BIODATA.sub($, rinci.pengirim.nama)) if rinci.pesan == ".my"
# end
TagujaManajer.memulai