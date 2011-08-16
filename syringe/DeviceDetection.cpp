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
					cout << "Failure.. thow something here instead" << endl;
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
}

iDeviceTarget DeviceDetection::getHardwareVersion() {
	return hwVersion;
}

iDeviceFirmware DeviceDetection::getFirmwareVersion() {
	return fwVersion;
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
		
	} else if (!strcmp(forKey,"ProductVersion")) {
		if (val == "1.0") {
			fwVersion = idf_10; 
		} else if (val == "1.0.0") {
			fwVersion = idf_100; 
		} else if (val == "1.0.1") {
			fwVersion = idf_101; 
		} else if (val == "1.0.2") {
			fwVersion = idf_102; 
		} else if (val == "1.1.1") {
			fwVersion = idf_111; 
		} else if (val == "1.1.2") {
			fwVersion = idf_112; 
		} else if (val == "1.1.3") {
			fwVersion = idf_113; 
		} else if (val == "1.1.4") {
			fwVersion = idf_114; 
		} else if (val == "2.0") {
			fwVersion = idf_20; 
		} else if (val == "2.0.1") {
			fwVersion = idf_201; 
		} else if (val == "2.0.2") {
			fwVersion = idf_202; 
		} else if (val == "2.1") {
			fwVersion = idf_21; 
		} else if (val == "2.2") {
			fwVersion = idf_22; 
		} else if (val == "2.2.1") {
			fwVersion = idf_221; 
		} else if (val == "3.0") {
			fwVersion = idf_30; 
		} else if (val == "3.0.1") {
			fwVersion = idf_301; 
		} else if (val == "3.1") {
			fwVersion = idf_31; 
		} else if (val == "3.1.2") {
			fwVersion = idf_312; 
		} else if (val == "3.1.3") {
			fwVersion = idf_313; 
		} else if (val == "3.2") {
			fwVersion = idf_32; 
		} else if (val == "3.2.1") {
			fwVersion = idf_321; 
		} else if (val == "3.2.2") {
			fwVersion = idf_322; 
		} else if (val == "4.0") {
			fwVersion = idf_40; 
		} else if (val == "4.0.1") {
			fwVersion = idf_401; 
		} else if (val == "4.0.2") {
			fwVersion = idf_402; 
		} else if (val == "4.1") {
			fwVersion = idf_41; 
		} else if (val == "4.2.1") {
			fwVersion = idf_421; 
		} else if (val == "4.2.5") {
			fwVersion = idf_425; 
		} else if (val == "4.2.6") {
			fwVersion = idf_426; 
		} else if (val == "4.2.7") {
			fwVersion = idf_427; 
		} else if (val == "4.2.8") {
			fwVersion = idf_428; 
		} else if (val == "4.3") {
			fwVersion = idf_43; 
		} else if (val == "4.3.1") {
			fwVersion = idf_431; 
		} else if (val == "4.3.2") {
			fwVersion = idf_432; 
		} else if (val == "4.3.3") {
			fwVersion = idf_433; 
		} else if (val == "4.3.4") {
			fwVersion = idf_434; 
		} else if (val == "4.3.5") {
			fwVersion = idf_435; 
		} else if (val == "5.0") {
			fwVersion = idf_50;
		} else {
			cout << "DEVELOPER MESSAGE: Unknown FirmwareVersion detected. Please fixme before release!" << endl;
		}
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

