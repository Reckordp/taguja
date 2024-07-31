class PabrikFiturBot
  def initialize
    @fitur = []
    @dilarang = []
    @menu = nil
  end

  def mulai
    @menu = @fitur.find { |ft| ft.class == FreeCommon::FiturMenu } 
    @menu.terima_deret_fitur(@fitur)
    @fitur.each(&:memulai)
  end

  def tambahkan_fitur(fitur_bot)
    @fitur.push(fitur_bot)
    @menu.terima_deret_fitur(@fitur) unless @menu.nil?
    return nil
  end

  def disable(kelazz)
    @dilarang.push(kelazz)
  end

  def jalankan_fitur
    pesan = TagujaManajer.pesan.dup
    pos = pesan.slice!(0, 1)
    return case pos
    when %q(.); jalankan_fitur_dalam(pesan)
    when 226.chr; jalankan_eval(pesan)
    end
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

  def memenuhi?(c, ftr)
    nama = ftr.class.to_s
    namespace_index = nama.index(%q(:))
    namespace = namespace_index.nil? ? String.new : nama[0, namespace_index]
    return true if @menu.nil?
    return true if namespace == %q(FreeHidden)
    return true if @menu.include_free?(ftr)
    if c < 1
      return true
    elsif c < 3
      return @menu.include_premium?(ftr)
    end
  end

  def seleksi_fitur(perintah)
    c = TagujaManajer.pengguna.status.cincin
    fitur_terseleksi = @fitur.select do |ftr|
      np = ftr.nama_perintah
      next false unless memenuhi?(c, ftr)
      next false if @dilarang.include?(ftr.class)
      next case np
      when Array
        np.any? { |nama| perintah[:nama] == nama }
      when String
        perintah[:nama] == np
      end
    end
    jawaban = fitur_terseleksi.collect do |ft|
      ft.lakukan(perintah)
      jwb = ft.jawaban_dari_bot.dup
      ft.jawaban_dari_bot.clear
      jwb
    end
    return jawaban.join("\n&\n")
  end

  def jalankan_eval(pesan)
    usr = TagujaManajer.pengguna
    script = pesan.dup
    return String.new if usr.nil?
    return String.new if usr.status.cincin > 0
    return String.new if script.slice!(0, 1) != 136.chr
    return String.new if script.slice!(0, 1) != 134.chr
    return String.new if script.slice!(0, 1) != 10.chr

    script.slice!(0, 1) while script[0] == 10.chr
    hasil = eval(script)
    return hasil.nil? ? %q(nil) : hasil.inspect
  end
end

class ::Taguja::Pengirim
  alias jid_lama jid
  def jid
    j = jid_lama
    return j.empty? ? chat : j
  end
end

class PengantarJS
  def method_missing(m, *argv)
    begin
      ref_js(m, *argv)
    rescue ArgumentError
      super
    end
  end
end

class String
  def allnum?
    self.bytes.all? { |num| num.between?(0x30, 0x39) }
  end
end

module TagujaManajer
  TAG_POSTFIX = %q(@s.whatsapp.net)

  @kendali_fitur = PabrikFiturBot.new
  @keterangan = nil
  @seragam = nil

  def self.memulai
    @keterangan = DataManajer.new
    mulai_tunnel
    @kendali_fitur.mulai
    Taguja.ganti_proses_pesan do |psn|
      TagujaManajer.terima_pesan(psn)
    end
  end

  def self.mulai_tunnel
  end

  def self.komisi_owner(money)
    owner = @keterangan.pengguna_dari_jid(@keterangan.jid_owner)
    owner.milik.money += money
  end

  def self.fitur
    return @kendali_fitur
  end

  def self.simpan_keterangan
    @keterangan.simpan
    return nil
  end

  def self.pengguna
    jid = @seragam.pengirim.jid
    if @keterangan.pengguna_tersedia?(jid)
      return @keterangan.pengguna_dari_jid(jid)
    else
      return @keterangan.pengguna_baru(jid)
    end
  end

  def self.terima_pesan(sr)
    @seragam = sr
    puts "#{pengirim.nama}$#{pengirim.chat}: #{pesan}"
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

class IndukFiturBot
  def self.inherited(turunan)
    TagujaManajer.fitur.tambahkan_fitur(turunan.new)
  end

  attr_reader :jawaban_dari_bot, :sasaran

  def initialize
    @jawaban_dari_bot = String.new
    @keterangan_kesempatan = {}
    @sasaran = nil
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

  def ket_kesempatan=(nilai)
    jid = TagujaManajer.pengirim.jid
    kk = @keterangan_kesempatan[jid] ||= Array.new
    kk.push(nilai)
    return nil
  end

  def pengguna
    return TagujaManajer.pengguna
  end

  def lakukan(config)
    if config[:kasus].empty?
      rutinitas
    else
      @sasaran = config[:kasus]
      kasus
    end
  end

  def kasus_angka?
    return sasaran.allnum?
  end

  def rutinitas
  end

  def kasus
  end
end