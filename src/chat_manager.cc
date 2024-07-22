#include <jenneire.h>
#include "chatm.h"

const mrb_data_type pesan_rincian_type = {
  "Rincian Pesan", NULL
};
struct RClass *tg, *rp, *pg;
mrb_callinfo ci_proses_pesan;

mrb_value callinfo_panggil(mrb_state*,mrb_value);

void muat_tambahan_ruby(char* kode, void* void_ptr) {
  mrb_state* diri;
  diri = (mrb_state*)void_ptr;
  mrb_load_string(diri, kode);
}

mrb_state* tangkap_diri(Taguja* kuasa) {
  return kuasa->shipped()->diri;
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

void InitChatManager(Taguja* kuasa) {
  mrb_state* diri;
  diri = tangkap_diri(kuasa);
  memset(&ci_proses_pesan, 0, sizeof(mrb_callinfo));
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
  // baca_keterangan_penting(DB_PLUGIN, muat_tambahan_js, isolate);
  baca_keterangan_penting(DB_SCRIPT, muat_tambahan_ruby, diri);
  if (diri->exc) {
    mrb_print_error(diri);
    kuasa->V8Keluar();
  } else {
    sumbang_alamat(muat_tambahan_ruby, diri);
  }
}

void PikirkanJawaban(pesan_rincian* pesan) {
  mrb_state* diri;
  struct RData* dt;
  mrb_value lembar, tunjangan;
  diri = tangkap_diri(pesan->kuasa);
  dt = mrb_data_object_alloc(diri, rp, pesan, &pesan_rincian_type);
  lembar = mrb_obj_value(dt);
  tunjangan = mrb_obj_new(diri, pg, 0, NULL);
  mrb_iv_set(diri, lembar, mrb_intern_lit(diri, "_pengirim"), tunjangan);
  mrb_iv_set(diri, tunjangan, mrb_intern_lit(diri, "_rincianpesan"), lembar);
  callinfo_panggil(diri, lembar);
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

mrb_value callinfo_panggil(mrb_state* diri, mrb_value rincian) {
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