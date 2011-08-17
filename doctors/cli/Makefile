all:
	@echo "Building cli doctor..."
	@g++ -I../../includes/ -ggdb -o injectpois0n -m32 -lxml2 -lz *.cpp ../../libs/*.a ../../syringe/*.o

clean:
	@rm injectpois0n
