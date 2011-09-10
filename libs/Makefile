LIBSDIR=staticlibs

UNAME := $(shell uname)

CFLAGS=-m32 -Icurl/include -I../includes

ifeq ($(UNAME), Darwin)
  CFLAGS+=-fPIC -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.6.sdk -DCURL_PULL_SYS_SOCKET_H=1
endif

ifeq ($(UNAME), Linux)
  CFLAGS+=-fPIC -DCURL_PULL_SYS_SOCKET_H=1
endif

WIN32=
ifeq ($(findstring CYG,$(UNAME)), CYG)
  WIN32=1
  CC=gcc-3
endif
ifeq ($(findstring MINGW,$(UNAME)), MINGW)
  WIN32=1
  CC=gcc
endif
ifdef WIN32
  CFLAGS+=-DWIN32 -DCURL_PULL_WS2TCPIP_H=1
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
	@make -C libtasn1 -f Makefile.static
	@cp libtasn1/libtasn1.a $(LIBSDIR)/
	@make -C libgpg-error -f Makefile.static
	@cp libgpg-error/libgpg-error.a $(LIBSDIR)/
	@make -C libgcrypt -f Makefile.static
	@cp libgcrypt/libgcrypt.a $(LIBSDIR)/
	@make -C gnutls -f Makefile.static
	@cp gnutls/libgnutls.a $(LIBSDIR)/
	@cp gnutls/libgnutlsxx.a $(LIBSDIR)/
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
	@make -C libtasn1 -f Makefile.static clean
	@make -C libgpg-error -f Makefile.static clean
	@make -C libgcrypt -f Makefile.static clean
	@make -C gnutls -f Makefile.static clean
	@make -C libimobiledevice -f Makefile.static clean
	@make -C curl -f Makefile.static clean

