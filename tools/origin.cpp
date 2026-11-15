// For transaction ids read from stdin: for every input that has a ring member of another asset,
// print every member of that ring with the transaction that created it. For a conversion the
// amounts are public, so if the creating transaction has exactly one output of the member's asset,
// that output's amount is the public minted amount.
// Columns: ORG, tx height, tx id, input index, claimed asset, member position (0 = oldest), ring
// size, global output index, member asset, member height, creating tx id, coinbase (0/1), source
// assets, destination assets, outputs of the member's asset in the creating tx, burnt, minted
// With "ages" as second argument it prints instead one line for EVERY input of those transactions,
// flagged or not: AGE, tx height, tx id, input index, claimed asset, ring size, number of foreign
// members, lowest and highest creation height in the ring.
// Usage: origin <lmdb-dir> [ages] < txids
#include <iostream>
#include <set>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "string_tools.h"
#include "misc_log_ex.h"
using namespace cryptonote;
int main(int argc, char** argv)
{
  if (argc < 2) { std::cerr << "usage: origin <lmdb-dir> [ages] < txids" << std::endl; return 2; }
  const bool ages = argc > 2 && std::string(argv[2]) == "ages";
  mlog_configure("", false); mlog_set_log_level(0);
  BlockchainDB* db = new_db(); db->open(argv[1], DBF_RDONLY);
  std::string line;
  while (std::getline(std::cin, line))
  {
    crypto::hash txid;
    if (!epee::string_tools::hex_to_pod(line, txid)) continue;
    blobdata bd; transaction tx;
    if (!db->get_tx_blob(txid, bd) || !parse_and_validate_tx_from_blob(bd, tx)) continue;
    const uint64_t h = db->get_tx_block_height(txid);
    for (size_t i = 0; i < tx.vin.size(); ++i)
    {
      const txin_zephyr_key& in = boost::get<txin_zephyr_key>(tx.vin[i]);
      const std::vector<uint64_t> abs = relative_output_offsets_to_absolute(in.key_offsets);
      size_t foreign = 0; uint64_t lo = UINT64_MAX, hi = 0;
      for (uint64_t g : abs)
      {
        output_data_t od = db->get_output_key(in.amount, g, false);
        if (std::string(od.asset_type, strnlen(od.asset_type, 8)) != in.asset_type) ++foreign;
        lo = std::min<uint64_t>(lo, od.height); hi = std::max<uint64_t>(hi, od.height);
      }
      if (ages)
      {
        std::cout << "AGE\t" << h << "\t" << line << "\t" << i << "\t" << in.asset_type << "\t" << abs.size() << "\t" << foreign << "\t" << lo << "\t" << hi << "\n";
        continue;
      }
      if (!foreign) continue;
      for (size_t m = 0; m < abs.size(); ++m)
      {
        output_data_t od = db->get_output_key(in.amount, abs[m], false);
        const std::string at(od.asset_type, strnlen(od.asset_type, 8));
        tx_out_index toi = db->get_output_tx_and_index(in.amount, abs[m]);
        blobdata cb; transaction ct;
        if (!db->get_pruned_tx_blob(toi.first, cb) || !parse_and_validate_tx_base_from_blob(cb, ct)) { std::cout << "ERR\t" << line << "\n"; continue; }
        const bool coinbase = ct.vin.size() == 1 && ct.vin[0].type() == typeid(txin_gen);
        std::set<std::string> s, d; size_t same = 0;
        if (!coinbase) for (const auto& vi : ct.vin) s.insert(boost::get<txin_zephyr_key>(vi).asset_type);
        for (const auto& vo : ct.vout) { std::string t; if (get_output_asset_type(vo, t)) { d.insert(t); if (t == at) ++same; } }
        std::string src, dst;
        for (auto& x : s) src += (src.empty() ? "" : ",") + x;
        for (auto& x : d) dst += (dst.empty() ? "" : ",") + x;
        std::cout << "ORG\t" << h << "\t" << line << "\t" << i << "\t" << in.asset_type << "\t" << m << "\t" << abs.size() << "\t" << abs[m] << "\t" << at << "\t" << od.height
                  << "\t" << epee::string_tools::pod_to_hex(toi.first) << "\t" << coinbase << "\t" << src << "\t" << dst << "\t" << same << "\t" << ct.amount_burnt << "\t" << ct.amount_minted << "\n";
      }
    }
  }
  db->close(); return 0;
}
