// Prints the block hash stored in a Zephyr LMDB chain at the given heights (and at the tip).
// A block hash commits to every earlier block and to every transaction in them, so equal hashes
// at one height on two nodes mean identical chain content up to that height.
// Usage: blockhash <lmdb-dir> [height...]
#include <iostream>
#include <vector>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "string_tools.h"
#include "misc_log_ex.h"
int main(int argc, char** argv)
{
  if (argc < 2) { std::cerr << "usage: blockhash <lmdb-dir> [height...]" << std::endl; return 2; }
  mlog_configure("", false); mlog_set_log_level(0);
  cryptonote::BlockchainDB* db = cryptonote::new_db(); db->open(argv[1], DBF_RDONLY);
  std::vector<uint64_t> hs; for (int i = 2; i < argc; ++i) hs.push_back(std::stoull(argv[i]));
  hs.push_back(db->height() - 1);
  for (uint64_t h : hs) std::cout << h << "\t" << epee::string_tools::pod_to_hex(db->get_block_hash_from_height(h)) << "\n";
  db->close(); return 0;
}
