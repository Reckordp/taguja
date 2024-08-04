#include "chatm.h"

#define DERET_PLUGIN(DEF) \
  DEF("Serial", serial)

#define DERET_SCRIPT(DEF) \
  DEF("Pangkalan", pangkalan) \
  DEF("Transaksi", transaksi) \
  DEF("SQL Statement", sql_statement) \
  DEF("Barang Ransel", barang_ransel) \
  DEF("Game", game) \
  DEF("User Personal", user_personal) \
  DEF("Owner Only", owner_only) \
  DEF("Premium Event", premium_event) \
  DEF("Premium Limit", premium_limit) \
  DEF("Premium Daily", premium_daily) \
  DEF("Premium Bet", premium_bet) \
  DEF("Free Common", free_common) \
  DEF("Free Common A", free_common_a) \
  DEF("Free InFlow", free_inflow) \
  DEF("Free OutFlow", free_outflow) \
  DEF("Medan", medan)

void JendelaInit(HINSTANCE*);
void PesanDatang(pesan_rincian*);
void Nusaterlogin(char*,char*);
void Nusaterlogout();
void TentukanPenguasa(Taguja*);