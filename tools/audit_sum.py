"""Sum the public amounts of all audit transactions (blocks 481500 to 535999) per asset.

Usage: audit_sum.py <work-dir> [http://node:17767]   (a restricted RPC node is enough)
Resumable: hashes and per-transaction results are cached in <work-dir>. Writes the list of
transactions to <work-dir>/audit-transactions.tsv and prints the summary.
"""
import json,urllib.request,collections,sys,time,os
S=sys.argv[1]; N=sys.argv[2] if len(sys.argv) > 2 else "http://127.0.0.1:17767"
def post(path,d,tries=8):
    for a in range(tries):
        try:
            return json.load(urllib.request.urlopen(urllib.request.Request(N+path,data=json.dumps(d).encode(),headers={'Content-Type':'application/json'}),timeout=180))
        except Exception as e:
            time.sleep(3+a*5); err=e
    raise err
def rpc(m,p): return post("/json_rpc",{"jsonrpc":"2.0","id":"0","method":m,"params":p})['result']
A,B=481500,535999
hf=S+"/audit_hashes.json"
if os.path.exists(hf): hashes=json.load(open(hf))
else:
    blocks=[]
    for s in range(A,B+1,1000):
        for h in rpc("get_block_headers_range",{"start_height":s,"end_height":min(s+999,B)})['headers']:
            if h['num_txes']>0: blocks.append((h['height'],h['major_version']))
    hashes=[]
    for h,v in blocks: hashes+=[[h,v,x] for x in rpc("get_block",{"height":h})['tx_hashes']]
    json.dump(hashes,open(hf,"w"))
print("txs",len(hashes),flush=True)
rf=S+"/audit_txs.jsonl"; done={}
if os.path.exists(rf):
    for l in open(rf):
        r=json.loads(l); done[r['tx']]=r
out=open(rf,"a")
todo=[c for c in hashes if c[2] not in done]
print("todo",len(todo),flush=True)
def handle(ch):
    r=post("/get_transactions",{"txs_hashes":[c[2] for c in ch],"decode_as_json":True,"prune":True})
    assert len(r['txs'])==len(ch)
    for (h,v,x),t in zip(ch,r['txs']):
        j=json.loads(t['as_json'])
        rec={"tx":x,"h":h,"hf":v,"src":sorted({list(i_.values())[0]['asset_type'] for i_ in j['vin']}),
             "dst":sorted({o['target']['tagged_key']['asset_type'] if 'tagged_key' in o['target'] else '?' for o in j['vout']}),
             "burnt":j['amount_burnt'],"minted":j['amount_minted'],"nin":len(j['vin']),"fee":j['rct_signatures'].get('txnFee',0)}
        out.write(json.dumps(rec)+"\n"); done[x]=rec
for i in range(0,len(todo),4):
    ch=todo[i:i+4]
    try: handle(ch)
    except Exception:
        for c in ch: handle([c])
    if i%2000==0: out.flush(); print("read",i,flush=True)
out.close()
# only the transactions of the requested range count, whatever else the cache holds
wanted={c[2] for c in hashes}
missing=wanted-set(done)
assert not missing, f"{len(missing)} transactions could not be read"
done={x:r for x,r in done.items() if x in wanted}
cnt=collections.Counter();burn=collections.Counter();mint=collections.Counter();perhf=collections.Counter();odd=[];fee=collections.Counter()
for r in done.values():
    k="+".join(r['src'])+"->"+"+".join(r['dst']); cnt[k]+=1;burn[k]+=r['burnt'];mint[k]+=r['minted'];perhf[(r['hf'],k)]+=r['minted'];fee[k]+=r['fee']
    if r['burnt']!=r['minted']: odd.append(r)
for k in sorted(cnt): print(f"{k:22s} txs {cnt[k]:6d} burnt {burn[k]/1e12:18,.4f} minted {mint[k]/1e12:18,.4f} fees {fee[k]/1e12:,.4f}")
print("per HF:",{f"{k[0]}:{k[1]}":round(v/1e12,2) for k,v in sorted(perhf.items())}); print("burnt!=minted:",len(odd),odd[:3])
EXT=511000  # hard fork 10: the extension of the audit window
rows=sorted(done.values(),key=lambda r:(r['h'],r['tx']))
with open(S+"/audit-transactions.tsv","w") as f:
    f.write("height\thard_fork\ttransaction\tasset\tamount_atomic\tfee_atomic\n")
    for r in rows: f.write(f"{r['h']}\t{r['hf']}\t{r['tx']}\t{'+'.join(r['src'])}\t{r['minted']}\t{r['fee']}\n")
for a in sorted({r['src'][0] for r in rows}):
    v=[r for r in rows if r['src'][0]==a]; ext=sum(r['minted'] for r in v if r['h']>=EXT)
    print(f"{a:8s} txs {len(v):6d} amount {sum(r['minted'] for r in v)/1e12:18,.2f} of which from block {EXT} on {ext/1e12:14,.2f} fees {sum(r['fee'] for r in v)/1e12:,.2f}")
z=sorted((r['minted'] for r in rows if r['src'][0]=="ZEPH"),reverse=True); t=sum(z)
print(f"ZEPH: largest transaction {z[0]/1e12:,.2f}; ten largest {100*sum(z[:10])/t:.1f} %; hundred largest {100*sum(z[:100])/t:.1f} % of the audited amount")
print("first and last block with an audit transaction:",rows[0]['h'],rows[-1]['h'])
print("DONE")
