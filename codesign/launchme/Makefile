PLATFORM=/Developer-old/Platforms/iPhoneOS.platform
SDKVER=4.2
SDK=$(PLATFORM)/Developer/SDKs/iPhoneOS$(SDKVER).sdk
CC=$(PLATFORM)/Developer/usr/bin/gcc-4.2
LD=$(PLATFORM)/Developer/usr/bin/ld
AS=$(PLATFORM)/Developer/usr/bin/as

CCFLAGS=-arch armv6 -isysroot=$(SDK) -I./include -I$(SDK)/usr/include -L$(SDK)/usr/lib
CCFLAGS_DYLIB=$(CCFLAGS) -dynamiclib -segaddr __DATA 0x11130000 -segaddr __TEXT 0x11120000 -segprot __TEXT rw rw
CCFLAGS_DYLIB2=$(CCFLAGS) -dynamiclib -segaddr __TEXT 0x1000000 -segaddr __ONE 0x11130000 -segaddr __ROP 0x11132000 -segprot __TEXT rw rw

all: kern_sploit

kern_sploit: kern_sploit.c
	$(CC) $(CCFLAGS) -o $@ $^
	ldid -S $@

clean:
	rm kern_sploit

