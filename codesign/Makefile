CC=gcc
CFLAGS=-dynamiclib -I../../includes/
DIRS=$(shell find . -type d -d 1)
RM=rm

all:
	@for d in $(DIRS); do \
		cd $$d; \
		echo "Building $$d Codesign Exploit... "; \
		make -C codesign/; \
		echo "[DONE]"; \
		echo "Building $$d... "; \
		$(CC) -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.6.sdk -m32 -o ../$$d.dylib -install_name $$d $(CFLAGS) `cat EXTRAFLAGS` *.c; \
		echo "[DONE]"; \
	done

clean:
	@$(RM) *.dylib
	@for d in $(DIRS); do \
		cd $$d; \
		make clean -C codesign/; \
	done

