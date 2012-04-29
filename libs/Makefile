LIBSDIR=staticlibs

MACHINE := $(shell $(CC) -dumpmachine)
PLATFORM := $(shell echo $(MACHINE) | cut -f 1 -d "-")

CFLAGS=-m32 -Icurl/include -I../includes

OPENSSL_MAKE=
OPENSSL_COPY=
OPENSSL_CLEAN=

ifeq ($(findstring darwin,$(MACHINE)), darwin)
  CFLAGS+=-fPIC -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.6.sdk -DCURL_PULL_SYS_SOCKET_H=1
endif

ifeq ($(findstring linux,$(MACHINE)), linux)
  CFLAGS+=-fPIC -DCURL_PULL_SYS_SOCKET_H=1
endif

WIN32=
ifeq ($(findstring cygwin,$(MACHINE)), cygwin)
  WIN32=1
  CC=gcc-3
endif
ifeq ($(findstring mingw32,$(MACHINE)), mingw32)
  WIN32=1
endif
ifdef WIN32
  CFLAGS+=-DWIN32 -DCURL_PULL_WS2TCPIP_H=1
  OPENSSL_MAKE=make -C openssl -f Makefile.static
  OPENSSL_COPY=@cp openssl/libssl.a openssl/libcrypto.a $(LIBSDIR)/
  OPENSSL_CLEAN=make -C openssl -f Makefile.static clean
endif

all:
	@PWD=$(CURDIR); \
	cd `dirname ${PWD}`; \
	git submodule init libs; \
	git submodule update libs; \
	cd ${PWD}
	@mkdir -p $(LIBSDIR)
	@make -C libusb -f Makefile.static
	@cp libusb/libusb-1.0.a $(LIBSDIR)/
	@make -C libxml2 -f Makefile.static
	@cp libxml2/libxml2.a $(LIBSDIR)/
	@make -C libplist -f Makefile.static
	@cp libplist/libplist.a $(LIBSDIR)/
	@cp libplist/libplist++.a $(LIBSDIR)/
	@make -C usbmuxd -f Makefile.static
	@cp usbmuxd/libusbmuxd.a $(LIBSDIR)/
	$(OPENSSL_MAKE)
	$(OPENSSL_COPY)
	@make -C libimobiledevice -f Makefile.static
	@cp libimobiledevice/libimobiledevice.a $(LIBSDIR)/
	@make -C curl -f Makefile.static
	@cp curl/libcurl.a $(LIBSDIR)/
	@gcc $(CFLAGS) -c *.c

clean:
	@rm -f *.o
	@rm -f $(LIBSDIR)/*.a
	@make -C libusb -f Makefile.static clean
	@make -C libxml2 -f Makefile.static clean
	@make -C libplist -f Makefile.static clean
	@make -C usbmuxd -f Makefile.static clean
	$(OPENSSL_CLEAN)
	@make -C libimobiledevice -f Makefile.static clean
	@make -C curl -f Makefile.static clean

