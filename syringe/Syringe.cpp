#include <Syringe/Syringe.h>
#include <DeviceDetection.h>
#include <libirecovery.h>

#include <iostream>
using namespace std;

Syringe::Syringe() {
	//Null out variables
	usableExploitCount = 0;
	_getRequiredFirmware = NULL;
	_getTargets = NULL;
	_getExploitType = NULL;
	_getExploitName = NULL;
	_exploit = NULL;
	exploitLoaded = false;
	//Start real code
	try {
		DeviceDetection *dd = new DeviceDetection();
		pois0n_device = dd->getHardwareVersion();
		pois0n_firmware = dd->getFirmwareVersion();
	} catch (SyringeBubble &b) {
		throw b;
	}
//	irecv_init();
//	irecv_set_debug_level(SHOWDEBUGGING);
	debug("Initializing libpois0n\n");
#ifdef __APPLE__
	system("killall -9 iTunesHelper 2>/dev/null");
#endif
#ifdef _WIN32
	system("TASKKILL /F /IM iTunes.exe > NUL");
	system("TASKKILL /F /IM iTunesHelper.exe > NUL");
#endif

}

Syringe::~Syringe() {

}

int Syringe::preloadExploits() {
	int exploitCount = 0;
	int i = 0;

	//We need to know how many exploits we have to load
	exploitCount = getExploitCount();

	//Create space to load the exploits and then load them...
	void *exploits[exploitCount];
	loadExploits(exploits, exploitCount);

	//Now we go through each exploit and see which ones are available for this device...
	usableExploits = (void **)malloc(exploitCount * sizeof(void*));
	for (i = 0; i < exploitCount; i++) {
		_getRequiredFirmware = (getRequiredFirmware_t)dlsym(exploits[i], "getRequiredFirmware");
		_getTargets = (getTargets_t)dlsym(exploits[i], "getTargets");
		if ((pois0n_device & _getTargets()) && _getRequiredFirmware() == 1 || _getRequiredFirmware() & pois0n_firmware) {
			usableExploits[usableExploitCount] = exploits[i];
			usableExploitCount++;
		} else {
			dlclose(exploits[i]);
		}
	}
	if (usableExploitCount == 0)
		throw SyringeBubble("Failed to find any suitable exploit for this device/firmware combo.");
	return usableExploitCount;

}

char *Syringe::getExploitName(int num) {
	if (!(usableExploitCount > 0 && num < usableExploitCount))
		throw SyringeBubble("Exploit index not found");
	_getExploitName = (getExploitName_t)dlsym(usableExploits[num], "getExploitName");
	return _getExploitName();
}

void Syringe::loadExploit(int num) {
	if (!(usableExploitCount > 0 && num < usableExploitCount))
		throw SyringeBubble("Exploit index not found");
	int i;
	for (i = 0; i < usableExploitCount; i++) {
		if (i != num) {
			dlclose(usableExploits[i]);
		} else {
			//Make sure all of our prototypes point to the current exploit's functions
			_getExploitName = (getExploitName_t)dlsym(usableExploits[num], "getExploitName");
			_getExploitType = (getExploitType_t)dlsym(usableExploits[num], "getExploitType");
			_getRequiredFirmware = (getRequiredFirmware_t)dlsym(usableExploits[num], "getRequiredFirmware");
			_getTargets = (getTargets_t)dlsym(usableExploits[num], "getTargets");
			_exploit = (exploit_t)dlsym(usableExploits[num], "exploit");
		}
	}
	exploitNum = num;
	exploitLoaded = true;
}

ExploitType Syringe::getExploitType() {
	if (!exploitLoaded)
		throw SyringeBubble("No exploit loaded");
	return _getExploitType();
}

bool Syringe::deviceIsReady() {
/*
	irecv_error_t error = IRECV_E_SUCCESS;

	//////////////////////////////////////
	// Begin
	// debug("Connecting to device\n");
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		debug("Device must be in DFU mode to continue\n");
		return -1;
	}
	irecv_event_subscribe(client, IRECV_PROGRESS, &recovery_callback, NULL);

	//////////////////////////////////////
	// Check device
	// debug("Checking the device mode\n");
	if (client->mode != kDfuMode) {
		debug("Device must be in DFU mode to continue\n");
		irecv_close(client);
		return -1;
	}
*/
	return true;
}

void Syringe::inject(int arg) {
	if (!exploitLoaded)
		throw SyringeBubble("No exploit loaded");

	if (_exploit() != 0) {
		throw SyringeBubble("Unable to inject exploit");
	}
/*
	debug("Preparing to upload iBSS\n");
	upload_ibss();

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to reconnect\n");
		return -1;
	}

	debug("Preparing to upload iBSS payload\n");
	if (upload_ibss_payload() < 0) {
		error("Unable to upload iBSS payload\n");
		return -1;
	}

	debug("Executing iBSS payload\n");
	if (execute_ibss_payload(arg) < 0) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}
*/
}

//Private Functions
int Syringe::getExploitCount() {
	int exploitCount = 0;
	DIR *dir = opendir(exploitPath);
	struct dirent *entry;
	while ((entry = readdir(dir))) {
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..") && strstr(entry->d_name, ext) != NULL) {
			exploitCount++;
		}
	}
	closedir(dir);
	return exploitCount;
}

void Syringe::loadExploits(void **exploits, int exploitCount) {
	int i = 0;
	char *path;
	struct dirent *entry;
	DIR *dir = opendir(exploitPath);
	while ((entry = readdir(dir))) {
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..") && strstr(entry->d_name, ext) != NULL && i < exploitCount) {
			path = (char *)malloc((strlen(exploitPath) + strlen(entry->d_name) + 1) * sizeof(char));
			strcat(path, exploitPath);
			strcat(path, entry->d_name);
			exploits[i] = dlopen(path, RTLD_LAZY);
			free(path);
			if (exploits[i] == NULL) {
				error("Failed to load exploit: %s\n", entry->d_name);
			} else {
				i++;
			}
		}
	}
	closedir(dir);
	if (i == 0)
		throw SyringeBubble("Failed to find or load any exploits.");
}

