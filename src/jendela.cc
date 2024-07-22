#include <zlib.h>
#include <jenneire.h>
#include "jendela.h"

#define MAX_CODE (80 * 58)

extern char _binary_penjawab_pesan_z_start[];
extern char _binary_penjawab_pesan_z_end[];

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
  code = (char*)malloc(MAX_CODE + 1);
  ukuran = _binary_penjawab_pesan_z_end - _binary_penjawab_pesan_z_start;
  if (decode_text(code, &panjang, _binary_penjawab_pesan_z_start, ukuran)) {
    code[panjang] = 0;
    ket->m_script(0, "Penjawab Pesan", code);
    ket->m_script(1, "(Other)", "");
  }
}

static DWORD gerbang_jenneire(void* void_ptr) {
  HINSTANCE inst;
  inst = *reinterpret_cast<HINSTANCE*>(void_ptr);

  InitCommonControls();
  buat_jendela(inst);
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