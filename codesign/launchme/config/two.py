#!/opt/local/bin/python2.6
import struct, sys, os
import warnings
warnings.simplefilter('error')

import config
cfg = config.openconfig()
arch = cfg['arch']
assert arch in ['armv6', 'armv7', 'i386']

launchd = cfg['#launchd']
cache = cfg.get('#dyld_shared_cache_armv7')
if not cache:
    cache = cfg.get('#dyld_shared_cache_armv6')

myreps = {
    '_getpid':                   launchd['-1'],
    '_unsetenv':                 launchd['0'],
    '_launch_data_new_errno':    launchd['1'],
    '_setrlimit':                launchd['2'],
    '__exit':                    launchd['3'],
    '_audit_token_to_au32':      launchd['4'],
    '_launch_data_unpack':       launchd['5'],
    '_launch_data_dict_iterate': launchd['6'],

    '_pthread_detach':           launchd['7']+4,
    '_strlcpy':                  launchd['7']+4,
    '_stat':                  launchd['12'],
}

f=open("../flat_interpose2.dylib", "rb")
fi=f.read()
f.close()

import commands, struct,ctypes
#BASE = 0x28000  #XXX
#BASE= 0x11120000
BASE = 0

def pack_adjust_off(off):
    return struct.pack("<L", ctypes.c_uint32(off - BASE).value)

for s in commands.getoutput("nm ../flat_interpose2.dylib").splitlines():
    t = s.split()
    off = t[0]
    name = t[-1]
    if name in myreps.keys():
        hex = struct.pack("<L", int(off,16))
        fi = fi.replace(hex, pack_adjust_off(myreps[name]) )
        print name

PIVOT_PLACEHOLDER=0xDEADB1F7   
fi = fi.replace(struct.pack("<L", PIVOT_PLACEHOLDER),pack_adjust_off(launchd['7']))

fi = fi.replace(struct.pack("<L", 0xFEED0007),pack_adjust_off(cache['k7']))
fi = fi.replace(struct.pack("<L", 0xFEED0011),pack_adjust_off(cache['k11']))
fi = fi.replace(struct.pack("<L", 0xFEED0012),pack_adjust_off(cache['k12']))

f=open("../flat_interpose22.dylib","wb")
f.write(fi)
f.close()
