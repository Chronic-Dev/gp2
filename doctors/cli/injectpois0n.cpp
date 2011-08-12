#include <syringe.h>
#include <SyringeBubble.h>
#include <iostream>

using namespace std;
int main(int argc, char **args) {
	int exploitCount = 0;
	int i = 0;
	Syringe *injector = new Syringe(iD_A4, idf_50);
	try {
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
		while (!injector->deviceIsReady()) {
			sleep(1);
		}
		cout << "Injecting..." << endl;
		injector->inject(0);
		cout << "Done!" << endl;
	} catch (SyringeBubble &bubble) {
		cout << bubble.getError() << endl;
	}
	return 0;
}

