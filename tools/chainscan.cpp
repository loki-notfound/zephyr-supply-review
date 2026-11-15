// Read-only scan of a Zephyr LMDB chain: for every non-coinbase transaction in [start,end)
//  RING: ring members whose stored asset type differs from the asset type the input claims
//  BPP : Bulletproof+ range proofs that do not verify, are missing, or do not match outPk
// Output: tab-separated lines on stdout. Usage: chainscan <lmdb-dir> <start> <end>
#include <iostream>
#include <set>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "ringct/rctTypes.h"
#include "ringct/bulletproofs_plus.h"
#include "string_tools.h"
#include "misc_log_ex.h"
using namespace cryptonote;
int main(int argc, char** argv)
{
  if (argc < 4) { std::cerr << "usage: chainscan <lmdb-dir> <start> <end>" << std::endl; return 2; }
  mlog_configure("", false); mlog_set_log_level(0);
  const uint64_t a = std::stoull(argv[2]); uint64_t b = std::stoull(argv[3]);
  BlockchainDB* db = new_db();
  db->open(argv[1], DBF_RDONLY);
  if (b > db->height()) b = db->height();
  uint64_t ntx = 0, nin = 0, nmem = 0, nbpp = 0, nring_bad = 0, nbpp_bad = 0;
  for (uint64_t h = a; h < b; ++h)
  {
    block blk;
    if (!parse_and_validate_block_from_blob(db->get_block_blob_from_height(h), blk)) { std::cout << "ERR\t" << h << "\tblock parse\n"; continue; }
    const unsigned hf = blk.major_version;
    for (const crypto::hash& txid : blk.tx_hashes)
    {
      blobdata bd; transaction tx;
      if (!db->get_tx_blob(txid, bd) || !parse_and_validate_tx_from_blob(bd, tx)) { std::cout << "ERR\t" << h << "\t" << txid << "\ttx parse\n"; continue; }
      ++ntx;
      const std::string id = epee::string_tools::pod_to_hex(txid);
      for (size_t i = 0; i < tx.vin.size(); ++i)
      {
        if (tx.vin[i].type() != typeid(txin_zephyr_key)) { std::cout << "ERR\t" << h << "\t" << id << "\tinput type\n"; continue; }
        const txin_zephyr_key& in = boost::get<txin_zephyr_key>(tx.vin[i]);
        ++nin;
        const std::vector<uint64_t> abs = relative_output_offsets_to_absolute(in.key_offsets);
        std::multiset<std::string> seen; bool bad = false;
        for (uint64_t off : abs)
        {
          ++nmem;
          std::string at;
          try { output_data_t od = db->get_output_key(in.amount, off, false); at = std::string(od.asset_type, strnlen(od.asset_type, 8)); }
          catch (...) { at = "?missing"; }
          seen.insert(at);
          if (at != in.asset_type) bad = true;
        }
        if (bad)
        {
          ++nring_bad;
          std::cout << "RING\t" << h << "\t" << hf << "\t" << id << "\t" << i << "\t" << in.asset_type << "\t";
          std::string prev; size_t c = 0; bool first = true;
          for (auto it = seen.begin();; ++it)
          {
            if (it == seen.end() || *it != prev) { if (c) { std::cout << (first ? "" : ",") << prev << ":" << c; first = false; } if (it == seen.end()) break; prev = *it; c = 0; }
            ++c;
          }
          std::cout << "\t" << abs.size() << "\n";
        }
      }
      const rct::rctSig& rv = tx.rct_signatures;
      std::string why;
      if (rv.type != rct::RCTTypeBulletproofPlus) why = "type=" + std::to_string((unsigned)rv.type);
      else if (rv.p.bulletproofs_plus.empty()) why = "no-proof";
      else if (rct::n_bulletproof_plus_amounts(rv.p.bulletproofs_plus) != rv.outPk.size()) why = "amount-count";
      else
      {
        std::vector<const rct::BulletproofPlus*> proofs;
        for (const auto& p : rv.p.bulletproofs_plus) proofs.push_back(&p);
        bool ok = false;
        try { ok = rct::bulletproof_plus_VERIFY(proofs); } catch (...) { ok = false; }
        ++nbpp;
        if (!ok) why = "verify-failed";
      }
      if (!why.empty())
      {
        ++nbpp_bad;
        std::string src, dst;
        std::set<std::string> s, d;
        for (const auto& vi : tx.vin) if (vi.type() == typeid(txin_zephyr_key)) s.insert(boost::get<txin_zephyr_key>(vi).asset_type);
        for (const auto& vo : tx.vout) { std::string t; if (get_output_asset_type(vo, t)) d.insert(t); }
        for (auto& x : s) src += (src.empty() ? "" : ",") + x;
        for (auto& x : d) dst += (dst.empty() ? "" : ",") + x;
        std::cout << "BPP\t" << h << "\t" << hf << "\t" << id << "\t" << why << "\t" << src << "\t" << dst << "\t" << tx.vin.size() << "\t" << tx.vout.size()
                  << "\t" << tx.amount_burnt << "\t" << tx.amount_minted << "\t" << rv.p.bulletproofs_plus.size() << "\n";
      }
    }
    if (h % 5000 == 0) std::cout << std::flush;
  }
  std::cout << "STAT\t" << a << "\t" << b << "\ttxs=" << ntx << "\tinputs=" << nin << "\tring_members=" << nmem << "\tring_bad_inputs=" << nring_bad
            << "\tbpp_checked=" << nbpp << "\tbpp_bad=" << nbpp_bad << std::endl;
  db->close();
  return 0;
}
