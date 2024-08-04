class PabrikFiturBot
  class Transaksi
    attr_accessor :status, :no, :tujuan, :reputasi, :beban, :jumlah
  end

  class Bandar
    attr_accessor :status, :no, :pelaku, :kerugian
  end

  def mulai_tunnel
    @no_transaksi = 1
    @no_karcis = 1
    @deret_tunnel = {}
    @game = Game::PabrikGame.new(@deret_tunnel)
  end

  def tunnel_tersedia(id, meth)
    @deret_tunnel[id] = meth
  end

  alias thk_mulai mulai
  def mulai
    thk_mulai
    @fitur.each do |ftr|
      ftr.map_tunnel do |tunnel|
        @deret_tunnel.fetch(tunnel) { minta_tunnel(ftr, tunnel) }
      end
    end
  end

  def minta_tunnel(ftr, tunnel)
    puts "#{ftr.class} membutuhkan tunnel #{tunnel}"
    return proc do |*args|
      ftr.jawab("Fitur tidak berjalan semestinya\n")
    end
  end

  def buat_transaksi(tujuan, reputasi, beban, jumlah)
    return Transaksi.new.tap do |t|
      t.status = true
      t.no = "THK-Jennerie-%05d" % @no_transaksi
      t.tujuan = tujuan
      t.reputasi = reputasi
      t.beban = beban
      t.jumlah = jumlah
      @no_transaksi += 1
    end
  end

  def buat_taruhan(pelaku, kerugian)
    return Bandar.new.tap do |b|
      b.status = true
      b.no = "BDR-Jennerie-%05d" % @no_karcis
      b.pelaku = pelaku
      b.kerugian = kerugian
      @no_karcis += 1
    end
  end

  def each_transaksi(&blk)
    @fitur.each do |ftr|
      next unless ftr.transaksi?
      ftr.kesempatan.each do |k|
        next unless k.is_a?(Transaksi)
        next unless k.status
        blk.call(k)
        k.status = false
      end
    end
  end

  def akumulasi_bandar
    @fitur.inject(0) do |acc, ftr|
      acc += ftr.kesempatan.inject(0) do |sub_acc, k|
        next sub_acc unless k.is_a?(Bandar)
        next sub_acc unless k.status
        k.status = false
        sub_acc += k.kerugian
      end
    end
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
    return false
  end
end

class PluginManajer
  def tambahkan(pengantar)
    @plugin[pengantar.rincian.id] = pengantar
  end

  def plugin_at(num)
    return @plugin["jennerie: source number %03d" % num]
  end
end

module TagujaManajer
  def self.mulai_tunnel
    fitur.mulai_tunnel
    fitur.tunnel_tersedia(:kini, proc { next @seragam })
    fitur.tunnel_tersedia(:tukar, proc { |ganti| @seragam = ganti; next nil })
    fitur.tunnel_tersedia(:tersedia?, @keterangan.method(:pengguna_tersedia?))
    fitur.tunnel_tersedia(:cari_by_jid, @keterangan.method(:pengguna_dari_jid))
    fitur.tunnel_tersedia(:kudeta_owner, @keterangan.method(:ganti_owner))
    fitur.tunnel_tersedia(:jid_owner, @keterangan.method(:jid_owner))
    fitur.tunnel_tersedia(:iterate_user, @keterangan.akses_seluruh_pengguna)
  end
end

class IndukFiturBot
  alias thk_initialize_asli initialize
  def initialize
    thk_initialize_asli
    @tunnel = []
  end

  def map_tunnel(&blk)
    @tunnel.map!(&blk)
  end

  def transaksi?
    return false
  end

  def taruhan?
    return false
  end

  def reputasi
    return 0
  end

  def transaksi_baru(jid, key, jumlah)
    return nil unless transaksi?
    t = TagujaManajer.fitur.buat_transaksi(jid, self.reputasi, key, jumlah)
    self.ket_kesempatan = t
    return t
  end

  def pasang_taruhan(nominal)
    return nil unless taruhan?
    t = TagujaManajer.fitur.buat_taruhan(TagujaManajer.pengirim.jid, nominal)
    self.ket_kesempatan = t
    return t
  end
end