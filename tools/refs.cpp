// Prints the global output indices of the given transactions, then every later transaction in
// [start,end) whose rings reference one of them. Usage: refs <lmdb-dir> <start> <end> <txid>...
#include <iostream>
#include <map>
#include <set>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "string_tools.h"
#include "misc_log_ex.h"
using namespace cryptonote;
int main(int argc, char** argv)
{
  if (argc < 4) { std::cerr << "usage: refs <lmdb-dir> <start> <end> <txid>..." << std::endl; return 2; }
  mlog_configure("", false); mlog_set_log_level(0);
  BlockchainDB* db = new_db(); db->open(argv[1], DBF_RDONLY);
  const uint64_t a = std::stoull(argv[2]), b = std::stoull(argv[3]);
  std::map<uint64_t, std::string> watch;
  for (int i = 4; i < argc; ++i)
  {
    crypto::hash txid; epee::string_tools::hex_to_pod(std::string(argv[i]), txid);
    blobdata bd; transaction tx; db->get_tx_blob(txid, bd); parse_and_validate_tx_from_blob(bd, tx);
    uint64_t tx_idx = 0; db->tx_exists(txid, tx_idx);
    auto oi = db->get_tx_amount_output_indices(tx_idx, 1);
    for (size_t o = 0; o < oi[0].size(); ++o)
    {
      std::string t; get_output_asset_type(tx.vout[o], t);
      watch[oi[0][o].first] = std::string(argv[i]).substr(0, 12) + ":" + std::to_string(o);
      std::cout << "OUT\t" << argv[i] << "\t" << o << "\t" << t << "\t" << oi[0][o].first << "\n";
    }
  }
  for (uint64_t h = a; h < b && h < db->height(); ++h)
  {
    block blk; parse_and_validate_block_from_blob(db->get_block_blob_from_height(h), blk);
    for (const auto& txid : blk.tx_hashes)
    {
      blobdata bd; transaction tx; if (!db->get_pruned_tx_blob(txid, bd) || !parse_and_validate_tx_base_from_blob(bd, tx)) continue;
      for (size_t i = 0; i < tx.vin.size(); ++i)
      {
        const txin_zephyr_key& in = boost::get<txin_zephyr_key>(tx.vin[i]);
        const auto abs = relative_output_offsets_to_absolute(in.key_offsets);
        for (size_t m = 0; m < abs.size(); ++m)
        {
          auto it = watch.find(abs[m]);
          if (it != watch.end())
          {
            std::set<std::string> d; for (const auto& vo : tx.vout) { std::string t; if (get_output_asset_type(vo, t)) d.insert(t); }
            std::string dst; for (auto& x : d) dst += (dst.empty() ? "" : ",") + x;
            std::cout << "REF\t" << h << "\t" << epee::string_tools::pod_to_hex(txid) << "\tinput " << i << "/" << tx.vin.size() << "\tpos " << m << "/" << abs.size()
                      << "\t" << it->second << "\t" << in.asset_type << "->" << dst << "\tvout " << tx.vout.size() << "\tburnt " << tx.amount_burnt << "\tminted " << tx.amount_minted << "\n";
          }
        }
      }
    }
  }
  db->close(); return 0;
}
