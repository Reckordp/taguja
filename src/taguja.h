#ifndef SRC_TAGUJA_H_
#define SRC_TAGUJA_H_

#include "node.h"
#include "node_mutex.h"
#include <unordered_map>
#include <map>
#include <windef.h>
#include <processthreadsapi.h>

extern "C" {
  #include <shipped.h>
}

#include <mruby/data.h>

class node::Environment;
class Taguja;
struct putaran;
typedef struct taguja_pesan_rincian pesan_rincian;
using namespace node;
using BuiltinCodeCacheMap =
    std::unordered_map<std::string,
                       std::unique_ptr<v8::ScriptCompiler::CachedData>>;
// using PersistentModule = v8::Persistent<v8::Module, v8::CopyablePersistentTraits<v8::Module>>;
using PersistentModule = v8::Persistent<v8::Module, v8::CopyablePersistentTraits<v8::Module>>;
using PenangkapLogin = void (*)(int,std::vector<std::vector<bool>>);
using PenangkapPesan = void (*)(pesan_rincian*);
using Nusalogin = void (*)(char*,char*);
using Nusalogout = void (*)();
// typedef struct {
// 	char folder[64];
// 	off_t patokan;
// } shipped_himpunan;

struct bungkus_lorong {
  HANDLE tanda;
  std::string id;
  std::string filename;
  v8::Persistent<v8::Value> inti;
  struct putaran* penjaga;
};

struct bungkus_lorong_esm {
  std::string id;
  std::string filename;
  v8::Persistent<v8::Module> inti;
};

struct putaran {
  DWORD id;
  HANDLE kendali;
  HANDLE tanda;
  int dalam_proses;
  Taguja* taguja;
  v8::Persistent<v8::Object> pendirian;
  v8::Persistent<v8::Function> portal;
  v8::MaybeLocal<v8::Value> result;
};

struct esm_rincian {
  std::string id;
  PersistentModule bawa;
  v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> exports;
};

typedef struct taguja_pesan_rincian {
  Taguja* kuasa;
  std::string id;
  std::string chat;
  std::string subjek;
  std::string sender;
  std::string nama;
  bool fromMe;
  std::string text;
  std::string respon;
} pesan_rincian;

class Taguja {
  public:
  Taguja(Environment*);
  ~Taguja();
  shipped_rincian* shipped();
  compiler_rincian* shipped_compiler_rincian();
  v8::Local<v8::Primitive> UndefinedA();
  void Masuk();
  int ReadEntry(char*, size_t*);
  int DecodeEntry(char*, size_t*);
  // v8::MaybeLocal<v8::Function> LoadSource(const char*);
  v8::MaybeLocal<v8::Value> LoadShipped(const char*);
  v8::MaybeLocal<v8::Value> LoadEntry();
  v8::MaybeLocal<v8::Value> LoadEntryFileOrZlib();
  void MulaiESM();
  v8::Local<v8::Module> BuatSyntheticModule(struct esm_rincian*, v8::Local<v8::Object>);
  bool CacheAvailableESM(const char*); 
  v8::MaybeLocal<v8::Module> ResolveESM(v8::Isolate*, const char*);
  void PanggilShipped(v8::Local<v8::Function>, struct bungkus_lorong*);
  v8::MaybeLocal<v8::Function> Compile(compiler_rincian*);
  void BungkusExports(v8::Local<v8::Module>, struct bungkus_lorong*);
  // void TahanESM(struct bungkus_lorong_esm*);
  v8::MaybeLocal<v8::Module> LoadESM(compiler_rincian*, struct bungkus_lorong_esm* = NULL); 
  v8::MaybeLocal<v8::Value> BuatPenahan(std::string);
  v8::Local<v8::Function> BungkusRequire(const char*, v8::Local<v8::Function>);
  void ProsesPutaranInternal(struct putaran*);
  static DWORD ProsesPutaran(void*);
  void UraiJSON(compiler_rincian*, struct bungkus_lorong*);
  void Logout();
  void V8Keluar();

  v8::Persistent<v8::Function> cjs_loader_;
  v8::Persistent<v8::Function> esm_translators_;
  v8::Persistent<v8::Function> internal_url_;
  v8::Persistent<v8::Function> login_qrcode_;
  v8::Persistent<v8::Function> respon_pesan_;
  v8::Persistent<v8::Function> login_nusa_;
  v8::Persistent<v8::Function> logout_nusa_;
  v8::Persistent<v8::Function> logout_;
  v8::Persistent<v8::Object> cl_module_;
  v8::Persistent<v8::Function> ecl_pengubah_cjs_;
  v8::Persistent<v8::Function> ecl_pengubah_lib_;
  v8::Persistent<v8::Object> entry_;
  v8::Persistent<v8::Object> entry_entry_;
  v8::Persistent<v8::Function> entry_logout_;
  v8::Persistent<v8::Function> cjs_source_cache_;
  PenangkapLogin penangkap_login_;
  PenangkapPesan penangkap_pesan_;
  Nusalogin nusa_login_;
  Nusalogout nusa_logout_;
  HANDLE instance;
  bool logined;

  private:
  mrb_state* diri_;
  Environment* env_;
  shipped_rincian shipped_;
  compiler_rincian crin_;
  BuiltinCodeCacheMap code_cache_;
  Mutex code_cache_mutex_;
  std::string lokasi_entry_;
  std::vector<struct putaran*> deret_putaran_;
  v8::Persistent<v8::Function> require_;
  v8::Persistent<v8::Object> kendali_circular_;
  std::map<std::string, PersistentModule> cache_esm_;
};

#endif