
all:
	@make -C cyanide/
	@cp -R cyanide/payloads includes/
	@make -C exploits/

clean:
	@make -C cyanide/ clean
	@rm -rf includes/payloads/
	@make -C exploits/ clean
