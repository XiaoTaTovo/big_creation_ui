from pathlib import Path
import concurrent.futures,json,subprocess,os,argparse,shutil
r=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--cc',default=os.environ.get('HOST_CC','gcc'));a=p.parse_args()
cc=shutil.which(a.cc) or a.cc
sources=[s for s in json.loads((r/'tools/sources.json').read_text())['V5F'] if '/lvgl/' in s]
for orientation in [0,1]:
 out=r/'build'/('host-landscape' if orientation else 'host-portrait');out.mkdir(parents=True,exist_ok=True)
 flags=['-std=c99','-O1','-DUI_LANDSCAPE='+str(orientation),'-I'+str(r/'V5F/lvgl'),'-I'+str(r/'V5F/User')]
 def run(args):
  p=subprocess.run(args,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,errors='replace')
  if p.returncode:raise RuntimeError(p.stdout)
  return p.stdout
 def compile(s):
  obj=out/(s.replace('/','_')+'.o');run([cc]+flags+['-c',str(r/s),'-o',str(obj)]);return obj
 with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:objs=list(pool.map(compile,sources+['V5F/User/ui_selftest.c','tests/render_ui.c']))
 rsp=out/'objects.rsp';rsp.write_text('\n'.join('"'+p.as_posix()+'"' for p in objs))
 exe=out/'render.exe';run([cc,'@'+str(rsp),'-o',str(exe),'-lm'])
 print(run([str(exe),str(out/'static.ppm'),str(out/'dynamic.ppm')]))
