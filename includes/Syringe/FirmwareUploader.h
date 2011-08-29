#include <libirecovery.h>
/**
 * The UploadArgs enum
 * Defines how many firmware/patches we want to upload
 */
typedef enum {
	U_INJECT_ONLY = 0, /**< Only injects the exploit */
	U_IBSS_ONLY = 1, /**< Injects the exploit and uploads the iBSS */
	U_IBSS_PATCHED = 2, /**< Patches the iBSS */
	U_IBEC = 3, /**< Uploads the iBEC */
	U_IBEC_PATCHED = 4, /**< Patches the iBEC */
	U_IBOOT = 5, /**< Uploads iBoot */
	U_IBOOT_PATCHED = 6, /**< Patches iBoot */
	U_RAMDISK = 7, /**< Uploads the ramdisk */
	U_JAILBREAK = 8 /**< Sends the jailbreak files to the ramdisk */
} UploadArgs;

class FirmwareUploader {
	public:
		/**
		 * The FirmwareUploader Constructor
		 * @throws SyringeBubble if device connection fails
		 */
		FirmwareUploader();
		/**
		 * The FirmwareUploader Deconstructor
		 */
		~FirmwareUploader();
		/**
		 * Fetches and Uploads firmware to the device based on the UploadArgs provided
		 * @throws SyringeBubble if any fetch or upload fails
		 * @param args The UploadArgs for uploading firmware
		 */
		void UploadFirmware(UploadArgs args);
	private:
		/**
		 * Fetches the requested image and caches it
		 * @param type The type of image to upload (iBSS, iBoot, kernelcache, etc)
		 * @param output The location to save the image
		 * @throws SyringeBubble if the image fetch fails
		 */
		void FetchImage(char *type, char *output);
		/**
		 * Uploads a firmware image to the device
		 * @param type The type of image to upload (iBSS, iBoot, kernelcache, etc)
		 * @throws SyringeBubble if counter reset fails or image upload fails
		 */
		void UploadImage(char *type);
		/**
		 * Uploads the image payload to the device
		 * @param type The type of image we are uploading the payload for (iBSS, iBoot, kernelcache, etc)
		 * @throws SyringeBubble if the payload upload fails
		 */ 
		void UploadImagePayload(char *type);
		/**
		 * Uploads the ramdisk image, device tree, and kernel to the device and executes the kernel
		 * @throws SyringeBubble if the ramdisk, kernel, or device tree upload fails to upload or if any commands fail
		 */
		void UploadRamdisk();
		/**
		 * Uploads the jailbreak files to the ramdisk
		 * @throws SyringeBubble if any file fails to upload or execute properly
		 */
		void UploadRamdiskFiles();
		irecv_client_t client; /**< The irecovery client */
		irecv_device_t device; /**< The irecovery device */
};

