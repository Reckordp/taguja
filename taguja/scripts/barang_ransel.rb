module Inventory
  BARANG_TOKO = {
    %q(Limit) => 1000, 
    %q(🥤 Potion) => 200, 
    %q(🍶 Aqua) => 25, 
    %q(🪙  Gold) => 6000, 
    %q(💎 Diamond) => 900, 
    %q(🪨 Batu) => 500, 
    %q(🪵 Kayu) => 500, 
    %q(🕸️ String) => 500, 
    %q(⛓️ Iron) => 800, 
    %q(📓 Buku) => 85000, 
    %q(🗑️ Sampah) => 10
  }
  TOKO_TEMPLATE = <<MENU
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
  
  class Barang
    attr_accessor :nama, :jumlah

    @tersedia = {}

    def self.tambah(nama, kelazz, efek=nil)
      @tersedia[nama] = { const: kelazz, efek: efek }
      return nil
    end

    def self.barangdarinama(nama)
      return @tersedia[nama]
    end

    def self.kelazzdarinama(nama)
      return barangdarinama(nama)[:const]
    end

    def efek
      return Barang.barangdarinama(nama)[:efek]
    end

    def jenis
      case self.class
      when Perlengkapan; :perlengkapan
      when Peralatan; :peralatan
      when Makanan; :makanan
      else; :unknown
      end
    end

    def melengkapi?
      return jenis == :perlengkapan
    end

    def bekerja?
      return jenis == :peralatan
    end

    def dikonsumsi?
      return jenis == :makanan
    end

    def pake
      return kenakan if melengkapi?
      return gunakan if bekerja?
      return konsumsi if dikonsumsi?
    end
  end

  class Perlengkapan < Barang
    def kenakan
    end

    def lepas
    end
  end

  class Peralatan < Barang
    def gunakan
    end

    def taruh
    end
  end

  class Makanan < Barang
    def konsumsi
      @jumlah -= 1
    end
  end

  # Barang.tambah(%q(Limit), Makanan, {  })
  Barang.tambah(%q(🥤 Potion), Makanan, { nyawa: 20 })
  Barang.tambah(%q(🍶 Aqua), Makanan, { nyawa: 5, tenaga: 40 })
  Barang.tambah(%q(🪙  Gold), Perlengkapan)
  Barang.tambah(%q(💎 Diamond), Perlengkapan)
  Barang.tambah(%q(🪨 Batu), Perlengkapan)
  Barang.tambah(%q(🪵 Kayu), Perlengkapan)
  Barang.tambah(%q(🕸️ String), Perlengkapan)
  Barang.tambah(%q(⛓️ Iron), Perlengkapan)
  Barang.tambah(%q(📓 Buku), Makanan, { intellegent: 65 })
  Barang.tambah(%q(🗑️ Sampah), Barang)
  Barang.tambah(%q(⚔️ Sword), Peralatan)
  Barang.tambah(%q(🥼 Armor), Peralatan)
end