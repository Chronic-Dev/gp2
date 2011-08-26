#include <Syringe/Syringe.h>
#include <iostream>

using namespace std;
int main(int argc, char **args) {
	int exploitCount = 0;
	int i = 0;
	try {
		Syringe *injector = new Syringe();
		char *devinfo = injector->getConnectedDeviceInfo();
		cout << "Found Device \"" << devinfo << "\" in Normal Mode" << endl;
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
		injector->inject(U_IBSS_ONLY);
		cout << "Done!" << endl;
	} catch (SyringeBubble &bubble) {
		cout << bubble.getError() << endl;
	}
	return 0;
}

