class FiturJekpot < IndukFiturBot
  PERINTAH = %q(jekpot)

  def rutinitas
    jawab %q(Hah apa?)
  end

  # Kasus jekpot
  # 1 : List Plugin
  # 2 : List Script
  # 3 : Aktifkan statement
  def kasus
    return unless pengguna.status.cincin == 0
    if kasus_angka?
      ucapan = case sasaran.to_i
      when 1
        Taguja.gramasi(SQLStatement::SEMUA_PLUGIN)
        row_plugin = Array.new(5)
        ucapan = String.new
        while Taguja.lembaran(%q(plugins), row_plugin)
          ucapan += "#{row_plugin[2]}\n"
        end
        jawab(ucapan)
      when 2
        Taguja.gramasi(SQLStatement::SEMUA_SCRIPT)
        row_script = Array.new(5)
        ucapan = String.new
        while Taguja.lembaran(%q(scripts), row_script)
          ucapan += "#{row_script[2]}\n"
        end
        jawab(ucapan)
      when 3
        TagujaManajer.fitur.disable(FreeHidden::FiturDatabase)
      end
    else
      return rutinitas
    end
  end
end
