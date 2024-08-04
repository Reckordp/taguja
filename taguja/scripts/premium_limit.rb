module PremiumLimit
  module Util
    def rutinitas
      self.ket_kesempatan = self.class.const_get(:LIMIT) if kesempatan.empty?
      return jawab(%q(Limit habis)) if kesempatan.first.zero?
      aktivitas
      kesempatan[0] -= 1
      jawab("\nSisa Limit: %d\n" % kesempatan.first)
    end

    def kasus
      return rutinitas
    end
  end

  class FiturNcene < IndukFiturBot
    include Util

    PERINTAH = %q(ncene)
    LIMIT = 10
    UCAPAN = <<LEMBARAN
| *PLAYERS* | *POINT* |
  *🤖 BOT:*      %d
  *👤 YOU:*     %d

Kamu *%s*, kamu Kehilangan *%d* Money 💹
LEMBARAN

    def aktivitas
      point = Array.new(2) { rand(100) } .sort.reverse
      kalah = point[0] > point[1]
      sanjungan = point[0] == point[1] ? %q(Seri) : kalah ? %q(Kalah) : %q(Menang)
      taruhan = TagujaManajer.fitur.akumulasi_bandar
      taruhan *= -1 if kalah
      pengguna.terkena_efek(:money, taruhan)
      jawab(UCAPAN % [*point, sanjungan, taruhan])
    end
  end

  class FiturAwmu < IndukFiturBot
    include Util
    LIMIT = 20
    PERINTAH = %q(awmu)
    def memulai; @tunnel.push(:war_player); end
    def aktivitas; jawab @tunnel[0].call; end
  end

  class FiturAwake < IndukFiturBot
    include Util
    LIMIT = 2
    PERINTAH = %q(awake)
    def memulai; @tunnel.push(:war_leave); end
    def aktivitas
      tim = @tunnel[0].call
      jawab(tim.nil? ? %q(Kamu belum join) : "Berhasil keluar dari tim *%s*" % tim)
    end
  end

  class FiturBolo < IndukFiturBot
    include Util
    LIMIT = 5
    PERINTAH = %q(bolo)
    def memulai; @tunnel.push(:war_join); end
    def aktivitas
      ssr = sasaran.dup
      return jawab(%q(penggunaan: _.bolo *Anggrek*_)) if ssr.empty?
      return jawab(%q(penggunaan: _.bolo *Anggrek*_)) unless ssr.index(32.chr).nil?
      ret = @tunnel[0].call(ssr)
      jawab(ret ? "Berhasil masuk tim *#{ssr}*" : %q(Sesi War telah dimulai))
    end
  end

  class FiturIzinmin < IndukFiturBot
    include Util
    LIMIT = 20
    PERINTAH = %q(karo)
    def memulai; @tunnel.push(:war_start); end
    def aktivitas
      begin
        taruhan = @tunnel[0].call
        jawab(%q(Memulai War!) + 10.chr)
        jawab(%q(Taruhan yang disepakati ) + taruhan.to_s + 10.chr)
        jawab(%q(Pakai *.mwuach* untuk menyerang))
      rescue Game::Perang::MasihAdaSesi
        jawab(%q(Sesi War masih panas berjalan))
      rescue Game::Perang::TidakAdaMusuh
        jawab(%q(Musuh tidak ditemukan))
      rescue Game::Perang::TidakBertaruh => e
        jawab(e.message + 10.chr + %q(Tidak memasang taruhan))
      end
    end
  end

  class FiturMwuah < IndukFiturBot
    include Util
    LIMIT = 80
    PERINTAH = %q(mwuach)
    def memulai; @tunnel.push(:war_attack); end
    def rutinitas; jawab(@tunnel[0].call(sasaran)); end
  end

  class FiturYa < IndukFiturBot
    include Util

    PERINTAH = %q(ya)
    LIMIT = 5
    UCAPAN = <<LEMBARAN
*––––––『 TRANSAKSI 』––––––*
*🔢 ID:* %s
*✔️ Status:* Sukses
*🧾 Reputasi:* %d
LEMBARAN
    TIDAK_DITEMUKAN = <<LEMBARAN
*–––『 KESALAHAN SISTEM 』–––*
*🧾 Reputasi:* %d

Tidak bisa transfer. Tujuan tidak ditemukan di database
LEMBARAN

    def memulai
      @tunnel.push(:tersedia?)
      @tunnel.push(:cari_by_jid)
    end

    def tersedia?(jid)
      return @tunnel[0].call(jid)
    end

    def cari_by_jid(jid)
      return @tunnel[1].call(jid)
    end

    def aktivitas
      TagujaManajer.fitur.each_transaksi do |invoice|
        kena = invoice.reputasi
        pengguna.terkena_efek(:reputasi, kena)
        usr = nil
        if tersedia?(invoice.tujuan)
          usr = cari_by_jid(invoice.tujuan)
        else
          next jawab(TIDAK_DITEMUKAN % kena)
        end
        if invoice.beban == %q(money)
          pengguna.terkena_efek(:money, invoice.jumlah * -1)
          usr.terkena_efek(:money, invoice.jumlah)
        else
          pengguna.terima(invoice.beban, invoice.jumlah * -1)
          usr.terima(invoice.beban, invoice.jumlah)
        end
        jawab(UCAPAN % [invoice.no, kena])
        jawab("\n")
      end
    end
  end
end