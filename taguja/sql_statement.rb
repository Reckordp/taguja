module SQLStatement
  SQ = 0x27.chr.freeze
  BUAT_TABEL = {
    PENGGUNA: %q(CREATE TABLE pengguna ( id SERIAL PRIMARY KEY, jid VARCHAR(32), created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP );), 
    KONDISI: %q(CREATE TABLE kondisi ( id SERIAL PRIMARY KEY, induk INT REFERENCES pengguna(id) ON DELETE CASCADE, cincin INT, nyawa INT, tenaga INT, reputasi INT, intellegent INT, warn INT, ban INT, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP );), 
    MILIK: %q(CREATE TABLE milik ( id SERIAL PRIMARY KEY, induk INT REFERENCES pengguna(id) ON DELETE CASCADE, kode VARCHAR(255), pendakian BIGINT, money BIGINT, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP );), 
    RANSEL: %q(CREATE TABLE ransel ( id BIGSERIAL PRIMARY KEY, induk INT REFERENCES milik(id) ON DELETE CASCADE, nama VARCHAR(255), jumlah INT, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP );), 
    PET: %q(CREATE TABLE peliharaan ( id BIGSERIAL PRIMARY KEY, induk INT REFERENCES milik(id) ON DELETE CASCADE, nama VARCHAR, level INT, exp INT, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP );), 
    JEJAK: %q(CREATE TABLE jejak (id SERIAL PRIMARY KEY, induk INT REFERENCES pengguna(id) ON DELETE CASCADE, level INT, exp INT, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP );)
  }
  PILIH_BARIS = {
    PENGGUNA: {
      SEMUA: %q(SELECT * FROM pengguna;), 
      BY_JID: "SELECT * FROM pengguna WHERE jid = #{SQ}%s#{SQ} LIMIT 1;"
    }
    KONDISI: {
      BY_PENGGUNA_ID: "SELECT * FROM kondisi WHERE induk = %d LIMIT 1;"
    }
    MILIK: {
      BY_PENGGUNA_ID: "SELECT * FROM milik WHERE induk = %d LIMIT 1;"
    }
  }
  TAMBAHKAN_BARIS = {
    PENGGUNA: "INSERT INTO pengguna ( jid ) VALUES ( #{SQ}%s#{SQ} );", 
    KONDISI: "INSERT INTO kondisi ( induk, cincin, nyawa, tenaga, reputasi, intellegent, warn, ban ) VALUES ( %d, %d, 100, 20, 1, 0, 0, 0 );", 
    MILIK: "INSERT INTO milik ( induk, kode, pendakian, money ) VALUES ( %d, #{SQ}%s#{SQ}, 0, 0 );"
  }
  UBAH_BARIS = {
    KONDISI: "UPDATE kondisi SET ( cincin, nyawa, tenaga, reputasi, intellegent, warn, ban ) = ( %d, %d, %d, %d, %d, %d, %d ) WHERE induk = %d;", 
    MILIK: "UPDATE milik SET ( pendakian, money ) = ( %d, %d ) WHERE induk = %d;"
  }
end