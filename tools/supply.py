#!/usr/bin/env python3
"""Zephyr emission schedule and reconciliation against a node. Approximate: the schedule is
simulated without block-weight penalties, so it runs about 28 ZEPH above the node's record.
The reconciliation from the database (ledger.cpp) is the authoritative one.

Usage: supply.py [http://node:17767]   (a restricted RPC node is enough)
Schedule: cryptonote_basic_impl.cpp get_block_reward (block-weight penalties ignored);
one-time mint: UNAUDITABLE_ZEPH_AMOUNT in block 536000, added to the generated-coins counter.
"""
import json, sys, urllib.request

CAP = 2**64 - 1
GENESIS = 500000 * 10**12
TAIL = 600000000000
MINT_HEIGHT, MINT = 536000, 1921650 * 10**12
AUDITED_ZEPH = 5164647669895047948  # sum of amount_minted of all ZEPH audit transactions, blocks 481500..536000
AUDIT_FEES = 503182236910000  # their fees; the node adds amount_minted + fee to the total (db_lmdb.cpp)

def generated_before(height, with_mint=True):
    gen = GENESIS
    for h in range(1, height):
        reward = max((CAP - gen) >> 20, TAIL)
        if with_mint and h == MINT_HEIGHT:
            reward += MINT
        gen += reward
    return gen

def rpc(node, method, params=None):
    body = {"jsonrpc": "2.0", "id": "0", "method": method}
    if params is not None:
        body["params"] = params
    req = urllib.request.Request(node + "/json_rpc", data=json.dumps(body).encode(), headers={"Content-Type": "application/json"})
    return json.load(urllib.request.urlopen(req, timeout=60))["result"]

def z(x):
    return f"{x / 1e12:,.2f}"

def main():
    node = sys.argv[1] if len(sys.argv) > 1 else "http://127.0.0.1:17767"
    # height and supply must belong to the same block: ask again if a block arrived in between
    for _ in range(10):
        height = rpc(node, "get_reserve_info")["height"]
        tally = {e["currency_label"]: int(e["amount"]) for e in rpc(node, "get_circulating_supply")["supply_tally"]}
        if rpc(node, "get_reserve_info")["height"] == height:
            break
    else:
        sys.exit("the chain kept moving; try again")
    total = tally["ZPH"]
    at_hf8, at_hf11 = generated_before(481500), generated_before(536000)
    after_mint, now = generated_before(536001), generated_before(height)
    total_after_hf11 = total - (now - after_mint)
    reserve_hf8 = total_after_hf11 - (after_mint - at_hf8) - AUDITED_ZEPH - AUDIT_FEES
    print("height", height)
    print("schedule before 481500          ", z(at_hf8))
    print("schedule before 536000          ", z(at_hf11))
    print("total ZPH on chain              ", z(total), " reserve", z(tally["DJED"]))
    print("total right after block 536000  ", z(total_after_hf11))
    print("excess at block 536000          ", z(total_after_hf11 - at_hf11 - (after_mint - at_hf11 - MINT)))
    print("  one-time mint                 ", z(MINT))
    print("  excess in the audited supply  ", z(total - now))
    print("excess now vs original schedule ", z(total - generated_before(height, with_mint=False)))
    print("reserve at hard fork 8 (inferred)", z(reserve_hf8))
    print("max genuine in wallets at HF 8  ", z(at_hf8 - reserve_hf8))

if __name__ == "__main__":
    main()
