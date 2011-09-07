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
#define exploitPath "exploits/" /**< The path to the exploits folder (Should be set by the makefile, but if not, we hardcode it here) */
#endif

#ifndef payloadPath
#define payloadPath "payloads/" /**< The path to the payloads folder (Should be set by the makefile, but if not, we hardcode it here) */
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
		/**
		 * The Syringe Constructor.
		 * @throws SyringeBubble If DeviceDetection fails on construct, getHardwareVersion, or getBuildVersion
		 * @return A Syringe! Duh.
		 */
		Syringe();
		/**
		 * The Alternate Syringe Constructor without device detection. Useful for debugging
		 * @param device an iDeviceTarget for the target device
		 * @param build char * Build Version for the firmware on the device
		 * @return A Syringe!!
		 */
		Syringe(iDeviceTarget device, char *build);
		/**
		 * The Syringe Deconstructor.
		 */
		~Syringe();
		/**
		 * Gets info about the device
		 * @returns char* allocated with the device name and firmware version number. Must be freed
		 */
		char *getConnectedDeviceInfo();
		void setProgressCallback();
		/**
		 * Loads exploits and sifts out unusable ones
		 * @throws SyringeBubble if there are now usable exploits for the current device/firmware
		 * @returns number of explits available for use
		 */
		int preloadExploits();
		/**
		 * Gets the exploit name for the specified exploit index
		 * @param num The index of the exploit to get the name of
		 * @throws SyringeBubble if the index described by num is out of bounds
		 * @returns char * The exploit name
		 */
		char *getExploitName(int num);
		/**
		 * Prepares the exploit for use for the specified exploit index
		 * @param num The index of the exploit to prepare
		 * @throws SyringeBubble if the index described by num is out of bounds
		 */
		void loadExploit(int num);
		/**
		 * Gets the exploit type for the currently loaded exploit
		 * Possible returns are: E_BOOTROM, E_IBOOT or E_USERLAND
		 * @throws SyringeBubble if no exploit has been loaded by loadExploit
		 * @returns ExploitType for the currently loaded exploit
		 */
		ExploitType getExploitType();
		/**
		 * Checks if the device is ready to be exploited
		 * @throws SyringeBubble if no exploit has been loaded by loadExploit
		 * @returns bool if we can inject the exploit
		 */
		bool deviceIsReady();
		/**
		 * Injects the exploit and does firmware upload if desired
		 * @throws SyringeBubble if no exploit has been loaded by loadExploit or if the exploit fails to inject
		 */
		void inject(UploadArgs arg);
	private:
		/**
		 * Gets the number of exploits in the exploit dir
		 * @returns int the number of exploits n the directory
		 */
		int getExploitCount();
		/**
		 * Loads all the exploits in the exploit dir
		 * @param exploits an array of pointers pointing to the return of dlopen on the exploits
		 * @param exploitCount the number of exploits in the directory as return by getExploitCount
		 * @throws SyringeBubble if no exploits were found or loaded
		 */
		void loadExploits(void **exploits, int exploitCount);
		//Variables
		iDeviceTarget pois0n_device; /**< The type of device to poison */
		char *pois0n_build; /**< The build of the device to poison */
		void **usableExploits; /**< Array of usable exploit pointers returned by dlopen */
		int exploitNum; /**< The currently selected exploit number */
		int usableExploitCount; /**< The number of usable exploits available after preloadExploits is done */
		isBuildSupported_t _isBuildSupported; /**< The isBuildSupported function of the loaded exploit */
		getTargets_t _getTargets; /**< The getTargets function of the loaded exploit */
		getExploitType_t _getExploitType; /**< The getExploitType function of the loaded exploit */
		getExploitName_t _getExploitName; /**< The getExploitName function of the loaded exploit */
		exploit_t _exploit; /**< The exploit function of the loaded exploit */
		DeviceDetection *dd; /**< The device detection class pointer */
		//Status variables
		bool exploitLoaded; /**< bool indicating if the user has selected an exploit for use */
};

