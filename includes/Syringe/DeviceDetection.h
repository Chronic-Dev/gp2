#include <exploit.h>

class DeviceDetection {
	public:
		/**
		 * The DeviceDetection Constructor
		 * @throws a SyringeBubble if the device isn't in normal mode or no connection can be made
		 */
		DeviceDetection();
		/**
		 * The DeviceDetection Deconstructor
		 */
		~DeviceDetection();
		/**
		 * Gets the type of device connected (iPhone, 3G, 3GS, etc)
		 * @returns iDeviceTarget describing the connected device's hardware
		 */
		iDeviceTarget getHardwareVersion();
		/**
		 * Gets the connected device's build version
		 * @returns char * The Device Build Version String
		 */
		char *getBuildVersion();
		/**
		 * Gets the hardware version as a string (iPhone, iPhone 3G, etc.)
		 * @returns char * The hardware version string
		 */
		char *getHardwareVersionString();
		/**
		 * Gets the firmware version as a string (4.2.1, 4.3.3, etc.)
		 * @returns char * The firmware version string
		 */
		char *getFirmwareVersionString();
		/**
		 * Gets the device name as a string (Jaywalker's iPhone, Posixninja's iPhone, etc)
		 * @returns char * The device name string
		 */
		char *getDeviceNameString();
		/**
		 * Gets the device's UDID as a string
		 * @returns char * The device's UDID
		 */
		char *getUDIDString();
	private:
		/**
		 * Gets the value for the key and assigns it
		 * @param value The pointer to store the value in
		 * @param forKey The key to get the value of
		 */
		void assignValue(char *value, char *forKey);
		iDeviceTarget hwVersion; /**< The device hardware version */
		char *buildVersion; /**< The device build version */
		char *hardwareversion; /**< The device hardware versions string */
		char *firmwareversion; /**< The device firmware version */
		char *devicename; /**< The device name */
		char *udid; /**< The device udid */
};

