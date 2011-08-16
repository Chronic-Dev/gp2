#include <exploit.h>

class DeviceDetection {
	public:
		DeviceDetection();
		~DeviceDetection();
		iDeviceTarget getHardwareVersion();
		iDeviceFirmware getFirmwareVersion();
	private:
		void assignValue(char *value, char *forKey);
		iDeviceTarget hwVersion;
		iDeviceFirmware fwVersion;
		char *devicename;
		char *udid;
};

