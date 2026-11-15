// Re-checks the miner transaction of every block in [start,end) of a Zephyr LMDB chain against the
// rule of Blockchain::validate_miner_transaction (v2.3.0): the outputs in the base asset must equal
// base reward - reserve share - yield share + fees paid in the base asset (plus the one-time amount
// in block 536000), and the outputs in any other asset must equal the fees paid in that asset.
// The base reward is the difference of the node's counter of generated coins between two blocks.
// That reward is also held against the node's own get_block_reward without a block-weight
// penalty: a block whose recorded reward is higher than the formula allows is reported (OVER),
// and blocks with a lower reward (a penalty was applied) are counted.
// Not checked: who receives the outputs (the governance share and its key), and unlock times.
// Prints one line per mismatch and counters. Usage: coinbase <lmdb-dir> <start> <end>
#include <iostream>
#include <map>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "cryptonote_basic/cryptonote_basic_impl.h"
#include "cryptonote_core/cryptonote_tx_utils.h"
#include "cryptonote_config.h"
#include "string_tools.h"
#include "misc_log_ex.h"
using namespace cryptonote;
int main(int argc, char** argv)
{
  if (argc < 4) { std::cerr << "usage: coinbase <lmdb-dir> <start> <end>" << std::endl; return 2; }
  mlog_configure("", false); mlog_set_log_level(0);
  uint64_t a = std::stoull(argv[2]), b = std::stoull(argv[3]);
  BlockchainDB* db = new_db(); db->open(argv[1], DBF_RDONLY);
  if (b > db->height()) b = db->height();
  if (a == 0) a = 1; // the genesis block has no reward rule
  uint64_t blocks = 0, txs = 0, bad = 0, over = 0, penalised = 0;
  std::map<std::string, boost::multiprecision::uint128_t> paid, fees;
  boost::multiprecision::uint128_t reward_total = 0, reserve_total = 0, yield_total = 0;
  for (uint64_t h = a; h < b; ++h)
  {
    block blk; parse_and_validate_block_from_blob(db->get_block_blob_from_height(h), blk);
    const uint8_t hf = blk.major_version;
    const bool mint = h == HF_VERSION_V11_FORK_HEIGHT;
    uint64_t base = db->get_block_already_generated_coins(h) - db->get_block_already_generated_coins(h - 1);
    if (mint) base -= UNAUDITABLE_ZEPH_AMOUNT;
    uint64_t formula = 0;
    get_block_reward(0, 0, db->get_block_already_generated_coins(h - 1), formula, hf);
    if (base > formula) { ++over; std::cout << "OVER\t" << h << "\t" << (unsigned)hf << "\tbase=" << base << "\tformula=" << formula << "\n"; }
    else if (base < formula) ++penalised;
    const uint64_t reserve = hf >= HF_VERSION_DJED ? get_reserve_reward(base, hf) : 0;
    const uint64_t yield = hf >= HF_VERSION_V6 ? get_zeph_yield_reward(base) : 0;
    const std::string coin = hf >= HF_VERSION_AUDIT ? "ZPH" : "ZEPH";
    std::map<std::string, uint64_t> fee, out;
    for (const crypto::hash& txid : blk.tx_hashes)
    {
      blobdata bd; transaction tx;
      if (!db->get_pruned_tx_blob(txid, bd) || !parse_and_validate_tx_base_from_blob(bd, tx)) { std::cout << "ERR\t" << h << "\tparse\n"; ++bad; continue; }
      ++txs;
      std::string t = boost::get<txin_zephyr_key>(tx.vin[0]).asset_type;
      if (hf >= HF_VERSION_AUDIT) { if (t == "ZEPH") t = "ZPH"; else if (t == "ZEPHUSD") t = "ZSD"; else if (t == "ZEPHRSV") t = "ZRS"; else if (t == "ZYIELD") t = "ZYS"; }
      fee[t] += tx.rct_signatures.txnFee;
    }
    for (const auto& o : blk.miner_tx.vout) { std::string t; get_output_asset_type(o, t); out[t] += o.amount; }
    bool ok = out[coin] == base - reserve - yield + fee[coin] + (mint ? UNAUDITABLE_ZEPH_AMOUNT : 0);
    for (const auto& e : out) if (e.first != coin && e.second != fee[e.first]) ok = false;
    for (const auto& e : fee) if (e.first != coin && e.second != out[e.first]) ok = false;
    if (!ok)
    {
      ++bad;
      std::cout << "BAD\t" << h << "\t" << (unsigned)hf << "\tbase=" << base << "\treserve=" << reserve << "\tyield=" << yield;
      for (const auto& e : out) std::cout << "\tout[" << e.first << "]=" << e.second;
      for (const auto& e : fee) std::cout << "\tfee[" << e.first << "]=" << e.second;
      std::cout << "\n";
    }
    ++blocks; reward_total += base; reserve_total += reserve; yield_total += yield;
    for (const auto& e : out) paid[e.first] += e.second;
    for (const auto& e : fee) fees[e.first] += e.second;
  }
  std::cout << "STAT\t" << a << "\t" << b << "\tblocks=" << blocks << "\ttxs=" << txs << "\tbad=" << bad << "\tover_formula=" << over << "\tpenalised=" << penalised << "\tbase_reward=" << reward_total << "\treserve_share=" << reserve_total << "\tyield_share=" << yield_total;
  for (const auto& e : paid) std::cout << "\tpaid[" << e.first << "]=" << e.second;
  for (const auto& e : fees) std::cout << "\tfees[" << e.first << "]=" << e.second;
  std::cout << std::endl;
  db->close(); return 0;
}
