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
      @kelompok = { %q(Free) => [], %q(Premium) => [], %q(Owner) => [] } 
      kelompokkan_fitur
      @kelompok.each do |nama, deret|
        @ucapan += "╭━ *「 #{nama.upcase} 」* ━━┈ ⳹\n"
        deret.each do |pr|
          @ucapan += "│ *◦➛ .#{pr}*\n"
        end
        @ucapan += %q(╰━━━━━━━━━━━━━━━━┈ ❋ཻུ۪۪⸙) + "\n"
      end
    end

    def perintah_dari_fitur(ftr)
      perintah = ftr.nama_perintah
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
      return baris
    end

    def include_premium?(ftr)
      kelazz = ftr.class.to_s
      return kelazz[0, 7] == %q(Premium)
    end

    def include_free?(ftr)
      kelazz = ftr.class.to_s
      return kelazz[0, 4] == %q(Free)
    end

    def terima_deret_fitur(d)
      @deret = d
    end

    def kelompokkan_fitur
      @deret.each do |ft|
        baris = perintah_dari_fitur(ft)
        next if baris.nil?

        kelazz = ft.class.to_s
        if baris == %q(jekpot)
          @kelompok[%q(Owner)].push(baris.dup)
        elsif kelazz[0, 7] == %q(Premium)
          jenis = kelazz[7, kelazz.index(%q(:)) - 7]
          baris += " ⚡#{jenis.downcase}"
          @kelompok[%q(Premium)].push(baris.dup)
        elsif kelazz[0, 4] == %q(Free)
          jenis = kelazz[4, kelazz.index(%q(:)) - 4]
          baris += " ⚡#{jenis.downcase}"
          @kelompok[%q(Free)].push(baris.dup)
        end
      end
      return nil
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

  class FiturPake < IndukFiturBot
    PERINTAH = %w( use gunakan pake )

    def rutinitas
      jawab(%q(penggunaan: _.use potion 1_))
    end

    def kasus
      ssr = sasaran.dup
      panjang = ssr.length
      urutan_nama = ssr.index(32.chr)
      return rutinitas if urutan_nama.nil?
      barang = ssr.slice!(0, urutan_nama)
      ssr.slice!(0, 1)
      urutan_jumlah = ssr.index(32.chr)
      return rutinitas unless urutan_jumlah.nil?
      return rutinitas unless ssr.index(10.chr).nil?
      return rutinitas unless ssr.bytes.all? { |num| num.between?(0x30, 0x39) }
      jumlah = ssr.to_i
      key = Inventory::BARANG_TOKO.keys.find do |nm|
        urutan = nm.index(32.chr)
        n = nm.dup
        n.slice!(0, urutan + 1) unless urutan.nil?
        n.downcase == barang.downcase
      end

      barang = pengguna.milik.inventory.find { |b| b.nama == key }
      if barang.nil?
        jawab("kamu tidak memiliki *#{key}*")
      elsif barang.efek.nil?
        jawab("*#{key}* tidak bisa dipakai")
      elsif barang.efek.empty?
        jawab("*#{key}* tidak memiliki efek")
      elsif barang.efek.is_a?(Hash)
        jawab("kamu menerima:\n")
        barang.efek.each do |atribut, banyak|
          total = banyak * jumlah
          jawab("#{atribut.to_s}: #{total}\n")
          pengguna.terkena_efek(atribut, total)
          jumlah.times { barang.pake }
        end
      else
        jawab("*#{key}* memiliki efek lain. Efek ini tidak dapat digunakan sekarang")
      end
    end
  end

  class FiturInventory < IndukFiturBot
    PERINTAH = %w( inv inventory )

    def rutinitas
      ucapan = String.new
      ucapan += "*Inventory milik _#{TagujaManajer.pengirim.nama}_*\n"
      ucapan += "━━━━━━━━━━━━━━━━┈\n"
      equipment = pengguna.milik.inventory.select { |b| b.is_a?(Inventory::Peralatan) }
      ucapan += "–––––– 『 EQUIPMENT 』––––––\n" unless equipment.empty?
      equipment.each do |e|
        ucapan += "- #{e.jumlah}x #{e.nama}\n"
      end
      perlengkapan = pengguna.milik.inventory.select { |b| b.is_a?(Inventory::Perlengkapan) }
      ucapan += "\n–––– 『 PERLENGKAPAN 』––––\n" unless perlengkapan.empty?
      perlengkapan.each do |e|
        ucapan += "- #{e.jumlah}x #{e.nama}\n"
      end
      konsumsi = pengguna.milik.inventory.select { |b| b.is_a?(Inventory::Makanan) }
      ucapan += "\n–––––– 『 KONSUMSI 』––––––\n" unless konsumsi.empty?
      konsumsi.each do |e|
        ucapan += "- #{e.jumlah}x #{e.nama}\n"
      end
      ucapan += "━━━━━━━━━━━━━━━━┈\n"
      jawab(ucapan)
    end
  end
end