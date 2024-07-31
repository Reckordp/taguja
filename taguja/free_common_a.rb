module FreeCommon
  class FiturMailBox < IndukFiturBot
    class Mail
      attr_accessor :dibuka, :subyek, :pengirim, :konten

      def baru?
        return @dibuka == false
      end
    end

    PERINTAH = %w( mailbox mb )
    UCAPAN = <<LEMBARAN
Ada %d pesan
Sebanyak %d belum terbaca
LEMBARAN

    def pesan_belum_terbaca
      return kesempatan.select(&:baru?)
    end

    def baru?
      return !pesan_belum_terbaca.empty?
    end

    def rutinitas
      jumlah = kesempatan.length
      bt = pesan_belum_terbaca.length
      jawab UCAPAN % [jumlah, bt]
    end

    def kasus
      case sasaran
      when %q(tambah)
        # menambah pesan
      when %q(buka)
        if baru?
          ucp = String.new
          pesan = pesan_belum_terbaca.last
          pesan.terbuka
          ucp += "Subyek: *#{pesan.subyek}*\n"
          ucp += "Dari: *#{pesan.pengirim}*\n"
          ucp += "=============================\n"
          ucp += pesan.konten
          jawab ucp
        else
          jawab %q(Tidak ada pesan baru)
        end
      when %q(semua)
        ucp = String.new
        kesempatan.each do |pesan|
          ucp += "Subyek: *#{pesan.subyek}*\n"
          ucp += "Dari: *#{pesan.pengirim}*\n\n"
        end
        ucp += "Total #{kesempatan.length} pesan"
        jawab ucp
      else
        deret = kesempatan.select do |pesan|
          pesan.subyek == sasaran
        end
        if deret.empty?
          jawab %q(× Pesan tidak ditemukan)
        else
          ucp = String.new
          deret.each do |pesan|
            pesan.terbuka if pesan.baru?
            ucp += "Subyek: *#{pesan.subyek}*\n"
            ucp += "Dari: *#{pesan.pengirim}*\n"
            ucp += "=============================\n"
            ucp += pesan.konten + "\n\n"
          end
          jawab ucp
        end
      end
    end
  end

  class FiturBatal < IndukFiturBot
    PERINTAH = %q(batal)
    UCAPAN = <<LEMBARAN
*––––––『 TRANSAKSI 』––––––*
*ID:* %s
*❌ Status:* Gagal
*🧾 Reputasi:* %d
LEMBARAN

    def rutinitas
      TagujaManajer.fitur.each_transaksi do |invoice|
        kena = invoice.reputasi * -1
        pengguna.terkena_efek(:reputasi, kena)
        jawab(UCAPAN % [invoice.no, kena])
      end
    end
  end

  class FiturPremium < IndukFiturBot
    PERINTAH = %w( prem premium )
    UCAPAN = <<LEMBARAN
*Kamu berhasil jadi premium!!!*
LEMBARAN

    def rutinitas
      pengguna.status.cincin = 2
      jawab(UCAPAN)
    end
  end

  class FiturKudeta < IndukFiturBot
    PERINTAH = %w( kdt kudeta )
    UCAPAN = <<LEMBARAN
Kudeta berhasil. Kamu sekarang adalah *owner*
LEMBARAN

    def memulai
      @tunnel.push(:kudeta_owner)
    end

    def rutinitas
      @tunnel.first.call(TagujaManajer.pengirim.jid)
      jawab(UCAPAN)
    end
  end

  class FiturOwner < IndukFiturBot
    PERINTAH = %q(oWNEr)

    def memulai
      @tunnel.push(:jid_owner)
    end

    def rutinitas
      jid = @tunnel.first.call
      jawab(%q(@) + jid.slice(0, jid.index(%q(@))))
    end
  end
end