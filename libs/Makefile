all:
	@PWD=$(CURDIR); \
	cd `dirname ${PWD}`; \
	git submodule init libs; \
	git submodule update libs; \
	cd ${PWD}
	@gcc -c -fPIC -I../includes -DCURL_STATICLIB=1 -mmacosx-version-min=10.5 -m32 -isysroot /Developer/SDKs/MacOSX10.5.sdk *.c
clean:
	@rm *.o
