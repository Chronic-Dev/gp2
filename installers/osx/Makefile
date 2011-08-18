all:
	@echo "Building OSX Installer DMG"
	@hdiutil convert GPInstaller.sparseimage -format UDBZ -o GPInstaller.dmg 1> /dev/null
	@echo "[DONE]"
clean:
	@rm *.dmg
