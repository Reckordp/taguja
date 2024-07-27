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
  
  class Barang
    attr_accessor :nama, :jumlah, :efek

    @tersedia = {}

    def self.tambah(nama, kelazz)
      @tersedia[nama] = kelazz
      return nil
    end

    def self.kelazzdarinama(nama)
      return @tersedia[nama]
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

  Barang.tambah(%q(Limit), Makanan)
  Barang.tambah(%q(🥤 Potion), Makanan)
  Barang.tambah(%q(🍶 Aqua), Makanan)
  Barang.tambah(%q(🪙  Gold), Perlengkapan)
  Barang.tambah(%q(💎 Diamond), Perlengkapan)
  Barang.tambah(%q(🪨 Batu), Perlengkapan)
  Barang.tambah(%q(🪵 Kayu), Perlengkapan)
  Barang.tambah(%q(🕸️ String), Perlengkapan)
  Barang.tambah(%q(⛓️ Iron), Perlengkapan)
  Barang.tambah(%q(📓 Buku), Makanan)
  Barang.tambah(%q(🗑️ Sampah), Barang)
end