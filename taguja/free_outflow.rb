module FreeOutflow
  class FiturBeli < IndukFiturBot
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
      return rutinitas unless ssr.bytes.all? { |num| num.between?(0x30, 0x39) }
      jumlah = ssr.to_i
      key = Inventory::BARANG_TOKO.keys.find do |nm|
        urutan = nm.index(32.chr)
        n = nm.dup
        n.slice!(0, urutan + 1) unless urutan.nil?
        n.downcase == barang.downcase
      end
      hrg = Inventory::BARANG_TOKO[key]
      kas = pengguna.milik.money
      if kas >= hrg
        pengguna.milik.money -= hrg
        TagujaManajer.komisi_owner(hrg)
        pengguna.terima(key, jumlah)
        jawab(%q(Pembelian berhasil!))
      else
        jawab("Uang kamu #{kas} tidak cukup untuk membeli:\n#{jumlah}x #{key} seharga #{hrg}")
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
end