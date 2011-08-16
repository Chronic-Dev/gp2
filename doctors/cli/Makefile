all:
	@echo "Building cli doctor..."
	@g++ -I../../includes/ -L/usr/local/lib -L/opt/local/lib -lplist -limobiledevice -ggdb -o injectpois0n *.cpp ../../syringe/*.o

clean:
	@rm injectpois0n
