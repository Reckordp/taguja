#include <jenneire.h>
#include "chatm.h"

struct RClass *tg, *rp, *pg, *ptj;
mrb_callinfo ci_proses_pesan;
mrb_callinfo ci_balik;
mrb_callinfo ci_lorongjs;

mrb_value proses_pesan_panggil(mrb_state*,mrb_value);
mrb_value balik_panggil(mrb_state*,mrb_value);
mrb_value lorongjs_panggil(mrb_state*,mrb_value);
void tambahan_js_bebas(mrb_state*,void*);

struct keterangan_tambahan_js {
  Taguja* kuasa;
  int penghitung;
};

const mrb_data_type pesan_rincian_type = {
  "Rincian Pesan", NULL
};
const mrb_data_type tambahan_js_type = {
  "Tambahan JS", tambahan_js_bebas
};

mrb_state* tangkap_diri(Taguja* kuasa) {
  return kuasa->shipped()->diri;
}

void muat_tambahan_js(char* kode, void* void_ptr) {
  struct keterangan_tambahan_js *ket;
  Taguja* kuasa;
  mrb_state* diri;
  rincian_tambahan_js* rtj;
  struct RClass *pjsCls;
  struct RData* dt;
  mrb_value lembar, pjs;
  ket = reinterpret_cast<struct keterangan_tambahan_js*>(void_ptr);
  ket->penghitung++;
  diri = tangkap_diri(ket->kuasa);
  rtj = reinterpret_cast<rincian_tambahan_js*>(malloc(sizeof(rincian_tambahan_js)));
  rtj->kuasa = ket->kuasa;
  rtj->id = ket->penghitung;
  ket->kuasa->TambahanJS(rtj, kode);

  if (rtj->bungkus && rtj->akar) {
    dt = mrb_data_object_alloc(diri, ptj, rtj, &tambahan_js_type);
    lembar = mrb_obj_value(dt);
    pjsCls = mrb_class_get(diri, "PengantarJS");
    pjs = mrb_obj_new(diri, pjsCls, 0, NULL);
    mrb_iv_set(diri, pjs, mrb_intern_lit(diri, "_penahan_tambahan_js"), lembar);
    lorongjs_panggil(diri, pjs);
  }
}

void muat_tambahan_ruby(char* kode, void* void_ptr) {
  mrb_state* diri;
  diri = reinterpret_cast<mrb_state*>(void_ptr);
  mrb_load_string(diri, kode);
}

void tambahan_js_bebas(mrb_state* diri, void* void_ptr) {
  rincian_tambahan_js* rtj;
  rtj = reinterpret_cast<rincian_tambahan_js*>(void_ptr);
  if (rtj->akar) delete rtj->akar;
  if (rtj->bungkus) delete rtj->bungkus;
  free(rtj);
}

mrb_value taguja_pengirim_id(mrb_state* diri, mrb_value self) {
  mrb_value rinci;
  pesan_rincian* pesan;

  rinci = mrb_iv_get(diri, self, mrb_intern_lit(diri, "_rincianpesan"));
  pesan = reinterpret_cast<pesan_rincian*>(DATA_PTR(rinci));
  return mrb_str_new_cstr_frozen(diri, pesan->id.c_str());
}

mrb_value taguja_pengirim_chat(mrb_state* diri, mrb_value self) {
  mrb_value rinci;
  pesan_rincian* pesan;

  rinci = mrb_iv_get(diri, self, mrb_intern_lit(diri, "_rincianpesan"));
  pesan = reinterpret_cast<pesan_rincian*>(DATA_PTR(rinci));
  return mrb_str_new_cstr_frozen(diri, pesan->chat.c_str());
}

mrb_value taguja_pengirim_subjek(mrb_state* diri, mrb_value self) {
  mrb_value rinci;
  pesan_rincian* pesan;

  rinci = mrb_iv_get(diri, self, mrb_intern_lit(diri, "_rincianpesan"));
  pesan = reinterpret_cast<pesan_rincian*>(DATA_PTR(rinci));
  return mrb_str_new_cstr_frozen(diri, pesan->subjek.c_str());
}

mrb_value taguja_pengirim_jid(mrb_state* diri, mrb_value self) {
  mrb_value rinci;
  pesan_rincian* pesan;

  rinci = mrb_iv_get(diri, self, mrb_intern_lit(diri, "_rincianpesan"));
  pesan = reinterpret_cast<pesan_rincian*>(DATA_PTR(rinci));
  return mrb_str_new_cstr_frozen(diri, pesan->sender.c_str());
}

mrb_value taguja_pengirim_nama(mrb_state* diri, mrb_value self) {
  mrb_value rinci;
  pesan_rincian* pesan;

  rinci = mrb_iv_get(diri, self, mrb_intern_lit(diri, "_rincianpesan"));
  pesan = reinterpret_cast<pesan_rincian*>(DATA_PTR(rinci));
  return mrb_str_new_cstr_frozen(diri, pesan->nama.c_str());
}

mrb_value taguja_rp_pengirim(mrb_state* diri, mrb_value self) {
  mrb_value pengirim;
  pengirim = mrb_iv_get(diri, self, mrb_intern_lit(diri, "_pengirim"));
  return pengirim;
}

mrb_value taguja_rp_pesan(mrb_state* diri, mrb_value self) {
  pesan_rincian* pesan;
  pesan = reinterpret_cast<pesan_rincian*>(DATA_PTR(self));
  return mrb_str_new_cstr_frozen(diri, pesan->text.c_str());
}

mrb_value taguja_rp_jawab(mrb_state* diri, mrb_value self) {
  char* jawab;
  pesan_rincian* pesan;

  mrb_get_args(diri, "z", &jawab);
  pesan = reinterpret_cast<pesan_rincian*>(DATA_PTR(self));
  pesan->respon = std::string(jawab);
  return mrb_nil_value();
}

mrb_value taguja_gpp(mrb_state* diri, mrb_value self) {
  struct RProc *p_pengabaran;
  mrb_value pengabaran;
  mrb_irep *irep;

  mrb_get_args(diri, "&!", &pengabaran);

  p_pengabaran = mrb_proc_ptr(pengabaran);
  irep = p_pengabaran->body.irep;
  ci_proses_pesan.proc = p_pengabaran;
  ci_proses_pesan.target_class = MRB_PROC_TARGET_CLASS(p_pengabaran);
  ci_proses_pesan.pc = irep->iseq;
  ci_proses_pesan.argc = 1;
  mrb_cv_set(diri, self, mrb_intern_lit(diri, "_kabar"), pengabaran);
  return mrb_nil_value();
}

mrb_value taguja_gramasi(mrb_state* diri, mrb_value self) {
  char *perintah;
  mrb_get_args(diri, "z", &perintah);
  intermediate_gramasi(perintah);
  return mrb_nil_value();
}

mrb_value taguja_lembaran(mrb_state* diri, mrb_value self) {
  char *tabel, **pot;
  mrb_value arr;
  size_t jumlah, ret;
  int arena;

  mrb_get_args(diri, "zA", &tabel, &arr);
  arena = mrb_gc_arena_save(diri);
  jumlah = RARRAY_LEN(arr);
  pot = reinterpret_cast<char**>(malloc(sizeof(char*) * jumlah));
  memset(pot, 0, sizeof(char*) * jumlah); 
  mrb_ary_clear(diri, arr);
  ret = intermediate_lembar(tabel, pot);
  if (ret) {
    for (size_t i = 0; i < jumlah; i++) {
      if (!pot[i]) continue;
      mrb_ary_push(diri, arr, mrb_str_new_cstr_frozen(diri, pot[i]));
    }
  }
  free(pot);
  mrb_gc_arena_restore(diri, arena);
  return ((ret)?(mrb_true_value()):mrb_false_value());
}

mrb_value taguja_balik(mrb_state* diri, mrb_value self) {
  struct RProc *p_pengabaran;
  mrb_value pengabaran, tempat;
  mrb_irep *irep;

  mrb_get_args(diri, "&!", &pengabaran);

  p_pengabaran = mrb_proc_ptr(pengabaran);
  irep = p_pengabaran->body.irep;
  ci_balik.proc = p_pengabaran;
  ci_balik.target_class = MRB_PROC_TARGET_CLASS(p_pengabaran);
  ci_balik.pc = irep->iseq;
  ci_balik.argc = 0;
  mrb_cv_set(diri, self, mrb_intern_lit(diri, "_balik"), pengabaran);
  // mrb_p(diri, diri->c->ci[-1].stackent[0]);
  tempat = diri->c->ci[-1].stackent[0];
  mrb_cv_set(diri, self, mrb_intern_lit(diri, "_balik_tempat"), tempat);
  return mrb_nil_value();
}

mrb_value taguja_balikjs(mrb_state* diri, mrb_value self) {
  struct RProc *p_pengabaran;
  mrb_value pengabaran, tempat;
  mrb_irep *irep;

  mrb_get_args(diri, "&!", &pengabaran);

  p_pengabaran = mrb_proc_ptr(pengabaran);
  irep = p_pengabaran->body.irep;
  ci_lorongjs.proc = p_pengabaran;
  ci_lorongjs.target_class = MRB_PROC_TARGET_CLASS(p_pengabaran);
  ci_lorongjs.pc = irep->iseq;
  ci_lorongjs.argc = 1;
  mrb_cv_set(diri, self, mrb_intern_lit(diri, "_lorongjs"), pengabaran);
  return mrb_nil_value();
}

mrb_value taguja_conn(mrb_state* diri, mrb_value self) {
  mrb_value pjs;
  pjs = mrb_cv_get(diri, self, mrb_intern_lit(diri, "_pemegang"));
  return pjs;
}

mrb_value ptj_ambil_id(mrb_state* diri, mrb_value self) {
  bungkus_lorong* bungkus;
  bungkus = reinterpret_cast<bungkus_lorong*>(DATA_PTR(self));
  return mrb_str_new_cstr_frozen(diri, bungkus->id.c_str());
}

mrb_value ptj_ambil_filename(mrb_state* diri, mrb_value self) {
  bungkus_lorong* bungkus;
  bungkus = reinterpret_cast<bungkus_lorong*>(DATA_PTR(self));
  return mrb_str_new_cstr_frozen(diri, bungkus->filename.c_str());
}

mrb_value ptj_ambil_dlmproses(mrb_state* diri, mrb_value self) {
  bungkus_lorong* bungkus;
  bungkus = reinterpret_cast<bungkus_lorong*>(DATA_PTR(self));
  if (!bungkus->penjaga) return mrb_nil_value();
  return ((bungkus->penjaga->dalam_proses)?(mrb_true_value()):(mrb_false_value()));
}

mrb_value pjs_ref_js(mrb_state* diri, mrb_value self) {
  mrb_sym nama_prop;
  mrb_value *args, pegang;
  mrb_int args_len;
  pesan_rincian* pesan;
  rincian_tambahan_js* rtj;

  mrb_get_args(diri, "n*!", &nama_prop, &args, &args_len);

  if (mrb_iv_defined(diri, self, mrb_intern_lit(diri, "_rincianpesan"))) {
    pegang = mrb_iv_get(diri, self, mrb_intern_lit(diri, "_rincianpesan"));
    pesan = reinterpret_cast<pesan_rincian*>(DATA_PTR(pegang));
    return pesan->kuasa->RefFromRuby(pesan->akar, nama_prop, args, args_len);
  } else if (mrb_iv_defined(diri, self, mrb_intern_lit(diri, "_penahan_tambahan_js"))) {
    pegang = mrb_iv_get(diri, self, mrb_intern_lit(diri, "_penahan_tambahan_js"));
    rtj = reinterpret_cast<rincian_tambahan_js*>(DATA_PTR(pegang));
    return rtj->kuasa->RefFromRuby(rtj->akar, nama_prop, args, args_len);
  }

  return mrb_nil_value();
}

extern "C" {
  typedef enum mrb_timezone {
    MRB_TIMEZONE_NONE   = 0,
    MRB_TIMEZONE_UTC    = 1,
    MRB_TIMEZONE_LOCAL  = 2,
    MRB_TIMEZONE_LAST   = 3
  } mrb_timezone;

  MRB_API mrb_value mrb_time_at(mrb_state *mrb, time_t sec, time_t usec, mrb_timezone timezone);
}

mrb_value time_atutc(mrb_state* diri, mrb_value self) {
  mrb_int sec;
  mrb_get_args(diri, "i", &sec);
  return mrb_time_at(diri, sec, 0, MRB_TIMEZONE_UTC);
}

void InitChatManager(Taguja* kuasa) {
  mrb_state* diri;
  diri = tangkap_diri(kuasa);
  memset(&ci_proses_pesan, 0, sizeof(mrb_callinfo));
  memset(&ci_balik, 0, sizeof(mrb_callinfo));
  memset(&ci_lorongjs, 0, sizeof(mrb_callinfo));
  tg = mrb_define_module(diri, "Taguja");
  pg = mrb_define_class_under(diri, tg, "Pengirim", diri->object_class);
  mrb_define_method(diri, pg, "id", taguja_pengirim_id, MRB_ARGS_NONE());
  mrb_define_method(diri, pg, "chat", taguja_pengirim_chat, MRB_ARGS_NONE());
  mrb_define_method(diri, pg, "subjek", taguja_pengirim_subjek, MRB_ARGS_NONE());
  mrb_define_method(diri, pg, "jid", taguja_pengirim_jid, MRB_ARGS_NONE());
  mrb_define_method(diri, pg, "nama", taguja_pengirim_nama, MRB_ARGS_NONE());
  rp = mrb_define_class_under(diri, tg, "RincianPesan", diri->object_class);
  mrb_define_method(diri, rp, "pengirim", taguja_rp_pengirim, MRB_ARGS_NONE());
  mrb_define_method(diri, rp, "pesan", taguja_rp_pesan, MRB_ARGS_NONE());
  mrb_define_method(diri, rp, "jawab", taguja_rp_jawab, MRB_ARGS_REQ(1));
  mrb_define_module_function(diri, tg, "ganti_proses_pesan", taguja_gpp, MRB_ARGS_REQ(1));
  mrb_define_module_function(diri, tg, "gramasi", taguja_gramasi, MRB_ARGS_REQ(1));
  mrb_define_module_function(diri, tg, "lembaran", taguja_lembaran, MRB_ARGS_REQ(2));
  mrb_define_module_function(diri, tg, "balik", taguja_balik, MRB_ARGS_BLOCK());
  mrb_define_module_function(diri, tg, "balikjs", taguja_balikjs, MRB_ARGS_BLOCK());
  mrb_define_module_function(diri, tg, "conn", taguja_conn, MRB_ARGS_NONE());
  mrb_define_const(diri, tg, "DATABASE_BARU", mrb_fixnum_value(database_baru));
  baca_keterangan_penting(DB_SCRIPT, muat_tambahan_ruby, diri);
  if (diri->exc) {
    mrb_print_error(diri);
    kuasa->V8Keluar();
  } else {
    sumbang_alamat(muat_tambahan_ruby, diri);
  }

  struct RClass *pjsCls;
  mrb_value pjs;
  pjsCls = mrb_class_get(diri, "PengantarJS");
  mrb_define_method(diri, pjsCls, "ref_js", pjs_ref_js, MRB_ARGS_ANY());
  pjs = mrb_obj_new(diri, pjsCls, 0, NULL);
  mrb_cv_set(diri, mrb_obj_value(tg), mrb_intern_lit(diri, "_pemegang"), pjs);

  struct RClass *timeCls;
  timeCls = mrb_class_get(diri, "Time");
  mrb_define_class_method(diri, timeCls, "atutc", time_atutc, MRB_ARGS_REQ(1));

  mrb_value tempat;
  if (mrb_cv_defined(diri, mrb_obj_value(tg), mrb_intern_lit(diri, "_balik"))) {
    tempat = mrb_cv_get(diri, mrb_obj_value(tg), mrb_intern_lit(diri, "_balik_tempat"));
    balik_panggil(diri, tempat);
  }
  mrb_cv_set(diri, mrb_obj_value(tg), mrb_intern_lit(diri, "_lorongjs"), mrb_false_value());

  ptj = mrb_define_class_under(diri, tg, "PenahanTambahanJS", diri->object_class);
  mrb_define_method(diri, ptj, "id", ptj_ambil_id, MRB_ARGS_NONE());
  mrb_define_method(diri, ptj, "filename", ptj_ambil_filename, MRB_ARGS_NONE());
  mrb_define_method(diri, ptj, "dalam_proses", ptj_ambil_dlmproses, MRB_ARGS_NONE());
}

void MasukLorongJS(Taguja* kuasa) {
  mrb_state* diri;
  struct RData* dt;
  mrb_value kabar;
  struct keterangan_tambahan_js ket;
  int arena;
  memset(&ket, 0, sizeof(struct keterangan_tambahan_js));
  ket.kuasa = kuasa;
  ket.penghitung = 0;
  diri = tangkap_diri(kuasa);
  arena = mrb_gc_arena_save(diri);
  kabar = mrb_cv_get(diri, mrb_obj_value(tg), mrb_intern_lit(diri, "_lorongjs"));
  if (mrb_nil_p(kabar)) return;
  baca_keterangan_penting(DB_PLUGIN, muat_tambahan_js, &ket);
  mrb_gc_arena_restore(diri, arena);
}

void PikirkanJawaban(pesan_rincian* pesan) {
  mrb_state* diri;
  struct RData* dt;
  mrb_value lembar, tunjangan, pjs;
  diri = tangkap_diri(pesan->kuasa);
  dt = mrb_data_object_alloc(diri, rp, pesan, &pesan_rincian_type);
  lembar = mrb_obj_value(dt);
  tunjangan = mrb_obj_new(diri, pg, 0, NULL);
  pjs = mrb_cv_get(diri, mrb_obj_value(tg), mrb_intern_lit(diri, "_pemegang"));
  mrb_iv_set(diri, lembar, mrb_intern_lit(diri, "_pengirim"), tunjangan);
  mrb_iv_set(diri, tunjangan, mrb_intern_lit(diri, "_rincianpesan"), lembar);
  mrb_iv_set(diri, pjs, mrb_intern_lit(diri, "_rincianpesan"), lembar);
  proses_pesan_panggil(diri, lembar);
}

static inline void
envadjust(mrb_state *mrb, mrb_value *oldbase, mrb_value *newbase, size_t oldsize)
{
  mrb_callinfo *ci = mrb->c->cibase;

  if (newbase == oldbase) return;
  while (ci <= mrb->c->ci) {
    struct REnv *e = ci->env;
    mrb_value *st;

    if (e && MRB_ENV_ONSTACK_P(e) &&
        (st = e->stack) && oldbase <= st && st < oldbase+oldsize) {
      ptrdiff_t off = e->stack - oldbase;

      e->stack = newbase + off;
    }

    if (ci->proc && MRB_PROC_ENV_P(ci->proc) && ci->env != MRB_PROC_ENV(ci->proc)) {
      e = MRB_PROC_ENV(ci->proc);

      if (e && MRB_ENV_ONSTACK_P(e) &&
          (st = e->stack) && oldbase <= st && st < oldbase+oldsize) {
        ptrdiff_t off = e->stack - oldbase;

        e->stack = newbase + off;
      }
    }

    ci->stackent = newbase + (ci->stackent - oldbase);
    ci++;
  }
}

mrb_value proses_pesan_panggil(mrb_state* diri, mrb_value rincian) {
  if (ci_proses_pesan.proc == NULL) return mrb_false_value();
  struct mrb_jmpbuf c_jmp, *sc_jmp = diri->jmp;
  mrb_value *stack_awal, *st, *oldbase, *newstack, hasil;
  off_t awal;
  mrb_callinfo *ci, *ci_awal;
  int room;

  if (ci_proses_pesan.proc == NULL) return mrb_nil_value();
  room = ci_proses_pesan.proc->body.irep->nregs + ci_proses_pesan.argc + 
    diri->c->ci->proc->body.irep->nregs;
  if (diri->c->stack + room >= diri->c->stend) {
    oldbase = diri->c->stbase;
    size_t oldsize = diri->c->stend - diri->c->stbase;
    size_t size = oldsize;
    size_t off = diri->c->stack - diri->c->stbase;
    size += room;
    newstack = (mrb_value *)mrb_realloc(diri, diri->c->stbase, sizeof(mrb_value) * size);
    diri->c->stbase = newstack;
    diri->c->stack = diri->c->stbase + off;
    diri->c->stend = diri->c->stbase + size;
    envadjust(diri, oldbase, newstack, oldsize);
  }

  MRB_TRY(&c_jmp) {
    diri->jmp = &c_jmp;
    ci_awal = diri->c->ci;
    ci = ci_awal + 1;
    stack_awal = diri->c->stack;
    awal = stack_awal - diri->c->stbase;
    st = stack_awal + ci_awal->proc->body.irep->nregs;

    diri->c->ci++;

    memcpy(ci, &ci_proses_pesan, sizeof(mrb_callinfo));
    diri->c->stack = st;
    ci->stackent = st;
    ci->acc = -1;

    for (int i = 0; i < ci->proc->body.irep->nregs; ++i) {
      st[i] = mrb_nil_value();
    }
    
    st[0] = mrb_top_self(diri);
    st[1] = rincian;
    diri->exc = NULL;

    hasil = mrb_vm_exec(diri, ci_proses_pesan.proc, ci_proses_pesan.proc->body.irep->iseq);

    diri->c->stack = diri->c->stbase + awal;

  } MRB_CATCH(&c_jmp) {
// terjadiError:
    // print_error();
    mrb_print_error(diri);
    return mrb_nil_value();

  } MRB_END_EXC(&c_jmp);

  diri->jmp = sc_jmp;
  return hasil;
}

mrb_value balik_panggil(mrb_state* diri, mrb_value self) {
  if (ci_balik.proc == NULL) return mrb_false_value();
  struct mrb_jmpbuf c_jmp, *sc_jmp = diri->jmp;
  mrb_value *stack_awal, *st, *oldbase, *newstack, hasil;
  off_t awal;
  mrb_callinfo *ci, *ci_awal;
  int room;

  if (ci_balik.proc == NULL) return mrb_nil_value();
  room = ci_balik.proc->body.irep->nregs + ci_balik.argc + 
    diri->c->ci->proc->body.irep->nregs;
  if (diri->c->stack + room >= diri->c->stend) {
    oldbase = diri->c->stbase;
    size_t oldsize = diri->c->stend - diri->c->stbase;
    size_t size = oldsize;
    size_t off = diri->c->stack - diri->c->stbase;
    size += room;
    newstack = (mrb_value *)mrb_realloc(diri, diri->c->stbase, sizeof(mrb_value) * size);
    diri->c->stbase = newstack;
    diri->c->stack = diri->c->stbase + off;
    diri->c->stend = diri->c->stbase + size;
    envadjust(diri, oldbase, newstack, oldsize);
  }

  MRB_TRY(&c_jmp) {
    diri->jmp = &c_jmp;
    ci_awal = diri->c->ci;
    ci = ci_awal + 1;
    stack_awal = diri->c->stack;
    awal = stack_awal - diri->c->stbase;
    st = stack_awal + ci_awal->proc->body.irep->nregs;

    diri->c->ci++;

    memcpy(ci, &ci_balik, sizeof(mrb_callinfo));
    diri->c->stack = st;
    ci->stackent = st;
    ci->acc = -1;

    for (int i = 0; i < ci->proc->body.irep->nregs; ++i) {
      st[i] = mrb_nil_value();
    }
    
    st[0] = self;
    diri->exc = NULL;

    hasil = mrb_vm_exec(diri, ci_balik.proc, ci_balik.proc->body.irep->iseq);

    diri->c->stack = diri->c->stbase + awal;

  } MRB_CATCH(&c_jmp) {
// terjadiError:
    // print_error();
    mrb_print_error(diri);
    return mrb_nil_value();

  } MRB_END_EXC(&c_jmp);

  diri->jmp = sc_jmp;
  return hasil;
}

mrb_value lorongjs_panggil(mrb_state* diri, mrb_value pjs) {
  if (ci_lorongjs.proc == NULL) return mrb_false_value();
  struct mrb_jmpbuf c_jmp, *sc_jmp = diri->jmp;
  mrb_value *stack_awal, *st, *oldbase, *newstack, hasil;
  off_t awal;
  mrb_callinfo *ci, *ci_awal;
  int room;

  if (ci_lorongjs.proc == NULL) return mrb_nil_value();
  room = ci_lorongjs.proc->body.irep->nregs + ci_lorongjs.argc + 
    diri->c->ci->proc->body.irep->nregs;
  if (diri->c->stack + room >= diri->c->stend) {
    oldbase = diri->c->stbase;
    size_t oldsize = diri->c->stend - diri->c->stbase;
    size_t size = oldsize;
    size_t off = diri->c->stack - diri->c->stbase;
    size += room;
    newstack = (mrb_value *)mrb_realloc(diri, diri->c->stbase, sizeof(mrb_value) * size);
    diri->c->stbase = newstack;
    diri->c->stack = diri->c->stbase + off;
    diri->c->stend = diri->c->stbase + size;
    envadjust(diri, oldbase, newstack, oldsize);
  }

  MRB_TRY(&c_jmp) {
    diri->jmp = &c_jmp;
    ci_awal = diri->c->ci;
    ci = ci_awal + 1;
    stack_awal = diri->c->stack;
    awal = stack_awal - diri->c->stbase;
    st = stack_awal + ci_awal->proc->body.irep->nregs;

    diri->c->ci++;

    memcpy(ci, &ci_lorongjs, sizeof(mrb_callinfo));
    diri->c->stack = st;
    ci->stackent = st;
    ci->acc = -1;

    for (int i = 0; i < ci->proc->body.irep->nregs; ++i) {
      st[i] = mrb_nil_value();
    }
    
    st[0] = mrb_top_self(diri);
    st[1] = pjs;
    diri->exc = NULL;

    hasil = mrb_vm_exec(diri, ci_lorongjs.proc, ci_lorongjs.proc->body.irep->iseq);

    diri->c->stack = diri->c->stbase + awal;

  } MRB_CATCH(&c_jmp) {
// terjadiError:
    // print_error();
    mrb_print_error(diri);
    return mrb_nil_value();

  } MRB_END_EXC(&c_jmp);

  diri->jmp = sc_jmp;
  return hasil;
}