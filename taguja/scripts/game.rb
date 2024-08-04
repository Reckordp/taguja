module Game
  class Engine
    def initialize(t)
      @tunnel = t
    end

    def tambah_tunnel(id, meth)
      @tunnel[id] = meth
    end

    def pengguna
      return TagujaManajer.pengguna
    end

    def teruskan(id)
      return @tunnel[id]
    end

    def method_missing(meth, *args)
      if @tunnel.has_key?(meth)
        return teruskan(meth).call(*args)
      else
        super
      end
    end
  end

  class Perang
    class MasihAdaSesi < StandardError; end
    class TidakAdaMusuh < StandardError; end
    class TidakBertaruh < StandardError; end
    class Tahanan
      attr_accessor :tag, :pengguna, :seragam, :nyawa, :kekuatan
    end

    attr_reader :engine

    def initialize(engine)
      @regu = {}
      @engine = engine
      @bandar = 0
      @status = { mulai: false, antrian: nil, urutan: 0 }
      engine.tambah_tunnel(:war_player, method(:info))
      engine.tambah_tunnel(:war_join, method(:gabung))
      engine.tambah_tunnel(:war_leave, method(:keluar))
      engine.tambah_tunnel(:war_start, method(:start))
      engine.tambah_tunnel(:war_attack, method(:serang))
    end

    def info
      ucapan = String.new
      return %q(*「 Player Kosong 」*) if @regu.empty?
      ucapan += @status[:mulai] ? %q(*Medan Perang*) : %q(*Waiting Room*)
      ucapan += 10.chr + 10.chr
      @regu.each do |nama, deret|
        ucapan += "「 #{nama} 」\n"
        deret.each do |tahanan| 
          status = @status[:mulai] ? " (HP: #{tahanan.nyawa} Might: #{tahanan.kekuatan})" : String.new
          ucapan += tahanan.tag + status + 10.chr
        end
        ucapan += 10.chr
      end
      return ucapan
    end

    def buat_tahanan
      return Tahanan.new.tap do |t|
        t.pengguna = engine.pengguna
        t.seragam = engine.kini
        t.tag = %q(@) + t.pengguna
          .jid
          .sub(TagujaManajer::TAG_POSTFIX, String.new)
      end
    end

    def temukan(deret, pengguna)
      return deret.find { |t| t.pengguna == pengguna }
    end

    def temukan_kini(deret)
      return temukan(deret, engine.pengguna)
    end

    def gabung(nama)
      return false if @status[:mulai]
      kosong = []
      @regu.each do |nm, deret|
        ter = temukan_kini(deret)
        deret.delete(ter) unless ter.nil?
        kosong.push(nm) if deret.empty? && nama != nm
      end
      kosong.each { |i| @regu.delete(i) }
      deret = @regu[nama] ||= Array.new
      deret.push(buat_tahanan)
      return true
    end

    def keluar
      nama = nil
      hapus = nil
      @regu.each do |nm, deret|
        ter = temukan_kini(deret)
        unless ter.nil?
          deret.delete(ter)
          nama = nm
          if @status[:mulai]
            @status[:antrian].delete(ter.pengguna)
          end
        end
        hapus = nm if deret.empty?
      end
      @regu.delete(hapus) unless hapus.nil?
      return nama
    end

    def start
      deret_peserta = []
      deret_taruhan = []
      deret_anggrem = []
      deret_aktivasi = []
      sg = engine.kini
      raise MasihAdaSesi if @status[:mulai]
      raise TidakAdaMusuh if @regu.size < 2
      @regu.each do |nama, deret|
        deret.each do |tahanan|
          pg = tahanan.pengguna
          sr = tahanan.seragam
          engine.tukar(sr)
          tr = TagujaManajer.fitur.akumulasi_bandar
          deret_peserta.push(pg)
          deret_taruhan.push(tr)
          deret_anggrem.push(tahanan.tag) if tr.zero?
          aktivasi = proc do
            tahanan.nyawa = 100
            tahanan.kekuatan = 20
          end
          deret_aktivasi.push(aktivasi)
        end
      end
      engine.tukar(sg)
      unless deret_anggrem.empty?
        raise TidakBertaruh, deret_anggrem.join(%q( ))
      end
      acc = deret_taruhan.min
      deret_peserta.each do |pg|
        pg.terkena_efek(:money, acc * -1)
        @bandar += acc
      end
      @status[:mulai] = true
      @status[:antrian] = deret_peserta
      @status[:urutan] = 0
      deret_aktivasi.each(&:call)
      return acc
    end

    def serang(kasus)
      umum = %q(penggunaan: _.mwuach *@tag*_)
      return %q(Tidak ada sesi) unless @status[:mulai]
      return umum if kasus.nil?
      return umum unless kasus.index(32.chr).nil?
      return umum unless kasus[0] == 64.chr

      giliran = @status[:antrian][@status[:urutan]]
      tag = %q(@) + giliran.jid.sub(TagujaManajer::TAG_POSTFIX, String.new)
      if giliran != engine.pengguna
        return "Bukan giliranmu\nGiliran " + tag
      end

      ssr = kasus.dup
      ssr.slice!(0, 1)
      sasaran = ssr + TagujaManajer::TAG_POSTFIX
      pts = nil
      if engine.tersedia?(sasaran)
        pts = engine.cari_by_jid(sasaran)
      else
        return "#{kasus} tidak ada dalam database"
      end

      pg_rg = nil
      pts_rg = nil
      pg_t = nil
      pts_t = nil
      @regu.each do |nama, deret|
        pg_t_p = temukan_kini(deret)
        pts_t_p = temukan(deret, pts)
        unless pg_t_p.nil?
          pg_rg = nama
          pg_t = pg_t_p
        end
        unless pts_t_p.nil?
          pts_rg = nama
          pts_t = pts_t_p 
        end
      end

      return %q(Kamu tidak mengikuti War) if pg_t.nil?
      return "#{kasus} tidak mengikuti War" if pts_t.nil?
      return "Kamu dan #{kasus} satu regu" if pg_rg == pts_rg

      pts_t.nyawa -= pg_t.kekuatan
      if pts_t.nyawa < 1
        id = @status[:antrian].index(pts_t.pengguna)
        @status[:antrian].delete_at(id)
        @status[:urutan] -= 1 unless id > @status[:urutan]
      end
      return selesai(pg_rg) if @status[:antrian].size < 2

      @status[:urutan] = (@status[:urutan] + 1) % @status[:antrian].size
      giliran = @status[:antrian][@status[:urutan]]
      tag = %q(@) + giliran.jid.sub(TagujaManajer::TAG_POSTFIX, String.new)
      return "#{pg_t.tag} menyerang #{pts_t.tag} sampai nyawanya berkurang "
       .concat("#{pg_t.kekuatan}\n(Sisa HP: #{pts_t.nyawa})\n\n")
       .concat("Giliran #{tag}")
    end

    def selesai(regu)
      @regu[regu].each { |t| t.pengguna.terkena_efek(:money, @bandar) }
      @status[:mulai] = false
      @status[:antrian] = nil
      @status[:urutan] = 0
      @bandar = 0
      return "War dimenangkan oleh #{regu}"
    end
  end

  class PabrikGame
    attr_reader :game

    def initialize(t)
      @game = []
      engine = Engine.new(t)
      @game.push(Perang.new(engine))
    end
  end
end