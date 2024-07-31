#include <zlib.h>
#include <jenneire.h>
#include "jendela.h"

#define MAX_CODE (80 * 256)

extern char _binary_penjawab_pesan_z_start[];
extern char _binary_penjawab_pesan_z_end[];
extern char _binary_pangkalan_z_start[];
extern char _binary_pangkalan_z_end[];
extern char _binary_transaksi_z_start[];
extern char _binary_transaksi_z_end[];
extern char _binary_sql_statement_z_start[];
extern char _binary_sql_statement_z_end[];
extern char _binary_barang_ransel_z_start[];
extern char _binary_barang_ransel_z_end[];
extern char _binary_user_personal_z_start[];
extern char _binary_user_personal_z_end[];
extern char _binary_owner_only_z_start[];
extern char _binary_owner_only_z_end[];
extern char _binary_premium_event_z_start[];
extern char _binary_premium_event_z_end[];
extern char _binary_premium_limit_z_start[];
extern char _binary_premium_limit_z_end[];
extern char _binary_premium_daily_z_start[];
extern char _binary_premium_daily_z_end[];
extern char _binary_premium_bet_z_start[];
extern char _binary_premium_bet_z_end[];
extern char _binary_free_common_z_start[];
extern char _binary_free_common_z_end[];
extern char _binary_free_common_a_z_start[];
extern char _binary_free_common_a_z_end[];
extern char _binary_free_inflow_z_start[];
extern char _binary_free_inflow_z_end[];
extern char _binary_free_outflow_z_start[];
extern char _binary_free_outflow_z_end[];
extern char _binary_medan_z_start[];
extern char _binary_medan_z_end[];

const char *nama_pangkalan = "jennerie";

Taguja* kuasa = NULL;

int decode_text(char* dest, size_t* length, char* decoded, size_t ukuran) {
  z_stream s;
  int res;

  memset(&s, 0, sizeof(z_stream));
  s.next_in = reinterpret_cast<Bytef*>(decoded);
  s.avail_in = ukuran;
  s.next_out = reinterpret_cast<Bytef*>(dest);
  s.avail_out = MAX_CODE;
  inflateInit(&s);
  res = inflate(&s, Z_FINISH);
  inflateEnd(&s);

  if (res != Z_OK && res != Z_STREAM_END && res != Z_BUF_ERROR) {
    return 0;
  } else {
    *length = MAX_CODE - s.avail_out;
    return 1;
  }
}

void mula_database(keterangan_mula_database* ket) {
  size_t ukuran, panjang;
  char* code; 
  code = reinterpret_cast<char*>(malloc(MAX_CODE + 1));

  ukuran = _binary_pangkalan_z_end - _binary_pangkalan_z_start;
  if (decode_text(code, &panjang, _binary_pangkalan_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(1, "Pangkalan", code);
  }

  ukuran = _binary_transaksi_z_end - _binary_transaksi_z_start;
  if (decode_text(code, &panjang, _binary_transaksi_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(2, "Transaksi", code);
  }

  ukuran = _binary_sql_statement_z_end - _binary_sql_statement_z_start;
  if (decode_text(code, &panjang, _binary_sql_statement_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(3, "SQL Statement", code);
  }

  ukuran = _binary_barang_ransel_z_end - _binary_barang_ransel_z_start;
  if (decode_text(code, &panjang, _binary_barang_ransel_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(4, "Barang Ransel", code);
  }

  ukuran = _binary_user_personal_z_end - _binary_user_personal_z_start;
  if (decode_text(code, &panjang, _binary_user_personal_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(5, "User Personal", code);
  }
  
  ket->m_script(6, "-", "");

  ukuran = _binary_owner_only_z_end - _binary_owner_only_z_start;
  if (decode_text(code, &panjang, _binary_owner_only_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(7, "Owner Only", code);
  }

  ukuran = _binary_premium_event_z_end - _binary_premium_event_z_start;
  if (decode_text(code, &panjang, _binary_premium_event_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(8, "Premium Event", code);
  }

  ukuran = _binary_premium_limit_z_end - _binary_premium_limit_z_start;
  if (decode_text(code, &panjang, _binary_premium_limit_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(9, "Premium Limit", code);
  }

  ukuran = _binary_premium_daily_z_end - _binary_premium_daily_z_start;
  if (decode_text(code, &panjang, _binary_premium_daily_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(10, "Premium Daily", code);
  }

  ukuran = _binary_premium_bet_z_end - _binary_premium_bet_z_start;
  if (decode_text(code, &panjang, _binary_premium_bet_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(11, "Premium Bet", code);
  }

  ukuran = _binary_free_common_z_end - _binary_free_common_z_start;
  if (decode_text(code, &panjang, _binary_free_common_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(12, "Free Common", code);
  }

  ukuran = _binary_free_common_a_z_end - _binary_free_common_a_z_start;
  if (decode_text(code, &panjang, _binary_free_common_a_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(13, "Free Common A", code);
  }

  ukuran = _binary_free_inflow_z_end - _binary_free_inflow_z_start;
  if (decode_text(code, &panjang, _binary_free_inflow_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(14, "Free InFlow", code);
  }

  ukuran = _binary_free_outflow_z_end - _binary_free_outflow_z_start;
  if (decode_text(code, &panjang, _binary_free_outflow_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(15, "Free OutFlow", code);
  }

  ket->m_script(16, "-", "");

  ukuran = _binary_medan_z_end - _binary_medan_z_start;
  if (decode_text(code, &panjang, _binary_medan_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(17, "Medan", code);
  }
  ket->m_script(18, "-", "");

  free(code);
}

void tg_simpan_database(mrb_state* diri) {
  struct RClass* tm;
  tm = mrb_module_get(diri, "TagujaManajer");
  mrb_funcall(diri, mrb_obj_value(tm), "simpan_keterangan", 0);
}

static DWORD gerbang_jenneire(void* void_ptr) {
  HINSTANCE inst;
  inst = *reinterpret_cast<HINSTANCE*>(void_ptr);

  InitCommonControls();
  buat_jendela(inst);
  tg_simpan_database(kuasa->shipped()->diri);
  if (kuasa) kuasa->V8Keluar();
  return 0;
}

void perform_logout() {
  if (kuasa) kuasa->Logout();
}

void JendelaInit(HINSTANCE* pinst) {
  HANDLE event_loop;
  DWORD loop_id;
  jenneire_mulai();
  event_loop = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gerbang_jenneire, pinst, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION, &loop_id);
  ResumeThread(event_loop);
  tentukan_perform_logout(perform_logout);
}

void PesanDatang(pesan_rincian* pesan) {
  std::string text = std::string(pesan->nama.length() > 0 ? pesan->nama + std::string(": ") : "") + pesan->text;
  if (!pesan->fromMe) {
    rekam_pesan(pesan->chat.c_str(), pesan->subjek.c_str(), text.c_str());
  } 
  PikirkanJawaban(pesan);
}

void Nusaterlogin(char* nama, char* id) {
  login_diterima(nama, id);
}

void Nusaterlogout() {
  logout_diproses();
}

void TentukanPenguasa(Taguja* cls) {
  kuasa = cls;
}