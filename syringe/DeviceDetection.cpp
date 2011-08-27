#include <Syringe/DeviceDetection.h>
#include <Syringe/SyringeBubble.h>
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>

#include <iostream>
using namespace std;

DeviceDetection::DeviceDetection() {
	//Initialize variables...
	idevice_error_t ret = IDEVICE_E_UNKNOWN_ERROR;
	idevice_t phone = NULL;
	lockdownd_client_t client = NULL;
	plist_t node = NULL;
	char *domain = NULL;
	plist_type node_type = (plist_type)NULL;
	int keyCount = 5; //The number of values below
	char *key[] = { "BuildVersion", "ProductVersion", "DeviceName", "HardwareModel", "UniqueDeviceID" }; //These are the values we need to store
	char *keyVal = NULL;

	//Now lets go...
	ret = idevice_new(&phone, NULL);
	if (ret != IDEVICE_E_SUCCESS) {
		throw SyringeBubble("No device found in Normal Mode");
		return; //Even though I know gcc will probably optimize this out, I feel safer knowing it's here
	}
	
	if (LOCKDOWN_E_SUCCESS != lockdownd_client_new(phone, &client, "ideviceinfo")) {
		idevice_free(phone);
		throw SyringeBubble("Could not connect to device.");
		return;
	}

	for (int i = 0; i < keyCount; i++) {
		if (lockdownd_get_value(client, domain, key[i], &node) == LOCKDOWN_E_SUCCESS) {
			node_type = plist_get_node_type(node);
			if (node_type != PLIST_DICT && node_type != PLIST_ARRAY) {
				plist_type t;
				if (!node) {
					throw SyringeBubble("Node failure");
				}

				if (plist_get_node_type(node) == PLIST_STRING) {
					plist_get_string_val(node, &keyVal);
					assignValue(keyVal, key[i]);
				}
			}
		}
	}
	if (domain != NULL)
		free(domain);
	lockdownd_client_free(client);
	idevice_free(phone);
}

DeviceDetection::~DeviceDetection() {
	//idevice_free(phone);
	free(buildVersion);
	free(udid);
	free(devicename);
	free(hardwareversion);
	free(firmwareversion);
}

iDeviceTarget DeviceDetection::getHardwareVersion() {
	return hwVersion;
}

char *DeviceDetection::getBuildVersion() {
	return buildVersion;
}

char *DeviceDetection::getHardwareVersionString() {
	return hardwareversion;
}

char *DeviceDetection::getFirmwareVersionString() {
	return firmwareversion;
}

char *DeviceDetection::getUDIDString() {
	return udid;
}

char *DeviceDetection::getDeviceNameString() {
	return devicename;
}

void DeviceDetection::assignValue(char *value, char *forKey) {
	string val(value);
	if (!strcmp(forKey,"BuildVersion")) {
		buildVersion = (char *)malloc(strlen(value) * sizeof(char));
		strcpy(buildVersion, value);
	} else if (!strcmp(forKey,"ProductVersion")) {
		firmwareversion = (char *)malloc(strlen(value) * sizeof(char));
		strcpy(firmwareversion, value);
	} else if (!strcmp(forKey,"DeviceName")) {
		devicename = (char *)malloc(strlen(value) * sizeof(char));
		strcpy(devicename, value);
	} else if (!strcmp(forKey,"HardwareModel")) {
		if (val == "M68AP") {
			hwVersion = iD_2G;
		} else if (val == "N82AP") {
			hwVersion = iD_3G;
		} else if (val == "N88AP") {
			hwVersion = iD_3GS_NEW;
		} else if (val == "N45AP") {
			hwVersion = iD_IPT;
		} else if (val == "N72AP") {
			hwVersion = iD_IPT2G;
		} else if (val == "N18AP") {
			hwVersion = iD_IPT3G;
		} else if (val == "N81AP") {
			hwVersion = iD_IPT4G;
		} else if (val == "K48AP") {
			hwVersion = iD_A4;
		} else if (val == "K93AP" || val == "K94AP" || val == "K95AP") {
			hwVersion = iD_IPAD2;
		} else if (val == "K66AP" /*ATV*/ || val == "N90AP" || val == "N92AP") {
			hwVersion = iD_A4;
		} else {
			cout << "DEVELOPER MESSAGE: Undefined hardware model. Please fixme before release!" << endl;
		}
		hardwareversion = (char*)malloc(strlen(value) * sizeof(char));
		strcpy(hardwareversion, value);
	} else if (!strcmp(forKey,"UniqueDeviceID")) {
		udid = (char *)malloc(strlen(value) * sizeof(char));
		strcpy(udid, value);
	} else {
		cout << "DEVELOPER MESSAGE: Undefined Key in Assignment Function. Please fixme before release!" << endl;
	}
}

