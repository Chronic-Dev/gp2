all:
	@mkdir -p build/exploits/
	@mkdir -p build/payloads/cyanide/
	@mkdir -p build/files/common/
	@make -C cyanide/
	@cp -R cyanide/payloads/* build/payloads/cyanide/
	@make -C exploits/
	@cp exploits/*.dylib build/exploits/
	
clean:
	@rm -rf build/
	@make -C cyanide/ clean
	@make -C exploits/ clean
