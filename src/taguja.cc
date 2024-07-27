#include <stdlib.h>
#include <zlib.h>
#include "env.h"
#include "env-inl.h"
#include "node_builtins.h"
#include "module_wrap.h"
#include "node_contextify.h"
#include <libplatform/libplatform.h>



using namespace v8;
using Loader = builtins::BuiltinLoader;
enum TagujaScriptType : int {
  kScript,
  kModule,
  kFunction,
};

enum HostDefinedOptions : int {
  kType = 8,
  kID = 9,
  kLength = 10,
};

MaybeLocal<Module> taguja_resolve_module_callback(Local<Context>, Local<String>, Local<FixedArray>, Local<Module>);
void shipped_require(const v8::FunctionCallbackInfo<v8::Value>&);
void lorong_compiler(compiler_rincian*);
void lorong_esm_compiler(compiler_rincian*);
void bungkus_lorong_free(mrb_state*, void*);
void bungkus_lorong_esm_free(mrb_state*, void*);

const struct mrb_data_type lorong_type = {
	"Lorong", bungkus_lorong_free
};

const struct mrb_data_type lorong_esm_type = {
	"LorongESM", bungkus_lorong_esm_free
};

#define LETAK_ENTRY "author/Reckordp/taguja.js"
#define UKURAN_SCRIPT (INT16_MAX)

// extern shipped_himpunan _binary_shipped_index_start[];
// extern shipped_himpunan _binary_shipped_index_end[];
extern char _binary_taguja_z_start[];
extern char _binary_taguja_z_end[];

std::vector<std::string> builtinids;
std::unordered_multimap<int, struct esm_rincian> moduleHash;

void shipped_resolve_filename(const FunctionCallbackInfo<Value>& args) {
  Environment* e = Environment::GetCurrent(args);
  Isolate* isolate = e->isolate();
  HandleScope scope(isolate);
  Local<Context> context = e->context();
  Local<String> request, baileys_string, errmsg;
  Local<Object> mod, options, err;
  Local<Value> result;
  MaybeLocal<Value> maybe;
  std::string tali;
  bool isMain;

  request = args[0].As<String>();
  mod = args[1].As<Object>();
  isMain = args[2].As<Boolean>()->BooleanValue(isolate);
  if (args.Length() > 3) options = args[3].As<Object>();
  node::Utf8Value id_v(isolate, request);
  const char* id = *id_v, *filename;
  result = Undefined(isolate);

  if (isMain) {
    // Memuat entry
    filename = strrchr(id, 92);
    if (filename) id = filename + 1;
    if (strcmp(id, "taguja.js") == 0) {
      result = OneByteString(isolate, LETAK_ENTRY);
    } else {
      tali = std::string("Tidak dapat menemukan Main ") + id;
      errmsg = OneByteString(isolate, tali.data(), tali.size());
      isolate->ThrowException(Exception::TypeError(errmsg));
      return;
    }
  } else {
    if (builtinids.size() < 1) {
      Loader* loader = Loader::GetInstance();
      builtinids = loader->GetBuiltinIds();
    }
    
    baileys_string = OneByteString(isolate, "baileys");
    std::string prefix = std::string("shipped:");
    std::string prefix_taguja = std::string("taguja:");
    std::string jalur;
    if (std::find(builtinids.begin(), builtinids.end(), id) != builtinids.end()) { 
      result = request;
    } else if (strcmp(id, "baileys") == 0) {
      result = baileys_string;
    } else if (strcmp(id, "@whiskeysockets/baileys") == 0) {
      result = baileys_string;
    } else if (strcmp(id, "translator_cjs") == 0) {
      jalur = prefix_taguja + "pengubah/cjs";
      result = OneByteString(isolate, jalur.c_str(), jalur.size());
    } else if (strcmp(id, "translator_lib") == 0) {
      jalur = prefix_taguja + "pengubah/lib";
      result = OneByteString(isolate, jalur.c_str(), jalur.size());
    } else if (strcmp(id, "taguja:entry/entry.js") == 0) {
      result = request;
    } else if (shipped_exist(e->taguja()->shipped(), id)) {
      jalur = prefix + id;
      result = OneByteString(isolate, jalur.c_str(), jalur.size());
    } else {
      maybe = {};
      errmsg = OneByteString(isolate, "Cannot find module '%module_name%'");
      err = Exception::Error(errmsg).As<Object>();
      err->Set(context, FIXED_ONE_BYTE_STRING(isolate, "code"), 
        FIXED_ONE_BYTE_STRING(isolate, "MODULE_NOT_FOUND")).Check();
      isolate->ThrowException(err);
      return;
    }
  }

  if (!result->IsUndefined()) {
    args.GetReturnValue().Set(result);
  }
}

MaybeLocal<Value> taguja_require(Environment* env, std::string id) {
  Isolate* isolate = env->isolate();
  EscapableHandleScope scope(isolate);
  Local<Context> context = env->context();
  Taguja* penyedia = env->taguja();
  Local<Value> value;
  Local<Object> target;
  Local<String> errmsg;

  MaybeLocal<Value> maybe;
  if (shipped_exist(env->taguja()->shipped(), id.c_str())) {
    maybe = env->taguja()->LoadShipped(id.c_str());
    if (!maybe.ToLocal(&value)) return {};
    if (value->IsProxy()) return scope.Escape(value);
    target = value.As<Object>();
  } else {
    errmsg = OneByteString(isolate, "Module tidak ditemukan");
    isolate->ThrowException(Exception::TypeError(errmsg));
    return {};
  }

  MaybeLocal<Value> hasil;
  hasil = target->Get(context, FIXED_ONE_BYTE_STRING(isolate, "exports"));
  return scope.EscapeMaybe(hasil);
}

MaybeLocal<Module> taguja_resolve_module_callback(Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer) {
  Environment *env = Environment::GetCurrent(context);
  Taguja *penyedia = env->taguja();
  mrb_state *diri = penyedia->shipped()->diri;
  Isolate* isolate = context->GetIsolate();
  TryCatch tc(isolate);
  MaybeLocal<Module> ret;
  String::Utf8Value specifier_v(isolate, specifier);
  const char *str = *specifier_v;

  if (penyedia->CacheAvailableESM(str)) {
    // std::cout << "Mencari " << str << std::endl;
    ret = penyedia->ResolveESM(isolate, str);
    if (!ret.IsEmpty()) return ret;
  }

  mrb_value modr = shipped_muat(penyedia->shipped(), str);

  if (tc.HasCaught() && !tc.HasTerminated()) {
    CHECK(!tc.Message().IsEmpty());
    CHECK(!tc.Exception().IsEmpty());
    Local<String> msg_s = tc.Message()->Get();
    Utf8Value msg_v(isolate, msg_s);
    const char *msg = *msg_v;
    // std::cout << "Error: " << msg << std::endl; 
    node::Abort();
    return MaybeLocal<Module>();
  }

  if (diri->exc) return MaybeLocal<Module>();
  if (mrb_nil_p(modr)) return MaybeLocal<Module>();
  if (mrb_symbol_p(modr)) {
    if (mrb_symbol(modr) == mrb_intern_lit(diri, "loading")) {
      return MaybeLocal<Module>();
    }
    return MaybeLocal<Module>();
  }

  struct bungkus_lorong_esm* bungkus;
  bungkus = reinterpret_cast<struct bungkus_lorong_esm*>(DATA_PTR(modr));
  Local<Module> inti;
  if (bungkus->inti.IsEmpty()) return MaybeLocal<Module>();
  inti = bungkus->inti.Get(isolate);
  return inti;
}

void taguja_genpath(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  MaybeLocal<Value> pathname;
  Local<Object> url;
  url = args[0].As<Object>();
  pathname = url->Get(context, FIXED_ONE_BYTE_STRING(isolate, "pathname"));
  args.GetReturnValue().Set(pathname.FromMaybe(FIXED_ONE_BYTE_STRING(isolate, "baileys").As<Value>()));
}

void taguja_cache_sedia(const FunctionCallbackInfo<Value>& args) {
  Environment *env = Environment::GetCurrent(args);
  Isolate *isolate = env->isolate();
  Local<Context> context = env->context();
  Taguja *penyedia = env->taguja();
  Utf8Value id_v(isolate, args[1]);
  const char *id = *id_v;
  const char *jalur = strchr(id, 58);
  std::string str(jalur ? jalur + 1 : id);

  Local<Object> result;
  if (str == LETAK_ENTRY) {
    result = penyedia->entry_.Get(isolate);
  } else if (str == "entry/entry.js") {
    result = penyedia->entry_entry_.Get(isolate);
  }

  if (!result.IsEmpty()) {
    args.GetReturnValue().Set(result);
    return;
  }

  MaybeLocal<Value> maybe;
  Local<Value> hasil;
  maybe = env->taguja()->LoadShipped(id);
  if (maybe.ToLocal(&hasil)) {
    args.GetReturnValue().Set(hasil);
  }
}

void taguja_bungkus_cjs(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate;
  Local<Context> context;
  Taguja* penyedia;
  MaybeLocal<Value> ret;
  Local<Value> mod, xpr, inti, baileys, entry, larik[2];
  Local<String> key_string, errmsg;
  Local<Object> obj, cache, instance;//, proto;
  Local<Function> resolve, teruskan;
  std::vector<Local<Value>> param{};

  isolate = args.GetIsolate();
  HandleScope scope(isolate);
  context = isolate->GetCurrentContext();
  penyedia = Environment::GetCurrent(isolate)->taguja();
  // Local<Object> global = context->Global();
  for (int i = 0; i < args.Length(); i++) {
    param.insert(param.begin() + i, args[i]);
  }
  // inti = global->Get(context, FIXED_ONE_BYTE_STRING(isolate, "_taguja_teruskan")).ToLocalChecked();
  teruskan = penyedia->cjs_loader_.Get(isolate);//inti.As<Function>();
  ret = teruskan->Call(context, Undefined(isolate), param.size(), param.data());
  mod = param[2];
  if (mod->IsObject()) {
    obj = mod.As<Object>();
  } else {
    errmsg = OneByteString(isolate, "CJSLoader: module undefined");
    isolate->ThrowException(Exception::TypeError(errmsg));
    return;
  }

  xpr = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "exports")).ToLocalChecked();
  if (xpr->IsObject()) {
    obj = xpr.As<Object>();
  } else {
    errmsg = OneByteString(isolate, "CJSLoader: module.exports undefined");
    isolate->ThrowException(Exception::TypeError(errmsg));
    return;
  }

  // inti = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "cjsParseCache")).ToLocalChecked();
  // if (inti->IsObject()) {
  //   obj = inti.As<Object>();
  //   inti = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "set")).ToLocalChecked();
  //   if (inti->IsFunction()) {
  //     penyedia->cjs_source_cache_.Reset(isolate, inti.As<Function>());
  //   } else {
  //     errmsg = OneByteString(isolate, "CJSLoader: cjsParseCache.set undefined");
  //     isolate->ThrowException(Exception::TypeError(errmsg));
  //     return;
  //   }
  // } else {
  //   errmsg = OneByteString(isolate, "CJSLoader: cjsParseCache undefined");
  //   isolate->ThrowException(Exception::TypeError(errmsg));
  //   return;
  // }

  inti = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "Module")).ToLocalChecked();
  if (inti->IsObject()) {
    // key_string = FIXED_ONE_BYTE_STRING(isolate, "_taguja_module");
    // global->Set(context, key_string, inti).Check();
    obj = inti.As<Object>();
    penyedia->cl_module_.Reset(isolate, obj);
  } else {
    errmsg = OneByteString(isolate, "CJSLoader: Module undefined");
    isolate->ThrowException(Exception::TypeError(errmsg));
    return;
  }

  resolve = Function::New(context, shipped_resolve_filename).ToLocalChecked();
  obj->Set(context, FIXED_ONE_BYTE_STRING(isolate, "_resolveFilename"), resolve).Check();
  inti = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "_cache")).ToLocalChecked();
  if (inti->IsObject()) {
    cache = inti.As<Object>();
  } else {
    errmsg = OneByteString(isolate, "CJSLoader: Module._cache undefined");
    isolate->ThrowException(Exception::TypeError(errmsg));
    return;
  }

  instance = Object::New(isolate);
  key_string = OneByteString(isolate, "__proto__");
  instance->Set(context, key_string, Null(isolate)).Check();
  key_string = OneByteString(isolate, "get");
  instance->Set(context, key_string, Function::New(context, taguja_cache_sedia).ToLocalChecked()).Check();
  Local<Proxy> penghubung = Proxy::New(context, cache, instance).ToLocalChecked();
  obj->Set(context, FIXED_ONE_BYTE_STRING(isolate, "_cache"), penghubung).Check();
}

void taguja_bungkus_esm(const FunctionCallbackInfo<Value>& args) {
  // Isolate* isolate;
  // Local<Context> context;
  // Taguja* penyedia;
  // MaybeLocal<Value> ret;
  // Local<Value> inti;
  // Local<Object> obj;
  // Local<Function> teruskan;
  // Local<Map> larik;
  // Local<String> errmsg;
  // std::vector<Local<Value>> param{};

  // isolate = args.GetIsolate();
  // HandleScope scope(isolate);
  // context = isolate->GetCurrentContext();
  // penyedia = Environment::GetCurrent(isolate)->taguja();
  // // Local<Object> global = context->Global();
  // for (int i = 0; i < args.Length(); i++) {
  //   param.insert(param.begin() + i, args[i]);
  // }
  // // inti = global->Get(context, FIXED_ONE_BYTE_STRING(isolate, "_taguja_teruskan")).ToLocalChecked();
  // teruskan = penyedia->esm_translators_.Get(isolate);//inti.As<Function>();
  // ret = teruskan->Call(context, Undefined(isolate), param.size(), param.data());
  // inti = param[2];
  // if (inti->IsObject()) {
  //   obj = inti.As<Object>();
  // } else {
  //   errmsg = OneByteString(isolate, "ESMLoader: module undefined");
  //   isolate->ThrowException(Exception::TypeError(errmsg));
  //   return;
  // }

  // inti = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "exports")).ToLocalChecked();
  // if (inti->IsObject()) {
  //   obj = inti.As<Object>();
  // } else {
  //   errmsg = OneByteString(isolate, "ESMLoader: module.exports undefined");
  //   isolate->ThrowException(Exception::TypeError(errmsg));
  //   return;
  // }

  // inti = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "translators")).ToLocalChecked();
  // if (inti->IsMap()) {
  //   larik = inti.As<Map>();
  // } else {
  //   errmsg = OneByteString(isolate, "ESMLoader: translators undefined");
  //   isolate->ThrowException(Exception::TypeError(errmsg));
  // }

  // inti = larik->Get(context, FIXED_ONE_BYTE_STRING(isolate, "commonjs")).ToLocalChecked();
  // if (inti->IsFunction()) {
  //   penyedia->ecl_pengubah_cjs_.Reset(isolate, inti.As<Function>());
  // } else {
  //   errmsg = OneByteString(isolate, "ESMLoader: translator for commonjs undefined");
  //   isolate->ThrowException(Exception::TypeError(errmsg));
  // }

  // inti = larik->Get(context, FIXED_ONE_BYTE_STRING(isolate, "builtin")).ToLocalChecked();
  // if (inti->IsFunction()) {
  //   penyedia->ecl_pengubah_lib_.Reset(isolate, inti.As<Function>());
  // } else {
  //   errmsg = OneByteString(isolate, "ESMLoader: translator for builtin undefined");
  //   isolate->ThrowException(Exception::TypeError(errmsg));
  // }
}

void taguja_bungkus_url(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate;
  Local<Context> context;
  Taguja* penyedia;
  MaybeLocal<Value> ret;
  Local<Value> inti;
  Local<Object> obj;
  Local<Function> teruskan, timpa;
  Local<String> errmsg;
  std::vector<Local<Value>> param{};

  isolate = args.GetIsolate();
  HandleScope scope(isolate);
  context = isolate->GetCurrentContext();
  penyedia = Environment::GetCurrent(isolate)->taguja();
  // Local<Object> global = context->Global();
  for (int i = 0; i < args.Length(); i++) {
    param.insert(param.begin() + i, args[i]);
  }
  // inti = global->Get(context, FIXED_ONE_BYTE_STRING(isolate, "_taguja_teruskan")).ToLocalChecked();
  teruskan = penyedia->internal_url_.Get(isolate);//inti.As<Function>();
  ret = teruskan->Call(context, Undefined(isolate), param.size(), param.data());
  inti = param[2];
  if (inti->IsObject()) {
    obj = inti.As<Object>();
  } else {
    errmsg = OneByteString(isolate, "InternalURL: module undefined");
    isolate->ThrowException(Exception::TypeError(errmsg));
    return;
  }

  inti = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "exports")).ToLocalChecked();
  if (inti->IsObject()) {
    obj = inti.As<Object>();
  } else {
    errmsg = OneByteString(isolate, "InternalURL: module.exports undefined");
    isolate->ThrowException(Exception::TypeError(errmsg));
    return;
  }

  inti = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "fileURLToPath")).ToLocalChecked();
  timpa = Function::New(context, taguja_genpath).ToLocalChecked();
  obj->Set(context, FIXED_ONE_BYTE_STRING(isolate, "fileURLToPath"), timpa.As<Value>()).Check();
  obj->Set(context, FIXED_ONE_BYTE_STRING(isolate, "fileURLTo1234"), inti).Check();
}

void entry_exports(Local<Name> property, const PropertyCallbackInfo<Value>& info) {
  Environment* e = Environment::GetCurrent(info);
  Isolate* isolate = e->isolate();
  HandleScope scope(isolate);
  MaybeLocal<Value> maybe;
  Local<Value> err, xpr;
  Local<String> errmsg;

  maybe = e->taguja()->LoadEntry();
  if (maybe.ToLocal(&xpr)) {
    info.GetReturnValue().Set(xpr);
  } else {
    errmsg = OneByteString(isolate, "Unable to load entry");
    err = Exception::Error(errmsg);
    isolate->ThrowException(err);
  }
}

void entry_entry_exports(Local<Name> property, const PropertyCallbackInfo<Value>& info) {
  Environment* e = Environment::GetCurrent(info);
  Isolate* isolate = e->isolate();
  HandleScope scope(isolate);
  MaybeLocal<Value> maybe;
  Local<Value> err, xpr;
  Local<String> errmsg;

  maybe = e->taguja()->LoadEntryFileOrZlib();
  if (maybe.ToLocal(&xpr)) {
    info.GetReturnValue().Set(xpr);
  } else {
    errmsg = OneByteString(isolate, "Unable to load entry/entry");
    err = Exception::Error(errmsg);
    isolate->ThrowException(err);
  }
}

void shipped_require(const FunctionCallbackInfo<Value>& args) {
  Environment *env = Environment::GetCurrent(args);
  // CHECK(args[0]->IsString());
  Isolate* isolate = env->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env->context();
  Taguja* penyedia = env->taguja();
  Local<String> errmsg;
  Local<Function> breq;
  Local<Value> salur[4];
  
  node::Utf8Value id_v(isolate, args[0]);
  const char* id_n = *id_v, *pembatas;
  pembatas = strchr(id_n, 58);
  if (pembatas) id_n = pembatas + 1;
  std::string id(id_n), buntut;
  // std::cout << "Require: " << id << std::endl;

  Local<Value> result;
  MaybeLocal<Value> maybe;
  if (std::find(builtinids.begin(), builtinids.end(), id) != builtinids.end()) { 
    salur[0] = OneByteString(isolate, id.c_str(), id.size());
    breq = env->builtin_module_require();
    maybe = breq->Call(context, Undefined(isolate), 1, salur);
  } else if (id.substr(0, 7) == "taguja/") {
    buntut = id.substr(id.find_first_of('/') + 1);
    if (buntut == "login") {
      breq = penyedia->login_qrcode_.Get(isolate);
    } else if (buntut == "respon") {
      breq = penyedia->respon_pesan_.Get(isolate);
    } else if (buntut == "terlogin") {
      breq = penyedia->login_nusa_.Get(isolate);
    } else if (buntut == "terlogout") {
      breq = penyedia->logout_nusa_.Get(isolate);
    } else if (buntut == "logout") {
      breq = penyedia->logout_.Get(isolate);
    }
    maybe = MaybeLocal<Value>(breq);
  } else {
    maybe = taguja_require(env, id);
  }

  if (maybe.ToLocal(&result)) {
    args.GetReturnValue().Set(result);
  } else {
    std::string msg = std::string("Gagal memuat ") + id;
    errmsg = OneByteString(isolate, msg.c_str(), msg.size());
    isolate->ThrowException(Exception::Error(errmsg));
  }
}

void bungkus_lorong_free(mrb_state* diri, void* ptr) {
  struct bungkus_lorong* p = reinterpret_cast<struct bungkus_lorong*>(ptr);
  // delete p->inti;
  delete p;
}

void bungkus_lorong_esm_free(mrb_state* diri, void* ptr) {
  struct bungkus_lorong_esm* p = reinterpret_cast<struct bungkus_lorong_esm*>(ptr);
  // delete p->inti;
  delete p;
}

void lorong_compiler(compiler_rincian* rinci) {
  mrb_state* diri;
  mrb_value pos;
  struct RData* tangkap;
  struct bungkus_lorong* bungkus;
  Taguja* penyedia;
  const char *id, *filename, *ext;
  int index;

  diri = rinci->diri;
  bungkus = new struct bungkus_lorong();
  tangkap = mrb_data_object_alloc(diri, diri->object_class, bungkus, &lorong_type);
  penyedia = reinterpret_cast<Taguja*>(rinci->kuasa);
  pos = rinci->lokasi; //mrb_funcall(diri, inti, "posisi", 0, NULL);
  id = RSTRING_PTR(pos);
  filename = strrchr(id, 47);
  if (!filename) filename = id;
  ext = strrchr(filename, 46);
  index = filename - id;
  rinci->id = id;

  MaybeLocal<Function> maybe;
  MaybeLocal<Module> result;
  Local<Function> fn;
  Local<Module> res;
  if (ext && strcmp(ext, ".json") == 0) {
    bungkus->id = std::string(id, RSTRING_LEN(pos));
    bungkus->filename = std::string(filename, RSTRING_LEN(pos) - index);
    rinci->hasil = mrb_obj_value(tangkap);
    penyedia->UraiJSON(rinci, bungkus);
    rinci->id = id;
  } else if (mrb_bool(rinci->arah)) {
    maybe = penyedia->Compile(rinci);
    if (maybe.ToLocal(&fn)) {
      bungkus->id = std::string(id, RSTRING_LEN(pos));
      bungkus->filename = std::string(filename, RSTRING_LEN(pos) - index);
      penyedia->PanggilShipped(fn, bungkus);
      rinci->id = id;
      rinci->hasil = mrb_obj_value(tangkap);
    } else {
      rinci->hasil = mrb_nil_value();
    }
  } else {
    result = penyedia->LoadESM(rinci);
    if (result.ToLocal(&res)) {
      bungkus->id = std::string(id, RSTRING_LEN(pos));
      bungkus->filename = std::string(filename, RSTRING_LEN(pos) - index);
      penyedia->shipped()->compiler = (compiler_panggilan)lorong_esm_compiler;
      penyedia->BungkusExports(res, bungkus);
      penyedia->shipped()->compiler = (compiler_panggilan)lorong_compiler;
      rinci->id = id;
      rinci->hasil = mrb_obj_value(tangkap);
    } else {
      rinci->hasil = mrb_nil_value();
    }
  }
}

void lorong_esm_compiler(compiler_rincian* rinci) {
  mrb_state* diri;
  mrb_value pos;
  struct RData* tangkap;
  struct bungkus_lorong_esm* bungkus;
  Taguja* penyedia;
  const char *id, *filename;
  int index;

  if (mrb_bool(rinci->arah)) {
    rinci->hasil = mrb_nil_value();
    return;
  }

  diri = rinci->diri;
  bungkus = new struct bungkus_lorong_esm();
  tangkap = mrb_data_object_alloc(diri, diri->object_class, bungkus, &lorong_esm_type);
  penyedia = reinterpret_cast<Taguja*>(rinci->kuasa);
  pos = rinci->lokasi; //mrb_funcall(diri, inti, "posisi", 0, NULL);
  id = RSTRING_PTR(pos);
  filename = strrchr(id, 47);
  if (!filename) filename = id;
  index = filename - id;

  MaybeLocal<Module> result;
  Local<Module> res;
  result = penyedia->LoadESM(rinci, bungkus);
  if (result.ToLocal(&res)) {
    bungkus->id = std::string(id, RSTRING_LEN(pos));
    bungkus->filename = std::string(filename, RSTRING_LEN(pos) - index);
    // penyedia->TahanESM(res, bungkus);
    rinci->id = id;
    rinci->hasil = mrb_obj_value(tangkap);
  } else {
    rinci->hasil = mrb_nil_value();
  }
}

void taguja_proxy(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Isolate* i = env->isolate();
  Local<Context> c = env->context();
  HandleScope scope(i);
  Taguja* penyedia = env->taguja();
  Local<Value> elm, dapat;
  Local<Object> pegang;
  Local<Object> target = args[0].As<Object>();
  Local<String> prop = args[1].As<String>();
  Utf8Value prop_v(i, prop);
  std::string key(*prop_v, prop_v.length());

  // Local<Object> global = c->Global();
  // Local<String> cns = OneByteString(i, "console");
  // Local<String> lg = OneByteString(i, "log");
  // v8::Local<v8::Value> cVal = global->Get(c, cns).ToLocalChecked();
  
  // if (cVal->IsObject()) {
  //   v8::Local<v8::Object> console = cVal.As<v8::Object>();
  //   v8::Local<v8::Value> log = console->Get(c, lg).ToLocalChecked();
  //   v8::Local<v8::Function> logFunc = log.As<v8::Function>();
  //   Local<Value> args[4];
  //   MaybeLocal<Value> ret;
  //   args[0] = prop;
  //   ret = logFunc->Call(c, console, 1, args);
  // }

  std::vector<std::string> variabel_kunci = {
    "KEY_BUNDLE_TYPE", 
    "NOISE_MODE", 
    "WA_CERT_DETAILS", 
    "MEDIA_HKDF_KEY_MAPPING", 
    "DEFAULT_ORIGIN", 
    "MEDIA_PATH_MAP", 
    "URL_EXCLUDE_REGEX", 
    "URL_REGEX", 
    "MEDIA_KEYS"
  };
  
  if (target->Has(c, prop).FromJust()) {
    elm = target->Get(c, prop).ToLocalChecked();
    args.GetReturnValue().Set(elm);
  } else if (std::find(variabel_kunci.begin(), variabel_kunci.end(), key) != variabel_kunci.end()) {
    elm = taguja_require(env, "baileys/lib/Defaults").ToLocalChecked();
    pegang = elm.As<Object>();
    dapat = pegang->Get(c, prop).ToLocalChecked();
    args.GetReturnValue().Set(dapat);
  } else {
    std::cout << "In Proxy: " << key << std::endl;
  }
}

MaybeLocal<Value> taguja_synthetic_eval(Local<Context> context, Local<Module> mod) {
  Isolate* isolate = context->GetIsolate();
  struct esm_rincian rinci;
  Local<String> export_name, default_string;
  Local<Value> export_value;
  Local<Object> xpr;
  Local<Array> ary;

  auto range = moduleHash.equal_range(mod->GetIdentityHash());
  for (auto it = range.first; it != range.second; ++it) {
    if (it->second.bawa == mod) {
      rinci = it->second;
    }
  }

  xpr = rinci.exports.Get(isolate);
  ary = xpr->GetPropertyNames(context).ToLocalChecked();
  default_string = FIXED_ONE_BYTE_STRING(isolate, "default");
  for (size_t i = 0; i < ary->Length(); i++) {
    export_name = ary->Get(context, i).ToLocalChecked().As<String>();
    if (export_name == default_string) continue;
    Utf8Value xpr_nm(isolate, export_name);
    export_value = xpr->Get(context, export_name).ToLocalChecked();
    mod->SetSyntheticModuleExport(isolate, export_name, export_value).Check();
  }
  mod->SetSyntheticModuleExport(isolate, default_string, xpr).Check();

  Local<Promise::Resolver> resolver;
  if (!Promise::Resolver::New(context).ToLocal(&resolver)) {
    return MaybeLocal<Value>();
  }

  resolver->Resolve(context, Undefined(isolate)).ToChecked();
  return resolver->GetPromise();
}

void taguja_login_qrcode(const FunctionCallbackInfo<Value>& info) {
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Integer> jumlah;
  Local<Array> data, d2;
  Local<Boolean> d3;
  std::vector<std::vector<bool>> d; 
  std::vector<bool> row;
  bool entry;
  size_t j;

  jumlah = info[0].As<Integer>();
  data = info[1].As<Array>();
  j = jumlah->Int32Value(context).ToChecked();
  for (size_t i = 0; i < j; i++) {
    row.clear();
    d2 = data->Get(context, i).ToLocalChecked().As<Array>();
    for (size_t ii = 0; ii < d2->Length(); ii++) {
      d3 = d2->Get(context, ii).ToLocalChecked().As<Boolean>();
      entry = d3->BooleanValue(isolate);
      row.insert(row.begin() + ii, entry);
    }
    d.insert(d.begin() + i, row);
  }
  Environment::GetCurrent(info)->taguja()->penangkap_login_(j, d);
}

#include <simdutf.h>

void taguja_respon_pesan(const FunctionCallbackInfo<Value>& info) {
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  pesan_rincian rinci;
  bungkus_js pegang;
  Local<Object> dari, global;
  Local<Value> prop;
  Local<String> tulisan;

  rinci.kuasa = Environment::GetCurrent(info)->taguja();
  dari = info[0].As<Object>();
  prop = dari->Get(context, FIXED_ONE_BYTE_STRING(isolate, "id")).ToLocalChecked();
  tulisan = prop.As<String>();
  node::Utf8Value strId(isolate, tulisan);
  rinci.id = std::string(*strId, strId.length());

  prop = dari->Get(context, FIXED_ONE_BYTE_STRING(isolate, "chat")).ToLocalChecked();
  tulisan = prop.As<String>();
  node::Utf8Value strChat(isolate, tulisan);
  rinci.chat = std::string(*strChat, strChat.length());

  prop = dari->Get(context, FIXED_ONE_BYTE_STRING(isolate, "subjek")).ToLocalChecked();
  tulisan = prop.As<String>();
  node::Utf8Value strSubjek(isolate, tulisan);
  rinci.subjek = std::string(*strSubjek, strSubjek.length());

  prop = dari->Get(context, FIXED_ONE_BYTE_STRING(isolate, "sender")).ToLocalChecked();
  tulisan = prop.As<String>();
  node::Utf8Value strSender(isolate, tulisan);
  rinci.sender = std::string(*strSender, strSender.length());

  prop = dari->Get(context, FIXED_ONE_BYTE_STRING(isolate, "nama")).ToLocalChecked();
  tulisan = prop.As<String>();
  node::Utf8Value strNama(isolate, tulisan);
  rinci.nama = std::string(*strNama, strNama.length());

  prop = dari->Get(context, FIXED_ONE_BYTE_STRING(isolate, "text")).ToLocalChecked();
  tulisan = prop.As<String>();
  node::Utf8Value strText(isolate, tulisan);
  rinci.text = std::string(*strText, strText.length());

  prop = dari->Get(context, FIXED_ONE_BYTE_STRING(isolate, "fromMe")).ToLocalChecked();
  Local<Boolean> bendera = prop.As<Boolean>();
  rinci.fromMe = bendera->BooleanValue(isolate);

  global = context->Global();
  prop = global->Get(context, FIXED_ONE_BYTE_STRING(isolate, "conn")).ToLocalChecked();
  Local<Object> akar = prop.As<Object>();
  rinci.akar = &pegang;
  pegang.akar.Reset(isolate, akar);

  size_t panjang, banget;
  char16_t* duaByte;
  rinci.respon = std::string("");
  rinci.kuasa->penangkap_pesan_(&rinci);
  if (rinci.respon.length() > 0) {
    dari = Object::New(isolate);
    panjang = rinci.respon.length();
    duaByte = reinterpret_cast<char16_t*>(malloc(sizeof(char16_t) * (panjang + 1)));
    banget = simdutf::convert_utf8_to_utf16(rinci.respon.c_str(), panjang, duaByte);
    duaByte[banget] = 0;
    tulisan = String::NewFromTwoByte(isolate, reinterpret_cast<uint16_t*>(duaByte)).ToLocalChecked();
    dari->Set(context, FIXED_ONE_BYTE_STRING(isolate, "pesan"), tulisan).Check();
    info.GetReturnValue().Set(dari);
    free(duaByte);
  } else {
    info.GetReturnValue().Set(Undefined(isolate));
  }
}

void taguja_nusa_login(const FunctionCallbackInfo<Value>& info) {
  Environment* env = Environment::GetCurrent(info);
  Taguja* penyedia = env->taguja();

  Local<String> local_nama, local_id;
  local_nama = info[0].As<String>();
  local_id = info[1].As<String>();
  Utf8Value str_nama(info.GetIsolate(), local_nama);
  Utf8Value str_id(info.GetIsolate(), local_id);

  penyedia->nusa_login_(*str_nama, *str_id);
  penyedia->logined = true;
  info.GetReturnValue().Set(Undefined(info.GetIsolate()));
}

void taguja_nusa_logout(const FunctionCallbackInfo<Value>& info) {
  Environment* env = Environment::GetCurrent(info);
  Taguja* penyedia = env->taguja();
  if (penyedia->logined) penyedia->nusa_logout_();
  info.GetReturnValue().Set(Undefined(info.GetIsolate()));
}

void taguja_logout(const FunctionCallbackInfo<Value>& info) {
  Isolate* i;
  Local<Context> c;
  Environment* env = Environment::GetCurrent(info);
  Local<Value> val;
  Local<String> errmsg;
  Local<Function> fn;
  i = env->isolate();
  c = env->context();
  val = info[0];
  if (val->IsFunction()) {
    fn = val.As<Function>();
    env->taguja()->entry_logout_.Reset(i, fn);
  } else {
    std::string msg = std::string("Argumen pertama harus fungsi");
    errmsg = OneByteString(i, msg.c_str(), msg.size());
    i->ThrowException(Exception::Error(errmsg));
  }
  info.GetReturnValue().Set(Undefined(i));
}

Taguja::Taguja(Environment* e): env_(e), logined(false) {
  // std::cout << "Taguja Buat!" << std::endl;
  diri_ = mrb_open();
  shipped_.diri = diri_;
  crin_.diri = diri_;
  crin_.kuasa = this;
  
  char jalur[PATH_MAX];
  size_t panjang;
  getcwd(jalur, PATH_MAX);
  panjang = strlen(jalur);
  for (size_t i = 0; i < panjang; i++) {
    if (jalur[i] == '\\') jalur[i] = '/';
  }
  lokasi_entry_ = std::string(jalur) + std::string("/entry.js");

  mrb_value *newstack;
  off_t oldsize, off;
  size_t size;

  oldsize = diri_->c->stend - diri_->c->stbase;
  off = diri_->c->stack - diri_->c->stbase;
  size = 0x40000;
  newstack = reinterpret_cast<mrb_value*>(mrb_realloc(diri_, diri_->c->stbase, sizeof(mrb_value) * size));
  diri_->c->stbase = newstack;
  diri_->c->stack = diri_->c->stbase + off;
  diri_->c->stend = diri_->c->stbase + size;
}

shipped_rincian* Taguja::shipped() {
  return &shipped_;
}

compiler_rincian* Taguja::shipped_compiler_rincian() {
  return &crin_;
}

v8::Local<v8::Primitive> Taguja::UndefinedA() {
  return Undefined(env_->isolate());
}

void Taguja::Masuk() {
  // std::cout << "Taguja Masuk!" << std::endl;
  Environment* e = env_;
  HandleScope scope(e->isolate());
  shipped_masuk(shipped());
  shipped_.compiler = (compiler_panggilan)lorong_compiler;
  shipped_.compilerrinc = shipped_compiler_rincian();

  // Local<Object> global, baileys, entry;
  Local<Function> makeReq;
  Local<Object> entry;
  makeReq = Function::New(e->context(), shipped_require).ToLocalChecked();
  require_.Reset(e->isolate(), makeReq);
  // global = e->context()->Global();
  // baileys = Object::New(e->isolate());
  // baileys->Set(e->context(), FIXED_ONE_BYTE_STRING(e->isolate(), "loaded"), True(e->isolate())).Check();
  // baileys
  //     ->SetAccessor(e->context(),
  //                   FIXED_ONE_BYTE_STRING(e->isolate(), "exports"),
  //                   baileys_exports,
  //                   nullptr,
  //                   MaybeLocal<Value>(),
  //                   DEFAULT,
  //                   None,
  //                   SideEffectType::kHasNoSideEffect)
  //     .Check();
  // global->Set(e->context(), FIXED_ONE_BYTE_STRING(e->isolate(), "_taguja_baileys"), baileys).Check();
  // baileys->Set(e->context(), FIXED_ONE_BYTE_STRING(e->isolate(), "panjant"), Undefined(e->isolate())).Check();
  entry = Object::New(e->isolate());
  entry->Set(e->context(), FIXED_ONE_BYTE_STRING(e->isolate(), "loaded"), True(e->isolate())).Check();
  entry
      ->SetAccessor(e->context(),
                    FIXED_ONE_BYTE_STRING(e->isolate(), "exports"),
                    entry_exports,
                    nullptr,
                    MaybeLocal<Value>(),
                    DEFAULT,
                    None,
                    SideEffectType::kHasNoSideEffect)
      .Check();
  // global->Set(e->context(), FIXED_ONE_BYTE_STRING(e->isolate(), "_taguja_entry"), entry).Check();
  entry_.Reset(e->isolate(), entry);
  entry = Object::New(e->isolate());
  entry->Set(e->context(), FIXED_ONE_BYTE_STRING(e->isolate(), "loaded"), True(e->isolate())).Check();
  entry
      ->SetAccessor(e->context(),
                    FIXED_ONE_BYTE_STRING(e->isolate(), "exports"),
                    entry_entry_exports,
                    nullptr,
                    MaybeLocal<Value>(),
                    DEFAULT,
                    None,
                    SideEffectType::kHasNoSideEffect)
      .Check();
  // global->Set(e->context(), FIXED_ONE_BYTE_STRING(e->isolate(), "_taguja_entry_entry"), entry).Check();
  entry_entry_.Reset(e->isolate(), entry);

  login_qrcode_.Reset(e->isolate(), Function::New(e->context(), taguja_login_qrcode).ToLocalChecked());
  respon_pesan_.Reset(e->isolate(), Function::New(e->context(), taguja_respon_pesan).ToLocalChecked());
  login_nusa_.Reset(e->isolate(), Function::New(e->context(), taguja_nusa_login).ToLocalChecked());
  logout_nusa_.Reset(e->isolate(), Function::New(e->context(), taguja_nusa_logout).ToLocalChecked());
  logout_.Reset(e->isolate(), Function::New(e->context(), taguja_logout).ToLocalChecked());

  Local<Object> kendali;
  Local<String> key_string;
  kendali = Object::New(e->isolate());
  key_string = OneByteString(e->isolate(), "__proto__");
  kendali->Set(e->context(), key_string, Null(e->isolate())).Check();
  key_string = OneByteString(e->isolate(), "get");
  kendali->Set(e->context(), key_string, Function::New(e->context(), taguja_proxy).ToLocalChecked()).Check();
  kendali_circular_.Reset(e->isolate(), kendali);
}

int Taguja::ReadEntry(char* dest, size_t* length) {
  size_t result;
  FILE* fp = fopen(lokasi_entry_.c_str(), "r");
  if (!fp) return 0;
  result = fread(reinterpret_cast<void*>(dest), 1, sizeof(char) * UKURAN_SCRIPT, fp);
  fclose(fp);
  dest[result] = 0;
  *length = result;
  return 1;
}

int Taguja::DecodeEntry(char* dest, size_t* length) {
  z_stream s;
  size_t ukuran;
  int res;

  memset(&s, 0, sizeof(z_stream));
  ukuran = _binary_taguja_z_end - _binary_taguja_z_start;
  s.next_in = reinterpret_cast<Bytef*>(_binary_taguja_z_start);
  s.avail_in = ukuran;
  s.next_out = reinterpret_cast<Bytef*>(dest);
  s.avail_out = UKURAN_SCRIPT;
  inflateInit(&s);
  res = inflate(&s, Z_FINISH);
  inflateEnd(&s);

  if (res != Z_OK && res != Z_STREAM_END && res != Z_BUF_ERROR) {
    return 0;
  } else {
    ukuran = UKURAN_SCRIPT - s.avail_out;
    dest[ukuran] = 0;
    *length = ukuran;
    return 1;
  }
}

MaybeLocal<Value> Taguja::LoadShipped(const char* id) {
  EscapableHandleScope scope(env_->isolate());
  CHECK_NOT_NULL(id);
  Isolate* iso;
  Local<Context> ctx;
  Local<Proxy> melayang;
  Local<Object> kendali;
  Local<String> key_string;

  iso = env_->isolate();
  ctx = env_->context();
  mrb_value modr = shipped_muat(shipped(), id);
  // std::cout << "module ruby " << id << " " << mrb_nil_p(modr) << mrb_symbol_p(modr) << std::endl;
  if (diri_->exc) return {};
  if (mrb_nil_p(modr)) return {};
  if (mrb_symbol_p(modr)) {
    if (mrb_symbol(modr) == mrb_intern_lit(diri_, "loading")) {
      kendali = kendali_circular_.Get(iso);
      melayang = Proxy::New(ctx, Object::New(iso), kendali).ToLocalChecked();
      return scope.Escape(melayang);
    }
  }

  struct bungkus_lorong* bungkus;
  Local<Object> mod;
  bungkus = reinterpret_cast<struct bungkus_lorong*>(DATA_PTR(modr));
  mod = bungkus->inti.Get(iso).As<Object>();
  return scope.Escape(mod);

  // Local<Boolean> loading;
  // loading = mod->Get(ctx, FIXED_ONE_BYTE_STRING(iso, "loading")).ToLocalChecked().As<Boolean>();
  // if (loading->BooleanValue(iso) && !bungkus->penjaga->dalam_proses) {
  //   WaitForSingleObject(bungkus->tanda, INFINITE);
  //   ProsesPutaranInternal(bungkus->penjaga);
  // }
  
  // xpr = mod->Get(ctx, FIXED_ONE_BYTE_STRING(iso, "exports")).ToLocalChecked();
  // if (maybe.ToLocal(&xpr)) {
  //   if (bungkus->id == "protobufjs/src/util/minimal.js") {
  //     Local<String> modJson = JSON::Stringify(ctx, xpr).ToLocalChecked();
  //     node::Utf8Value json(iso, modJson);
  //     std::string str(*json);
  //     std::cout << str << std::endl; 
  //   }
  //   return scope.Escape(xpr);
  // }
  // return {};
}

MaybeLocal<Value> Taguja::LoadEntry() {
  EscapableHandleScope scope(env_->isolate());
  Isolate* isolate = env_->isolate();
  Local<Context> context = env_->context();
  Local<Value> args[2], target;
  Local<Object> mod, sewenang;
  Local<Function> compile;
  Local<String> key_string;
  MaybeLocal<Value> maybe, result, hasil;
  std::string script = "const entry = require('taguja:entry/entry.js')";

  std::string filename = std::string("taguja:") + LETAK_ENTRY;
  maybe = BuatPenahan(filename);
  if (maybe.ToLocal(&target)) {
    sewenang = target.As<Object>();
    key_string = FIXED_ONE_BYTE_STRING(isolate, "_compile");
    compile = sewenang->Get(context, key_string).ToLocalChecked().As<Function>();
  } else {
    return {};
  }

  args[0] = String::NewFromUtf8(isolate, script.c_str(), NewStringType::kNormal, script.size()).ToLocalChecked();
  args[1] = OneByteString(isolate, filename.c_str(), filename.size());
  result = compile->Call(context, target, 2, args);
  key_string = FIXED_ONE_BYTE_STRING(isolate, "exports");
  hasil = sewenang->Get(context, key_string);
  return scope.EscapeMaybe(hasil);
}

v8::MaybeLocal<v8::Value> Taguja::LoadEntryFileOrZlib() {
  EscapableHandleScope scope(env_->isolate());
  Isolate* isolate = env_->isolate();
  char* script;
  size_t ukuran;

  script = reinterpret_cast<char*>(malloc(sizeof(char) * UKURAN_SCRIPT));
  if (access(lokasi_entry_.c_str(), F_OK) != -1) {
    if (!ReadEntry(script, &ukuran)) {
      free(script);
      return {};
    }
  } else {
    if (!DecodeEntry(script, &ukuran)) {
      free(script);
      return {};
    }
  }

  if (!shipped_lengkap_p(shipped())) {
    shipped_lengkapi(shipped());
  }
  crin_.sumber = script;
  crin_.length = ukuran;
  crin_.id = "taguja:entry/entry.js";

  Loader* loader = Loader::GetInstance();
  builtinids = loader->GetBuiltinIds();

  MaybeLocal<Function> maybe;
  Local<Function> portal;
  struct bungkus_lorong bungkus;
  maybe = Compile(shipped_compiler_rincian());
  free(script);
  if (maybe.ToLocal(&portal)) {
    MulaiESM();
    bungkus.id = std::string("taguja:entry/entry.js");
    PanggilShipped(portal, &bungkus);
    return scope.Escape(bungkus.inti.Get(isolate));
  } else {
    return {};
  }
}

void Taguja::MulaiESM() {
  Isolate* i = env_->isolate();
  Local<Context> c = env_->context();
  HandleScope scope(i);
  // Local<Object> tagujaJS;
  // Local<Array> esm_load;
  // Local<Function> cjs = ecl_pengubah_cjs_.Get(i);
  // Local<Function> lib = ecl_pengubah_lib_.Get(i);
  // Local<Function> resolv_handler, reject_handler;
  Local<Function> builtin;
  MaybeLocal<Value> ret;
  Local<Value> res, args[3];
  Local<Object> obj;
  Local<Module> mod;
  Local<Promise> result;//, lain;
  // Persistent<Object> tahan;
  PersistentModule bawa;
  // loader::ModuleWrap *pegang;
  // std::shared_ptr<MicrotaskQueue> mq;
  // size_t urutan;
  std::string nama;
  struct esm_rincian rincian;
  std::vector<std::string> kebutuhan_lib = {
    "url", 
    "http", 
    "https", 
    "util", 
    "zlib", 
    "stream", 
    "events"
  };
  std::vector<std::string> kebutuhan_shipped = {
    "form-data", 
    "proxy-from-env", 
    "follow-redirects"
  };

  // urutan = 0;
  // tagujaJS = Object::New(i);
  // esm_load = Array::New(i);
  args[1] = Null(i);
  args[2] = False(i);
  builtin = env_->builtin_module_require();
  // resolv_handler = Function::New(c, taguja_resolve_handler).ToLocalChecked();
  // reject_handler = Function::New(c, taguja_reject_handler).ToLocalChecked();

  for (size_t j = 0; j < kebutuhan_lib.size(); j++) {
    nama = kebutuhan_lib[j];
    args[0] = OneByteString(i, nama.c_str(), nama.size());
    ret = builtin->Call(c, Undefined(i), 1, args);
    res = ret.ToLocalChecked();
    if (res->IsObject()) {
      obj = res.As<Object>();
      rincian.id = nama;
      mod = BuatSyntheticModule(&rincian, obj);
      bawa.Reset(i, mod);
      moduleHash.emplace(mod->GetIdentityHash(), rincian);
      cache_esm_.emplace(kebutuhan_lib[j], bawa);
    }
    // if (res->IsPromise()) {
    //   result = res.As<Promise>();
    //   bawa.Reset(i, result);
    //   cache_esm_.emplace(kebutuhan_lib[j], bawa);
    //   esm_load->Set(c, urutan, result).Check();
    //   urutan++;
      // lain = result->Then(c, resolv_handler, reject_handler).ToLocalChecked();
      // lain = lain->Catch(c, reject_handler).ToLocalChecked();
      // if (result->State() == Promise::kRejected) {
      //   std::cout << "1MulaiESM[lib]: " << nama << " tidak bisa dimuat" << std::endl;
      //   continue;
      // } else {
      //   res = result->Result();
      //   if (!res->IsObject()) continue;
      //   obj = res.As<Object>();
      //   tahan.Reset(i, obj);
      //   pegang = static_cast<loader::ModuleWrap*>(BaseObject::FromJSObject(obj));
      //   mod = pegang->module_.Get(i);
      //   bawa.Reset(i, mod);
      //   mod->InstantiateModule(c, taguja_resolve_module_callback).Check();
      //   // mq = pegang->contextify_context_->microtask_queue();
      //   if (mod->Evaluate(pegang->context()).IsEmpty()) {
      //     std::cout << "MulaiESM: Evaluate Gagal" << std::endl;
      //     bawa.Reset();
      //   } else {
      //     std::cout << "MulaiESM: Evaluate Sukses" << std::endl; // 0x70bdc1
      //     cache_esm_.emplace(nama, bawa);
      //     // mq->PerformCheckpoint(i);
      //   }
      // }
    // }
  }

  for (size_t j = 0; j < kebutuhan_shipped.size(); j++) {
    rincian.id = kebutuhan_shipped[j];
    ret = taguja_require(env_, rincian.id);
    res = ret.ToLocalChecked();
    if (res->IsObject()) {
      obj = res.As<Object>();
      mod = BuatSyntheticModule(&rincian, obj);
      bawa.Reset(i, mod);
      moduleHash.emplace(mod->GetIdentityHash(), rincian);
      cache_esm_.emplace(kebutuhan_shipped[j], bawa);
    }
    // if (res->IsPromise()) {
    //   result = res.As<Promise>();
    //   bawa.Reset(i, result);
    //   cache_esm_.emplace(kebutuhan_shipped[j], bawa);
    //   esm_load->Set(c, urutan, result).Check();
    //   urutan++;
      // lain = result->Then(c, resolv_handler, reject_handler).ToLocalChecked();
      // lain = lain->Catch(c, reject_handler).ToLocalChecked();
      // while(result->State() == Promise::kPending) {
      //   Sleep(200);
      // }
      // if (result->State() == Promise::kRejected) {
      //   std::cout << "2MulaiESM[shipped]: " << nama << " tidak bisa dimuat" << std::endl;
      //   continue;
      // } else {
      //   res = result->Result();
      //   if (!res->IsObject()) continue;
      //   obj = res.As<Object>();
      //   tahan.Reset(i, obj);
      //   pegang = static_cast<loader::ModuleWrap*>(BaseObject::FromJSObject(obj));
      //   mod = pegang->module_.Get(i);
      //   bawa.Reset(i, mod);
      //   mod->InstantiateModule(c, taguja_resolve_module_callback).Check();
      //   // mq = pegang->contextify_context_->microtask_queue();
      //   if (mod->Evaluate(pegang->context()).IsEmpty()) {
      //     std::cout << "MulaiESM: Evaluate Gagal" << std::endl;
      //     bawa.Reset();
      //   } else {
      //     std::cout << "MulaiESM: Evaluate Sukses" << std::endl;
      //     cache_esm_.emplace(nama, bawa);
      //     // mq->PerformCheckpoint(i);
      //   }
      // }
    // }
  }

  // Local<Object> g = c->Global();
  // tagujaJS->Set(c, FIXED_ONE_BYTE_STRING(i, "esm_promise"), esm_load).Check();
  // g->Set(c, FIXED_ONE_BYTE_STRING(i, "taguja"), tagujaJS).Check();
}

Local<Module> Taguja::BuatSyntheticModule(struct esm_rincian* rincian, v8::Local<v8::Object> obj) {
  Isolate *i = env_->isolate();
  Local<Context> c = env_->context();
  Local<Array> ary;
  Local<Module> mod;
  Local<Value> entry;
  Local<String> biasa, prop, nama;
  std::vector<Local<String>> deret_export{};
  
  biasa = FIXED_ONE_BYTE_STRING(i, "default");
  nama = OneByteString(i, rincian->id.c_str(), rincian->id.size());
  ary = obj->GetPropertyNames(c).ToLocalChecked();
  rincian->exports.Reset(i, obj);
  for (size_t j = 0; j < ary->Length(); j++) {
    entry = ary->Get(c, j).ToLocalChecked();
    prop = entry.As<String>();
    if (prop == biasa) biasa.Clear();
    deret_export.insert(deret_export.begin() + j, prop);
  }
  if (!biasa.IsEmpty()) {
    deret_export.insert(deret_export.end(), biasa);
  }
  mod = Module::CreateSyntheticModule(i, nama, deret_export, taguja_synthetic_eval);
  rincian->bawa.Reset(i, mod);
  return mod;
}


bool Taguja::CacheAvailableESM(const char* id) {
  const auto cache_it = cache_esm_.find(id);
  if (cache_it == cache_esm_.end()) {
    return false;
  }
  if (cache_it->second.IsEmpty()) return false;
  return true;
}

MaybeLocal<Module> Taguja::ResolveESM(Isolate* i, const char* id) {
  Local<Context> c = i->GetCurrentContext();
  Local<Module> barang;
  // Promise::PromiseState tanda;
  // Persistent<Object> tahan;
  // Persistent<Module> bawa;
  // loader::ModuleWrap* pegang;
  // Local<Value> res;
  // Local<Object> obj;
  // Local<Module> mod;

  const auto promise_it = cache_esm_.find(id);
  if (promise_it == cache_esm_.end()) {
    return {};
  }
  barang = promise_it->second.Get(i);
  // tanda = barang->State();
  // if (tanda != Promise::kFulfilled) {
  //   if (tanda == Promise::kRejected) {
  //     std::cout << "ResolveESM: " << id << " terjadi kesalahan" << std::endl;
  //   }
  //   return {};
  // }

  // res = barang->Result();
  // if (!res->IsObject()) return {};
  // obj = res.As<Object>();

  // tahan.Reset(i, obj);
  // pegang = static_cast<loader::ModuleWrap*>(BaseObject::FromJSObject(obj));
  // mod = pegang->module_.Get(i);
  if (barang->GetStatus() == Module::Status::kEvaluated) return barang;

  // bawa.Reset(i, mod);
  barang->InstantiateModule(c, taguja_resolve_module_callback).Check();
  // mq = pegang->contextify_context_->microtask_queue();
  if (barang->Evaluate(c).IsEmpty()) {
    // std::cout << "ResolveESM: Evaluate Gagal" << std::endl;
    // bawa.Reset();
    return {};
  }// else {
    // std::cout << "ResolveESM: Evaluate Sukses" << std::endl;
    // mq->PerformCheckpoint(i);
  // }
  return barang;
}

void Taguja::PanggilShipped(Local<Function> portal, struct bungkus_lorong* bungkus) {
  EscapableHandleScope scope(env_->isolate());
  Isolate* isolate = env_->isolate();
  Local<Context> context = env_->context();
  Local<Value> mod = BuatPenahan(bungkus->id).ToLocalChecked();
  Local<Object> obj = mod.As<Object>();
  bungkus->inti.Reset(isolate, mod);
  bungkus->tanda = CreateEvent(NULL, TRUE, TRUE, NULL);

  struct putaran* penjaga = new struct putaran();
  penjaga->taguja = this;
  penjaga->pendirian.Reset(isolate, obj);
  penjaga->portal.Reset(isolate, portal);
  // penjaga->kendali = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Taguja::ProsesPutaran, penjaga, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION, &penjaga->id);
  penjaga->tanda = bungkus->tanda;
  penjaga->dalam_proses = FALSE;
  // ResumeThread(penjaga->kendali);
  bungkus->penjaga = penjaga;
  deret_putaran_.push_back(penjaga);
  ProsesPutaranInternal(penjaga);
}

MaybeLocal<Function> Taguja::Compile(compiler_rincian* rinci) {
  EscapableHandleScope scope(env_->isolate());
  CHECK_NOT_NULL(rinci);

  Isolate* isolate = env_->isolate();
  Local<Context> context = env_->context();
  std::vector<Local<String>> parameters = {
        FIXED_ONE_BYTE_STRING(env_->isolate(), "exports"),
        FIXED_ONE_BYTE_STRING(env_->isolate(), "require"),
        FIXED_ONE_BYTE_STRING(env_->isolate(), "module")
    };
  
  Local<String> source = String::NewFromUtf8(isolate, rinci->sumber, 
    NewStringType::kNormal, rinci->length).ToLocalChecked();
  std::string filename_s = std::string("shipped:") + rinci->id;
  Local<String> filename =
      OneByteString(isolate, filename_s.c_str(), filename_s.size());
  ScriptOrigin origin(isolate, filename, 0, 0, true);

  ScriptCompiler::CachedData* cached_data = nullptr;
  {
    // Note: The lock here should not extend into the
    // `CompileFunction()` call below, because this function may recurse if
    // there is a syntax error during bootstrap (because the fatal exception
    // handler is invoked, which may load built-in modules).
    Mutex::ScopedLock lock(code_cache_mutex_);
    auto cache_it = code_cache_.find(rinci->id);
    if (cache_it != code_cache_.end()) {
      // Transfer ownership to ScriptCompiler::Source later.
      cached_data = cache_it->second.release();
      code_cache_.erase(cache_it);
    }
  }

  const bool has_cache = cached_data != nullptr;
  ScriptCompiler::CompileOptions options =
      has_cache ? ScriptCompiler::kConsumeCodeCache
                : ScriptCompiler::kEagerCompile;
  ScriptCompiler::Source script_source(source, origin, cached_data);

  MaybeLocal<Function> maybe_fun =
      ScriptCompiler::CompileFunction(context,
                                      &script_source,
                                      parameters.size(),
                                      parameters.data(),
                                      0,
                                      nullptr,
                                      options);

  // This could fail when there are early errors in the built-in modules,
  // e.g. the syntax errors
  Local<Function> fun;
  if (!maybe_fun.ToLocal(&fun)) {
    // In the case of early errors, v8 is already capable of
    // decorating the stack for us - note that we use CompileFunction
    // so there is no need to worry about wrappers.
    return MaybeLocal<Function>();
  }

  // Generate new cache for next compilation
  std::unique_ptr<ScriptCompiler::CachedData> new_cached_data(
      ScriptCompiler::CreateCodeCacheForFunction(fun));
  CHECK_NOT_NULL(new_cached_data);

  {
    Mutex::ScopedLock lock(code_cache_mutex_);
    const auto it = code_cache_.find(rinci->id);
    // TODO(joyeecheung): it's safer for each thread to have its own
    // copy of the code cache map.
    if (it == code_cache_.end()) {
      code_cache_.emplace(rinci->id, std::move(new_cached_data));
    } else {
      it->second.reset(new_cached_data.release());
    }
  }

  return scope.Escape(fun);
}

void Taguja::BungkusExports(Local<Module> mod, struct bungkus_lorong* bungkus) {
  Isolate* isolate = env_->isolate();
  Local<Context> context = env_->context();
  Persistent<Module> pegang;
  Local<Value> result;

  pegang.Reset(isolate, mod);
  mod->InstantiateModule(context, taguja_resolve_module_callback).Check();
  result = mod->Evaluate(context).ToLocalChecked();

  Local<Value> baja = BuatPenahan(bungkus->id).ToLocalChecked();
  Local<Object> obj = baja.As<Object>();
  obj->Set(context, FIXED_ONE_BYTE_STRING(isolate, "loaded"), True(isolate)).Check();
  obj->Set(context, FIXED_ONE_BYTE_STRING(isolate, "exports"), result).Check();
  bungkus->inti.Reset(isolate, baja);
}

// void Taguja::TahanESM(struct bungkus_lorong_esm* bungkus) {
//   Isolate* isolate = env_->isolate();
//   Local<Context> context = env_->context();
//   Platform* pf;
//   Local<Function> pengubah = ecl_pengubah_.Get(isolate);
//   MaybeLocal<Value> ret;
//   Local<Value> res;
//   Local<String> errmsg;
//   Local<Promise> result, lain;
//   Local<Object> obj;
//   Persistent<Object> tahan;
//   Local<Module> mod;
//   loader::ModuleWrap *pegang;
//   std::shared_ptr<MicrotaskQueue> mq;
//   std::vector<Local<Value>> param = {
//     OneByteString(isolate, bungkus->id.c_str(), bungkus->id.size()), 
//     Null(isolate), 
//     False(isolate)
//   };
// }

MaybeLocal<Module> Taguja::LoadESM(compiler_rincian* rinci, struct bungkus_lorong_esm* bungkus) {
  EscapableHandleScope scope(env_->isolate());
  CHECK_NOT_NULL(rinci);

  Isolate* isolate = env_->isolate();
  Local<Context> context = env_->context();
  MaybeLocal<Module> maybe;

  Local<PrimitiveArray> host_defined_options = PrimitiveArray::New(isolate, HostDefinedOptions::kLength);
  host_defined_options->Set(isolate, HostDefinedOptions::kType, Number::New(isolate, TagujaScriptType::kModule));
  Local<String> source = String::NewFromUtf8(isolate, rinci->sumber, NewStringType::kNormal, rinci->length).ToLocalChecked();
  std::string filename_s = std::string("shipped_esm:") + rinci->id;
  Local<String> filename = OneByteString(isolate, filename_s.c_str(), filename_s.size());
  ScriptOrigin origin(isolate, filename, 0, 0, true, -1, Local<Value>(), false, false, true, host_defined_options);
  ScriptCompiler::CachedData* cached_data = nullptr;
  ScriptCompiler::Source script_source(source, origin, cached_data);
  maybe = ScriptCompiler::CompileModule(isolate, &script_source);
  if (bungkus) {
    Local<Module> mod;
    // bungkus->inti = new Persistent<Module>();
    if (maybe.ToLocal(&mod)) {
      bungkus->inti.Reset(isolate, mod);
      mod->InstantiateModule(context, taguja_resolve_module_callback).Check();
      if (mod->Evaluate(context).IsEmpty()) {
        bungkus->inti.Reset();
      }
    }
  }
  return maybe;
}

MaybeLocal<Value> Taguja::BuatPenahan(std::string id) {
  Isolate* isolate = env_->isolate();
  Local<Context> context = env_->context();
  EscapableHandleScope scope(isolate);
  Local<Value> args[4];
  MaybeLocal<Value> res;

  args[0] = OneByteString(isolate, id.c_str(), id.size());
  args[1] = Null(isolate);
  res = cl_module_.Get(isolate)->CallAsConstructor(context, 1, args);
  return scope.EscapeMaybe(res);
}

Local<Function> Taguja::BungkusRequire(const char *id, Local<Function> asal) {
  Isolate* isolate = env_->isolate();
  Local<Context> context = env_->context();
  // Local<Object> global = context->Global();

  if (strcmp(id, "internal/modules/cjs/loader") == 0) {
    cjs_loader_.Reset(isolate, asal);
    // global->Set(context, FIXED_ONE_BYTE_STRING(isolate, "_taguja_teruskan"), asal).Check();
    return Function::New(context, taguja_bungkus_cjs).ToLocalChecked();
  } else if (strcmp(id, "internal/modules/esm/translators") == 0) {
    esm_translators_.Reset(isolate, asal);
    return Function::New(context, taguja_bungkus_esm).ToLocalChecked();
  } else if (strcmp(id, "internal/url") == 0) {
    internal_url_.Reset(isolate, asal);
    return Function::New(context, taguja_bungkus_url).ToLocalChecked();
  }

  return asal;
}

void Taguja::ProsesPutaranInternal(struct putaran *penjaga) {
  Isolate* isolate = env_->isolate();
  Local<Context> context = env_->context();
  HandleScope scope(isolate);
  Local<Object> obj;
  Local<Value> xpr;
  Local<Function> require, portal;

  // fn(this.exports, requireFn, this, process, internalBinding, primordials);
  // TryCatch aman(isolate);
  penjaga->dalam_proses = TRUE;
  obj = penjaga->pendirian.Get(isolate);
  portal = penjaga->portal.Get(isolate);
  xpr = obj->Get(context, FIXED_ONE_BYTE_STRING(isolate, "exports")).ToLocalChecked();
  require = require_.Get(isolate);
  std::vector<Local<Value>> arguments = { xpr, require, obj };
  penjaga->result = portal->Call(context, Undefined(isolate), arguments.size(), arguments.data());
  // if (aman.HasCaught()) {
  //   node::Utf8Value msg_v(isolate, aman.Message()->Get());
  //   const char* msg = *msg_v;
  //   mrb_raise(diri_, diri_->eStandardError_class, msg);
  //   return;
  // }
  obj->Set(context, FIXED_ONE_BYTE_STRING(isolate, "loaded"), True(isolate)).Check();
  SetEvent(penjaga->tanda);
}

DWORD Taguja::ProsesPutaran(void* ptr) {
  struct putaran *penjaga = reinterpret_cast<struct putaran*>(ptr);
  penjaga->taguja->ProsesPutaranInternal(penjaga);
  return 0;
}

Taguja::~Taguja() {
  struct putaran* putar;
  for (size_t i = 0; i < deret_putaran_.size(); i++)
  {
    putar = deret_putaran_[i];
    // TerminateThread(putar->kendali, 0);
    // CloseHandle(putar->kendali);
    // delete putar->pendirian;
    // delete putar->portal;
    delete putar;
  }
  mrb_close(diri_);
}

void Taguja::UraiJSON(compiler_rincian* rinci, struct bungkus_lorong* bungkus) {
  Isolate *i = env_->isolate();
  Local<Context> c = env_->context();
  MaybeLocal<Value> maybe;
  Local<Value> hasil;
  Local<Object> obj;
  Local<String> id;
  maybe = JSON::Parse(c, OneByteString(i, rinci->sumber, rinci->length));
  if (maybe.ToLocal(&hasil)) {
    obj = BuatPenahan(bungkus->id).ToLocalChecked().As<Object>();
    obj->Set(c, FIXED_ONE_BYTE_STRING(i, "loaded"), True(i)).Check();
    obj->Set(c, FIXED_ONE_BYTE_STRING(i, "exports"), hasil).Check();
    bungkus->inti.Reset(i, obj);
  } else {
    rinci->hasil = mrb_nil_value();
  }
}

MaybeLocal<Value> Taguja::ObjJSFromRuby(Isolate* iso, mrb_value asal) {
  EscapableHandleScope scope(iso);
  Local<Array> deret;
  Local<Object> duet;
  Local<String> json;
  Local<Value> tempel, hasil;
  mrb_value serial;
  char16_t *wideString;
  size_t banget;

  if (mrb_string_p(asal)) {
    hasil = OneByteString(iso, RSTRING_PTR(asal), RSTRING_LEN(asal));
  } else if (mrb_symbol_p(asal)) {
    hasil = OneByteString(iso, mrb_sym2name(diri_, mrb_symbol(asal)));
  } else if (mrb_array_p(asal)) {
    deret = Array::New(iso, RARRAY_LEN(asal));
    for (size_t i = 0; i < RARRAY_LEN(asal); i++) {
      if (ObjJSFromRuby(iso, RARRAY_PTR(asal)[i]).ToLocal(&tempel)) {
        deret->Set(env_->context(), i, tempel).Check();
      } else {
        return {};
      }
    }
    hasil = deret;
  } else if (mrb_hash_p(asal)) {
    serial = mrb_funcall(diri_, asal, "to_json", 0);
    wideString = reinterpret_cast<char16_t*>(malloc(sizeof(char16_t) * (RSTRING_LEN(serial) + 1)));
    banget = simdutf::convert_utf8_to_utf16(RSTRING_PTR(serial), RSTRING_LEN(serial), wideString);
    wideString[banget] = 0;
    json = String::NewFromTwoByte(iso, reinterpret_cast<uint16_t*>(wideString)).ToLocalChecked();
    free(wideString);
    hasil = JSON::Parse(env_->context(), json).ToLocalChecked();
  } else if (mrb_true_p(asal)) {
    hasil = True(iso);
  } else if (mrb_false_p(asal)) {
    hasil = False(iso);
  } else if (mrb_nil_p(asal)) {
    hasil = Null(iso);
  } else {
    mrb_raise(diri_, mrb_exc_get(diri_, "ArgumentError"), "Argumen tidak bisa diconvert");
    return {};
  }
  return scope.Escape(hasil);
}

mrb_value callback_js_func(mrb_state* diri, mrb_value self) {
  mrb_raise(diri, mrb_exc_get(diri, "RuntimeError"), "Belum sempurna, masih perbaikan");
  return mrb_nil_value();
}

mrb_value Taguja::ObjRubyFromJS(Isolate* iso, Local<Value> asal) {
  mrb_value pegang;
  Local<Value> tempel, taruh;
  Local<Array> ary;
  Local<String> untaian;
  if (asal->IsString()) {
    Utf8Value str(iso, asal);
    return mrb_str_new(diri_, *str, str.length());
  } else if (asal->IsArray()) {
    ary = asal.As<Array>();
    pegang = mrb_ary_new(diri_);
    for (size_t i = 0; i < ary->Length(); i++) {
      taruh = ary->Get(env_->context(), i).ToLocalChecked();
      mrb_ary_push(diri_, pegang, ObjRubyFromJS(iso, taruh));
    }
    return pegang;
  } else if (asal->IsObject()) {
    untaian = JSON::Stringify(env_->context(), asal).ToLocalChecked();
    Local<Context> c = env_->context();
    Local<Object> global = c->Global();
    Local<String> cns = OneByteString(iso, "console");
    Local<String> lg = OneByteString(iso, "log");
    v8::Local<v8::Value> cVal = global->Get(c, cns).ToLocalChecked();
    
    if (cVal->IsObject()) {
      v8::Local<v8::Object> console = cVal.As<v8::Object>();
      v8::Local<v8::Value> log = console->Get(c, lg).ToLocalChecked();
      v8::Local<v8::Function> logFunc = log.As<v8::Function>();
      Local<Value> args[4];
      MaybeLocal<Value> ret;
      args[0] = untaian;
      ret = logFunc->Call(c, console, 1, args);
    }
    Utf8Value str2(iso, untaian);
    
    pegang = mrb_str_new(diri_, *str2, str2.length());
    // return mrb_funcall(diri_, mrb_obj_value(mrb_class_get(diri_, "JSON")), "parse", 1, pegang);
    return mrb_nil_value();
  } else if (asal->IsNullOrUndefined()) {
    return mrb_nil_value();
  } else if (asal->IsTrue()) {
    return mrb_true_value();
  } else if (asal->IsFalse()) {
    return mrb_false_value();
  } else if (asal->IsFunction()) {
    RProc* h = mrb_proc_new_cfunc(diri_, callback_js_func);
    return mrb_obj_value(h);
  } else {
    mrb_raise(diri_, mrb_exc_get(diri_, "ArgumentError"), "JS::Value tidak bisa diconvert");
  }
  return mrb_nil_value();
}

mrb_value Taguja::RefFromRuby(bungkus_js* bjs, mrb_sym nama, mrb_value* argv, mrb_int len) {
  Isolate *iso;
  Local<Context> ctx;
  Local<Value> tempel, nilai, taruh;
  Local<Object> sumber;
  Local<Function> fn;
  std::vector<Local<Value>> param{};
  mrb_value pegang;
  int arena;

  arena = mrb_gc_arena_save(diri_);
  iso = env_->isolate();
  ctx = env_->context();
  sumber = bjs->akar.Get(iso);
  if (!sumber->Has(ctx, OneByteString(iso, mrb_sym2name(diri_, nama))).ToChecked()) {
    mrb_raise(diri_, mrb_exc_get(diri_, "ArgumentError"), "Attribute tidak ditemukan");
  }
  nilai = sumber->Get(ctx, OneByteString(iso, mrb_sym2name(diri_, nama))).ToLocalChecked();
  if (nilai->IsFunction()) {
    fn = nilai.As<Function>();
    for (size_t i = 0; i < len; i++) {
      if (ObjJSFromRuby(iso, argv[i]).ToLocal(&tempel)) {
        param.push_back(tempel);
      } else {
        return mrb_nil_value();
      }
    }
    taruh = fn->Call(ctx, Undefined(iso), len, param.data()).ToLocalChecked();
  } else {
    taruh = nilai;
  }
  
  pegang = ObjRubyFromJS(iso, taruh);
  mrb_gc_arena_restore(diri_, arena);
  return pegang;
}

void Taguja::Logout() {
  Isolate *i = env_->isolate();
  Local<Context> c = env_->context();
  Local<Value> res;
  Local<Function> fn;
  fn = entry_logout_.Get(i);
  res = fn->Call(c, Undefined(i), 0, NULL).ToLocalChecked();
}

void Taguja::V8Keluar() {
  env_->Exit(0);
}