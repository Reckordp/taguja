class StatusUser
  attr_accessor :cincin, :nyawa, :tenaga, :reputasi
  attr_accessor :intellegent, :warn, :ban
end

class MilikUser
  attr_accessor :serial, :pendakian, :money, :inventory, :pet
end

class JejakUser
  attr_accessor :level, :exp
end

class Owner
  def jid
    return %q(62881036418478@s.whatsapp.net)
  end
end

class Pengguna
  attr_reader :jid, :status, :milik, :jejak
  def initialize(jid)
    @jid = jid
    @status = StatusUser.new
    @milik = MilikUser.new
    @jejak = JejakUser.new
    @milik.inventory = []
  end

  def punya?(nama, jumlah)
    barang = @milik.inventory.find { |b| b.nama == nama }
    unless barang.nil?
      return true unless barang.jumlah < jumlah
    end
    return false
  end

  def terima(nama, jumlah)
    barang = @milik.inventory.find { |b| b.nama == nama }
    if barang.nil?
      barang = Inventory::Barang.kelazzdarinama(nama).new.tap do |b|
        b.nama = nama
        b.jumlah = jumlah
        @milik.inventory.push(b)
      end
    else
      barang.jumlah += jumlah
    end
  end

  def terkena_efek(atribut, jumlah)
    meth = "#{atribut}=".to_sym
    if status.respond_to?(atribut)
      asal = status.send(atribut)
      status.send(meth, asal + jumlah)
    elsif milik.respond_to?(atribut)
      asal = milik.send(atribut)
      milik.send(meth, asal + jumlah)
    elsif jejak.respond_to?(atribut)
      asal = jejak.send(atribut)
      jejak.send(meth, asal + jumlah)
    end
  end
end

class DataManajer
  include SQLStatement

  def initialize
    @owner = Owner.new
    @kereta = {}
    Taguja.balik do 
      if database_baru?
        self.buat_skema
      else
        self.memulai
      end
    end
  end

  def database_baru?
    return Taguja::DATABASE_BARU == 1
  end

  def jid_owner
    return @owner.jid
  end

  def buat_skema
    Taguja.gramasi(BUAT_TABEL[:PENGGUNA])
    Taguja.gramasi(BUAT_TABEL[:KONDISI])
    Taguja.gramasi(BUAT_TABEL[:MILIK])
    Taguja.gramasi(BUAT_TABEL[:RANSEL])
    Taguja.gramasi(BUAT_TABEL[:PET])
    Taguja.gramasi(BUAT_TABEL[:JEJAK])
  end

  def memulai
    Taguja.gramasi(PILIH_BARIS[:PENGGUNA][:SEMUA])
    prosedur = Array.new
    row_pengguna = Array.new(3)
    while Taguja.lembaran(%q(pengguna), row_pengguna)
      user = Pengguna.new(row_pengguna[1])
      psd = proc do |id, usr|
        Taguja.gramasi(PILIH_BARIS[:KONDISI][:BY_PENGGUNA_ID] % id)
        row_kondisi = Array.new(10)
        if Taguja.lembaran(%q(kondisi), row_kondisi)
          usr.status.tap do |s|
            s.cincin = row_kondisi[2].to_i
            s.nyawa = row_kondisi[3].to_i
            s.tenaga = row_kondisi[4].to_i
            s.reputasi = row_kondisi[5].to_i
            s.intellegent = row_kondisi[6].to_i
            s.warn = row_kondisi[7].to_i
            s.ban = row_kondisi[8].to_i
          end
        end

        Taguja.gramasi(PILIH_BARIS[:MILIK][:BY_PENGGUNA_ID] % id)
        row_milik = Array.new(6)
        if Taguja.lembaran(%q(milik), row_milik)
          usr.milik.tap do |m|
            m.serial = row_milik[2]
            m.pendakian = row_milik[3].to_i
            m.money = row_milik[4].to_i
            m.inventory = []
            m.pet = []
          end
        end
      end

      prosedur.push({ id: row_pengguna[0].to_i, usr: user, muat: psd })
      @kereta[row_pengguna[1]] = user
    end
    prosedur.each { |bentuk| bentuk[:muat].call(bentuk[:id], bentuk[:usr]) }
  end

  def pengguna_tersedia?(jid)
    return @kereta.has_key?(jid)
  end

  def pengguna_baru(jid)
    c = @owner.jid == jid ? 0 : 3
    seri = TagujaManajer.plugin.plugin_at(1).exports(jid)
    Taguja.gramasi(TAMBAHKAN_BARIS[:PENGGUNA] % jid)
    Taguja.gramasi(PILIH_BARIS[:PENGGUNA][:BY_JID] % jid)
    row_pengguna = Array.new(3)
    if Taguja.lembaran(%q(pengguna), row_pengguna)
      induk = row_pengguna[0].to_i
      Taguja.gramasi(TAMBAHKAN_BARIS[:KONDISI] % [induk, c])
      Taguja.gramasi(TAMBAHKAN_BARIS[:MILIK] % [induk, seri])
    end
    return Pengguna.new(jid).tap do |usr|
      usr.status.tap do |s|
        s.cincin = c
        s.nyawa = 100
        s.tenaga = 20
        s.reputasi = 1
        s.intellegent = 0
        s.warn = 0
        s.ban = 0
      end
      usr.milik.tap do |m|
        m.serial = seri
        m.pendakian = 0
        m.money = 0
        m.inventory = []
        m.pet = []
      end
      @kereta[jid] = usr
    end
  end

  def pengguna_dari_jid(jid)
    return @kereta[jid]
  end

  def ganti_owner(jid_owner_baru)
    owner = pengguna_dari_jid(jid_owner)
    pengganti = pengguna_dari_jid(jid_owner_baru)
    owner.status.cincin += 1
    pengganti.status.cincin = 0
    @owner.define_singleton_method(:jid) { jid_owner_baru }
  end

  def simpan
    @kereta.each do |jid, usr|
      Taguja.gramasi(PILIH_BARIS[:PENGGUNA][:BY_JID] % jid)
      row_pengguna = Array.new(3)
      if Taguja.lembaran(%q(pengguna), row_pengguna)
        usr.status.tap do |s|
          Taguja.gramasi(UBAH_BARIS[:KONDISI] % [s.cincin, s.nyawa, s.tenaga, s.reputasi, s.intellegent, s.warn, s.ban, row_pengguna[0].to_i])
        end
        usr.milik.tap do |m|
          Taguja.gramasi(UBAH_BARIS[:MILIK] % [m.pendakian, m.money, row_pengguna[0].to_i])
        end
      end
    end
  end

  def akses_seluruh_pengguna
    return proc do |&blk|
      @kereta.each do |k, pengguna|
        next if k == jid_owner
        blk.call(pengguna)
      end
    end
  end
end