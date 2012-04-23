#include <Syringe/Syringe.h>
#include <iostream>

using namespace std;
int main(int argc, char **args) {
	int exploitCount = 0;
	int i = 0;
	iDeviceTarget device;
	char *build = NULL;
	char *ipsw = NULL;
	UploadArgs opts = U_JAILBREAK;
	int useCustomDevice = 0;
	try {
		if (argc > 1) {
			if (!strcmp(args[1], "--help") || !strcmp(args[1], "-h")) {
				cout << args[0] << " [--device DEVICE --build FIRMWARE_BUILD] [--ipsw /path/to/ipsw]" << endl;
				return 0;
			}
			for (i = 1; i < argc; i++) {
				if (!strcmp(args[i], "--device") || !strcmp(args[i], "-d")) {
					i++;
					if (!strcmp("2G", args[i])) {
						device = iD_2G;
					} else if (!strcmp(args[i], "IPT")) {
						device = iD_IPT;
					} else if (!strcmp(args[i], "3G")) {
						device = iD_3G;
					} else if (!strcmp(args[i], "IPT2G")) {
						device = iD_IPT2G;
					} else if (!strcmp(args[i], "3GS_OLD")) {
						device = iD_3GS_OLD;
					} else if (!strcmp(args[i], "3GS_NEW")) {
						device = iD_3GS_NEW;
					} else if (!strcmp(args[i], "IPT3G")) {
						device = iD_IPT3G;
					} else if (!strcmp(args[i], "4")) {
						device = iD_4;
					} else if (!strcmp(args[i], "IPAD")) {
						device = iD_IPAD;
					} else if (!strcmp(args[i], "ATV2")) {
						device = iD_ATV2;
					} else if (!strcmp(args[i], "IPAD2")) {
						device = iD_IPAD2;
					} else if (!strcmp(args[i], "IPT4G")) {
						device = iD_IPT4G;
					} else if (!strcmp(args[i], "IPAD3")) {
						device = iD_IPAD3;
					} else if (!strcmp(args[i], "ATV3")) {
						device = iD_ATV3;
					} else {
						i--;
						cout << "Invalid option for " << args[i] << endl;
						cout << "Valid options are:" << endl;
						cout << "\t2G      - iPhone 2G" << endl;
						cout << "\tIPT     - iPod Touch (first gen)" << endl;
						cout << "\t3G      - iPhone 3G" << endl;
						cout << "\tIPT2G   - iPod Touch (second gen)" << endl;
						cout << "\t3GS_OLD - iPhone 3GS (old bootrom)" << endl;
						cout << "\t3GS_NEW - iPhone 3GS (new bootrom)" << endl;
						cout << "\tIPT3G   - iPod Touch (thrid gen)" << endl;
						cout << "\t4       - iPhone 4" << endl;
						cout << "\tIPAD    - iPad" << endl;
						cout << "\tATV2    - AppleTV 2" << endl;
						cout << "\tIPAD2   - iPad 2" << endl;
						cout << "\tIPT4G   - iPod Touch (fourth gen)" << endl;
						cout << "\tIPAD3   - iPad 3" << endl;
						cout << "\tATV3    - AppleTV 3" << endl;
						return 0;
					}
					useCustomDevice = 1;
				} else if (!strcmp(args[i], "--build") || !strcmp(args[i], "-b")) {
					i++;
					build = args[i];
				} else if (!strcmp(args[i], "--ipsw") || !strcmp(args[i], "-i")) {
					i++;
					ipsw = args[i];
				} else if (!strcmp(args[i], "--opts") || !strcmp(args[i], "-o")) {
					i++;
					if (!strcmp(args[i], "jailbreak")) {
						opts = U_JAILBREAK;
					} else if (!strcmp(args[i], "ramdisk")) {
						opts = U_RAMDISK;
					} else if (!strcmp(args[i], "iboot")) {
						opts = U_IBOOT_PATCHED;
					} else if (!strcmp(args[i], "ibss")) {
						opts = U_IBSS_PATCHED;
					} else if (!strcmp(args[i], "ibss_only")) {
						opts = U_IBSS_ONLY;
					} else if (!strcmp(args[i], "inject")) {
						opts = U_INJECT_ONLY;
					} else {
						i--;
						cout << "Invalid option for " << args[i] << endl;
						cout << "Valid options are:" << endl;
						cout << "\tinject    - Only injects the exploit" << endl;
						cout << "\tibss_only - Injects the exploit and loads the iBSS without cyanide" << endl;
						cout << "\tibss      - Loads cyanide in the iBSS" << endl;
						cout << "\tiboot     - Loads cyanide in iBoot" << endl;
						cout << "\tramdisk   - Loads a custom ramdisk" << endl;
						cout << "\tjailbreak - Jailbreaks or tether boots the device" << endl;
					}
				}
			}
		}
		Syringe *injector;
		if (useCustomDevice == 1) {
			if (ipsw != NULL && useCustomDevice == 1) {
				injector = new Syringe(ipsw, device, build);
			} else {
				injector = new Syringe(device, build);
			}
		} else {
			if (ipsw != NULL)
				injector = new Syringe(ipsw);
			else
				injector = new Syringe();
			char *devinfo = injector->getConnectedDeviceInfo();
			cout << "Found Device \"" << devinfo << "\" in Normal Mode" << endl;
		}
		exploitCount = injector->preloadExploits();
		if (exploitCount > 1) {
			cout << "Please select an exploit:" << endl;
			for (i = 0; i < exploitCount; i++) {
				cout << "\t" << (i + 1) << ". " << injector->getExploitName(i) << endl;
			}
			cout << "> ";
			cin >> i;
			i--;
		} else {
			i = 0;
		}
		cout << "Loading " << injector->getExploitName(i) << "..." << endl;
		injector->loadExploit(i);
		ExploitType eType = injector->getExploitType();
		if (eType == eBOOTROM) {
			cout << "Please put your device in DFU mode..." << endl;
		} else if (eType == eIBOOT) {
			cout << "Please put your device in Recovery mode..." << endl;
		}
		while (!injector->deviceIsReady()) {
			sleep(1);
		}
		cout << "Injecting..." << endl;
		injector->inject(opts);
		cout << "Done!" << endl;
	} catch (SyringeBubble &bubble) {
		cout << bubble.getError() << endl;
	}
	return 0;
}

