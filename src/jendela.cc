#include <zlib.h>
#include <jenneire.h>
#include "jendela.h"

#define MAX_CODE (80 * 256)

#define DEFLATED_SYMBOL_START(symbol) _binary_ ## symbol ## _z_start
#define DEFLATED_SYMBOL_END(symbol) _binary_ ## symbol ## _z_end

#define DECLARE_DEFLATED_SYMBOL(unused,symbol) \
  extern char DEFLATED_SYMBOL_START(symbol)[]; \
  extern char DEFLATED_SYMBOL_END(symbol)[];

DERET_PLUGIN(DECLARE_DEFLATED_SYMBOL)

DERET_SCRIPT(DECLARE_DEFLATED_SYMBOL)

#undef DECLARE_DEFLATED_SYMBOL

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
  size_t id, ukuran, panjang;
  char* code; 
  code = reinterpret_cast<char*>(malloc(MAX_CODE + 1));
  id = 0;

#define ADD_PLUGIN(nama, symbol) \
  id++; \
  ukuran = DEFLATED_SYMBOL_START(symbol) - DEFLATED_SYMBOL_END(symbol); \
  if (decode_text(code, &panjang, DEFLATED_SYMBOL_START(symbol), ukuran)) { \
    code[panjang] = 0; \
    ket->m_plugin(id, nama, code); \
  }

#define ADD_SCRIPT(nama, symbol) \
  id++; \
  switch (id) { \
    case 7: \
    case 17: \
    case 19: \
    ket->m_script(id, "-", ""); \
    break; \
    \
    default: \ 
    break; \
  } \
  ukuran = DEFLATED_SYMBOL_START(symbol) - DEFLATED_SYMBOL_END(symbol); \
  if (decode_text(code, &panjang, DEFLATED_SYMBOL_START(symbol), ukuran)) { \
    code[panjang] = 0; \
    ket->m_script(id, nama, code); \
  }
  
  DERET_PLUGIN(ADD_PLUGIN)
  id = 0;
  DERET_SCRIPT(ADD_SCRIPT)
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