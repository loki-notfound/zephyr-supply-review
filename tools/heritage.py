#!/usr/bin/env python3
"""Share of the lines a commit adds that occur verbatim in another repository.

  heritage.py <repo> <commit> <other-repo> <other-commit> [file...]

Counts the lines that <commit> adds to C/C++ sources under src/ (against its first parent),
after collapsing whitespace, leaving out lines shorter than 25 characters and comment lines, and
leaving out lines that already occur anywhere in the parent's sources (for Zephyr's first commit
those are Monero's). A line counts as shared if the same normalised line occurs anywhere in the
sources of <other-commit>. With file arguments only those files are counted.
"""
import re, subprocess, sys, collections

def git(repo, *a):
    return subprocess.run(["git", "-C", repo, *a], check=True, capture_output=True).stdout.decode("utf-8", "replace")

def norm(line):
    return re.sub(r"\s+", " ", line).strip()

def keep(n):
    return len(n) >= 25 and not n.startswith(("//", "/*", "*"))

SRC = re.compile(r"^src/.*\.(cpp|h|hpp|c|inl)$")

def tree_lines(repo, commit):
    out = set()
    for f in git(repo, "ls-tree", "-r", "--name-only", commit).splitlines():
        if SRC.match(f):
            for l in git(repo, "show", f"{commit}:{f}").splitlines():
                n = norm(l)
                if keep(n):
                    out.add(n)
    return out

def main():
    repo, commit, other, ocommit, *only = sys.argv[1:]
    base = tree_lines(repo, commit + "^")
    theirs = tree_lines(other, ocommit)
    added = collections.defaultdict(list)
    cur = None
    for l in git(repo, "diff", "--no-renames", "-U0", commit + "^", commit, "--", "src").splitlines():
        if l.startswith("+++ "):
            cur = l[6:] if l.startswith("+++ b/") else None
        elif l.startswith("+") and cur and SRC.match(cur):
            n = norm(l[1:])
            if keep(n) and n not in base:
                added[cur].append(n)
    tot = hit = 0
    for f in sorted(added, key=lambda f: -len(added[f])):
        if only and f not in only:
            continue
        a = added[f]; h = sum(1 for n in a if n in theirs)
        tot += len(a); hit += h
        print(f"{h:6d} {len(a):6d} {100 * h / len(a):5.1f} %  {f}")
    print(f"{hit:6d} {tot:6d} {100 * hit / max(tot, 1):5.1f} %  total, {len([f for f in added if not only or f in only])} files")

main()
