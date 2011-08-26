#include <exploit.h>
#include "DeviceDetection.h"
#include "SyringeBubble.h"
#include "FirmwareUploader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>

#ifndef exploitPath
#define exploitPath "exploits/"
#endif

#ifdef _WIN32
#define ext ".dll"
#else
#ifdef __APPLE__
#define ext ".dylib"
#else
#define ext ".so"
#endif
#endif

class Syringe {
	public:
		Syringe();
		~Syringe();
		char *getConnectedDeviceInfo();
		void setProgressCallback();
		int preloadExploits();
		char *getExploitName(int num);
		void loadExploit(int num);
		ExploitType getExploitType();
		bool deviceIsReady();
		void inject(UploadArgs arg);
	private:
		int getExploitCount();
		void loadExploits(void **exploits, int exploitCount);
		//Variables
		iDeviceTarget pois0n_device;
		char *pois0n_build;
		void **usableExploits;
		int exploitNum;
		int usableExploitCount;
		isBuildSupported_t _isBuildSupported;
		getTargets_t _getTargets;
		getExploitType_t _getExploitType;
		getExploitName_t _getExploitName;
		exploit_t _exploit;
		DeviceDetection *dd;
		//Status variables
		bool exploitLoaded;
};

