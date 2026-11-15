#!/usr/bin/env python3
"""Renders the reports of this repository as a small static site.

  site/build.py [repository-root] [output-dir]      defaults: the parent of site/, <root>/docs

The Markdown files stay the only source: REPORT.md becomes index.html, REPORT-FULL.md becomes
full.html and README.md becomes verify.html. The pages use one stylesheet, no scripts and nothing
loaded from elsewhere. Needs Python 3 with the "markdown-it-py" package, which reads Markdown
the way GitHub does.
"""
import html
import os
import re
import shutil
import sys

from markdown_it import MarkdownIt

REPO = "https://github.com/loki-notfound/zephyr-supply-review"
PAGES = [  # source, output, navigation label
    ("REPORT.md", "index.html", "Summary"),
    ("REPORT-FULL.md", "full.html", "Full report"),
    ("README.md", "verify.html", "Check it yourself"),
]
OUT = {src: out for src, out, _ in PAGES}
PARTS = [  # the full report in parts: title, what it holds, ids of its sections
    ("Question and findings", "The question, the answer in short, the evidence behind each finding, and what stays open.", ["s1", "s1a", "s1b"]),
    ("Method and origin", "How the checks were made, their limits, and where the code comes from.", ["s2", "s3"]),
    ("The defects and the chain", "The two defects in the code and the transactions that used them.", ["s4", "s5"]),
    ("The audit and the supply", "Timeline, how the audit worked, the reconciliation and the one-time mint.", ["s6"]),
    ("Context and handling", "Haven, how the incident was handled, and the code today.", ["s7", "s8", "s9"]),
    ("Reproduction and sources", "The tools, their results, and every text that is quoted.", ["s10", "s11"]),
]
SECTION = re.compile(r"^(\d+[a-z]?)\.\s|^(\d+\.\d+[a-z]?)\s")
BASIS = re.compile(r"\[((?:chain|code|stated)(?:[,;][^\]\[]*)?)\]")
SECTION_REF = re.compile(r"§(\d+(?:\.\d+)?[a-z]?)")


def slug(value, separator):
    """Numbered headings get the id of their number (6.3 -> s6-3), others a plain slug."""
    m = SECTION.match(value)
    if m:
        return "s" + (m.group(1) or m.group(2)).replace(".", "-")
    return re.sub(r"[^a-z0-9]+", separator, value.lower()).strip(separator)


def text_nodes(page, fn):
    """Applies fn to the text between tags, leaving out links, code and preformatted text."""
    out, skip = [], 0
    for part in re.split(r"(<[^>]+>)", page):
        if part.startswith("<"):
            name = re.match(r"</?([a-zA-Z0-9]+)", part)
            if name and name.group(1) in ("a", "code", "pre"):
                skip += -1 if part.startswith("</") else 1
            out.append(part)
        else:
            out.append(part if skip else fn(part))
    return "".join(out)


def link(target, page_out):
    """Rewrites a link of the Markdown sources to where it lives on the site."""
    if re.match(r"^[a-z]+:|^#", target):
        return target
    path, _, frag = target.partition("#")
    if path in OUT:
        return ("" if OUT[path] == page_out and frag else OUT[path]) + ("#" + frag if frag else "")
    return f"{REPO}/blob/main/{path}" + ("#" + frag if frag else "")


def headings(body):
    """Gives every h2 and h3 an id and returns the page with a nested table of contents."""
    toc, seen = [], set()

    def add(m):
        level, inner = int(m.group(1)), m.group(2)
        plain = html.unescape(re.sub(r"<[^>]+>", "", inner))
        sid = slug(plain, "-")
        while sid in seen:
            sid += "-"
        seen.add(sid)
        name = html.escape(BASIS.sub("", plain).strip())
        if level == 2 or not toc:
            toc.append((sid, name, []))
        else:
            toc[-1][2].append((sid, name, []))
        return f'<h{level} id="{sid}">{inner}</h{level}>'
    return re.sub(r"<h([23])>(.*?)</h\1>", add, body, flags=re.S), toc


def toc_html(entries):
    if not entries:
        return ""
    items = "".join(f'<li><a href="#{sid}">{name}</a>{toc_html(sub)}</li>' for sid, name, sub in entries)
    return f"<ol>{items}</ol>"


def parts(toc):
    """The contents of the full report grouped into parts: for the sidebar and as an index."""
    by_id = {sid: (sid, name, sub) for sid, name, sub in toc}
    side, cards = "", ""
    for n, (title, what, ids) in enumerate(PARTS, 1):
        entries = [by_id[i] for i in ids if i in by_id]
        side += f'<p class="part">Part {n} · {title}</p>{toc_html(entries)}'
        links = "".join(f'<li><a href="#{sid}">{name}</a></li>' for sid, name, _ in entries)
        cards += (f'<section><p class="part">Part {n}</p><h3>{title}</h3><p>{what}</p><ul>{links}</ul></section>')
    return side, f'<nav class="parts" aria-label="Parts of this report">{cards}</nav>'


def render(root, src, out, sections):
    text = open(os.path.join(root, src), encoding="utf-8").read()
    text = re.sub(r"<!--.*?-->\n?", "", text, flags=re.S)
    body, toc = headings(MarkdownIt("commonmark").enable("table").render(text))
    title = re.search(r"<h1[^>]*>(.*?)</h1>", body, re.S)
    title = re.sub(r"<[^>]+>", "", title.group(1)) if title else src
    body = re.sub(r'href="([^"]+)"', lambda m: f'href="{html.escape(link(html.unescape(m.group(1)), out))}"', body)
    body = body.replace("<table>", '<div class="table"><table>').replace("</table>", "</table></div>")
    # short cells that hold only a date, a time or a number stay on one line
    body = re.sub(r"<td>(\s*[0-9][0-9 :.,/−+-]{0,18}\s*)</td>", r'<td class="nw">\1</td>', body)
    body = text_nodes(body, lambda s: BASIS.sub(r'<span class="basis">\1</span>', s))

    def ref(m):
        sid = "s" + m.group(1).replace(".", "-")
        if sid not in sections:
            return m.group(0)
        return f'<a class="ref" href="{"" if out == "full.html" else "full.html"}#{sid}">{m.group(0)}</a>'
    body = text_nodes(body, lambda s: SECTION_REF.sub(ref, s))
    return title, body, toc


def page(title, body, toc, out):
    nav = "".join(
        f'<a href="{o}"{" aria-current=\"page\"" if o == out else ""}>{label}</a>' for _, o, label in PAGES)
    side = toc_html([(sid, name, []) for sid, name, _ in toc])
    if out == "full.html":
        side, index = parts(toc)
        first = body.index("<h2")  # the index of parts goes in front of the first section
        body = body[:first] + index + body[first:]
    return f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>{html.escape(title)}</title>
<meta name="description" content="A review of the Zephyr Protocol supply after the 2025 chain audit: what the public code and the public chain show.">
<meta name="referrer" content="no-referrer">
<link rel="stylesheet" href="style.css">
</head>
<body class="{out.split('.')[0]}">
<a class="skip" href="#content">Skip to the text</a>
<header class="site">
<a class="name" href="index.html">Zephyr supply review</a>
<nav aria-label="Pages">{nav}<a href="{REPO}">Repository</a></nav>
</header>
<div class="layout">
<aside>
<details class="toc"><summary>On this page</summary>{side}</details>
<nav class="toc" aria-label="On this page"><p class="label">On this page</p>{side}</nav>
</aside>
<main id="content">
{body}
</main>
</div>
<footer class="site">
<p>Reports: <a href="{REPO}/blob/main/LICENSE-REPORTS.md">CC BY 4.0</a>. Programs: <a href="{REPO}/blob/main/LICENSE">BSD 3-Clause</a>.
Corrections of fact are welcome: <a href="{REPO}/issues">open an issue</a>.</p>
</footer>
</body>
</html>
"""


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.abspath(sys.argv[1]) if len(sys.argv) > 1 else os.path.dirname(here)
    dst = os.path.abspath(sys.argv[2]) if len(sys.argv) > 2 else os.path.join(root, "docs")
    os.makedirs(dst, exist_ok=True)
    full = open(os.path.join(root, "REPORT-FULL.md"), encoding="utf-8").read()
    sections = {slug(h, "-") for h in re.findall(r"(?m)^#{2,3} (.+)$", full) if SECTION.match(h)}
    for src, out, _ in PAGES:
        title, body, toc = render(root, src, out, sections)
        with open(os.path.join(dst, out), "w", encoding="utf-8") as f:
            f.write(page(title, body, toc, out))
    shutil.copyfile(os.path.join(here, "style.css"), os.path.join(dst, "style.css"))
    open(os.path.join(dst, ".nojekyll"), "w").close()
    print(f"site written to {dst}")


main()
