module FreeCommon
  class FiturMy < IndukFiturBot
    PERINTAH = %q(my)
    UCAPAN = <<RINCIAN
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
      jawab(UCAPAN.sub(%q($), TagujaManajer.pengirim.nama))
    end
  
    def kasus
      return rutinitas
    end
  end

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


  class InfoFitur < IndukFiturBot
    PERINTAH = %w(info)
    UCAPAN = <<LEMBARAN
*PROJECT* : Jennerie
*COORDINATE* : Jenneire
*INTERFACE* : Nusa
*DUMPER* : Shipped
*SUPPORT* : mruby
*BACKEND* : Taguja
*DESK* : nodejs (modification)
*ENGINE* : V8
*MACHINE* : Microsoft Windows
*REPOSITORY* : https://github.com/Reckordp/taguja
LEMBARAN
    def rutinitas
      jawab(UCAPAN)
    end
  end

  class FiturMenu < IndukFiturBot
    PERINTAH = %w( m3nu menu )

    def memulai
      @ucapan = String.new
      kelompokkan_fitur.each do |nama, deret|
        @ucapan += "╭━ *「 #{nama.upcase} 」* ━━┈ ⳹\n"
        deret.each do |pr|
          @ucapan += "│ *◦➛ .#{pr}*\n"
        end
        @ucapan += %q(╰━━━━━━━━━━━━━━━━┈ ❋ཻུ۪۪⸙) + "\n"
      end
    end

    def terima_deret_fitur(d)
      @deret = d
    end

    def kelompokkan_fitur
      kelompok = { %q(Free) => [], %q(Premium) => [], %q(Owner) => [] } 
      @deret.each do |ft|
        perintah = ft.nama_perintah
        baris = nil
        case perintah
        when String
          baris = perintah.dup
        when Array
          baris = String.new
          perintah.each do |nama|
            baris = nama.dup if baris.length < nama.length
          end
        end

        kelazz = ft.class.to_s
        if baris == %q(jekpot)
          kelompok[%q(Owner)].push(baris.dup)
        elsif kelazz[0, 7] == %q(Premium)
          jenis = kelazz[7, kelazz.index(%q(:)) - 7]
          baris += " ⚡#{jenis.downcase}"
          kelompok[%q(Premium)].push(baris.dup)
        elsif kelazz[0, 4] == %q(Free)
          jenis = kelazz[4, kelazz.index(%q(:)) - 4]
          baris += " ⚡#{jenis.downcase}"
          kelompok[%q(Free)].push(baris.dup)
        end
      end
      return kelompok
    end

    def rutinitas
      jawab(@ucapan)
    end
  end

  class FiturUN < IndukFiturBot
    PERINTAH = %w( fis kim bio )

    def label(usr)
      c = usr.status.cincin
      if c.between?(1, 2)
        return "Premium"
      elsif c == 0
        return "*Owner*"
      elsif c == 3
        return "Free"
      else
        return "Unknown"
      end
    end

    def rutinitas
      usr = pengguna
      ucapan = <<LEMBARAN
\u232c Nama = #{TagujaManajer.pengirim.nama}
\u232c Tingkat = #{label(usr)}
\u232c Nyawa = #{usr.status.nyawa}
\u232c Tenaga = #{usr.status.tenaga}
\u232c Reputasi = #{usr.status.reputasi}
\u232c Intellegent = #{usr.status.intellegent}
\u232c Pendakian = #{usr.milik.pendakian}
\u232c Uang = #{usr.milik.money}
\u232c Level = #{usr.jejak.level}
\u232c Experience = #{usr.jejak.exp}
LEMBARAN
      jawab(ucapan)
    end
  end
end