all:
	@mkdir -p build/exploits/
	@mkdir -p build/payloads/cyanide/
	@mkdir -p build/files/
	@make -C cyanide/
	@cp -R cyanide/payloads/* build/payloads/cyanide/
	@make -C exploits/
	@cp exploits/*.dylib build/exploits/
	@make -C syringe/
	@make -C doctors/
	@cp doctors/cli/injectpois0n build/

clean:
	@rm -rf build/
	@make -C cyanide/ clean
	@make -C exploits/ clean
	@make -C syringe/ clean
	@make -C doctors/ clean
