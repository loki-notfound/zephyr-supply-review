# zephyr-supply-review

Is the supply of Zephyr Protocol (ZEPH) right? A review of what the public code and the public
chain show: how coins beyond the emission came to exist, what the chain audit of 2025 could and
could not do about them, and whether the number the chain shows today can be relied on.

Nobody asked me to look into it. I did because the question whether the supply is right, and
whether anyone outside the project had checked it, kept coming back in the Zephyr community.
The rule I held to: nothing is claimed that cannot be checked. Every finding names whether it rests on the code, on the chain,
or on what the project itself wrote, and the tools to repeat each check are in this repository.

I hold a small amount of ZEPH and have no other relationship with the project.

- [`REPORT.md`](REPORT.md): the short report. Start here.
- [`REPORT-FULL.md`](REPORT-FULL.md): the full report, with every commit, code line, transaction id,
  quotation and source.

<!-- note:made-with -->
## How this was made

This review was made with AI assistance, on my own initiative and under my direction.

- **Analysis and writing.** I used Claude Fable 5.2, an AI model made by Anthropic, to read the
  source code, to write and run the programs in `tools/`, to compare their results and to draft
  the reports.
- **Sources.** The statements on Discord, on X and on the project's website were transcribed by
  a second AI model (Claude Sonnet 5.5, Anthropic) working in my own browser, on my instructions
  and with read-only use of my accounts. Nothing was posted or changed.
- **Review.** Two other AI models (GPT-6 Astra by OpenAI and Gemini 4 Hydrogen by Google) each
  reviewed a draft independently, with the code and the results at hand. Their findings were
  checked one by one; those that held were corrected in the text or the tools.

I take responsibility for what is published here. I checked the code, the programs, their
results and the quotations myself, some in full and some by sampling, and I compared the key
quotations with the originals. If something is wrong, the error is mine.

The programs and their results are published so that no finding depends on trusting me or any
model: each one can be repeated.
<!-- /note:made-with -->

## In one paragraph

Zephyr's multi-asset code came from Haven Protocol and carried two consensus defects: range
proofs were not verified until hard fork 6, and ring members were not required to match the asset
type of the input until hard fork 7. Three transactions with invalid range proofs and thirty with
ring members of another asset are on the chain. After the audit of 2025 the chain holds 2,067,576
ZEPH more than was emitted, of which 1,921,650 is a one-time mint for an exchange balance that was
not audited. Amounts are hidden, so the chain shows that this happened and when, not who did it
or how much each transaction created. Both defects are closed in the current release. Up to
block 902083 every transaction passes the node's checks on range proof, balance, conversion
amount and ring signatures, no output was spent twice, and no block paid out more than reward
and fees allow. The review establishes what happened and what the supply is; it does not judge
who is to blame.

## What is here

| Path | Contents |
|---|---|
| `tools/` | the programs: a read-only chain scanner, a re-validation of every transaction, a check of every key image and of every block's miner payout, a rebuild of emission and reserve from genesis, a direct test of the range-proof defect, a code comparison, and two scripts that query a node |
| `results/` | what those programs printed on a fully synced chain up to block 902083, one file per program |
| `sources/` | transcripts of the project's Discord channels, of its posts on X and of the web pages that the reports quote, and the release notes |
| `docs/`, `site/` | the reports as a small static site, and the script that renders it from the Markdown files |

## Checking it yourself

You need a fully synced Zephyr node, version 2.3.0, and for the C++ tools a normal build of
`ZephyrProtocol/zephyr` at tag `v2.3.0`.

1. **Same chain.** The block hash at height 902083 must be
   `c2e6d09f5eeb37453fe34a1245fc6bb92a137441375256acf120da8ef1338eba`. A block hash commits to
   every earlier block and transaction, so if it matches, your node holds the chain these results
   were made on.

   ```
   curl -s http://127.0.0.1:17767/json_rpc -H 'Content-Type: application/json' \
     -d '{"jsonrpc":"2.0","id":"0","method":"get_block_header_by_height","params":{"height":902083}}'
   ```

2. **Build the tools.** They compile against the libraries of your Zephyr build and change
   nothing in it.

   ```
   tools/build.sh /path/to/zephyr/build/dir bin
   ```

3. **Run them** on the node's database directory (`~/.zephyr/lmdb` by default). All of them open
   it read-only; stop the daemon first or work on a copy.

   | Command | What it shows | Result to expect |
   |---|---|---|
   | `bin/gaptest LMDB 300000` | the old check accepts a transaction with a corrupted range proof, the new one rejects it | `results/gaptest.txt` |
   | `bin/chainscan LMDB 0 902084` | ring members of another asset, and range proofs that do not verify | 3 `BPP` lines and 49 `RING` lines, as in `results/scan-findings.tsv`; counters per range in `results/scan-stats.tsv` |
   | `bin/ringdetail LMDB < txids` | every foreign ring member of the flagged transactions | `results/ringdetail.tsv` |
   | `bin/refs LMDB 289260 300000 ID1 ID2 ID3` | later rings that reference the outputs of the three transactions with invalid proofs (their ids are the `BPP` lines of the scan) | `results/refs-bpp.tsv` |
   | `bin/ledger LMDB 89300 274662 274663 360000 420760 433000 481500 481717 508000 511000 536000 536001 536010` | coins generated, the reserve and the asset totals, rebuilt from block 0 | `results/ledger.tsv` |
   | `bin/revalidate LMDB 0 902084` | every transaction against the checks the code applies at its height | no failure lines; counters as in `results/revalidate-stats.tsv` (made in ranges of 5,000 blocks) |
   | `bin/keyimages LMDB` | every key image on the chain, and any that occurs twice | one `STAT` line with `duplicates=0`, as in `results/keyimages.tsv` |
   | `bin/coinbase LMDB 0 902084` | the miner transaction of every block: amounts per asset against reward, shares and fees, and the reward against the emission formula | one `STAT` line with `bad=0` and `over_formula=0`; totals per fork period in `results/coinbase.tsv` |
   | `bin/origin LMDB < txids` | the transaction that created each ring member of the flagged inputs | `results/origin.tsv` |
   | `bin/origin LMDB ages < txids` | the oldest and newest ring member of every input of the flagged transactions | `results/ring-ages.tsv` |
   | `bin/blockhash LMDB 289262 360000 433000 481500 536000` | block hashes of the database | `results/blockhash-local.tsv` |

   `txids` holds the ids of the thirty flagged transactions, one per line:
   `grep '^RING' results/scan-findings.tsv | cut -f4 | sort -u > txids`.
   `chainscan` and `revalidate` take a block range, so they can be run in parallel over ranges of
   a few thousand blocks, as the results here were.

4. **The code comparison** needs clones of both repositories and no node:

   ```
   python3 tools/heritage.py /path/to/zephyr 1f3b2f496 /path/to/haven-main 211ddae93
   ```

   The last line reads `1042   2710  38.5 %  total, 85 files`; the full output is in
   `results/heritage.txt`.

5. **Through RPC only**, a restricted node is enough:

   ```
   python3 tools/supply.py http://127.0.0.1:17767
   python3 tools/audit_sum.py workdir http://127.0.0.1:17767
   ```

   `supply.py` prints the reconciliation for the height your node is at; `results/supply.txt` is
   what it printed at height 907724. `audit_sum.py` reads the 15,040 audit transactions, writes
   them to `workdir/audit-transactions.tsv` and prints the sums per asset; both are in
   `results/`. It can be interrupted and resumed.

## Limits

- Transaction amounts are hidden. Nothing here says how many coins a given transaction created.
- The real spend inside a ring cannot be identified.
- Statements of the project are quoted from texts that were read in full and are marked as statements;
  they are not verified unless the report says so.
- The results end at block 902083 (2026-11-07).

## Licence

Programs: BSD 3-Clause ([`LICENSE`](LICENSE)). Reports: CC BY 4.0
([`LICENSE-REPORTS.md`](LICENSE-REPORTS.md)).

Corrections of fact are welcome: open an issue.
