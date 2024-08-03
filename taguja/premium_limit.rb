module PremiumLimit
  module Util
    def rutinitas
      self.ket_kesempatan = self.class.const_get(:LIMIT) if kesempatan.empty?
      return jawab(%q(Limit habis)) if kesempatan.first.zero?
      aktivitas
      kesempatan[0] -= 1
      jawab("\nSisa Limit: %d\n" % kesempatan.first)
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
    PERINTAH = %q(awmu)

    def rutinitas
      jawab "Implement code #102"
    end
  end

  class FiturAwake < IndukFiturBot
    PERINTAH = %q(awake)

    def rutinitas
      jawab "Implement code #103"
    end
  end

  class FiturBolo < IndukFiturBot
    PERINTAH = %q(bolo)

    def rutinitas
      jawab "Implement code #104"
    end
  end

  class FiturIzinmin < IndukFiturBot
    PERINTAH = %q(karo)

    def rutinitas
      jawab "Implement code #105"
    end
  end

  class FiturMwuah < IndukFiturBot
    PERINTAH = %q(mwuach)

    def rutinitas
      jawab "Implement code #106"
    end
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