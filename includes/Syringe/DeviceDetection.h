#include <exploit.h>

class DeviceDetection {
	public:
		DeviceDetection();
		~DeviceDetection();
		iDeviceTarget getHardwareVersion();
		iDeviceFirmware getFirmwareVersion();
		char *getHardwareVersionString();
		char *getFirmwareVersionString();
		char *getDeviceNameString();
		char *getUDIDString();
	private:
		void assignValue(char *value, char *forKey);
		iDeviceTarget hwVersion;
		iDeviceFirmware fwVersion;
		char *hardwareversion;
		char *firmwareversion;
		char *devicename;
		char *udid;
};

