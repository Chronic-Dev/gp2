all:
	@mkdir -p build
	@mkdir -p build/exploits/
	@mkdir -p build/payloads/cyanide/
	@mkdir -p build/files/
	@make -C libs/
	@make -C payloads/
	@cp -R payloads/cyanide/payloads/* build/payloads/cyanide/
	@make -C exploits/
	@cp exploits/*.dylib build/exploits/
	@make -C anthrax/
	@cp anthrax/ramdisk.dmg build/anthrax.dmg
	@make -C syringe/
	@make -C doctors/
	@cp doctors/cli/injectpois0n build/

clean:
	@echo "Cleaning..."
	@rm -rf build/
	@make -C libs/ clean
	@make -C payloads/ clean
	@make -C exploits/ clean
	#@make -C anthrax/ clean
	@make -C syringe/ clean
	@make -C doctors/ clean

package: all
	@echo "Building Package..."
