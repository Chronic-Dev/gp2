#include <libirecovery.h>

typedef enum {
	U_IBSS_ONLY = 1,
	U_IBSS_PATCHED = 2,
	U_IBEC = 3,
	U_IBEC_PATCHED = 4,
	U_IBOOT = 5,
	U_IBOOT_PATCHED = 6,
	U_RAMDISK = 7,
	U_JAILBREAK = 8
} UploadArgs;

class FirmwareUploader {
	public:
		FirmwareUploader();
		~FirmwareUploader();
		void UploadFirmware(UploadArgs args);
	private:
		void FetchImage(char *type, char *output);
		void UploadImage(char *type);
		void UploadImagePayload(char *type);
		void UploadRamdisk();
		void UploadRamdiskFiles();
		irecv_client_t client;
		irecv_device_t device;
};

