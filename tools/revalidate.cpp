// Re-runs, for every non-coinbase transaction in [start,end) of a Zephyr LMDB chain, the checks
// the node applies with the function in force at that height:
//  SEM  : rct::verRctSemanticsSimple (below hard fork 6) or rct::verRctSemanticsZeph (from 6),
//         with the pricing record of the block named by the transaction for conversions
//  MINT : rct::validateMintedAmount for conversions below hard fork 6
//  SIG  : ring signatures and commitments, rct::verRctNonSemanticsSimple, rings read from the chain
// Prints one line per failure and counters. Usage: revalidate <lmdb-dir> <start> <end>
#include <iostream>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "cryptonote_core/cryptonote_tx_utils.h"
#include "cryptonote_core/blockchain.h"
#include "ringct/rctSigs.h"
#include "string_tools.h"
#include "misc_log_ex.h"
using namespace cryptonote;
int main(int argc, char** argv)
{
  if (argc < 4) { std::cerr << "usage: revalidate <lmdb-dir> <start> <end>" << std::endl; return 2; }
  mlog_configure("", false); mlog_set_log_level(0);
  const uint64_t a = std::stoull(argv[2]); uint64_t b = std::stoull(argv[3]);
  BlockchainDB* db = new_db(); db->open(argv[1], DBF_RDONLY);
  if (b > db->height()) b = db->height();
  uint64_t ntx = 0, nconv = 0, sem_bad = 0, mint_bad = 0, sig_bad = 0, other = 0;
  for (uint64_t h = a; h < b; ++h)
  {
    block blk; parse_and_validate_block_from_blob(db->get_block_blob_from_height(h), blk);
    const uint8_t hf = blk.major_version;
    for (const crypto::hash& txid : blk.tx_hashes)
    {
      blobdata bd; transaction tx;
      if (!db->get_tx_blob(txid, bd) || !parse_and_validate_tx_from_blob(bd, tx)) { std::cout << "ERR\t" << h << "\tparse\n"; ++other; continue; }
      ++ntx;
      const std::string id = epee::string_tools::pod_to_hex(txid);
      std::string src, dst; transaction_type type;
      if (!get_tx_asset_types(tx, txid, src, dst, false) || !get_tx_type(src, dst, type)) { std::cout << "TYPE\t" << h << "\t" << (unsigned)hf << "\t" << id << "\n"; ++other; continue; }
      const bool audit = type == transaction_type::AUDIT_ZEPH || type == transaction_type::AUDIT_STABLE || type == transaction_type::AUDIT_RESERVE || type == transaction_type::AUDIT_YIELD;
      const bool conv = src != dst && !audit;
      oracle::pricing_record pr;
      if (conv)
      {
        ++nconv;
        block prb; parse_and_validate_block_from_blob(db->get_block_blob_from_height(tx.pricing_record_height), prb);
        pr = prb.pricing_record;
      }
      auto sem = [&](uint8_t v) {
        try {
          if (v >= HF_VERSION_V6) return rct::verRctSemanticsZeph(tx.rct_signatures, pr, type, src, dst, tx.amount_burnt, tx.amount_minted, tx.vout, tx.vin, v);
          return rct::verRctSemanticsSimple(tx.rct_signatures, pr, type, src, dst, tx.amount_burnt, tx.vout, tx.vin, v);
        } catch (...) { return false; }
      };
      bool ok = sem(hf);
      std::string note;
      if (!ok && hf > 1 && sem(hf - 1)) { ok = true; note = "passes-with-previous-hf"; }
      if (!ok) { ++sem_bad; std::cout << "SEM\t" << h << "\t" << (unsigned)hf << "\t" << id << "\t" << src << "->" << dst << "\tburnt=" << tx.amount_burnt << "\tminted=" << tx.amount_minted << "\n"; }
      else if (!note.empty()) std::cout << "NOTE\t" << h << "\t" << (unsigned)hf << "\t" << id << "\t" << note << "\n";
      if (conv && hf < HF_VERSION_V6)
      {
        bool m = false, m_prev = false;
        try { m = rct::validateMintedAmount(tx.rct_signatures, tx.amount_burnt, tx.amount_minted, pr, src, dst, hf); } catch (...) {}
        if (!m && hf > 1) { try { m_prev = rct::validateMintedAmount(tx.rct_signatures, tx.amount_burnt, tx.amount_minted, pr, src, dst, hf - 1); } catch (...) {} }
        if (!m && m_prev) { m = true; std::cout << "NOTE\t" << h << "\t" << (unsigned)hf << "\t" << id << "\tminted-amount-passes-with-previous-hf\n"; }
        if (!m) { ++mint_bad; std::cout << "MINT\t" << h << "\t" << (unsigned)hf << "\t" << id << "\t" << src << "->" << dst << "\tburnt=" << tx.amount_burnt << "\tminted=" << tx.amount_minted << "\n"; }
      }
      // signatures
      bool sig = false;
      try {
        std::vector<std::vector<rct::ctkey>> pubkeys(tx.vin.size());
        for (size_t i = 0; i < tx.vin.size(); ++i)
        {
          const txin_zephyr_key& in = boost::get<txin_zephyr_key>(tx.vin[i]);
          for (uint64_t off : relative_output_offsets_to_absolute(in.key_offsets))
          {
            output_data_t od = db->get_output_key(in.amount, off, true);
            pubkeys[i].push_back(rct::ctkey{rct::pk2rct(od.pubkey), od.commitment});
          }
        }
        transaction t2 = tx;
        if (Blockchain::expand_transaction_2(t2, get_transaction_prefix_hash(tx), pubkeys, hf))
          sig = rct::verRctNonSemanticsSimple(t2.rct_signatures);
      } catch (...) { sig = false; }
      if (!sig) { ++sig_bad; std::cout << "SIG\t" << h << "\t" << (unsigned)hf << "\t" << id << "\t" << src << "->" << dst << "\tinputs=" << tx.vin.size() << "\n"; }
    }
    if (h % 2000 == 0) std::cout << std::flush;
  }
  std::cout << "STAT\t" << a << "\t" << b << "\ttxs=" << ntx << "\tconversions=" << nconv << "\tsem_bad=" << sem_bad << "\tmint_bad=" << mint_bad << "\tsig_bad=" << sig_bad << "\tother=" << other << std::endl;
  db->close(); return 0;
}
