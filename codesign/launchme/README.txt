Incomplete codesign - variation of comex's launchd interposition trick

1. find gadgets addrs for target device
    cd config
    python config.py iPhone3,1_4.2.1
2. compile dylib and patch gadgets palceholders
    make
3. boot ramdisk, mount data & system partitions
    overrides.plist => /mnt2/db/launchd.db/com.apple.launchd/overrides.plist
    flat_interpose.dylib  =>  /mnt1/usr/lib/flat_interpose.dylib
    kern_sploit  =>   /mnt1/usr/lib/kern_sploit
    chmod +x /mnt1/usr/lib/kern_sploit
4. reboot and kern_sploit should be running