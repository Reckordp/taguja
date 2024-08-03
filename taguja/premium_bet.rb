module PremiumBet
  DASAR_NOMINAL = [1, 2, 5]
  @nomer = 0
  @kelipatan = 10

  def self.ambil_nominal
    urutan = @nomer < 10 ? @nomer % 3 : 0
    @kelipatan *= 10 if urutan.zero?
    nominal = DASAR_NOMINAL[urutan] * @kelipatan
    @nomer += 1
    return nominal
  end

  module Bet
    def self.included(kelazz)
      n = PremiumBet.ambil_nominal
      kelazz.define_method(:nominal) { next n }
    end

    def taruhan?
      return true
    end

    def rutinitas
      money = pengguna.milik.money
      if nominal > money
        return jawab("Tidak bisa bertaruh #{nominal}, moneymu hanya #{money}")
      end
      t = pasang_taruhan(nominal)
      if t.nil?
        jawab("Taruhan gagal dipasang")
      else
        tempat = nominal.to_s
        sisa = tempat.size
        urutan = []
        while sisa > 0
          urutan.push(sisa)
          sisa -= 3
        end
        until urutan.empty?
          u = urutan.shift
          tempat.insert(u, %q(.))
        end
        tempat.slice!(-1, 1)
        jawab("Bertaruh #{tempat}")
      end
    end
  end

  class FiturAngzai < IndukFiturBot
    PERINTAH = %q(angzai)
    include Bet
  end

  class FiturAzza < IndukFiturBot
    PERINTAH = %q(azza)
    include Bet
  end

  class FiturHah < IndukFiturBot
    PERINTAH = %q(hah)
    include Bet
  end

  class FiturLah < IndukFiturBot
    PERINTAH = %q(lah)
    include Bet
  end

  class FiturEs < IndukFiturBot
    PERINTAH = %q(es)
    include Bet
  end

  class FiturGas < IndukFiturBot
    PERINTAH = %q(gas)
    include Bet
  end

  class FiturSekop < IndukFiturBot
    PERINTAH = %q(sekop)
    include Bet
  end

  class FiturSok < IndukFiturBot
    PERINTAH = %q(sok)
    include Bet
  end

  class FiturNgerina < IndukFiturBot
    PERINTAH = %q(ngerina)
    include Bet
  end

  class FiturEsqiel < IndukFiturBot
    PERINTAH = %q(esqiel)
    include Bet
  end

  class FiturSoop < IndukFiturBot
    PERINTAH = %q(soop)
    include Bet
  end

  class FiturSoof < IndukFiturBot
    PERINTAH = %q(soof)
    include Bet
  end

  class FiturSp < IndukFiturBot
    PERINTAH = %q(sp)
    include Bet
  end

  class FiturPawang < IndukFiturBot
    PERINTAH = %q(pawang)
    include Bet
  end

  class FiturMbergis < IndukFiturBot
    PERINTAH = %q(mbergis)
    include Bet
  end
end