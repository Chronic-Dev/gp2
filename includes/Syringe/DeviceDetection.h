#include <exploit.h>

class DeviceDetection {
	public:
		DeviceDetection();
		~DeviceDetection();
		iDeviceTarget getHardwareVersion();
		char *getBuildVersion();
		char *getHardwareVersionString();
		char *getFirmwareVersionString();
		char *getDeviceNameString();
		char *getUDIDString();
	private:
		void assignValue(char *value, char *forKey);
		iDeviceTarget hwVersion;
		char *buildVersion;
		char *hardwareversion;
		char *firmwareversion;
		char *devicename;
		char *udid;
};

