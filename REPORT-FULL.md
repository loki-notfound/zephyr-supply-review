# Is Zephyr's supply right? Full report

*Version 1.0, 2026-11-14. The supply incident of 2023 to 2025, its cause, the audit and the chain
since. An independent review, made on the author's own initiative for the Zephyr community.*

**Rule of this report: nothing is claimed that cannot be checked.** Every finding names its
basis and, where possible, the command or file that reproduces it (§10, and the evidence
register in §1a). Inferences are labelled as inferences. What we could not establish is listed
in §1b.

This report separates three kinds of statement and marks each:

- **[chain]** read from a full copy of the Zephyr main chain, or from a synced node, on 2026-11-14;
- **[code]** read from the public source repositories, with commit and line;
- **[stated]** said by the project, an exchange or a mining service, quoted from a text we read
  in full: release notes, notices and pages on the web, the project's Discord, read from
  transcripts of four of its channels and searches of the rest and cited as [stated, Discord]
  with the date, and the project's posts on X, read from transcripts of selected posts and of
  searches of the account and cited as [stated, X]. All of them are listed in §11.

Nothing in this report rests on a text that was not read in full.

Where the evidence allows more than one explanation, all are given. Times are in UTC where the
source gives one; a commit is dated by its author date in UTC, and a web page that shows only a
date keeps that date (§11).

## 1. The question and the answer

**The question.** Is the supply of ZEPH right? Taken apart, that is five questions that public
data can answer. This review establishes what happened and what the supply is; it does not judge
who is to blame.

1. **Can the number the chain shows be relied on?** Yes, as a count, up to block 902083
   (2026-11-07), where our copy of the chain ends. Up to that block
   every transaction passes the checks that keep a transaction from creating coins (range proof
   and balance, conversion amount, ring signatures), no key image occurs twice, and no block pays
   out more than the reward formula and the fees allow (§5.4). The totals of all four assets
   agree with the coins generated and the public conversions at both points where they can be
   compared, right after the audit and at that last block (§6.3, §6.3a). What such checks cannot exclude is a flaw in the rules themselves that nobody knows of
   (question 5).
2. **What went wrong before the audit?** Two defects in consensus code that Zephyr took over
   from Haven Protocol (§3, §4). Range proofs were not verified until 2024-10-13, so a
   transaction could create coins; ring members could be of another asset until 2025-01-23, so
   one asset could be spent as another. Both were used: three transactions with invalid range
   proofs and fourteen that spent a stable-coin output as ZEPH are on the chain (§5). Both
   defects are closed.
3. **What could the audit do, and what could it not?** It made the supply countable for the
   first time: every holder moved each asset once with the amount in the open (§6.2). It could
   not separate coins that had been emitted from coins that had not, because amounts and
   histories are hidden and the chain offers no way to tell them apart. After the audit the
   supply table held 2,067,576 ZEPH more than had been emitted: 1,921,650 minted once for an
   exchange balance that was not audited in time, and 145,926 inside the audited supply (§6.3,
   §6.4). The project announced the mint and how it is offset: it was booked as emission, so
   block rewards are about 18 % lower for as long as they follow the emission curve, about ten
   more years. By a projection that offsets all but about 124,000 ZEPH of the mint before the
   fixed tail reward takes over. The 145,926 are not offset.
4. **What stays unknown?** How many coins each defect produced, who made the transactions, and
   whether the unaudited exchange balance consisted of emitted coins (§1b). On the accounting of
   §6.5, defect B can account for at most about 262,688 ZEPH of the excess. For the rest the project's own report
   says: "We were not able to determine if or how much net chain-wide inflation originated from
   this vector" [stated, X, 2025-09-06].
5. **Has anyone outside the project reviewed the node's code?** Not that we could find. In 2025
   the project said no third-party review was planned; the audit by the firm Sherlock that it
   commissioned in 2026 covers its EVM bridge, by the scope it published, not the node; in the
   project's words, "Scoped to the EVM bridge only ... Not the L1" (§8).
   This review is not a code audit either: it checks the chain against the rules as they are
   written.

**In short.** Up to the block we checked, the number the chain shows is a sound count. At the
end of the audit (block 536000) it was 2,067,576 ZEPH higher than ordinary emission to that
block: 1,921,650 that the project minted for an exchange balance that was not audited, for the
larger part offset by lower block rewards in the years after, and 145,926 among the audited
coins, which the audit had no way to tell from the rest.

**The findings in summary.**

1. Zephyr's multi-asset code was taken from Haven Protocol in May 2023 [code]. Two defects in the
   consensus rules came with it or were made in adapting it; the project wrote in August 2025
   that "both Zephyr exploits were inherited from "audited" Haven code portions" [stated, X]:
   - **A. Range proofs of transactions were not verified** from the launch (2023-05-29) until hard
     fork 6 (block 360000, 2024-10-13). A range proof is what prevents a transaction from
     creating coins. Haven's fix of 2024-12-11, titled "fix range proof exploit", adds the
     missing verification to the same function. [code, executed test]
   - **B. Ring members were not required to have the asset type the input claimed** until hard
     fork 7 (block 433000, 2025-01-23). An output of one asset could be spent as another asset
     with the same amount ("asset morphing"). [code]
2. Transactions that only these defects made possible are on the main chain [chain]:
   - A: exactly **three** transactions with invalid range proofs were accepted, in blocks 289260
     and 289262 (2024-07-07, six minutes apart). No other transaction up to block 902083 (§5.4) has
     an invalid proof. The project itself wrote on 2025-01-23 of "three suspect transactions that
     fail the updated range proofs code" [stated, Discord].
   - B: **14** transactions in which an input claiming ZEPH has a ring made up entirely of
     stable-coin (ZSD) outputs, from 2023-10-29 to 2024-09-30, so that a stable-coin output was
     necessarily spent as ZEPH; and **16** further transactions (35 inputs) with one or two ring
     members of another asset, which admit two readings, a decoy or a real spend; the rings do
     not decide between them (§5.3). Together that is 30
     transactions, the number the project gave on 2025-01-23 for "transactions ... with
     cross-asset ring members" [stated, Discord].
   The amounts are hidden by design. The chain does not show how many coins these transactions
   created, nor who made them.
3. The project closed B with a hotfix (v2.0.2), then ran a chain-wide audit (hard forks 8 to 11,
   2025-03-31 to 2025-06-15) in which every holder had to move each asset once with a public
   amount. Coins not moved became invalid.
4. Result, reconciled by us against the coins the node recorded as generated [chain, code]: after the audit the
   node's supply table records **9,749,946 ZEPH** where **7,682,370** had been emitted:
   **2,067,576 more than was emitted**. The project's own estimate of 2025-06-11 is +2,056,803
   [stated, Discord].
   Of that, **1,921,650 ZEPH** is a one-time mint in block 536000 which the release notes
   and the project's announcements attribute to an exchange balance that was not audited (MEXC)
   [stated]; **145,926 ZEPH** is
   excess inside the audited supply itself (audited coins plus the recorded reserve). Coins
   that exist beyond what was generated were not emitted; genuine coins that were never audited reduce
   the visible excess, so the amount of non-emitted coins honoured is at least this figure.
   The reserve it depends on was rebuilt from genesis and agrees with the node's record to within
   0.001 ZEPH (§6.3a).
5. By the project's estimate the other assets came in 256,634 units short in total (ZSD, ZRS,
   ZYS); rebuilt from the chain, 262,688 units are missing (§6.3a). Defect B turns one unit of
   another asset into one ZEPH, so it cannot account for more than about that much of the ZEPH
   excess; the project's own estimate for it is about 200,000.
   At least about 1.8 million of the excess is therefore not explained by defect B. Defect A is
   the only other known cause, but nothing shows how much the three transactions created (§6.5).
   The project's post-audit report says the same of that cause: "We were not able to determine if
   or how much net chain-wide inflation originated from this vector" [stated, X, 2025-09-06].
6. The one-time mint was booked as emission. The parameter that sets the emission curve is
   unchanged, so block rewards are about 18 % lower than on the original schedule until the
   fixed tail reward is reached [code, chain]. By a projection without penalties that offsets
   about 1.8 million ZEPH of the mint; about 124,000 of it, and the 145,926, stay above the
   original schedule (§6.4). The lower rewards are borne by miners, the reserve share and the
   yield share.
7. Both defects are closed in the current release (v2.3.0, published 2025-06-11). The public
   repository's only branch still ends at the commit of that release (checked 2026-11-14) [code].

## 1a. Evidence register

| # | Finding | Basis | Check it yourself |
|---|---|---|---|
| 1 | 38 % of the non-Monero lines of Zephyr's first commit occur verbatim in Haven (1,042 of 2,710) | code comparison | `heritage.py`; §2; both repositories are public |
| 2 | The pre-fork-6 function does not verify Bulletproof+ proofs | code, and an executed test | `gaptest.cpp`; the quoted lines in §4.1 |
| 3 | The same omission is in Haven's code of the fork date and first appears there on 2023-04-25 | code | Haven commits `211ddae93`, `141d4a359`, tag v3.0.8 |
| 4 | Until fork 7 the ring-member asset check compared the input's claim with itself | code | the quoted lines in §4.2 |
| 5 | Three accepted transactions have invalid range proofs; none other in blocks 0 to 902083 | chain scan | `chainscan.cpp`; ids in §5.1; present on a second node |
| 6 | Fourteen ZEPH inputs have rings consisting only of ZSD outputs | chain scan; two of them re-checked output by output on a second node | `chainscan.cpp`, `ringdetail.tsv`; ids in §5.2 |
| 7 | Thirty-five further inputs have one or two foreign ring members; in 32 of them no ring member is newer than a few blocks after a hard fork, and in ten of the transactions that holds for every input | chain scan | `ringdetail.tsv`, `origin.tsv`, `ring-ages.tsv`; §5.3 |
| 8 | 15,040 audit transactions; amounts per asset | node RPC | `audit_sum.py` |
| 9 | Emission schedule; one-time mint of 1,921,650 ZEPH in block 536000, booked as emission | code, block 536000, block headers | `supply.py`; §6.4 |
| 10 | 2,067,576 ZEPH more recorded than emitted at block 536000 | derived from 8 and 9, the node's supply table and its record of coins generated | `ledger.cpp`, `supply.py` |
| 11 | The reserve counter agrees with a rebuild from genesis to 0.001 ZEPH at each of five points of comparison; 262,688 units of the other assets are missing after the audit | rebuilt from genesis from public conversion amounts | `ledger.cpp`, `ledger.tsv` |
| 12 | The mint is for MEXC's unaudited balance; why it was not audited; the balance was checked from the wallet seed and paid out | the project's statements on Discord and in the release notes; not verifiable on chain | §6.4 |
| 13 | Both defects are closed in v2.3.0; the public repository has no commit after that release | code | §4, §9 |
| 14 | 30 transactions with cross-asset ring members; three that fail the range-proof check | the project's Discord post of 2025-01-23, independently of our scan | §5, §11 |
| 15 | The project's own supply figures at height 536010 give the same excess (145,925.75 by both routes) and the same reserve | the project's Discord announcement of hard fork 11; the node's record of coins generated | §6.3, §6.3a |
| 16 | Every transaction in blocks 0 to 902083 passes the released code's semantics, minted-amount and ring-signature checks for its height; no key image occurs twice | re-validation of the whole chain | `revalidate.cpp`, `revalidate-stats.tsv`, `keyimages.cpp`, `keyimages.tsv`; §5.4 |
| 17 | Both defects were inherited from Haven code; no third-party review of the code is planned | the project's AMA article on X of 2025-08-25 | §3, §8, §11 |
| 18 | Every block's miner transaction pays out, per asset, exactly reward minus shares plus fees, and no block's reward is above the emission formula | re-check of all 902,083 blocks after genesis | `coinbase.cpp`, `coinbase.tsv`; §5.4 |

## 1b. What this report does not establish

- How many coins the three invalid-proof transactions or the morphing transactions created.
- Who made them, and whether one party or several.
- Which defect accounts for how much of the excess.
- Whether the coins in the unaudited exchange balance had been emitted or not. (Why they were
  not audited, how the balance was checked and that it was paid out are stated by the project,
  §6.4; we cannot verify those statements on chain.)
- Whether the sixteen partial-ring transactions are abuse (§5.3).
- The intent behind the correction of defect A in v2.0.0. The project says it was a by-product
  of refactoring and that the earlier check was an oversight (§4.1); the code is consistent with
  that.
- How many genuine coins were lost because their holders did not audit them.
- That no third defect existed. Every transaction passes the checks of the released code that we
  re-ran (§5.4), but a defect in those checks themselves, as A and B were, would not show that
  way, and not every consensus rule was re-run (§2).
- What the project said on Medium, or in the parts of its website, its Discord and its X account
  we did not read (§11). None of that is used here.

## 2. Method and limits

<!-- note:made-with -->
This review was made with AI assistance (Claude Fable 5.2, Anthropic); the
[README](README.md#how-this-was-made) says how. The programs and their results are published so
that no finding depends on trusting the author or the model.
<!-- /note:made-with -->

**Sources.** The repositories `ZephyrProtocol/zephyr` (tag v2.3.0, `67c5f53b`),
`haven-protocol-org/haven-main` (tip of the main branch `70d01079`, 2024-12-11), the release notes of all
Zephyr releases (GitHub API), Zephyr's whitepaper, a full Zephyr chain database synced from
genesis, and a second, separately synced node for RPC queries and cross-checks.

The node that synced the database was built from the zephyr-backports fork, tag `v2.3.0-sec.2`
(https://github.com/zephyr-backports/zephyr): v2.3.0 with security fixes taken over from Monero
(memory safety, denial of service, dependency updates). These fixes do not change which blocks or
transactions are valid. That the database holds the
same chain as the original software was checked directly: its block hashes at heights 289262,
360000, 433000, 481500, 536000 and at its last block, 902083
(`c2e6d09f5eeb37453fe34a1245fc6bb92a137441375256acf120da8ef1338eba`), are identical to those of a
separately synced node running the official v2.3.0 binary. A block hash commits to every earlier
block and every transaction in them, so the two chains are identical up to that block. The scan
programs were built against the libraries of an unmodified v2.3.0 build. Anyone can repeat the
scan on a database synced with the stock release and compare the hash above first.

**Code comparison.** A script (`heritage.py`) takes the lines that Zephyr's first own commit adds
to the C and C++ sources, collapses whitespace, leaves out lines shorter than 25 characters,
comment lines and lines that already occur in the commit's parent (Monero), and counts how many of
the rest occur anywhere in Haven's sources of the same day (`211ddae93`).

**Executed test of defect A.** A program linked against the libraries built from v2.3.0 takes a
real transfer from block 300002, calls both verification functions, flips one bit in the range
proof and calls them again:

```
height 300002 hf 5 tx 4f9693d5b598435f33f00b15ba62c3a433caae2c9108dd73d994bf6161504e89 outputs 2
original proof : bulletproof_plus_VERIFY=1  verRctSemanticsSimple(pre-HF6 path)=1  verRctSemanticsZeph(HF6+ path)=1
corrupted proof: bulletproof_plus_VERIFY=0  verRctSemanticsSimple(pre-HF6 path)=1  verRctSemanticsZeph(HF6+ path)=0
```

**Chain scan.** A read-only program walks every block, parses every non-coinbase transaction
and (1) looks up, for each ring member of each input, the asset type stored with that output and
compares it with the asset type the input claims; (2) verifies the Bulletproof+ range proofs with
the library's own verifier. Below block 435000, the scan's first range boundary after hard
fork 7 (block 433000): 837,680 transactions,
4,842,712 inputs, 77,483,392 ring members, 837,680 proofs. The rest of the chain is scanned as a
control (§5.4). The asset type of a ring member is the one the node stored when the output was
created; for two of the fourteen full-ring transactions all sixteen members were looked up again
on the second node through the transactions that created them, with the same result.

**Re-validation.** A second read-only program re-runs, for every non-coinbase transaction, the
checks the released code applies at the transaction's height: the semantics check
(`verRctSemanticsSimple` below hard fork 6, `verRctSemanticsZeph` from it; for conversions with
the pricing record of the block the transaction names), the minted amount of conversions below
hard fork 6 (`validateMintedAmount`), and the ring signatures and commitments
(`verRctNonSemanticsSimple`) with the rings read from the chain. A transaction that fails the
semantics check under its block's fork version is tried once more under the previous version and
would be reported with a note, and so would a minted amount that passes only under the previous
version; no such case occurred. Key images and miner transactions have their own checks, below.
Not re-checked at all: unlock times and the minimum age of spent outputs, the validity of the
pricing record a conversion names, the reserve-ratio conditions under which conversions are
allowed, and the restriction of transaction types during the audit forks.

**Key images.** A further program collects the key image of every input on the chain and reports
any that occurs twice. A key image is derived from the output that is spent, whatever asset the
input claims, so a second spend of one output would show up.

**Miner payouts.** A third read-only program applies the rule of the node's
`validate_miner_transaction` to the miner transaction of every block after genesis: its outputs
in the base coin must equal the block's reward minus the reserve share and the yield share, plus
the fees paid in the base coin (plus the one-time amount in block 536000), and its outputs in any
other asset must equal the fees paid in that asset. The reward of a block is taken from the
node's counter of coins generated and is held against the node's reward formula without a
block-weight penalty, which is the most a block may be given. The program checks amounts per
asset; it does not check who receives them (the governance share and its key) or unlock times.

**Origin of ring members.** For the 49 flagged inputs a fourth program looks up, for every ring
member, the transaction that created it, its kind and its public amounts (§5.2, §5.3).

**Supply.** The emission schedule was recomputed from `get_block_reward` (cap 2^64 − 1 atomic
units, genesis reward 500,000 ZEPH, then (cap − generated) >> 20 per block), and compared with
the coins generated per block as the node stored them (§6.3a). The reconciliation of §6.3 uses
the database (to block 902083); the one later figure in §6.4 comes from a node's
`get_circulating_supply` at height 907724 (`supply.txt`). All 15,040 transactions of blocks
481500 to 535999 were read and their public `amount_burnt` / `amount_minted` summed per asset
(`audit-transactions.tsv`, `audit-summary.txt`).

**Limits.**
- Amounts in transactions are hidden. The scan shows *that* a defect was used and *when*, never
  *how much*.
- The real spend inside a ring cannot be identified. Where only some ring members have a foreign
  asset type, neither a spend through defect B nor a harmless decoy is proven (§5.3).
- A scan can only find forms of abuse it looks for. The chain scan checks the two known defects.
  The re-validation shows that every transaction passes the checks as they are coded, which a
  transaction exploiting a defect in the checks themselves, as A and B were, passes as well.
  Neither is a proof that no third defect existed.
- Of the project's posts on X and of its website we read what is listed in §11. Its articles on
  Medium were not read and are not used.
- No historical supply table is stored. The reserve and the asset totals at earlier heights were
  rebuilt from genesis (§6.3a) and agree with every figure they can be compared with.

## 3. Where the code comes from [code]

| Layer | Origin |
|---|---|
| Base | Monero master up to 2024-03-12 (replayed by Zephyr; last shared commit `a2e8d1d42`, 2023-04-26) |
| Multi-asset mechanics | Haven Protocol, `haven-main` as of 2023-05-26 (v3.1.9e, `211ddae93`), a development branch rebased onto Monero v0.18.1.2; Haven's own hard-fork table has its next main-net forks on 2023-08-29 |
| Stable-coin rules | Zephyr's own code after the Djed design (reserve ratio 4.0 to 8.0, spot and moving-average price) |

Of the substantive, non-Monero lines that Zephyr's first own commit
[`1f3b2f496`](https://github.com/ZephyrProtocol/zephyr/commit/1f3b2f496cc4fe195e45112e4147e59de85fc963)
("init Zephyr code", 2023-05-26) adds to the sources, 38 % occur verbatim in Haven (1,042 of
2,710 lines in 85 files; `rctSigs.cpp` 54 %, `blockchain.cpp` 41 %, `db_lmdb.cpp` 83 %;
`heritage.py`); renamed identifiers do not
match, so the real share is higher. Five files still carry Haven's copyright notice. Zephyr went
live three days later (genesis 2023-05-29).

The project has said so itself. Asked in its community AMA whether there would be a third-party
review of the code, it answered ([article of 2025-08-25](https://x.com/zephyr_org/status/1960023215444488459))
[stated, X]: "Frankly, we did place trust the Haven audits and that turned out to be a massive
mistake for us; both Zephyr exploits were inherited from "audited" Haven code portions - No
Zephyr specific implementations/code (Djed & ZYS etc) has ever been exploited". For the two
defects of §4 the code shows the same.

## 4. The two defects

### 4.1 Defect A: range proofs not verified (launch to hard fork 6)

A range proof shows that a hidden amount is not negative. Without it, a transaction can have one
output of a huge amount and one of a "negative" amount that cancel in the balance check.

Zephyr's verification function, as launched
([`1f3b2f496`, `rctSigs.cpp` L1645-L1652](https://github.com/ZephyrProtocol/zephyr/blob/1f3b2f496cc4fe195e45112e4147e59de85fc963/src/ringct/rctSigs.cpp#L1645-L1652))
and unchanged in v2.3.0 for blocks below hard fork 6
([v2.3.0, L1588-L1590 and L1763-L1770](https://github.com/ZephyrProtocol/zephyr/blob/v2.3.0/src/ringct/rctSigs.cpp#L1763-L1770)):

```cpp
const bool bulletproof_plus = is_rct_bulletproof_plus(rv.type);
CHECK_AND_ASSERT_MES(bulletproof_plus, false, "Only bulletproofs supported for Stablero");
CHECK_AND_ASSERT_MES(rv.outPk.size() == n_bulletproof_plus_amounts(rv.p.bulletproofs_plus), false, "Mismatched sizes of outPk and bulletproofs_plus");
...
for (size_t i = 0; i < rv.p.bulletproofs.size(); i++)
  proofs.push_back(&rv.p.bulletproofs[i]);

if (!proofs.empty() && !verBulletproof(proofs))
```

The function requires the Bulletproof+ type, whose proofs are in `rv.p.bulletproofs_plus`, and
then verifies the other vector, `rv.p.bulletproofs`, which is empty for such a transaction. It was
the only semantics check on all three paths (mempool, block, pool re-check).

**Origin.** Haven's function of the same name and structure at Zephyr's fork date
([`211ddae93`, L1667-L1668 and L1983](https://github.com/haven-protocol-org/haven-main/blob/211ddae93e545d7bea4c0985c0ce457d80f176ab/src/ringct/rctSigs.cpp#L1983)).
In Haven's history the function first accepts Bulletproof+ types, while still verifying only the
old vector, in commit `141d4a359ce50001b35df83aa2a26314af4b73cf` (2023-04-25, the rebase work onto
Monero v0.18); before the rebase (tag v3.0.8) the function required the old Bulletproof type and
verified it, which was correct. The omission is therefore present in Haven's rebase work one month before Zephyr's first commit
contains it. From which block Haven's own main net ran this function was not established by us.

**Fix in Zephyr.** Commit
[`427277f7d`](https://github.com/ZephyrProtocol/zephyr/commit/427277f7df11ead579f4f3a50ee9f0bee3d218db)
(2024-09-29, "Add Zephyr Yield Shares (ZYS) for version 2.0.0 (hardfork v6)") adds a new function
`verRctSemanticsZeph`, used from hard fork 6, which verifies the right vector
([v2.3.0, L1935-L1943](https://github.com/ZephyrProtocol/zephyr/blob/v2.3.0/src/ringct/rctSigs.cpp#L1935-L1943)).
The release notes of v2.0.0 describe the yield feature and do not mention range proofs or a
security fix; neither does the announcement of the release on the project's website
([2024-09-30](https://zephyrprotocol.com/blog/2024-09-30-Zephyr-Protocol-v2-0-0-ZSD-Yield-Hardfork-Annoucement)). The project's own account, given in its Discord on 2025-01-13
([message](https://discord.com/channels/1112605073948819559/1112605075416817686/1328217871263731722)) [stated, Discord]:

> The 3 transactions from 6 months ago (July 7 2024) stand out because they fail an updated range
> proofs check relating to bulletproofs/bulletproofs+ that was introduced in Zephyr v2.0.0 as we
> refactored that section of the code with our Zephyr specific requirements due to ZYS
> implementation based on Moneros current implementation. [...] The way the check was implemented
> prior to Zephyr v2.0.0 was incorrect, an oversight on our part. Because these transactions fail
> the new corrected check it's implies that an exploit attempt was made.

This agrees with the code on every point we can test: the date, the incorrect check before
v2.0.0, and the corrected check in the function added for the yield feature. By this account the
correction was a by-product of refactoring, not a fix made in the knowledge of the defect, and
the three transactions were noticed later (the first announcement is of 2024-12-29, after Haven's
exploit; §6.1a). Intent cannot be verified from code, but nothing we found contradicts it.

Two further statements on X [stated, X]. On 2025-01-06
([post](https://x.com/zephyr_org/status/1876209588460249297)): "In our current version, it is no longer possible to
submit transactions using the same method that allowed those suspicious transactions to be
accepted." In the post-audit report of 2025-09-06
([article](https://x.com/zephyr_org/status/1964371016303407282)): "we overhauled our codebase and refactored our
range-proofs implementation. When these new stricter checks were applied to our historical chain
during the audit investigation, the three pre-existing transactions did not validate under the
new rules." The report speaks of "stricter" checks; it does not repeat the statement of
2025-01-13 that the earlier check was incorrect.

**Fix in Haven.** Commit
[`6f361aadf`](https://github.com/haven-protocol-org/haven-main/commit/6f361aadf3f6b4f0307dc79bb2f2b7a938bce22a)
("fix range proof exploit", 2024-12-11), ten weeks after Zephyr's.

### 4.2 Defect B: ring members of another asset type (hard fork 3 to hard fork 7)

Each input names an asset type and a ring of earlier outputs, one of which is really spent. The
check that every ring member has the input's asset type
([v2.3.0, `blockchain.cpp` L4133-L4136](https://github.com/ZephyrProtocol/zephyr/blob/v2.3.0/src/cryptonote_core/blockchain.cpp#L4133-L4136))
was fed, until hard fork 7, with the asset type claimed by the input itself, so it compared the
claim with the claim. The fix, commit
[`13a7e9f94`](https://github.com/ZephyrProtocol/zephyr/commit/13a7e9f94ba96a1f322986634ac5115f603e7458)
(dated 2025-02-03, "Hardfork v7, v2.0.2"; [v2.3.0, L233-L245](https://github.com/ZephyrProtocol/zephyr/blob/v2.3.0/src/cryptonote_core/blockchain.cpp#L233-L245)):

```cpp
if (hf_version >= HF_VERSION_V7) {
  if (!vis.handle_output(output_index.unlock_time, output_index.asset_type, output_index.pubkey, output_index.commitment))
  ...
} else {
  if (!vis.handle_output(output_index.unlock_time, tx_in_to_key.asset_type, output_index.pubkey, output_index.commitment))
```

The hotfix went out as binaries on 2025-01-21 and the fork was on 2025-01-23; the source commit is
dated eleven days after the fork, and the tag `v2.0.2` in the repository points to a commit of
2024-10-14 that does not contain it. The project had said the code "will be published shortly"
(2025-01-23). Publishing the source of a fix for a live defect only after the fork is common
practice. The tag was not moved afterwards, so anyone checking `v2.0.2` in the repository will
not find the fix there.

The project's description after the fork (2025-01-23) [stated, Discord]: "an issue that allowed
transactions to spend outputs of a different asset type without proper enforcement. In short,
this bug made it possible for one asset (e.g., ZSD, ZRS, ZYS) to be spent as if it were another
asset (e.g., ZEPH), with wallet code modifications." It credits "the anonymous developer and
contributor who first discovered and alerted us to this issue".

The defect matters from hard fork 3 (block 89300, 2023-10-01), when a second asset first
existed. The same call is in Haven's code at the fork date and at the tip of its main branch; whether Haven
enforced the rule by another route was not established by us.

What the defect permits: an output holding *n* units of the stable coin can be spent as *n*
ZEPH. In the conversions of §5.2 one ZEPH bought between 1.5 and 45 stable coins, so such a
spend was worth that multiple, and the proceeds could be converted back into the stable coin
through the protocol. Whether and how often this was repeated is not visible.

## 5. What the chain shows [chain]

### 5.1 Three transactions with invalid range proofs

| Block | Time (UTC) | Transaction | Shape |
|---|---|---|---|
| 289260 | 2024-07-07 00:53 | `b17ef3d2e65ab980c8bd6cbe3210a8b3c9e417fb9621209380f7c1f756fcb2ed` | 1 input, 2 outputs, ZEPH |
| 289262 | 2024-07-07 00:59 | `4e7ea2cc5484508bd17ff43f3fb8fbd69a22062f24a869794c513d89aee3eb6d` | 1 input, 2 outputs, ZEPH |
| 289262 | 2024-07-07 00:59 | `d3abcf2bc0a173d6d8af19d88b2d7d610385abf258590d24ca82fc586017c6dd` | 1 input, 2 outputs, ZEPH |

These are the only transactions in blocks 0 to 902083 whose range proof fails. Number and date agree
with the project's Discord announcement of 2025-01-06, which speaks of "the three suspect
transactions from 6 months ago" and a week later gives the date itself, "July 7 2024" [stated,
Discord]. On 2025-01-23 the project wrote that they "fail the updated range
proofs code" [stated, Discord], which is what the scan finds. An output of each transaction is referenced as one of the two newest ring members of another
transaction within 12 to 36 blocks; beyond that the trail cannot be followed. How much was created
is not visible; three outputs can carry any amount.

### 5.2 Fourteen transactions whose ZEPH input has a ring of stable-coin outputs

In each, the single input claims ZEPH and all 16 ring members are ZEPHUSD (ZSD) outputs. A wallet
that follows the protocol builds rings from outputs of the asset it spends, so such a ring means
a stable-coin output was spent as ZEPH. Three are plain transfers; eleven are conversions back
into the stable coin with a small public amount, the rest returning as ZEPH change of hidden size.

| Block | Date | Transaction | Public conversion |
|---|---|---|---|
| 109160 | 2023-10-29 | `2eefb4f30e60d447d35311f37aae53822b94559ca4baf53154d5321d67657944` | transfer |
| 109771 | 2023-10-30 | `a6576052a91d5d0ec7b0ac7efae2ad4d04a8c84e2a6c197336b69157288ae919` | transfer |
| 110768 | 2023-11-01 | `3d6192dab6d83f3f561128d4911fb434c14a859743a17d40f0176a31a8e2b4a9` | transfer |
| 125921 | 2023-11-22 | `6ade0f71eec20e1d5e79051810cfe1235932a4bb5d5fc9136052427f0ba38e0d` | 1 ZEPH to 41.86 ZSD |
| 125958 | 2023-11-22 | `4cabb05a1165d17a7d7428d2fcb28c7bbf844065f9a8c5e230716abc876b2a58` | 1 ZEPH to 42.41 ZSD |
| 126176 | 2023-11-23 | `f8463dfd612b1a3d64ad8e885d9540ce15457e30df4bd52afe25d825862bde53` | 0.2 ZEPH to 9.04 ZSD |
| 131262 | 2023-11-30 | `aad269a5eb651d186dec8ce6f3123197a8bfd54e24006793a7341ddd12a8003d` | 30 ZEPH to 857.48 ZSD |
| 184308 | 2024-02-12 | `22bda20ad1a9f999759264cf3d87d75fa223a28c48d29e4351448b30f73a7784` | 0.5 ZEPH to 8.84 ZSD |
| 231091 | 2024-04-17 | `8130dd967fcc1a244357e3a932309e6e08adecb98b391caa1da27cf6eca6d316` | 2 ZEPH to 19.60 ZSD |
| 277611 | 2024-06-20 | `9ddfc42c903d4c73cb05c5096479119fab01b7bd5793bc7e848e322e406b6915` | 0.5 ZEPH to 1.32 ZSD |
| 282759 | 2024-06-28 | `29dde085c95a9677ae1dfb93d5ec3ad6a24b1ba4e4209a63e39a57cee78faf62` | 1 ZEPH to 3.83 ZSD |
| 294167 | 2024-07-13 | `6e5ea6d0695e541a7dacecdc7d6eb4d60f6827014f29840b5b59c4811cf957ea` | 10 ZEPH to 27.99 ZSD |
| 295137 | 2024-07-15 | `78ebdc8427f8c44157ebe6ccc297a7e26802159a63214836de069850c6853877` | 2 ZEPH to 6.04 ZSD |
| 350992 | 2024-09-30 | `1f3769a36d75a502a2774cae759ab25b460c151294e443d986a58a25c528feed` | 3 ZEPH to 4.56 ZSD |

That a stable-coin output was spent as ZEPH in each of the fourteen follows from the rings.
Why is not on the chain. Explanations: deliberate use of defect B with a modified wallet (the
project's account: "with wallet code modifications", "most of which morphed ZSD → ZEPH"
[stated, Discord, 2025-01-23]);
or a defect in some wallet software that mislabelled an input. We did not examine wallet
releases for such a defect. The first such transaction is four weeks after the stable coin went
live, the last eleven months later.

The project's own count and estimate [stated, Discord, 2025-01-23]: "30 transactions have been
identified with cross-asset ring members"; "in 10 out of these 30 transactions (due to specific
patterns), roughly 95,000 ZSD were morphed into ZEPH. For the other 20 transactions, we do not
have a way to determine the total amount morphed"; and "we estimate ~200,000 ZEPH in 'inflation'
(and an equal amount of ZSD + ZRS total number in 'deflation') from this exploit—this is our best
educated guess and not a confirmed figure". Our scan finds 30 transactions as well (14 here, 16
in §5.3). We could not reproduce the 95,000 figure: the public amounts in these transactions are
the small conversions in the table above. The post-audit report of 2025-09-06 repeats it and adds
a bound for the rest: "Pattern analysis inferred ~95k ZSD morphed in 10 of those transactions and
an upper bound in the low‑hundreds‑of‑thousands of ZSD/ZRS across the remainder" [stated, X].

We looked for a pattern that would give amounts (`origin.cpp`, `origin.tsv`). For every ring
member of the fourteen transactions we looked up the transaction that created it. Of the 224
members, 69 come from conversions of ZEPH into the stable coin, 31 are stable-coin change of
conversions back into ZEPH, 77 come from stable-coin transfers and 47 are fee outputs of miner
transactions: the mix a wallet picking stable-coin decoys produces. A conversion states the
amount it mints in the open, but each of these 100 conversions has two stable-coin outputs, so
the amount of a single ring member is not public even there, and which member was spent is not
visible. Three members were created by another of the fourteen: the ring of `4cabb05a…` (block
125958) contains a stable-coin output of `6ade0f71…` (block 125921), and the ring of `78ebdc84…`
(block 295137) contains both stable-coin outputs of `6e5ea6d0…` (block 294167). That ties these
transactions to each other; it does not give an amount. The developer's figure rests on
reasoning or data we do not have.

### 5.3 Sixteen transactions with one or two foreign ring members

35 inputs in 16 transactions have a ring of the claimed asset with one (30 inputs) or two (5
inputs) members of another asset: 27 inputs with reserve-coin (ZRS) members in ZEPH rings, 5 with
ZSD members, 1 with both, 1 with a yield-coin member, and 1 ZSD input with a ZEPH member. In
every case the foreign member is among the four newest of the ring, and it was created either in
the first six blocks after a hard fork (89302 to 89306 after fork 3; 360002 to 360005 after
fork 6) or 12 to 54 blocks before the spending transaction. One transaction (block 91669,
`1975764671c5565220dd8764af9a3ecfab23cacd5f512b349a17bb6c665e30d6`, 38 inputs) has such a member
in 13 inputs, all created in blocks 89302 to 89305.

Looked at as whole rings (`origin.tsv`, `ring-ages.tsv`), these inputs fall into three groups:
- **29 inputs in 10 transactions, after hard fork 3.** The foreign member was created in blocks
  89302 to 89306, and no member of any of these rings is newer than block 89321. Yet the
  transactions are in blocks 89697 to 113049, that is 397 to 23,749 blocks after the fork. The
  same holds for the other 43 inputs of these ten transactions, which have no foreign member:
  their rings, too, end by block 89321. A wallet normally takes most decoys from the most recent
  blocks; here every ring of the transaction was drawn from outputs that end a few blocks after
  the fork, which is where the first outputs of the new assets are.
- **3 inputs in 3 transactions, after hard fork 6** (blocks 361396, 361463 and 362266). The
  foreign member, created in blocks 360002 to 360005, is the newest member of its ring. Two of
  these transactions have a second input, and its ring is an ordinary recent one (newest members
  in blocks 361447 and 362250).
- **3 inputs in 3 transactions** (blocks 116489, 119074 and 353852). The ring is an ordinary
  recent one and the foreign member is its newest, created 54, 25 and 12 blocks earlier: twice a
  ZSD member in a ZEPH ring, once a ZEPH member in a ZSD ring.

Two readings:
- **Decoys picked across assets**, with no gain to anyone. In the first group nothing sets the
  flagged inputs apart from the other inputs of the same transactions except the foreign member
  itself: all 72 rings were drawn from the same bounded set of outputs, whose newest end holds
  the first reserve-coin and stable-coin outputs. We did not establish which software builds
  such rings. The wallet has selected decoys by asset type since commit `6e98722af`
  (2023-08-11), before hard fork 3, and the mechanism that Monero's wallet has for bounding
  decoys at a fork is not active in Zephyr's source (its fork height is set to 99999999).
- **Real spends through defect B.** A ring can be built at will, so this cannot be excluded for
  any of the sixteen, and it had a motive: in the conversions that created these reserve-coin
  outputs, right after the fork, one ZEPH bought two reserve coins, so spending such an output
  as ZEPH would have doubled it. In the second and third group the foreign member is the newest
  of its ring, the position a real spend often has; that is a heuristic, not proof. The project
  reports "fewer transactions involving ZRS → ZEPH" [stated, Discord, 2025-01-23].

The ring ages show that the ten transactions of the first group share one way of selecting ring
members. They do not tell a foreign decoy from a foreign real spend: a list of outputs that was
cached at the fork, a transaction prepared then and sent later, a fault in a wallet's indexing
and a deliberately built ring would all look the same. Neither reading is proven for any of the
sixteen, and this report does not prefer one. The list is in the evidence files.

### 5.4 Controls: the whole chain re-checked

The whole database was scanned: blocks 0 to 902083 (the copy ends on 2026-11-07), 1,265,146
transactions, 8,028,931 inputs, 128,462,896 ring members, 1,265,146 range proofs checked, no
parse errors. Findings in total: the 3 failing proofs of §5.1 and the 49 inputs of §5.2 and §5.3,
nothing else. The last transaction with a foreign ring member is in block 362266; from hard
fork 7 (block 433000) on there is none (427,466 transactions from block 435000 on alone), and
none of the 604,107 proofs from hard fork 6 on fails. The
two rules are enforced by consensus from those forks on, so this part checks the scanner, not
the chain.

The re-validation (§2) covered the same 1,265,146 transactions, 33,504 of them conversions
(12,428 below hard fork 6). None fails the semantics check, the minted-amount check or the
ring-signature check of its height, none passes only under the rules of the previous fork, and
none could not be parsed or typed. Every transaction on the chain, the 33 of §5.1 to §5.3
included, therefore passes these checks as the released code applies them at its height: those
33 were accepted because of what the checks did not cover, not because a check was bypassed. For these checks it also follows that no earlier version of the
software accepted a transaction that v2.3.0 would refuse.

The key images of all 8,028,931 inputs are distinct (`keyimages.cpp`, §2): no output was spent
twice, under one asset name or under two.

The miner transactions were checked separately (`coinbase.cpp`, §2). In each of the 902,083
blocks after genesis the outputs equal, to the atomic unit and per asset, the block's reward
minus the reserve and yield shares plus the fees; block 536000 carries the one-time 1,921,650
ZEPH on top. No block's reward is above the emission formula; 5,412 blocks have a lower one,
which is what the block-weight penalty does. So no block paid out more than reward and fees
allow.

What these controls leave out is listed in §2. They show that the chain follows the rules as
they are coded for the things that decide how many coins exist; they are not a replay of every
consensus rule.

## 6. The audit and the supply

### 6.1 Timeline

| Date | Event | Basis |
|---|---|---|
| 2023-05-26 / 05-29 | First Zephyr commit with Haven's asset code; genesis | code, chain |
| 2023-10-01 | Hard fork 3: stable coin and reserve coin go live | chain |
| 2023-10-29 | First full-ring ZSD-as-ZEPH transaction | chain |
| 2024-06-19 | v1.1.0: sync stall at block 274662 fixed; reserve counter set by hand (§6.3a) | release notes, code |
| 2024-07-07 | Three transactions with invalid range proofs | chain |
| 2024-09-30 | Last full-ring ZSD-as-ZEPH transaction | chain |
| 2024-10-13 | Hard fork 6 (v2.0.0): range proofs verified again, as a by-product of the yield feature by the project's later account | code, release notes, Discord |
| 2024-11-19 | Haven starts its own supply audit (its hard fork 25) | Haven code |
| 2024-12-11 | Haven's repository: commit "fix range proof exploit", merged into the main branch the same day; no branch of that repository has a later commit | Haven code |
| 2024-12-29 | Zephyr announces on Discord that it has identified three transactions "that align with suspicious activity", refers to "the invalid range-proof issue that affected 50 Haven transactions", and says this "does not mean Zephyr was successfully exploited" (§6.1a) | Discord |
| 2025-01-02 | Zephyr's year review: "Work is underway to faciltiate conducting a comprehensive chain audit"; expected to take "the next four weeks, plus adequete time to allow everyone to particupate" | Discord |
| 2025-01-06 / 07 | Zephyr announces on Discord that it has asked all exchanges to disable ZEPH deposits until a chain audit is done, because of "the three suspect transactions from 6 months ago" (§6.1a). A mining service reports deposits blocked at MEXC, XeggeX, CoinEx, NonKYC and TradeOgre | Discord; Kryptex notice of 2025-01-07 |
| 2025-01-13 | Project explains the three transactions: they "fail an updated range proofs check ... introduced in Zephyr v2.0.0"; the earlier check "was incorrect, an oversight on our part" (§4.1) | Discord, general channel |
| 2025-01-20 | Project: "MEXC did give me a range ... its 2-3M" | Discord, general channel |
| 2025-01-21 | v2.0.2 announced: "An issue was uncovered that necessitates this pre-emptive fork ahead of the audit update"; fork set less than 40 hours ahead | Discord; release notes |
| 2025-01-22 | MEXC: "As requested by the ZEPHYR (ZEPH) project team, MEXC suspended deposits of ZEPH" | MEXC announcement |
| 2025-01-23 | Hard fork 7. Post-fork update: defect B described, credited to an anonymous developer; 30 transactions; estimate of about 200,000 ZEPH; "distinct from the 'range proofs' issue ... which involves three suspect transactions that fail the updated range proofs code" | Discord; code |
| 2025-02-19 | Audit fork announced for 2025-03-31; the hotfix "has ultimately set us back" | Discord |
| 2025-03-31 | Hard fork 8: only audit transactions; full resync required | release notes, chain |
| 2025-05-07 | Hard fork 9: all transactions paused. Project: most exchanges have finished, "MEXC has yet to audit all of their coins"; estimated supply "~9.5M" against "the ~7.4M we would have had if it had not been for any exploit(s)" | chain; Discord |
| 2025-05-09 | Extension announced: "MEXC has the largest wallet ... and the coins that are contained in their wallet are user funds". Published table (day 37): 7,296,446 ZEPH audited including 2,187,899 in the reserve, against 7,395,570 reported | Discord |
| 2025-05-11 | Hard fork 10: audit window reopened "to ensure that critical services and partners ... have been able to get all of their assets audited" | release notes |
| 2025-05-14 | MEXC: "Due to the hard fork of ZEPHYR (ZEPH), MEXC suspended ZEPH deposits and withdrawals" | MEXC announcement |
| 2025-05-21 | Project: MEXC has "been running the `audit_all` command for over 24 hours"; "some limitation in their server environment". Last resort named: "to simply mint the coins they can't audit in the next update and send them to MEXC" | Discord, minor-announcements |
| 2025-05-27 | Project: "we are going to have to move to the last resort option of minting them their difference in the next hardfork" | Discord, minor-announcements |
| 2025-06-01 | Project: agreement with MEXC on the mint; "They have strict OpSec restrictions which has caused issues with them not being able to ... complete their audit" | Discord, minor-announcements |
| 2025-06-11 | v2.3.0 announced: "a mint for their unauditable balance (1,921,650 ZEPH)"; estimated change against the reported supply ZEPH +2,056,803, ZSD −106,320.68, ZRS −146,210.32, ZYS −4,103.28; "we are adjusting our emission schedule to reflect this" so that "the 18.4M Max supply of ZEPH (before tail emission) remains" | Discord; release notes |
| 2025-06-15 | Hard fork 11 (v2.3.0): normal operation; old asset types invalid; one-time mint. Project publishes `supply_info` at height 536010 | chain; Discord |
| 2025-06-15 | Project on X: "$ZEPH was underreported by +2.39M, bringing total circulating supply to 9.75M" (§6.3) | X |
| 2025-06-16 | "Working with MEXC to get them online ASAP! Other exchanges should be online." | Discord |
| 2025-06-17 | Project: "I received their exchange wallet seed to confirm the balance. I have now done so and paid them to their new exchange wallet." Screenshot of the wallet: "Balance: 1921649.728246938366 ... ZEPH (unaudited)" | Discord, minor-announcements |
| 2025-07-28 | Project's overview article: "an inflation of ~2.39M $ZEPH compared to the expected emission schedule" (§6.3) | X |
| 2025-08-25 | Project's AMA article: no third-party review planned; "both Zephyr exploits were inherited from "audited" Haven code portions" (§3, §8) | X |
| 2025-08-27 / 2025-09-06 | Project's post-audit report, on its website as "Zephyr Audit – Postmortem" and ten days later on X (§4.1, §5.2, §6.5, §8) | website; X |
| 2026-07-14 / 2026-08-10 | Project selects the firm Sherlock for an external audit of its EVM bridge; first round reported complete (§8) | Discord |

Normal transfers were not possible for 54,500 blocks, about eleven weeks. The post-audit report
dates hard fork 10 to "May 9, 2025" and has hard fork 11 "activated at block 536,000" on "Jun 11,
2025"; those are the days of the announcements. On the chain the two forks are on 2025-05-11 and
2025-06-15, and the project's own post of 2025-06-11 says "approx. June 15" [stated, X].

Two more points where later summaries by the project differ from its own record, noted for
readers who compare sources. The post-audit report's timeline has exchanges asked to disable
deposits on 2024-12-29; the announcements of that request, on Discord and on X, are of
2025-01-06. On 2026-04-13 the project wrote "We disclosed the morphing bug publicly in Dec 2024"
([reply](https://x.com/zephyr_org/status/2043732782224957593)) [stated, X]; the announcement of
2024-12-29 concerns the three range-proof transactions and does not mention ring members, and
defect B was announced on 2025-01-23, two days after the hotfix release (§6.1a).

### 6.1a What the project said before the audit [stated, Discord]

All texts were posted in the project's Discord announcements channel by the account "fennzar".

2024-12-29 ([message](https://discord.com/channels/1112605073948819559/1112605856488493176/1322858944879792189)):

> We've identified three transactions in Zephyr that align with suspicious activity also confirmed
> by @Kayaba. This does not mean Zephyr was successfully exploited. There's still much we simply
> don't—and in some cases can't—know, such as:
> - The specific exploit methods or their outcomes.
> - Whether the invalid range-proof issue that affected 50 Haven transactions is exactly the same
>   in kind as what occurred with the 3 transactions on Zephyr.
>
> [...] If a major exploit had truly succeeded on Zephyr, we would almost certainly see
> catastrophic effects: A price crash to $0. Unexpected amounts in exchange books or balances.
> Breakdowns in our stablecoin mechanics (ZEPH Reserve drained etc). None of these signs have
> appeared [...]
>
> Moreover, our protocol relies on differing mechanics (namely DJED) and was written by the Zephyr
> team—it diverges significantly from Haven's implementation.

and in its second part ([message](https://discord.com/channels/1112605073948819559/1112605856488493176/1322858954241478678)):

> Even though a large-scale exploit seems unlikely, we're still treating this with due caution.
> [...] If significant unauthorized minting of ZEPH or other assets did occur (which we do not
> believe is the case), we have options to address and rectify it.

2025-01-06, "ZEPH Exchange Deposits Disabled & Upcoming Chain Audit" ([message](https://discord.com/channels/1112605073948819559/1112605856488493176/1325691467523686452)):

> we simply can't *definitively* rule out any possibilities of the result of the three suspect
> transactions from 6 months ago - until a final resolution and result is determined by the Audit
> process.
>
> As it stands, especially by the effect of exchanges disabling their deposits, we can know that
> it is NOT possible that millions of excess ZEPH has been sold off on exchanges and thus in the
> hands of legitimate users. [...] As it stands, only 30% of ZEPH is in the reserve which is very
> much in line with what we would expect [...]
>
> In addition, we know from exchanges that their wallet balances are expected and reasonable, all
> totaling to well under our reported supply of 6.4M - as well as their books, trading activity
> etc.

and in its second part ([message](https://discord.com/channels/1112605073948819559/1112605856488493176/1325691481251512331)):

> Best case scenario: Whatever was attempted was not successful, and no unauthorized ZEPH was
> minted and thus sold off on exchanges.
>
> Worse case scenario: The transactions did result in unauthorized ZEPH minting, and a necessarily
> limited amount of ZEPH has been slowly sold off over the last 6 months, that has flown "under
> the radar", now in legitimate users hands. With the exchanges disabling deposits, this flow has
> been stopped but there will be an inflationary effect post audit. Any large amounts audited will
> need to be investigated and if we believe that any amounts were a direct result of the 3
> transactions, we will effectively blacklist them.
>
> [...] Coins not audited will be rendered null and void. Exchanges will participate and therefore
> coins you have on exchanges will be audited for you.

On X the project posted on the same subject that day
([2025-01-06](https://x.com/zephyr_org/status/1876209588460249297)) [stated, X], more directly on two points: "the
evidence suggests that these transactions could have led to the unauthorized minting of
additional ZEPH", and "The audit will have to be comprehensive and impossible amounts will be
blacklisted. Post-audit, any coins not audited will be null and void."

How this relates to what we measured:
- The question the project left open on 2024-12-29 can be answered from public data: the three
  transactions of 2024-07-07 have range proofs that do not verify, and the function that
  validated them did not check range proofs (§4.1, §5.1). That is the same omission, in the same
  function, as in Haven's code.
- That the protocol "was written by the Zephyr team" and "diverges significantly from Haven's
  implementation" holds for the Djed rules. It does not hold for the transaction verification concerned: that function is in
  Zephyr's first commit in Haven's form (§3, §4.1). Eight months later the project wrote that
  both exploits "were inherited from "audited" Haven code portions" (§3) [stated, X].
- The announcement of 2024-12-29 does not mention ring members or asset types. Defect B was
  announced separately on 2025-01-23 as "distinct from the 'range proofs' issue".
- The announcement of 2025-01-06 foresaw a blacklist for amounts traced to the three
  transactions. The released code contains none, and none was announced in the channel; whether
  any amount was traced is not stated in what we read.
- The "reported supply of 6.4M" equals the coins generated by that date (6,431,888 ZEPH through block
  420759). The supply the project reported before the audit was therefore the
  schedule, not a count of coins; no such count existed (§6.3).
- 30 % of that is about 1.93 million ZEPH in the reserve; the reserve rebuilt from the chain was
  1,890,614 ZEPH at that block (29.4 %). The statement was accurate.
- Around the turn of the year the project did not believe significant unauthorized minting had
  occurred. Five months later the supply table held 2,067,576 ZEPH more than the schedule, 1,921,650 of it
  minted for an exchange balance that had not been audited (§6.3, §6.4). The statement of January
  concerns coins sold on exchanges to other users; the later figures do not say where the excess
  coins were, so the two are not directly comparable. We record both.

### 6.2 How the audit worked [code, chain]

Amounts cannot be audited from outside. The audit forks therefore accepted only "audit
transactions": a holder spends old-type outputs and receives new-type outputs, and the
transaction states the amount in the open (`amount_burnt` of the old type equals `amount_minted`
of the new type; this held for all 15,040). The supply tables were rebuilt from these statements.
From hard fork 11 the old asset types are rejected.

This establishes how many coins were *presented*. It does not establish which of them had been
emitted: a coin that was never emitted and is moved through an audit transaction is an audited
coin. With hidden amounts and histories no audit of this kind can do more.
The project described the result in the same terms when asked how the numbers can be verified
([reply of 2025-06-11](https://x.com/zephyr_org/status/1932786647004840331)) [stated, X]: "Only balances that went
through the audit process have been verified and migrated, these are now part of the official
on-chain state. Yes Zephyr uses private transactions, we can't see individual histories - but we
can define a clear, verified supply moving forward."

| Asset (old to new) | Audit transactions | Amount audited | of which in the extension |
|---|---|---|---|
| ZEPH to ZPH | 13,300 | 5,164,647.67 | 245,009.22 |
| ZEPHRSV to ZRS | 1,318 | 1,023,509.12 | 17,781.93 |
| ZEPHUSD to ZSD | 262 | 88,314.92 | 908.19 |
| ZYIELD to ZYS | 160 | 185,474.08 | 462.58 |

The largest single audit transaction is 323,181 ZEPH; the ten largest are 26.8 % and the hundred
largest 60.3 % of the audited ZEPH (transactions, not holders).

### 6.3 Reconciliation of ZEPH

Before the audit the chain kept no count of ZEPH in wallets (the old "ZEPH" tally is the
reserve). Since hard fork 8 the node reports a total per asset; the ZEPH total includes the ZEPH
held in the reserve.

| Figure | ZEPH | Basis |
|---|---|---|
| Coins generated through block 481499 (before hard fork 8), as recorded by the node | 7,108,089 | chain |
| Coins generated through block 536000, without the one-time mint | 7,682,370 | chain |
| Total ZEPH in the supply table at height 902084 | 12,355,841 | chain |
| minus coins generated in blocks 536001 to 902083 | −2,605,896 | chain |
| **Total right after block 536000** | **9,749,946** | derived |
| **Excess over emission at block 536000** | **2,067,576** | derived |
| of which the one-time mint in block 536000 | 1,921,650 | code, chain |
| of which excess inside the audited supply | 145,926 | derived |
| Reserve at hard fork 8, rebuilt from genesis (§6.3a) | 2,088,864 | chain |
| Most that could have been emitted into wallets by hard fork 8 (generated − reserve) | 5,019,225 | derived |
| Holders' audit transactions, amounts plus fees | 5,165,151 | chain |
| Wallet claims honoured (audited 5,164,648 + one-time mint 1,921,650) | 7,086,298 | chain |

All figures in this table come from one database (blocks 0 to 902083). The 145,926 can be reached
two ways with the same result to the last digit shown: total minus coins generated, or audited
amounts plus fees minus what could have been emitted into wallets. The table shows whole ZEPH,
rounded from the exact figures; sums of the rounded entries can differ by one.

What the total right after block 536000 consists of, as a check: 5,164,648 ZEPH from the audit
transactions, 2,289,862 in the reserve, 373,779 paid to miners in blocks 481500 to 535999
(`coinbase.tsv`), and 1,921,657 paid in block 536000 (the one-time mint and that block's
reward). Together 9,749,946.

**The project's own figures** [stated, Discord]. On 2025-05-07: "we will end with a higher
reported circulating amount than the ~7.4M we would have had if it had not been for any
exploit(s). The estimated circulating supply for ZEPH is going to be ~9.5M". On 2025-06-11:
estimated new supply 9,706,880 ZEPH, an estimated change of +2,056,803 against the supply then
reported. Our figure at the fork block is 2,067,576.

On X the project gave a larger figure [stated, X]. Its thread of 2025-06-15
([post](https://x.com/zephyr_org/status/1934252619540664495)) says "$ZEPH was underreported by +2.39M, bringing total
circulating supply to 9.75M", with a table: ZEPH reported 7,360,588.16, audited 9,750,021.39,
difference +2,389,433.23; ZSD 473,976.71 and 419,218.23; ZRS 1,166,020.32 and 1,028,603.22; ZYS
189,500.28 and 185,474.35. Its overview article of 2025-07-28 speaks of "an inflation of ~2.39M
$ZEPH compared to the expected emission schedule". The audited figures are those of `supply_info`
at height 536010 and agree with ours. The base is not the emission at the fork. All four
"reported" figures of that table are those of the project's own audit-log table of 2025-05-03,
day 32 of the audit ([message](https://discord.com/channels/1112605073948819559/1359471488364970104/1368212122236096554)) [stated,
Discord, audit-log-updates], six weeks before hard fork 11. The coins generated through block
536009 are 7,682,445.64 ZEPH without the one-time mint (§6.3a), 321,857 more than that figure.
So the +2.39 million sets the audited total of June against the reported supply of early May.
Against the coins generated at the fork the excess is 2,067,576; the project's own estimate of
2025-06-11, repeated in its post-audit report of 2025-09-06 and on 2026-04-13, is +2,056,803.

The tables the project published during the audit count as "audited" the reserve, the audit
transactions and the newly mined coins, and set them against a "reported" figure. On 2025-05-09,
after the first window: 7,296,446 ZEPH audited (of which 2,187,899 in the reserve) against
7,395,570 reported. On 2025-05-16, the last table: 7,439,432 against 7,458,205. Up to then the
audited total was below the reported one. The excess appears only afterwards, with the remaining
audits of the extension and the one-time mint of 1,921,650. The project's reserve figures in these
tables agree with our rebuild (§6.3a).

**Check against the project's own figures.** In its Discord announcement of hard fork 11 the
project published the output of the wallet's `supply_info` at height 536010: 9,750,021.39 ZEPH in
total, 2,339,897.346230 ZEPH in the reserve and 282,410.979309 ZSD in the yield reserve [stated,
Discord, 2025-06-15]. The node's record of coins generated through block 536009 is 9,604,095.64,
so that total is 145,925.75 ZEPH above emission: the figure in the table, from a different height
and a different source. Our rebuilt reserve after block 536009 is 2,339,897.346856 and our rebuilt
yield reserve 282,410.979309 (§6.3a).

### 6.3a The reserve and the other assets, rebuilt from genesis [chain]

The reserve is a counter kept by the node, and the project set it by hand once (block 274662,
v1.1.0). To test it we rebuilt it from block 0 from public data only: the reserve's share of
every block reward, plus the ZEPH paid in by every mint conversion, minus the ZEPH paid out by
every redeem conversion (`amount_burnt` and `amount_minted` are public in conversions). The same
pass rebuilds the totals of the other assets and reads the coins generated per block as the node
stored them.

| After block | Rebuilt reserve (ZEPH) | Compared with | Difference |
|---|---|---|---|
| 274661 | 1,355,089.749101175250 | constant in the source for that height, 1,355,089.749101175250 | none |
| 274662 | 1,355,092.382800 | value the project set by hand, 1,355,092.382175 | 0.000625 |
| 507999 | 2,187,899.36 | project's audit tables, 2,187,899 | none at their precision |
| 536009 | 2,339,897.346856 | project's `supply_info` at 536010, 2,339,897.346230 | 0.000625 |
| 902083 | 4,926,030.856934 | node's reserve at height 902084, 4,926,030.856309 | 0.000625 |

The hand-set value differs from the rebuilt one by 0.000625 ZEPH, and the difference is the same
at the two later points of comparison. At every point where the reserve counter can be compared
it agrees with the rebuild to within a thousandth of a ZEPH, and the correction of 2024 set it
to what the public data give. No historical values of the counter are stored, so heights in
between cannot be compared. Through block
481499 the reserve received 1,237,538 ZEPH in block rewards and 4,108,725 ZEPH from mint
conversions, and paid out 3,257,399 ZEPH in redeem conversions; there were 23,076 conversions of
all kinds.

The other assets, rebuilt the same way (conversions and yield rewards), against the figures the
project called "reported" in its audit tables:

| Asset | Rebuilt | Project's "reported" | Supply table after the audit (height 536010) | Missing |
|---|---|---|---|---|
| ZRS (after block 481499) | 1,166,020.32 | 1,166,020.32 | 1,028,603.22 (rebuilt to the same block: 1,171,111.52) | 142,508.30 |
| ZYS (after block 481499) | 189,500.28 | 189,500.28 | 185,474.35 | 4,025.93 |
| ZSD (after block 507999) | 475,010.68 | 475,010.68 | 419,218.23 (rebuilt to the same block: 535,371.54) | 116,153.31 |

The "reported" figures are thus what the public conversions add up to, and after the audit
262,688 units of the other assets are missing from the supply table: 116,153 ZSD, 142,508 ZRS and
4,026 ZYS. The yield reserve rebuilt to block 536009 equals the project's figure to the last
digit. The same three differences stand at the tip of the database (height 902084: 142,508.30
ZRS, 116,153.31 ZSD, 4,025.93 ZYS), so between those two points the node's totals have moved by
exactly what the public conversions add up to.

**Emission.** The coins generated per block were read from the database instead of being
simulated. A simulation without block-weight penalties ends 28.33 ZEPH above the node's record
at block 902083; 5,412 blocks carry a penalty and no block's reward is above the formula (§5.4).
The figures in this report use the node's record.

**What the excess means.** The accounting result needs no assumption: at block 536000 the supply
table holds 2,067,576 ZEPH more than ordinary emission to that block, 1,921,650 of it issued by
the one-time mint. What it says about earlier coins rests on the project's statement that the
mint replaced an existing balance that could not be audited (§6.4). On that statement the figure
is a net one: coins honoured that were never emitted, minus emitted coins that were never
audited (lost keys, holders who missed both windows). The first quantity is then at least
2,067,576 and higher by the unknown second. Both parts rest on public data: the
1,921,650 of the one-time mint is a constant in the code and an output on the chain, and the
145,926 follows from the audited amounts, the coins generated and the rebuilt reserve.

**Where the non-emitted coins were** cannot be decided from the chain. The two limiting cases,
both on the project's statement about that balance:
- the unaudited exchange balance consisted almost entirely of them, and what the other holders
  audited was almost entirely emitted coins (their excess is only 145,926);
- the exchange balance consisted of emitted coins, in which case it belongs inside the
  5,019,225, and about two million non-emitted ZEPH were among the coins other holders audited.

Anything between is possible, and this report does not choose. An exchange holds what its
customers deposited; nothing in this evidence establishes anything about the exchange's conduct.

### 6.4 The one-time mint [code, chain, stated]

- Constant `UNAUDITABLE_ZEPH_AMOUNT`, 1,921,650 ZEPH
  ([`cryptonote_config.h` L188](https://github.com/ZephyrProtocol/zephyr/blob/v2.3.0/src/cryptonote_config.h#L188)),
  required as second coinbase output of block 536000
  ([`blockchain.cpp` L1443-L1456](https://github.com/ZephyrProtocol/zephyr/blob/v2.3.0/src/cryptonote_core/blockchain.cpp#L1443-L1456)).
- On chain: block 536000 (`33ee82a6ee048d66355eac94d61e86471f4f1ff703cf7769fcada0bd36b28d6f`),
  miner transaction `cac7f5bd918a9d55fa0071cb01007e1e4471734878bbec896d762a69df0489c1`, second
  output 1,921,650.000000000000 ZPH.
- The output is paid to `GOV_WALLET_ADDRESS_2`, an address the project fixed in the source
  ([`cryptonote_tx_utils.cpp` L238-L258](https://github.com/ZephyrProtocol/zephyr/blob/v2.3.0/src/cryptonote_core/cryptonote_tx_utils.cpp#L238-L258));
  nothing in the code ties it to an exchange.
- For whom [stated, Discord and release notes]: 2025-05-07, "we have had confirmation that MEXC
  has yet to audit all of their coins"; 2025-05-09, "MEXC has the largest wallet, with the most
  amount of coins to audit which was always going to be more challenging than regular users, and
  the coins that are contained in their wallet are user funds"; 2025-06-11, "Due to MEXC being
  unable to audit their entire wallet balance, we are including a mint for their unauditable
  balance (1,921,650 ZEPH) in this Hardfork"; 2025-06-16, "Working with MEXC to get them online
  ASAP!".
- Why the balance was not audited [stated, Discord, minor-announcements and general]: on
  2025-05-21, "they have been running the `audit_all` command for over 24 hours ... The command is
  running, just slowly, which means there is some limitation in their server environment"; on
  2025-05-24, 48 hours "on a new environment"; on 2025-06-01, "strict OpSec restrictions" kept
  MEXC from acting on the project's suggestions. The project wanted the balance audited, not
  minted: "I really want them to complete the audit in this on-chain, clear way", and of the
  mint, "it's self evident just how much of a mess that is, it would be a last resort"
  (2025-05-21). The mint was decided on 2025-05-27.
- How the amount was established and handed over, as reported on 2025-06-17 [stated, Discord]: "I received their
  exchange wallet seed to confirm the balance. I have now done so and paid them to their new
  exchange wallet", with a wallet screenshot showing "Balance: 1921649.728246938366 ... ZEPH
  (unaudited)". The constant of 1,921,650 was already in the release of 2025-06-11, so the figure
  was known to the project before this check, which confirmed it to within 0.28 ZEPH; the
  announcements do not say how it was supplied. Earlier the project had
  MEXC's holdings only as a range: "its 2-3M" (2025-01-20), "less than 3M but they didn't share
  with me exact figures" (2025-04-14). In the same conversations it wrote: "MEXC & TO figures are on
  the highside, but you can make some assumptions about the rest and it tallies up to reasonable
  levels. If exchanges had like 10M in wallets then the scope would be self evident"
  (2025-01-20), and "We are still waiting on MEXC to complete their audit ... As it stands at the
  moment once they put theirs through we will be in the ballpark of the expected amount"
  (2025-04-14) [stated, Discord, general].
- What this does and does not show. By this account the mint replaced old-type coins that existed
  as unspent outputs in one wallet, not a figure on paper. We cannot verify the screenshot, that
  the wallet is MEXC's, or the payment to the new wallet: amounts and addresses are hidden.
  Nothing on the chain names an exchange. MEXC's public notices mention the suspensions only.
- The project announced the change to emission openly (2025-06-11): "we are adjusting our emission
  schedule to reflect this. Our emission and new emission rate will be effectively at the end of
  Year 3 for ZEPH. This was done to ensure that the 18.4M Max supply of ZEPH (before tail
  emission) remains."
  That limit is the one in the project's whitepaper: "Total Supply: 18.4 Million (+ tail
  emission 0.6 ZEPH p/block)" [stated, whitepaper, p. 3].
- Accounting: the amount is added to the block's base reward and so to the counter of generated
  coins. The base reward, by the emission formula on that counter, fell from 10.27 ZEPH in block
  536000 to 8.43 ZEPH in block 536001, about 18 % less. The lead over the original schedule was
  2,067,576 ZEPH at block 536000 and about 1,494,000 at height 907724 (2026-11-14, `supply.txt`).
  It keeps shrinking only while rewards follow the emission curve. The reward has a floor of 0.6
  ZEPH per block, the tail reward; the mint brings the moment that floor is reached forward, and
  once both schedules pay the floor the difference no longer changes. A projection without
  block-weight penalties puts that at block 3,307,370 with the mint and 3,513,570 without it,
  about ten years after the audit, with 123,720 ZEPH of the mint still ahead of the original
  schedule. So lower rewards offset about 1.8 million of the mint, not all of it, and the
  145,926 are not offset at all. Lower rewards mean less for miners, for the reserve share and
  for the yield share; the minted coins themselves stay in the supply.

### 6.5 The other assets

ZSD, ZRS and ZYS have no emission schedule; they arise from conversions, whose amounts are
public. Rebuilt from those (§6.3a), 262,688 units are missing after the audit. The project's estimate of 2025-06-11 against its reported
supply [stated, Discord]: ZSD −106,320.68, ZRS −146,210.32, ZYS −4,103.28, together 256,634 units
fewer than reported. It adds (2025-05-07): "The other assets look like they will come in under
expected, and we know that some of those assets were likely morphed into ZEPH."

**What follows for the two defects** (inference). Under defect B one unit of another asset
becomes one ZEPH and the old count of that asset keeps it. A unit that was morphed into ZEPH and
stayed there therefore shows up as a unit missing at the audit, next to units that were simply
not audited: units missing = net morphing into ZEPH + units not audited. (Morphing the other way
would offset it; one of the sixteen partial rings runs that way, §5.3.) On this accounting,
and with the totals of the other assets being what the public conversions add up to, defect B
can explain at most about 262,688 ZEPH of the excess (the units missing by our rebuild, §6.3a; the
project's estimate of June was 256,634); the project's own best guess for morphing is about
200,000. Of the excess of 2,067,576 at least about 1.8 million is therefore not explained by
defect B. The only other known defect is A, and three
transactions with invalid range proofs exist; but the chain does not show what they created, so
we do not attribute the remainder to them. It is unexplained by B, and A could explain it.

The project's post-audit report leaves the same question open [stated, X, 2025-09-06]. Of the
range-proof transactions: "We were not able to determine if or how much net chain-wide inflation
originated from this vector, but the possibility that this contributed could also not be ruled
out." Of the outcome: the other assets "fell below pre‑audit expectations; consistent with
ZSD/ZRS → ZEPH morphing". The report gives the deltas (ZEPH +2,056,803; ZSD −106,320.68; ZRS
−146,210.32; ZYS −4,103.28) and does not say what accounts for the part of the ZEPH figure that
morphing cannot.

## 7. Haven, where the code came from [code; stated, Discord]

What can be shown about Haven from its public repository, cloned on 2026-11-14:

| | Haven Protocol | Zephyr Protocol |
|---|---|---|
| Defect A in the code | from commit `141d4a359` (2023-04-25) | from the first commit (2023-05-26), live from genesis (2023-05-29) |
| Corrected | commit `6f361aadf`, "fix range proof exploit", 2024-12-11 | commit `427277f7d`, 2024-09-29, in force from block 360000 (2024-10-13) |
| Own supply audit | hard fork 25 "supply audit" on 2024-11-19, end fork on 2025-01-02 (hard-fork table) | hard forks 8 to 11, 2025-03-31 to 2025-06-15 |
| Repository | last commit 2024-12-11 | last commit 2025-06-10 |

Zephyr's correction is ten weeks earlier than Haven's. Haven's code has a supply-audit
transaction type that carries an amount proof, after which old funds can no longer be spent
(`rctSigs.cpp`, `RCTTypeSupplyAudit`). Zephyr's audit, four months later, has the same shape:
holders move their coins once through an audit transaction, and unaudited coins become invalid.
Of the 591 substantive lines that Zephyr's audit commit (`7475f2d5a`) adds, 2 occur in Haven's
code (`heritage.py`), so the idea is shared and the implementation is Zephyr's own.

What Zephyr itself wrote about Haven [stated, Discord]: on 2024-12-29, "the invalid range-proof
issue that affected 50 Haven transactions"; on 2024-12-30, "The reason why the Haven team were
made aware was because of the absolutely INSANE amount of coins on TradeOgre." We did not verify
either statement and did not scan Haven's chain. What happened to Haven as a project, and how
other chains of the same lineage dealt with similar defects, is outside what we could establish
from sources we read in full, and is left out.

## 8. How the incident was handled

Observations, not a verdict. This report does not judge who is to blame; the weighing is left to
the reader.

**What the project did.**
- It made public announcements at each of these steps: the three transactions (2024-12-29), the deposit stop
  (2025-01-06), defect B with its own count and estimate two days after the hotfix, the expected
  inflation before the audit closed (2025-05-07), and the mint with its amount and the change to
  emission (2025-06-11).
- It published its own post-audit report (2025-08-27 on its website, 2025-09-06 on X), with the
  figures and with sections on what went well and what did not, and it names the origin of both
  defects (§3).
- Both defects are closed. The chain kept producing blocks throughout, while ordinary
  transfers and conversions were off for 54,500 blocks (§6.1). Defect A was closed
  by the v2.0.0 release, by the project's account before it knew of the defect (§4.1); defect B
  by a hotfix in the month in which, by the project's account, it was reported ("Jan 2025")
  [stated, X, 2025-07-28].
- The audit produced, for the first time, supply figures that anyone can verify from the chain.
- The emission parameter was not raised: the one-time mint was booked as emission and is, for
  the larger part, offset by lower rewards in the years after (§6.4).
- The project wanted the exchange balance audited on chain and called minting it "a last
  resort" (§6.4).
- The accounting can be reproduced from public source code and a public chain, which is what
  made this review possible. The attribution of the mint and the check of the balance are the
  project's statements.

**What stays open.**
- Defect A was live for sixteen and a half months and defect B for almost sixteen. On
  2025-01-06 the project wrote that its work "includes a comprehensive review of our entire
  codebase, alongside the already planned audit" [stated, X]; we found no account of the outcome
  of such a review. A search of the project's Discord server and of its X account on 2026-11-14
  returned none, and none of the 34 announcements from that date to the end of 2025 reports on
  one (§11). Asked on 2025-08-25 whether there would be a third-party
  review of the code, it answered: "None are planned, but they aren't ruled out!", and said it
  had placed its trust in the audits of Haven's code (§3) [stated, X].
- In July 2026 the project announced "We have selected Sherlock for the external audit"
  [stated, Discord, 2026-07-14]. That audit concerns the project's EVM bridge. The scope the
  project published names "off-chain bridge repository, API, watchers and relayers" and
  "Solidity smart contracts" [stated, Discord, community-funding, 2026-07-28]; the node's
  consensus code is not in it. The project had said so beforehand. Asked whether the audit
  would also look at the base chain, an account with the team role "Marketing Lead" answered:
  "Scoped to the EVM bridge only: the smart contracts plus the off-chain bridge API/watchers.
  Not the L1.", and added: "That's not to say we'll never look at the L1... as the protocol
  scales, a dedicated L1-focused review is a reasonable step" [stated, Discord, general,
  2026-07-08]. The first round of the bridge audit was reported complete on 2026-08-10 [stated,
  Discord]. We found no third-party review of Zephyr's node code: not in the project's
  statements, and not in its public repositories of the node and the wallets, which hold no
  audit report and no reference to one (checked 2026-11-14).
- The correction of defect A in v2.0.0 was not announced as a security fix at the time; by the
  project's account it was not known to be one. The project disclosed the substance afterwards:
  on 2024-12-29 as a possibility, on 2025-01-13 in full ("incorrect, an oversight on our part"),
  and on 2025-01-23 as distinct from asset morphing. The fullest of these statements is in the
  general chat, not in an announcement or the release notes.
- The project's estimate for defect B is about 200,000 ZEPH; the excess is about 2.07 million.
  The project's post-audit report says it could not determine how much came from the
  range-proof transactions, so what accounts for the difference is open by its own account as
  well (§6.5).
- The audit could not remove coins that had not been emitted and were presented; it turned them
  into audited coins. That is a limit of the method, even if every holder takes part (§6.2).
- Holders who did not move their coins in the two windows lost them. Their number is unknown.
- The one-time mint is 1,921,650 ZEPH, a quarter of everything emitted by then, created by a
  constant in the source and paid to a project-designated address. The amount was fixed in the
  release before the project checked the balance from the exchange's wallet seed; the check, the
  identity of the wallet and the payout rest on the project's word and cannot be verified on
  chain. Its cost falls on miners and on the reserve and yield shares of later blocks.
- We found no independent post-mortem; the public account we could read consists of the
  project's posts, its own post-audit report and the release notes.

## 9. State of the code on 2026-11-14 [code]

- Both defects are closed in v2.3.0, the current release. Blocks below hard fork 6 are,
  necessarily, still validated with the old function; that affects history only.
- The public repository's only branch ends at the v2.3.0 commit of 2025-06-10; the release was
  published the next day. There is no later commit and no later release (checked 2026-11-14).

This report does not assess the present economic state of the protocol, such as its reserve
ratio, the price or the access to exchanges. Those are different questions from whether the
supply is right.

## 10. Reproducing this report

Tools are in `tools/`, their results in `results/`, the transcripts in `sources/`:

| File | Purpose |
|---|---|
| `chainscan.cpp` | the scan of §2: foreign ring members and range proofs, per block range |
| `scan-findings.tsv`, `scan-stats.tsv` | all findings and the per-range counters of the scan |
| `gaptest.cpp`, `gaptest.txt` | the executed test of defect A and what it printed |
| `ringdetail.cpp`, `ringdetail.tsv` | every foreign ring member of the 30 flagged transactions, with global output index and creation height |
| `origin.cpp`, `origin.tsv`, `ring-ages.tsv` | the creating transaction of every ring member of the 49 flagged inputs, and the oldest and newest member of every ring of the 30 transactions |
| `refs.cpp`, `refs-bpp.tsv` | later rings that reference the outputs of the three invalid-proof transactions |
| `revalidate.cpp`, `revalidate-stats.tsv` | the re-validation of §2 and the counters per block range; it printed no failure line |
| `keyimages.cpp`, `keyimages.tsv` | the check that no key image occurs twice |
| `coinbase.cpp`, `coinbase.tsv` | the miner-payout check of §2, with totals per fork period |
| `ledger.cpp`, `ledger.tsv` | coins generated, the reserve and the asset totals rebuilt from genesis, with snapshots at the fork heights |
| `blockhash.cpp`, `blockhash-local.tsv` | block hashes of the scanned database at the fork heights and its tip, for comparison with any other node |
| `audit_sum.py`, `audit-transactions.tsv`, `audit-summary.txt` | the audit transactions read through a node's RPC: the list, and the sums per asset |
| `supply.py`, `supply.txt` | emission formula and reconciliation through a node's RPC; it simulates the schedule without penalties and so ends 28 ZEPH above the node's record |
| `heritage.py`, `heritage.txt` | the code comparison of §2 and §7 and what it printed |
| `sources/discord-announcements.txt`, `sources/discord-channels.txt`, `sources/discord-sherlock.txt`, `sources/discord-code-review.txt`, `sources/x-posts.txt`, `sources/web-sources.txt`, `sources/release-notes.txt` | the transcripts of the statements that this report quotes (§11) |

The C++ programs are built against the libraries of a normal build of `ZephyrProtocol/zephyr`
v2.3.0 (compile flags and link line of `zephyr-blockchain-stats`, libraries in one linker group)
and open the LMDB database read-only. Any fully synced v2.3.0 node database gives the same
result. All results in this directory were produced with the programs as they are here. The RPC queries need only a restricted node:

```
get_circulating_supply   get_reserve_info   get_block_header_by_height
get_block_headers_range  get_block          /get_transactions (decode_as_json, prune)
```

## 11. Sources

Code and releases:
- https://github.com/ZephyrProtocol/zephyr (tags v0.1.0 to v2.3.0). The notes of its fifteen
  releases, as the GitHub API gives them, are in `sources/release-notes.txt`.
- https://github.com/haven-protocol-org/haven-main
- https://zephyrprotocol.com/docs/zephyr_whitepaper.pdf

Statements read by us:
- Web pages, transcript taken on 2026-11-14 (`sources/web-sources.txt`):
  - MEXC, 2025-01-22: https://www.mexc.com/announcements/article/suspension-of-deposits-for-zeph-17827791521494
  - MEXC, 2025-05-14: https://www.mexc.com/announcements/article/suspension-of-zeph-deposits-and-withdrawals-17827791524070
  - Kryptex, 2025-01-07: https://www.kryptex.com/en/news/zephyr-exchnage-pending-en
  - The project's website, "Zephyr Audit – Postmortem", dated 2025-08-27: https://zephyrprotocol.com/blog/2025-08-27-Zephyr-Audit-Postmortem.
    Apart from its title and two headings its text is the same, line for line, as the post-audit
    report the project published on X on 2025-09-06, which is the one this report cites.
  - The project's website, announcement of v2.0.0, dated 2024-09-30: https://zephyrprotocol.com/blog/2024-09-30-Zephyr-Protocol-v2-0-0-ZSD-Yield-Hardfork-Annoucement
  - The index of the project's blog (13 posts), and its bridge proposal of 2025-09-25, which was
    read and is not cited. The other ten posts were not read; two of them, of 2023-06-01 and
    2023-06-15, were searched for any mention of a review or audit of the software (fourth
    Discord transcript, below).
- Zephyr's Discord announcements channel: transcript of 64 messages by the account "fennzar" in
  six date ranges between 2024-06-10 and 2025-10-31, taken on 2026-11-14 from the rendered channel
  (timestamps and message ids from the page). The published copy leaves out the twelve
  announcements of July to October 2025, none of which is cited.
- A second Discord transcript of the same date: the channels "audit-log-updates" (whole channel,
  25 messages, 2025-04-09 to 2025-05-16) and "minor-announcements" (2025-04-02 to 2025-07-31, 14
  messages), and a server search for messages by "fennzar" containing MEXC, unauditable, "range
  proof", morph or blacklist (36 further messages, mostly from the general channel). Discord's
  search may be incomplete. The published copy keeps the two channels in full and, of the search,
  the seven cited messages.
- A third Discord transcript, taken on 2026-11-14 for one question: what the project has said
  about an external audit. It holds a server search for six terms (Sherlock, "bridge audit",
  "audit contest", "security audit", auditor, "audited by"), the same search limited to the
  account "fennzar", and the channels "announcements", "minor-announcements" and
  "bridge-testing" read from 2025-08-01. Messages by the accounts "fennzar" and "hydratedGoose"
  (shown with the team role "Marketing Lead") were copied; messages by other members were
  counted, not copied. The published copy holds the four cited items.
- A fourth Discord transcript, taken on 2026-11-14 for one question: whether anyone has reviewed
  or audited the node software. It holds a server search for fifteen terms ("code review", "code
  audit", "audit of the code", "audited code", "third party audit", "3rd party", "external
  audit", "peer review", "reviewed by", "bug bounty", "security review", codebase, the names of
  two audit firms, "Haven audit"), the same searches limited to the account "fennzar", three
  searches for the review of the codebase promised on 2025-01-06, and the channel
  "announcements" read from 2025-01-06 to the end of 2025 (34 messages). It also holds four
  searches of the X account and six pages of the project's website. Messages by "fennzar",
  "hydratedGoose" and accounts shown with the role "Mod" or "X-Desk" were copied; messages by other members
  were counted, not copied. Two of the searches were read to their sixtieth result only. The
  published copy holds the one cited item.
- The project's X account @zephyr_org: transcript of 173 post entries taken on 2026-11-14 from
  the rendered pages (timestamps from the pages' `datetime` attribute, UTC). It holds seven given
  posts with the @zephyr_org posts shown with them, and four searches of the account: all posts
  from 2024-12-10 to 2025-02-28 (23 results) and from 2025-03-20 to 2025-07-15 (54 results);
  posts from 2025-07-15 to 2025-12-31 containing audit, supply, MEXC, "range proof", morph,
  inflation, exploit or mint (14 results); posts since 2024-09-01 containing v2.0.0, hardfork or
  "hard fork" (36 results). Videos were read from their first frame only; replies by the account
  under other accounts' posts may be missing, and X's search may be incomplete. The published
  copy holds the cited posts with their context: the post-audit report in full, the history
  chapter of the overview article, the cited question and answer of the AMA article, the whole
  thread of 2025-06-15, and each reply together with the post it answers.

Messages and posts cited, by date. A Discord link opens only for members of the project's server,
and X does not show every post to visitors without an account; the transcripts are there for
that reason. Inside a quotation the source's own quotation marks are shown as single quotes,
its bold and italics are not reproduced, and omissions are marked with "..." or "[...]".

| Date | Where | Link | Cited in |
|---|---|---|---|
| 2024-12-29 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1322858944879792189) [2](https://discord.com/channels/1112605073948819559/1112605856488493176/1322858954241478678) | §6.1, §6.1a, §7 |
| 2024-12-30 | Discord, general | [1](https://discord.com/channels/1112605073948819559/1112605075416817686/1323101153449480293) | §7 |
| 2025-01-02 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1324168891647397948) | §6.1 (first of five messages) |
| 2025-01-06 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1325691467523686452) [2](https://discord.com/channels/1112605073948819559/1112605856488493176/1325691481251512331) | §5.1, §6.1, §6.1a |
| 2025-01-06 | X | [1](https://x.com/zephyr_org/status/1876209588460249297) | §4.1, §6.1a, §8 |
| 2025-01-13 | Discord, general | [1](https://discord.com/channels/1112605073948819559/1112605075416817686/1328217871263731722) | §4.1, §5.1, §6.1, §8 |
| 2025-01-20 | Discord, general | [1](https://discord.com/channels/1112605073948819559/1112605075416817686/1330843937648410635) [2](https://discord.com/channels/1112605073948819559/1112605075416817686/1330845887580340264) | §6.1, §6.4 |
| 2025-01-21 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1331136889872191541) | §6.1 |
| 2025-01-23 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1331846800767713281) [2](https://discord.com/channels/1112605073948819559/1112605856488493176/1331846830748864512) [3](https://discord.com/channels/1112605073948819559/1112605856488493176/1331846862793084959) | §1, §4.2, §5.1 to §5.3, §6.1, §6.1a |
| 2025-02-19 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1341686311936987176) | §6.1 |
| 2025-04-14 | Discord, general | [1](https://discord.com/channels/1112605073948819559/1112605075416817686/1361284789159858207) [2](https://discord.com/channels/1112605073948819559/1112605075416817686/1361290568507985970) | §6.4 |
| 2025-05-03 | Discord, audit-log-updates | [1](https://discord.com/channels/1112605073948819559/1359471488364970104/1368212122236096554) | §6.3 |
| 2025-05-07 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1369634054516899870) [2](https://discord.com/channels/1112605073948819559/1112605856488493176/1369634061143900181) | §6.1, §6.3, §6.4, §6.5 |
| 2025-05-09 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1370262965470629978) | §6.1, §6.3, §6.4 |
| 2025-05-16 | Discord, audit-log-updates | [1](https://discord.com/channels/1112605073948819559/1359471488364970104/1372889014855663696) | §6.3 |
| 2025-05-21 | Discord, minor-announcements | [1](https://discord.com/channels/1112605073948819559/1179739666434498570/1374653773690966110) | §6.1, §6.4, §8 |
| 2025-05-24 | Discord, general | [1](https://discord.com/channels/1112605073948819559/1112605075416817686/1375777565796466790) | §6.4 |
| 2025-05-27 | Discord, minor-announcements | [1](https://discord.com/channels/1112605073948819559/1179739666434498570/1376889077193769122) | §6.1, §6.4 |
| 2025-06-01 | Discord, minor-announcements | [1](https://discord.com/channels/1112605073948819559/1179739666434498570/1378565857126387773) | §6.1, §6.4 |
| 2025-06-11 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1382206300896628746) | §1, §6.1, §6.3, §6.4, §6.5 |
| 2025-06-11 | X, post and a reply | [1](https://x.com/zephyr_org/status/1932718400146448736) [2](https://x.com/zephyr_org/status/1932786647004840331) | §6.1, §6.2 |
| 2025-06-15 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1383781909716271154) | §6.1, §6.3 |
| 2025-06-15 | X, second post of a thread of eight | [1](https://x.com/zephyr_org/status/1934252619540664495) | §6.1, §6.3 |
| 2025-06-16 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1383992838965952655) | §6.1, §6.4 |
| 2025-06-17 | Discord, minor-announcements | [1](https://discord.com/channels/1112605073948819559/1179739666434498570/1384673840084418620) | §6.1, §6.4 |
| 2025-07-28 | X, overview article | [1](https://x.com/zephyr_org/status/1949870293406666898) | §6.1, §6.3, §8 |
| 2025-08-25 | X, AMA article | [1](https://x.com/zephyr_org/status/1960023215444488459) | §1, §3, §6.1, §6.1a, §8 |
| 2025-09-06 | X, post-audit report | [1](https://x.com/zephyr_org/status/1964371016303407282) | §1, §4.1, §5.2, §6.1, §6.3, §6.5 |
| 2026-04-13 | X, reply | [1](https://x.com/zephyr_org/status/2043732782224957593) | §6.1, §6.3 |
| 2026-07-08 | Discord, general | [1](https://discord.com/channels/1112605073948819559/1112605075416817686/1524482538028925000) | §1, §8 |
| 2026-07-14 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1526384501843034273) | §6.1, §8 |
| 2026-07-28 | Discord, community-funding | [1](https://discord.com/channels/1112605073948819559/1439644649265303665/1531604756433469480) | §8 |
| 2026-08-10 | Discord, announcements | [1](https://discord.com/channels/1112605073948819559/1112605856488493176/1536191562755608637) | §6.1, §8 |

Not read, and not used: the project's articles on Medium; ten of the thirteen posts on its website's blog (two of them were searched for one question, see above); the rest of its X account.

Not consulted: Telegram, Reddit and bitcointalk discussions; the rest of Zephyr's Discord (the general chat was only searched, not read); the parties themselves.
