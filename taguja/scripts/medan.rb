TagujaManajer.memulai

module FreeHidden
  class FiturDatabase < IndukFiturBot
    PERINTAH = %q(db)
    ERROR_1 = %q([x] Tidak adanya spasi akan menyulitkan bacaan)
    ERROR_2 = %q([x] Harus ada token INTO)
    ERROR_3 = %q([x] Harus ada token SET)
    ERROR_S = %q([x] SELECT [*] yang dipakai disini)
    ERROR_F = %q([x] FROM tidak dimasukkan)

    def rutinitas
      jawab(%q(Masukkan statement SQL))
    end

    def kasus
      return jawab(%q([x] Enter setelah .db)) if sasaran[0] != 10.chr
      statement = sasaran.dup
      statement.slice!(0, 1) while statement[0] == 10.chr
      aksi = %w( SELECT INSERT UPDATE )
      urutan = statement.index(32.chr)
      return jawab(%q([x] Statement tidak lengkap)) if urutan.nil?
      prt = statement.slice!(0, urutan)
      terpilih = aksi.find { |s| s == prt }
      return jawab("[x] Aksi #{prt} tidak tersedia") if terpilih.nil?
      return jawab(ERROR_1) if statement.slice!(0, 1) != 32.chr
      mengeluarkan = false
      tabel = nil
      case terpilih
      when %q(SELECT)
        return jawab(ERROR_S) if statement.slice!(0, 1) != %q(*)
        return jawab(ERROR_1) if statement.slice!(0, 1) != 32.chr
        return jawab(ERROR_F) if statement.slice!(0, 4) != %q(FROM)
        return jawab(ERROR_1) if statement.slice!(0, 1) != 32.chr
        urutan = statement.index(32.chr)
        if urutan.nil?
          tabel = statement.dup
          tabel.slice!(-1, 1) if tabel[-1] == %q(;)
        else
          tabel = statement[0, urutan]
        end
        mengeluarkan = true
      when %q(INSERT)
        return jawab(ERROR_2) if statement[0, 4] != %q(INTO)
      when %q(UPDATE)
        urutan = statement.index(32.chr)
        return jawab(ERROR_1) if urutan.nil?
        statement.slice!(0, urutan + 1)
        return jawab(ERROR_3) if statement[0, 3] != %q(SET)
      end

      stmt = sasaran.dup
      stmt += %q(;) if stmt[-1] != %q(;)
      Taguja.gramasi(stmt)
      if mengeluarkan && !tabel.nil?
        ucapan = String.new
        tampung = Array.new(20)
        while Taguja.lembaran(tabel, tampung)
          tampung.each_with_index do |nilai, i|
            ucapan += "#{i + 1}: #{nilai}\n"
          end
          ucapan += "\n"
        end
        jawab(ucapan)
      else
        jawab(%q(oke))
      end
    end
  end
end