// For transaction ids read from stdin: print every ring member whose stored asset type differs
// from the asset type the input claims. Columns: DET, tx height, tx id, input index, claimed asset,
// member position in ring (0 = oldest), ring size, global output index, member asset, member height,
// then per transaction: TX, height, id, source assets, destination assets, inputs, outputs, burnt, minted, fee
// Usage: ringdetail <lmdb-dir> < txids
#include <iostream>
#include <set>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "string_tools.h"
#include "misc_log_ex.h"
using namespace cryptonote;
int main(int argc, char** argv)
{
  if (argc < 2) { std::cerr << "usage: ringdetail <lmdb-dir> < txids" << std::endl; return 2; }
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
    std::set<std::string> s, d;
    for (size_t i = 0; i < tx.vin.size(); ++i)
    {
      const txin_zephyr_key& in = boost::get<txin_zephyr_key>(tx.vin[i]);
      s.insert(in.asset_type);
      const std::vector<uint64_t> abs = relative_output_offsets_to_absolute(in.key_offsets);
      for (size_t m = 0; m < abs.size(); ++m)
      {
        output_data_t od = db->get_output_key(in.amount, abs[m], false);
        const std::string at(od.asset_type, strnlen(od.asset_type, 8));
        if (at != in.asset_type)
          std::cout << "DET\t" << h << "\t" << line << "\t" << i << "\t" << in.asset_type << "\t" << m << "\t" << abs.size() << "\t" << abs[m] << "\t" << at << "\t" << od.height << "\n";
      }
    }
    for (const auto& vo : tx.vout) { std::string t; if (get_output_asset_type(vo, t)) d.insert(t); }
    std::string src, dst;
    for (auto& x : s) src += (src.empty() ? "" : ",") + x;
    for (auto& x : d) dst += (dst.empty() ? "" : ",") + x;
    std::cout << "TX\t" << h << "\t" << line << "\t" << src << "\t" << dst << "\t" << tx.vin.size() << "\t" << tx.vout.size() << "\t" << tx.amount_burnt << "\t" << tx.amount_minted << "\t" << tx.rct_signatures.txnFee << "\n";
  }
  db->close(); return 0;
}
