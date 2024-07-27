module FreeInflow
  class FiturDaily < IndukFiturBot
    PERINTAH = %q(daily)
    UCAPAN = <<LEMBARAN
Selamat kamu mendapatkan:

+5000 Exp
+5000 Money
LEMBARAN

    def rutinitas
      pengguna.milik.money += 5000
      jawab(UCAPAN)
    end
  end

  class FiturWeekly < IndukFiturBot
    PERINTAH = %q(weekly)
    UCAPAN = <<LEMBARAN
Selamat kamu mendapatkan:

+20000 Exp
+20000 Money
+20 Limit
LEMBARAN

    def rutinitas
      pengguna.milik.money += 20000
      jawab(UCAPAN)
    end
  end

  class FiturMonthly < IndukFiturBot
    PERINTAH = %q(monthly)
    UCAPAN = <<LEMBARAN
Selamat kamu mendapatkan:

+40000 Exp
+40000 Money
+40 Limit
LEMBARAN

    def rutinitas
      pengguna.milik.money += 40000
      jawab(UCAPAN)
    end
  end
end