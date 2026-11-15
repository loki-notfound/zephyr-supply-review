// Direct test of the two semantics functions of ZephyrProtocol/zephyr v2.3.0 on a real transfer
// from the chain: original proof, then the same transaction with a corrupted Bulletproof+ proof.
// Usage: gaptest <lmdb-dir> <height-with-a-ZEPH-transfer>
#include <iostream>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "cryptonote_core/cryptonote_tx_utils.h"
#include "ringct/rctSigs.h"
#include "ringct/bulletproofs_plus.h"
#include "string_tools.h"
#include "misc_log_ex.h"
using namespace cryptonote;
int main(int argc, char** argv)
{
  if (argc < 3) { std::cerr << "usage: gaptest <lmdb-dir> <height-with-a-ZEPH-transfer>" << std::endl; return 2; }
  mlog_configure("", false); mlog_set_log_level(0);
  BlockchainDB* db = new_db(); db->open(argv[1], DBF_RDONLY);
  uint64_t h = std::stoull(argv[2]);
  for (; h < db->height(); ++h)
  {
    block blk; parse_and_validate_block_from_blob(db->get_block_blob_from_height(h), blk);
    for (const auto& txid : blk.tx_hashes)
    {
      blobdata bd; transaction tx; db->get_tx_blob(txid, bd); parse_and_validate_tx_from_blob(bd, tx);
      std::string src, dst; transaction_type type;
      if (!get_tx_asset_types(tx, txid, src, dst, false) || !get_tx_type(src, dst, type)) continue;
      if (type != transaction_type::TRANSFER || src != "ZEPH") continue;
      std::cout << "height " << h << " hf " << (unsigned)blk.major_version << " tx " << epee::string_tools::pod_to_hex(txid) << " outputs " << tx.vout.size() << "\n";
      const oracle::pricing_record pr;
      rct::rctSig rv = tx.rct_signatures;
      auto run = [&](const char* label) {
        std::vector<const rct::BulletproofPlus*> p; for (auto& x : rv.p.bulletproofs_plus) p.push_back(&x);
        bool bpp = false; try { bpp = rct::bulletproof_plus_VERIFY(p); } catch (...) {}
        std::cout << label << ": bulletproof_plus_VERIFY=" << bpp
                  << "  verRctSemanticsSimple(pre-HF6 path)=" << rct::verRctSemanticsSimple(rv, pr, type, src, dst, tx.amount_burnt, tx.vout, tx.vin, 5)
                  << "  verRctSemanticsZeph(HF6+ path)=" << rct::verRctSemanticsZeph(rv, pr, type, src, dst, tx.amount_burnt, tx.amount_minted, tx.vout, tx.vin, 6) << "\n";
      };
      run("original proof ");
      rv.p.bulletproofs_plus[0].r1.bytes[0] ^= 1;
      run("corrupted proof");
      db->close(); return 0;
    }
  }
}
