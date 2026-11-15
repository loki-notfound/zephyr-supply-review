// Collects the key image of every input of every transaction in a Zephyr LMDB chain and reports
// any key image that occurs more than once. A key image is derived from the output that is spent,
// whatever asset the input claims, so two spends of one output would show up here.
// Prints one DUP line per repeated key image and a STAT line. Usage: keyimages <lmdb-dir>
#include <iostream>
#include <unordered_set>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "string_tools.h"
#include "misc_log_ex.h"
using namespace cryptonote;
int main(int argc, char** argv)
{
  if (argc < 2) { std::cerr << "usage: keyimages <lmdb-dir>" << std::endl; return 2; }
  mlog_configure("", false); mlog_set_log_level(0);
  BlockchainDB* db = new_db(); db->open(argv[1], DBF_RDONLY);
  const uint64_t top = db->height();
  std::unordered_set<crypto::key_image> seen;
  uint64_t txs = 0, inputs = 0, dup = 0, other = 0;
  for (uint64_t h = 0; h < top; ++h)
  {
    block blk; parse_and_validate_block_from_blob(db->get_block_blob_from_height(h), blk);
    for (const crypto::hash& txid : blk.tx_hashes)
    {
      blobdata bd; transaction tx;
      if (!db->get_pruned_tx_blob(txid, bd) || !parse_and_validate_tx_base_from_blob(bd, tx)) { std::cout << "ERR\t" << h << "\tparse\n"; ++other; continue; }
      ++txs;
      for (const auto& vi : tx.vin)
      {
        if (vi.type() != typeid(txin_zephyr_key)) { std::cout << "ERR\t" << h << "\tinput type\n"; ++other; continue; }
        ++inputs;
        const crypto::key_image& ki = boost::get<txin_zephyr_key>(vi).k_image;
        if (!seen.insert(ki).second) { ++dup; std::cout << "DUP\t" << h << "\t" << epee::string_tools::pod_to_hex(txid) << "\t" << epee::string_tools::pod_to_hex(ki) << "\n"; }
      }
    }
  }
  std::cout << "STAT\t0\t" << top << "\ttxs=" << txs << "\tinputs=" << inputs << "\tdistinct_key_images=" << seen.size() << "\tduplicates=" << dup << "\tother=" << other << std::endl;
  db->close(); return 0;
}
