module FreeOutflow
  module Util
    def key_of_real_item(barang)
      return Inventory::BARANG_TOKO.keys.find do |nm|
        urutan = nm.index(32.chr)
        n = nm.dup
        n.slice!(0, urutan + 1) unless urutan.nil?
        n.downcase == barang.downcase
      end
    end
  end

  class FiturBeli < IndukFiturBot
    include Util

    PERINTAH = %w( beli buy )
    TOKO = <<MENU
*🎒 SHOP*

*🧪 Penggunaan :*
_.beli <item> <jumlah>_
Contoh penggunaan: _*.beli potion 1*_

*📮 Note :*
bila sudah tidak ada harganya, berarti sudah tidak bisa dibeli / sudah level max


🛍️ List Barang:
━━━━━━━━━━━━━━━━━━━
*♻ Barang   | 💲 Harga beli*
━━━━━━━━━━━━━━━━━━━
MENU

    def rutinitas
      lembaran = TOKO.dup
      Inventory::BARANG_TOKO.each do |nama, harga|
        lembaran += "*#{nama}:* #{harga}\n"
      end
      lembaran += %q(━━━━━━━━━━━━━━━━━━━)
      Taguja.conn.sendMessage(TagujaManajer.pengirim.chat, { text: lembaran })
      jawab("Money kamu #{pengguna.milik.money}")
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
      return rutinitas unless ssr.allnum?
      jumlah = ssr.to_i
      key = key_of_real_item(barang)
      return rutinitas if key.nil?
      hrg = Inventory::BARANG_TOKO[key]
      kas = pengguna.milik.money
      if kas < hrg
        jawab("Uang kamu #{kas} tidak cukup untuk membeli:\n")
        jawab("  #{jumlah}x #{key} seharga #{hrg}")
      else
        pengguna.milik.money -= hrg
        TagujaManajer.komisi_owner(hrg)
        pengguna.terima(key, jumlah)
        jawab(%q(Pembelian berhasil!))
      end
    end
  end

  class FiturBlackStar < IndukFiturBot
    PERINTAH = %w( bs blackstar )
  end

  class FiturUpgrade < IndukFiturBot
    PERINTAH = %w( up upgrade )
  end

  class FiturRepair < IndukFiturBot
    PERINTAH = %w( rr repair )
  end

  class FiturTransfer < IndukFiturBot
    include Util

    PERINTAH = %w( tf transfer )
    DERET_BEBAN = %w( money potion batu kayu string iron buku sampah )
    UCAPAN = <<LEMBARAN
*––––––『 TRANSFER 』––––––*
*ID:* %s
*🗂️ Type:* %s
*🧮 Count:* %d
*📨 To:* %s

⏰ Timeout *60* detik

CONFIRM TRANSFER!
✔️ : .ya
✖️ : .batal
LEMBARAN

    def transaksi?
      return true
    end

    def reputasi
      return 20
    end

    def rutinitas
      jawab(%q(penggunaan: _.tf money 1000 *@tag*_) + 10.chr)
      jawab(%q(Transferable item:) + 10.chr)
      deret = DERET_BEBAN.dup
      jawab(%q(  - ) + deret.shift + 10.chr)
      deret.each do |b|
        jawab("  - #{key_of_real_item(b)}\n")
      end
    end

    def milik_kurang
      jawab(%q(Barang yang kamu miliki lebih kecil dari jumlah transfer))
    end

    def kasus
      awal = sasaran.dup
      ssr = sasaran.dup
      urutan_beban = ssr.index(32.chr)
      return rutinitas if urutan_beban.nil?
      beban = ssr.slice!(0, urutan_beban)
      ssr.slice!(0, 1)
      urutan_jumlah = ssr.index(32.chr)
      return rutinitas if urutan_jumlah.nil?
      jumlah = ssr.slice!(0, urutan_jumlah)
      ssr.slice!(0, 1)
      tag = ssr.dup
      return rutinitas unless tag[0] == 64.chr
      return rutinitas unless tag.index(32.chr).nil?
      return rutinitas unless tag.index(10.chr).nil?
      jid = tag + TagujaManajer::TAG_POSTFIX

      if DERET_BEBAN.include?(beban) && jumlah.allnum?
        key = nil
        if beban == %q(money)
          key = beban
          return milik_kurang if pengguna.milik.money < jumlah.to_i
        else
          key = key_of_real_item(beban)
          return rutinitas if key.nil?
          return milik_kurang unless pengguna.punya?(key, jumlah.to_i)
        end
        t = transaksi_baru(jid, key, jumlah.to_i)
        if t.nil?
          jawab(%q(Gagal membuat transaksi))
        else
          jawab(UCAPAN % [t.no, beban, jumlah.to_i, tag])
        end
      else
        rutinitas
      end
    end
  end
end