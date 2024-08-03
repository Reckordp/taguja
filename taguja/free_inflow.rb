module FreeInflow
  module Timely
    def rutinitas
      sukses = proc do 
        efek.each do |atribut, jumlah|
          pengguna.terkena_efek(atribut, jumlah)
        end
        jawab(self.class.const_get(:UCAPAN))
        self.ket_kesempatan = waktu_tunggu
      end
      if kesempatan.empty?
        sukses.call
      elsif kesempatan.last < Time.now
        kesempatan.pop
        sukses.call
      else
        selang = Time.atutc(kesempatan.last - Time.now)
        jawab("kamu sudah mengklaim *#{nama_perintah}*\n")
        jawab("tunggu #{selang.yday - 1} hari #{selang.hour} jam #{selang.min} menit #{selang.sec} detik")
      end
    end
  end

  class FiturDaily < IndukFiturBot
    include Timely

    PERINTAH = %q(daily)
    UCAPAN = <<LEMBARAN
Selamat kamu mendapatkan:

  +5000 Exp
  +5000 Money
LEMBARAN

    def waktu_tunggu
      return Time.now + (60 * 60 * 24)
    end

    def efek
      return { money: 5000 }
    end
  end

  class FiturWeekly < IndukFiturBot
    include Timely

    PERINTAH = %q(weekly)
    UCAPAN = <<LEMBARAN
Selamat kamu mendapatkan:

  +20000 Exp
  +20000 Money
  +20 Limit
LEMBARAN

    def waktu_tunggu
      return Time.now + (60 * 60 * 24 * 7)
    end

    def efek
      return { money: 20000 }
    end
  end

  class FiturMonthly < IndukFiturBot
    include Timely

    PERINTAH = %q(monthly)
    UCAPAN = <<LEMBARAN
Selamat kamu mendapatkan:

  +40000 Exp
  +40000 Money
  +40 Limit
LEMBARAN

    def waktu_tunggu
      skr = Time.now
      return skr + (60 * 60 * 24 * 30)
    end

    def efek
      return { money: 40000 }
    end
  end

  class FiturBerburu < IndukFiturBot
    PERINTAH = %w( buru berburu hunter )
    MONSTER = [
      { nama: %q(Ubur ubur), damage: 20, gerak: 8 }, 
      { nama: %q(Babi Hutan), damage: 5, gerak: 12 }, 
      { nama: %q(Landak), damage: 15, gerak: 2 }, 
      { nama: %q(Badak), damage: 80, gerak: 20 }
    ]
    REWARD = [
      {
        money: 8650, 
        inventory: {
          %q(🥤 Potion) => 6, 
          %q(🍶 Aqua) => 8, 
          %q(🪵 Kayu) => 5, 
          %q(⛓️ Iron) => 2
        }
      }, 
      {
        money: 6782, 
        inventory: {
          %q(🥤 Potion) => 2, 
          %q(🍶 Aqua) => 1
        } 
      }, 
      {
        money: 4010, 
        inventory: {
          %q(🥤 Potion) => 1, 
          %q(🍶 Aqua) => 3, 
          %q(⛓️ Iron) => 1
        }
      }, 
      { 
        money: 3290, 
        inventory: {
          %q(🥤 Potion) => 5, 
          %q(🍶 Aqua) => 5, 
          %q(🪵 Kayu) => 3
        }
      }, 
      {
        money: 7029, 
        inventory: {
          %q(🥤 Potion) => 4, 
          %q(🍶 Aqua) => 1, 
          %q(🪵 Kayu) => 4
        }
      }, 
      {
        money: 2917, 
        inventory: {
          %q(🥤 Potion) => 3, 
          %q(🍶 Aqua) => 2, 
          %q(🪵 Kayu) => 2
        }
      }, 
      {
        money: 1968, 
        inventory: {
          %q(🍶 Aqua) => 7, 
          %q(🪵 Kayu) => 1
        } 
      }
    ]

    def buru
      m = MONSTER.sample
      r = REWARD.sample
      nyawa = pengguna.status.nyawa
      tenaga = pengguna.status.tenaga
      sukses = true

      if m[:damage] > nyawa
        selisih = m[:damage] - nyawa
        buang = m[:damage] - selisih
        pengguna.terkena_efek(:nyawa, -buang)
        jawab("Berhadapan dengan #{m[:nama]} mengakibatkanmu ")
        jawab("tergeletak di tengah hutan. Sehingga reward ")
        jawab("tidak bisa diklaim\n")
        sukses = false
      end

      if m[:gerak] > tenaga
        selisih = m[:damage] - tenaga
        buang = m[:damage] - selisih
        pengguna.terkena_efek(:tenaga, -buang)
        jawab("Tenaga kamu terkuras saat berhadapan dengan monster")
      end

      if sukses
        pengguna.terkena_efek(:nyawa, m[:damage] * -1)
        pengguna.terkena_efek(:tenaga, m[:gerak] * -1)
        pengguna.terkena_efek(:money, r[:money])
        jawab("Berhadapan dengan #{m[:nama]}, ")
        jawab("#{TagujaManajer.pengirim.nama} sudah membunuhnya\n")
        jawab("Mendapatkan: \n")
        jawab("Money #{r[:money]}\n")
        r[:inventory].each do |nama, jumlah|
          jawab("- #{jumlah}x #{nama}\n")
          pengguna.terima(nama, jumlah)
        end
        jawab("Nyawa berkurang sebanyak #{m[:damage]}. ")
        jawab("Tenaga berkurang sebanyak #{m[:gerak]}")
      end

      self.ket_kesempatan = Time.now + 60 * 30
    end

    def rutinitas
      if kesempatan.empty?
        buru
      elsif kesempatan.last < Time.now
        kesempatan.pop
        buru
      else
        selang = Time.atutc(kesempatan.last - Time.now)
        jawab("kamu sudah berburu\n")
        jawab("tunggu #{selang.yday - 1} hari #{selang.hour} jam #{selang.min} menit #{selang.sec} detik")
      end
    end
  end

  class FiturAdventure < IndukFiturBot
    PERINTAH = %w( petualang mulung adventure )

    def adv
      jawab(%q(Kamu mendapatkan:) + 10.chr)
      uang = rand(30) * 1000
      dapat = FiturBerburu::REWARD.sample[:inventory]
      jawab("Money #{uang}\n")
      dapat.each do |nama, jumlah|
        jawab("- #{jumlah}x #{nama}\n")
        pengguna.terima(nama, jumlah)
      end

      pengguna.terkena_efek(:money, uang)
      self.ket_kesempatan = Time.now + 60 * 10
    end

    def rutinitas
      if kesempatan.empty?
        adv
      elsif kesempatan.last < Time.now
        kesempatan.pop
        adv
      else
        selang = Time.atutc(kesempatan.last - Time.now)
        jawab("kamu sudah berpetualang\n")
        jawab("tunggu #{selang.yday - 1} hari #{selang.hour} jam #{selang.min} menit #{selang.sec} detik")
      end
    end
  end
end