LIBSDIR=staticlibs

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
	@gcc -c -fPIC -I../includes -DCURL_STATICLIB=1 -mmacosx-version-min=10.5 -m32 -isysroot /Developer/SDKs/MacOSX10.6.sdk *.c

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

