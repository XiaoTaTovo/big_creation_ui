"""Standalone build; never programs or resets a board."""
from pathlib import Path
import argparse, concurrent.futures, json, os, shutil, subprocess
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser()
p.add_argument('--orientation',choices=['portrait','landscape'],required=True)
p.add_argument('--toolchain',help='WCH GCC12 bin directory')
p.add_argument('--jobs',type=int,default=8)
a=p.parse_args()
def find_tools():
    if a.toolchain: return Path(a.toolchain)
    if os.environ.get('WCH_GCC_BIN'): return Path(os.environ['WCH_GCC_BIN'])
    found=shutil.which('riscv-wch-elf-gcc')
    if found: return Path(found).parent
    local=Path('D:/software/Code/MounRiver_Studio3/resources/app/resources/win32/components/WCH/Toolchain/RISC-V Embedded GCC12/bin')
    if local.exists(): return local
    raise SystemExit('Set WCH_GCC_BIN or --toolchain to WCH GCC12 bin directory')
tools=find_tools(); suffix='.exe' if os.name=='nt' else ''
def tool(name):return str(tools/('riscv-wch-elf-'+name+suffix))
out=ROOT/'build'/a.orientation;out.mkdir(parents=True,exist_ok=True)
flags=['-march=rv32imac_zba_zbb_zbc_zbs_xw','-mabi=ilp32','-msmall-data-limit=8','-msave-restore','-Os','-g','-ffunction-sections','-fdata-sections','-fno-common','-fsigned-char','-Wall','-Wextra','-Werror=implicit-function-declaration']
manifest=json.loads((ROOT/'tools/sources.json').read_text(encoding='utf-8'))
def run(args):
    result=subprocess.run(args,cwd=ROOT,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,errors='replace')
    if result.returncode:raise RuntimeError(result.stdout)
    return result.stdout
for core in ['V3F','V5F']:
    dest=out/core;dest.mkdir(exist_ok=True)
    includes=['Common/Debug','Common/Core','Common/Peripheral/inc',core+'/User']
    if core=='V5F':includes+=['V5F/lvgl']
    common=flags+['-DCore_'+core,'-DUI_LANDSCAPE='+str(int(a.orientation=='landscape'))]+['-I'+str(ROOT/x) for x in includes]
    def compile_one(src):
        obj=dest/(src.replace('/','_')+'.o')
        mode=['-x','assembler-with-cpp'] if src.endswith('.S') else ['-std=gnu99']
        log=run([tool('gcc')]+common+mode+['-c',str(ROOT/src),'-o',str(obj)])
        return obj,log
    with concurrent.futures.ThreadPoolExecutor(max_workers=a.jobs) as pool:
        items=list(pool.map(compile_one,manifest[core]))
    elf=dest/(core+'.elf');script=ROOT/core/'Ld'/('Link_'+core.lower()+'.ld')
    rsp=dest/'objects.rsp';rsp.write_text('\n'.join('"'+obj.as_posix()+'"' for obj,_ in items))
    log=run([tool('gcc')]+flags+['-T',str(script),'-nostartfiles','-Wl,--gc-sections','-Wl,-Map,'+str(dest/(core+'.map')),'--specs=nano.specs','--specs=nosys.specs','-o',str(elf),'@'+str(rsp),'-lm'])
    for fmt,ext in [('binary','bin'),('ihex','hex')]:run([tool('objcopy'),'-O',fmt,str(elf),str(dest/(core+'.'+ext))])
    size=run([tool('size'),str(elf)]);print(size,end='')
    (dest/'build.log').write_text(''.join(x[1] for x in items)+log+size,encoding='utf-8')
v3=(out/'V3F/V3F.bin').read_bytes();v5=(out/'V5F/V5F.bin').read_bytes()
if len(v3)>0x10000:raise SystemExit('V3 overlaps V5 image')
merged=v3+b'\xff'*(0x10000-len(v3))+v5
(out/'Merge.bin').write_bytes(merged)
assert merged[:len(v3)]==v3 and merged[0x10000:]==v5
(out/'build-info.json').write_text(json.dumps({'orientation':a.orientation,'v3_bytes':len(v3),'v5_bytes':len(v5),'merge_bytes':len(merged),'v5_offset':65536,'compiler':run([tool('gcc'),'--version']).splitlines()[0]},indent=2))
print('SUCCESS:',out/'Merge.bin')
