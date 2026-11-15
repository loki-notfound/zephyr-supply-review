# Is Zephyr's supply right? What the code and the chain show

*Version 1.0, 2026-11-14. Short version; the [full report](REPORT-FULL.md) has every commit, code
line, transaction id and the tools to repeat each check.*

Nobody asked me to do this. I took it on because one question kept coming back in the
community: is the supply right, has anyone outside the project ever checked it, and does the
audit of the bridge by Sherlock cover that? So I looked at what happened around the 2025 audit. I have
tried to claim nothing I cannot back up. Each point below says where it comes from: the public
**code**, the **chain** itself, or what a project has **said**. Where I only have the project's
word, or could not decide between explanations, I say so. Every quotation comes from
a text that was read in full. Corrections of fact are welcome.

This review establishes what happened and what the supply is. It does not judge who is to
blame.

## The question, and the answer in short

Is the supply of ZEPH right? I split that into five questions that public data can answer.

A few terms first:

- **ZEPH** is the base coin; **ZSD** is the stable coin, **ZRS** the reserve coin and **ZYS** the
  yield coin. One can be converted into another through the protocol, and a conversion states
  its amounts in the open. In an ordinary transfer the amounts are hidden.
- **Emitted** coins are the ones the block rewards have produced, by a fixed schedule.
- A **range proof** is the part of a transaction that proves no coins are created in it.
- A **ring** hides which earlier output a transaction really spends among fifteen decoys.
- A **hard fork** is a change of the rules from a given block on. The **consensus code** is the
  part of the node software that enforces those rules. Zephyr's is built on Monero's.
- The **reserve** is the ZEPH that backs the stable coin. The supply figures here include it.
- Two kinds of audit appear below. The **chain audit** of 2025 counted the coins. A **code
  audit** is a review of the software by outsiders; that is something else.

**1. Can the number the chain shows be relied on?** Yes, as a count, up to block 902083
(2026-11-07), where my copy of the chain ends. Up to that block every transaction passes the
checks that keep a transaction from creating coins, no output was spent twice, and no block
paid its miner more than reward and fees allow. The totals of all four assets agree with the
coins generated and the public conversions at both points where they can be compared: right
after the audit and at that last block. What such checks cannot exclude is a flaw in the rules themselves that nobody knows of
(question 5).

**2. What went wrong before the audit?** Two defects in consensus code that Zephyr took over
from Haven Protocol. Until 2024-10-13 a transaction could create coins, and from 2023-10-01
until 2025-01-23 one asset could be spent as another. Both were used. Both are closed.

**3. What could the audit do, and what could it not?** It made the supply countable for the
first time. It could not separate coins that had been emitted from coins that had not: amounts
and histories are hidden, and the chain offers no way to tell them apart. After the audit the
chain held 2,067,576 ZEPH more than had been emitted: 1,921,650 minted once for an exchange
balance that was not audited in time, and 145,926 among the audited coins. Zephyr announced the
mint and how it is offset: block rewards are about 18 % lower for as long as they follow the
emission curve, about ten more years. By my projection that offsets about 1.8 million of the
mint, not all of it. The 145,926 are not offset.

**4. What stays unknown?** How many coins each defect produced, who made the transactions, and
whether the unaudited exchange balance consisted of emitted coins. The second defect can
explain at most about 262,688 ZEPH of the excess. Of the three transactions with invalid range
proofs Zephyr says itself that it was "not able to determine if or how much" inflation came
from them.

**5. Has anyone outside the project reviewed the code of the node?** Not that I could find. In
2025 Zephyr said no third-party review was planned. The audit by Sherlock that it commissioned in 2026 covers the bridge:
the scope Zephyr published names the bridge's smart contracts and off-chain services, not the
node. Asked about it in July 2026, Zephyr's marketing lead wrote: "Scoped to the EVM bridge
only ... Not the L1" (the L1 is Zephyr's own chain), and that a review of it is "a reasonable
step" as the protocol grows. This review is not a code audit either: it checks the chain against the rules as they are
written.

**In short:** up to the block I checked, the number the chain shows is a sound count. At the
end of the audit (block 536000) it was 2,067,576 ZEPH higher than ordinary emission to that
block: 1,921,650 that Zephyr minted for an exchange balance that was not audited, for the larger
part offset by lower block rewards in the years after, and 145,926 among the audited coins,
which the audit had no way to tell from the rest.

## The findings behind the answers

**1. Zephyr's asset code came from Haven Protocol** (code). Of the non-Monero lines in Zephyr's
first commit (2023-05-26), 38 % are word for word Haven's code of that day. The Djed rules on
top are Zephyr's own. Zephyr has said the same about the defects: "both Zephyr exploits were
inherited from "audited" Haven code portions" (X, 2025-08-25).

**2. Two consensus defects** (code):

- **A. Range proofs were not checked** from launch (2023-05-29) until hard fork 6 (2024-10-13).
  A range proof is what stops a transaction from creating coins. The omission is already in
  Haven's code a month before Zephyr copied it; Haven fixed the same function on 2024-12-11
  under the title "fix range proof exploit". I confirmed it by running the code: the old check
  accepts a transaction with a deliberately broken proof, the new one rejects it.
- **B. Ring members could be of another asset** from hard fork 3 (2023-10-01) until hard fork 7
  (2025-01-23). An output of the stable coin could be spent as the same number of ZEPH ("asset
  morphing").

**3. Both were used** (chain). I scanned every transaction from genesis to block 902083:
1,265,146 transactions, 128 million ring members, every range proof.

- Exactly **three transactions with invalid range proofs**, all on 2024-07-07 within six
  minutes (blocks 289260 and 289262). Zephyr reported the same: "the three suspect
  transactions from 6 months ago" (2025-01-06), which "fail the updated range proofs code"
  (2025-01-23).
- **Fourteen transactions** in which a ZEPH input has a ring made only of stable-coin outputs,
  from 2023-10-29 to 2024-09-30. A stable-coin output was necessarily spent as ZEPH there.
- **Sixteen more transactions** with one or two foreign ring members. The foreign member may be
  a decoy picked across assets, which harms nobody, or the output that was really spent, which
  is the abuse. In ten of them every ring of the transaction ends a few blocks after hard fork
  3, although the transaction is up to a month later; that shows an unusual way of picking ring
  members, not which of the two it was. I cannot tell for any of the sixteen.
- That is 30 transactions in all, the number Zephyr gave on 2025-01-23 for transactions "with
  cross-asset ring members".
- Nothing after the fixes.
- I also re-ran, on every transaction, the node's checks on range proof, balance, conversion
  amount and ring signatures, checked that no output was spent twice, and checked the miner
  payout of every block. Nothing fails.

Amounts are hidden, so the chain shows that it happened and when, not how much or by whom.

**4. How the fixes were made** (code, release notes, Discord). Defect A was closed in v2.0.0 by a
new verification function. The release notes present that release as the yield feature and
mention no security fix. Zephyr's own account (2025-01-13): the three transactions "fail an
updated range proofs check ... introduced in Zephyr v2.0.0 as we refactored that section of the
code", and the earlier check "was incorrect, an oversight on our part". So by its account the
gap was closed as a by-product, and the three transactions were noticed only after Haven's
exploit. The code is consistent with that. Defect B was closed by the v2.0.2 hotfix and explained
two days later: reported by an anonymous developer, 30 transactions, and "our best educated
guess" of about 200,000 ZEPH of inflation.

**4a. What Zephyr said beforehand** (Discord).

- 2024-12-29: it had "identified three transactions in Zephyr that align with suspicious
  activity", this "does not mean Zephyr was successfully exploited", and it did not know whether
  "the invalid range-proof issue that affected 50 Haven transactions is exactly the same in
  kind". It is the same: same omission, same function (points 1 to 3). The text also says the
  protocol "was written by the Zephyr team" and "diverges significantly from Haven's
  implementation". That holds for the Djed rules, not for the verification code at issue.
- 2025-01-06: exchanges were asked to stop deposits. It was "NOT possible that millions of
  excess ZEPH has been sold off on exchanges"; exchange balances were "expected and reasonable,
  all totaling to well under our reported supply of 6.4M". In the worst case "a necessarily
  limited amount" had been sold, and amounts traced to the three transactions would be
  blacklisted. The released code has no blacklist.
- That 6.4 million is the number of coins generated by that date. No count of coins in wallets
  existed before the audit.

**5. The audit** (code, chain). From 2025-03-31 to 2025-06-15 normal transfers were off and
every holder had to move each asset once with the amount in the open. Coins not moved became
invalid. I summed all 15,040 audit transactions: 5,164,648 ZEPH, 1,023,509 ZRS, 88,315 ZSD and
185,474 ZYS.

This method counts the coins that were presented. It cannot tell which of them had been
emitted, and with hidden amounts and histories no audit of this kind can.

What the chain held right after the audit is these audited coins plus three other things:
5,164,648 ZEPH audited, 2,289,862 in the reserve, 373,779 paid to miners while the audit ran,
and 1,921,657 paid in block 536000 (the one-time mint and that block's reward). Together
9,749,946 ZEPH, the first line of the table below.

**6. The supply** (code, chain).

| | ZEPH |
|---|---|
| On chain right after the audit (block 536000) | 9,749,946 |
| Emitted up to that block, without the mint | 7,682,370 |
| **More than was emitted** | **2,067,576** |
| of which a one-time mint in block 536000 | 1,921,650 |
| of which excess in the audited supply itself | 145,926 |

- Zephyr's own numbers agree. On 2025-06-11 it estimated +2,056,803 ZEPH against the reported
  supply, and on 2025-06-15 it published 9,750,021 ZEPH at block 536010, the same total as mine
  by a different route (145,925.75 ZEPH above emission either way).
- The 2,067,576 is a floor. Genuine coins that were never audited make the visible excess
  smaller.
- Both parts rest on public data. The mint is a constant in the code and an output on the
  chain. The 145,926 depends on the reserve, which I rebuilt from block 0 from the public
  conversion amounts: it matches the node's counter to within 0.001 ZEPH at every point where
  the two can be compared, including the value the project set by hand in 2024.

**6a. Which defect explains it** (Zephyr's figures, and my inference). Morphing turns one unit
of another asset into one ZEPH, so every morphed unit is missing from that asset at the audit.
Rebuilt from the chain, 262,688 units of the other assets are missing after the audit (Zephyr's
own estimate was 256,634). Defect B can therefore explain at most about that much, and Zephyr's
own guess is about 200,000. At least 1.8 million of the excess is not explained by defect B.
Defect A is the only other known cause, but the chain does not show what the three transactions created, so I do not
attribute the rest to them. Zephyr's post-audit report (X, 2025-09-06) leaves it open as well: "We
were not able to determine if or how much net chain-wide inflation originated from this vector,
but the possibility that this contributed could also not be ruled out."

**7. The one-time mint** (code, chain, Discord).

- Block 536000 pays 1,921,650 ZEPH to an address that the project fixed in the source code.
- Zephyr announced it with the amount on 2025-06-11: "Due to MEXC being unable to audit their
  entire wallet balance, we are including a mint for their unauditable balance (1,921,650 ZEPH)
  in this Hardfork." Earlier it had written that MEXC "has yet to audit all of their coins"
  (2025-05-07) and that these "are user funds" (2025-05-09).
- Why it was not audited, per Zephyr: MEXC ran the audit command for days without it finishing,
  "some limitation in their server environment", and "strict OpSec restrictions" ruled out the
  workarounds. Zephyr called the mint "a last resort".
- How the amount was checked, per Zephyr (2025-06-17): "I received their exchange wallet seed to
  confirm the balance. I have now done so and paid them to their new exchange wallet." The
  screenshot shows 1,921,649.73 ZEPH, unaudited. The amount was already in the release, six
  days before that check; Zephyr does not say how it was supplied.
- All of this rests on Zephyr's statements. Nothing on the chain names an exchange, and I cannot
  verify the wallet, the screenshot or the payout.
- The mint was booked as emission, as Zephyr announced: "we are adjusting our emission schedule
  to reflect this", so that the 18.4 million limit remains (the whitepaper: "Total Supply:
  18.4 Million (+ tail emission 0.6 ZEPH p/block)"). Block rewards have been about 18 %
  lower since: miners, the reserve share and the yield share receive less, while the minted
  coins stay in the supply. The lower rewards last until the fixed tail reward of 0.6 ZEPH per
  block is reached, about ten years after the audit. By my projection they offset about 1.8
  million of the mint; about 124,000 of it stays above the original schedule for good.

**8. Where the extra coins were** I cannot say, and I do not want to guess. Either the unaudited
exchange balance was mostly made of them and the audited coins were nearly all genuine, or that
balance was genuine and about two million extra coins were among what others audited, or
something in between. Both cases take Zephyr's word that the mint replaced an existing balance.
An exchange holds what its customers deposit; nothing here establishes anything about the
exchange's conduct.

## Haven, where the code came from (code)

- Haven's repository has the same omission from 2023-04-25 and corrects it on 2024-12-11
  ("fix range proof exploit"), ten weeks after Zephyr's correction was written and eight after
  it took effect. That is the last change on the main branch of Haven's repository; nothing in it
  is newer than that day.
- Haven's code has a supply audit of its own, in force from 2024-11-19, in which holders move
  funds through an audit transaction and old funds become unspendable. Zephyr's audit has the
  same shape but its own code: 2 of the 591 lines it adds occur in Haven.
- I did not scan Haven's chain, and I leave out what I could not read at the source about Haven
  as a project or about other chains.

## What I could not establish

- How many coins each defect produced, and who was behind the transactions.
- The intent behind the fix of defect A; I have Zephyr's account and the code, which agree.
- Anything about the MEXC balance beyond Zephyr's own statements.
- How many holders lost genuine coins by missing the audit.
- Whether anyone outside the project has reviewed the code of the node. Asked about a
  third-party review, Zephyr answered "None are planned, but they aren't ruled out!" (X,
  2025-08-25). The audit by Sherlock of 2026 concerns the bridge, and Zephyr's public
  repositories of the node and the wallets hold no audit report and no reference to one.
- That there was no third defect. I looked for these two and re-ran the checks named under point 3; a
  flaw in those checks themselves, like A and B, would not show up that way.
- What Zephyr said in places I did not read. The full report lists every source I used and
  what I left unread.

## Checking my work

Every finding can be repeated with a synced v2.3.0 node and the public repositories. My copy of
the chain ends at block 902083 (2026-11-07); it is identical, by block hash at that block, to
the chain of a node running the official v2.3.0 binary. The full report lists the transaction ids, the code lines with
links, the programs and their results.

<!-- note:made-with -->
I made this review with AI assistance (Claude Fable 5.2, Anthropic); the
[README](README.md#how-this-was-made) says how. That is why every finding comes with the means
to repeat it.
<!-- /note:made-with -->

## Author's note

The review above sticks to what can be shown and does not judge. This note is my opinion, and
only that.

I think the team handled a bad situation well. Two defects in inherited code had been live for
well over a year. Both are closed. Once the scope became clear, the team asked exchanges to
stop deposits and ran a migration that gave the chain a supply that can be counted. They published
figures along the way, credited the developer who found the second defect, announced the
mint before it happened and wrote their own postmortem. A small team did this in a little
over five months. It could have ended far worse.

It was not flawless. Ordinary transfers were off for about 76 days, some early statements were
more certain than the evidence allowed, and a part of the excess is still unexplained. As far
as I could find, nobody outside the project has reviewed the node's code, and I hope that
follows.

I should say where I stand. I bought a small amount of ZEPH in 2023, lost sight of the project
and missed the audit, so those coins are gone. None of that changes how I see it.

If I had to grade them, I would give them a B to B− overall.
