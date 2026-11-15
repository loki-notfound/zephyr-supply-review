// Rebuilds, from a Zephyr LMDB chain and public data only:
//  - coins generated per block as stored by the node, against the emission formula
//  - the ZEPH reserve: reserve share of every block reward, plus ZEPH paid in by mint
//    conversions, minus ZEPH paid out by redeem conversions (amount_burnt / amount_minted)
//  - totals of the other assets from conversions and yield rewards
// Prints snapshots at the given heights. Usage: ledger <lmdb-dir> <height>...
#include <iostream>
#include <set>
#include <boost/multiprecision/cpp_int.hpp>
#include "blockchain_db/blockchain_db.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "cryptonote_core/cryptonote_tx_utils.h"
#include "cryptonote_config.h"
#include "misc_log_ex.h"
using namespace cryptonote;
typedef boost::multiprecision::int128_t i128;
static std::string z(i128 v) { bool n = v < 0; if (n) v = -v; std::string s = (v / 1000000000000).str() + "." ; std::string f = (v % 1000000000000).str(); s += std::string(12 - f.size(), '0') + f; return (n ? "-" : "") + s; }
int main(int argc, char** argv)
{
  if (argc < 2) { std::cerr << "usage: ledger <lmdb-dir> <height>..." << std::endl; return 2; }
  mlog_configure("", false); mlog_set_log_level(0);
  BlockchainDB* db = new_db(); db->open(argv[1], DBF_RDONLY);
  std::set<uint64_t> snaps; for (int i = 2; i < argc; ++i) snaps.insert(std::stoull(argv[i]));
  const uint64_t top = db->height();
  const uint64_t CAP = (uint64_t)(-1), MINT_H = 536000, MINT = 1921650ull * 1000000000000ull;
  i128 reserve = 0, zsd = 0, zrs = 0, zys = 0, yield_rsv = 0, formula_gen = 0, in_zeph = 0, out_zeph = 0, rsv_reward = 0;
  uint64_t prev_gen = 0, penal_blocks = 0, nconv = 0; i128 penal_total = 0;
  for (uint64_t h = 0; h < top; ++h)
  {
    block blk; parse_and_validate_block_from_blob(db->get_block_blob_from_height(h), blk);
    const uint8_t hf = blk.major_version;
    const uint64_t gen = db->get_block_already_generated_coins(h);
    uint64_t base = gen - prev_gen; prev_gen = gen;
    // emission simulated from genesis without block-weight penalties; once one block has had a
    // penalty the simulated counter runs ahead, so most later blocks differ by a tiny amount
    uint64_t fr = h == 0 ? 500000000000000000ull : std::max<uint64_t>((CAP - (uint64_t)formula_gen) >> 20, 600000000000ull);
    if (h == MINT_H) { fr += MINT; }
    formula_gen += fr;
    if (base != fr) { ++penal_blocks; penal_total += (i128)fr - (i128)base; }
    if (h == MINT_H) base -= MINT;
    if (hf >= 3)
    {
      uint64_t rr = get_reserve_reward(base, hf);
      if (hf >= HF_VERSION_V6)
      {
        const uint64_t yz = get_zeph_yield_reward(base);
        rr += yz;
        if (!blk.pricing_record.has_missing_rates(hf) && blk.pricing_record.reserve_ratio > 2 * COIN && blk.pricing_record.reserve_ratio_ma > 2 * COIN)
        { const uint64_t y = zeph_to_zephusd(yz, blk.pricing_record, hf); zsd += y; yield_rsv += y; }
      }
      reserve += rr; rsv_reward += rr;
    }
    for (const crypto::hash& txid : blk.tx_hashes)
    {
      blobdata bd; transaction tx;
      if (!db->get_pruned_tx_blob(txid, bd) || !parse_and_validate_tx_base_from_blob(bd, tx)) { std::cout << "ERR tx " << h << "\n"; continue; }
      if (!tx.amount_burnt && !tx.amount_minted) continue;
      std::string src, dst; transaction_type t;
      if (!get_tx_asset_types(tx, txid, src, dst, false) || !get_tx_type(src, dst, t)) { std::cout << "ERR type " << h << "\n"; continue; }
      switch (t)
      {
        case transaction_type::MINT_STABLE:    reserve += tx.amount_burnt; in_zeph += tx.amount_burnt; zsd += tx.amount_minted; ++nconv; break;
        case transaction_type::REDEEM_STABLE:  zsd -= tx.amount_burnt; reserve -= tx.amount_minted; out_zeph += tx.amount_minted; ++nconv; break;
        case transaction_type::MINT_RESERVE:   reserve += tx.amount_burnt; in_zeph += tx.amount_burnt; zrs += tx.amount_minted; ++nconv; break;
        case transaction_type::REDEEM_RESERVE: zrs -= tx.amount_burnt; reserve -= tx.amount_minted; out_zeph += tx.amount_minted; ++nconv; break;
        case transaction_type::MINT_YIELD:     yield_rsv += tx.amount_burnt; zys += tx.amount_minted; ++nconv; break;
        case transaction_type::REDEEM_YIELD:   zys -= tx.amount_burnt; yield_rsv -= tx.amount_minted; ++nconv; break;
        default: break; // audit transactions carry amounts but move nothing between assets
      }
    }
    if (snaps.count(h + 1) || h + 1 == top)
      std::cout << "AFTER_BLOCK\t" << h << "\thf=" << (unsigned)hf << "\tgenerated_db=" << z(gen) << "\tgenerated_formula=" << z(formula_gen)
                << "\tblocks_differing_from_simulation=" << penal_blocks << "\tsimulation_minus_recorded=" << z(penal_total)
                << "\treserve=" << z(reserve) << "\treserve_rewards=" << z(rsv_reward) << "\tzeph_in=" << z(in_zeph) << "\tzeph_out=" << z(out_zeph)
                << "\tZSD=" << z(zsd) << "\tyield_reserve_zsd=" << z(yield_rsv) << "\tZRS=" << z(zrs) << "\tZYS=" << z(zys) << "\tconversions=" << nconv << std::endl;
  }
  std::cout << "NODE_TALLY at height " << top << ":";
  for (const auto& p : db->get_circulating_supply()) std::cout << " " << p.first << "=" << p.second;
  std::cout << std::endl;
  db->close(); return 0;
}
