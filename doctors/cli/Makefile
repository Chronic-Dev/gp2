OS=osx

all:
	@echo "Building cli doctor..."
	@g++ -I../../includes/ -ggdb -o injectpois0n -m32 -lxml2 -lz -framework IOKit -framework Foundation -lcurl *.cpp ../../libs/$(OS)/*.a ../../libs/*.o ../../syringe/*.o

clean:
	@rm injectpois0n
