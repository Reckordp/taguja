class PabrikFiturBot
  class Transaksi
    attr_accessor :status, :no, :tujuan, :reputasi, :beban, :jumlah
  end

  def mulai_tunnel
    @no_transaksi = 1
    @deret_tunnel = {}
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
end

module TagujaManajer
  def self.mulai_tunnel
    fitur.mulai_tunnel
    fitur.tunnel_tersedia(:tersedia?, @keterangan.method(:pengguna_tersedia?))
    fitur.tunnel_tersedia(:cari_by_jid, @keterangan.method(:pengguna_dari_jid))
    fitur.tunnel_tersedia(:kudeta_owner, @keterangan.method(:ganti_owner))
    fitur.tunnel_tersedia(:jid_owner, @keterangan.method(:jid_owner))
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

  def reputasi
    return 0
  end

  def transaksi_baru(jid, key, jumlah)
    return nil unless transaksi?
    t = TagujaManajer.fitur.buat_transaksi(jid, self.reputasi, key, jumlah)
    self.ket_kesempatan = t
    return t
  end
end