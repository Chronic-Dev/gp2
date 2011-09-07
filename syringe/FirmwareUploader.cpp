#include <Syringe/FirmwareUploader.h>
#include <cstdlib>
#include <libpartial.h>
#include <Syringe/SyringeBubble.h>
#include <cstring>
#include <iostream>

FirmwareUploader::FirmwareUploader() {
	irecv_client_t client = NULL;
	irecv_device_t device = NULL;

	irecv_init();
	irecv_error_t error = IRECV_E_SUCCESS;
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		irecv_exit();
		throw SyringeBubble("Failed to connect to device");
	}

	error = irecv_get_device(client, &device);
	if (device == NULL) {
		irecv_close(client);
		irecv_exit();
		throw SyringeBubble("Failed to get device");
	}
}

FirmwareUploader::~FirmwareUploader() {
	irecv_close(client);
	irecv_exit();
}

void FirmwareUploader::UploadFirmware(UploadArgs args) {
	try {
		if (client->mode == kDfuMode) { //We start with the iBSS
			UploadImage("iBSS");
			if (args > U_IBSS_ONLY) {
				UploadImagePayload("iBSS");
				if (args > U_IBSS_PATCHED) {
					UploadImage("iBEC");
					if (args > U_IBEC) {
						UploadImagePayload("iBEC");
						if (args > U_IBEC_PATCHED) {
							UploadImage("iBoot");
						}
					}
				}
			}
		}

		if (args > U_IBOOT) {
			UploadImagePayload("iBoot");
			if (args > U_IBOOT_PATCHED) {
				UploadRamdisk();
				if (args > U_RAMDISK) {
					UploadRamdiskFiles();
				}
			}
		}
	} catch (SyringeBubble &b) {
		throw b;
	}
}

void FirmwareUploader::UploadImage(char *type) {
	char image[255];
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(image, '\0', 255);
	if (strcmp(type, "kernelcache"))
		snprintf(image, 254, "%s.%s", type, device->model);
	else
		snprintf(image, 254, "kernelcache.release.%c%c%c", device->model[0], device->model[1], device->model[2]);

	FetchImage(type, image);

	if (client->mode != kDfuMode) {
		error = irecv_reset_counters(client);
		if (error != IRECV_E_SUCCESS) {
			throw SyringeBubble("Failed to reset counters.");
		}
	}

	error = irecv_send_file(client, image, 1);
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to upload image.");
	}
}

void FirmwareUploader::FetchImage(char *type, char *output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);

	if (!strcmp(type, "iBSS") || !strcmp(type, "iBEC")) {
		snprintf(name, 63, "%s.%s.RELEASE.dfu", type, device->model);
		snprintf(path, 254, "Firmware/dfu/%s", name);
	} else {
		snprintf(name, 63, "%s.%s.img3", type, device->model);
		snprintf(path, 254, "Firmware/all_flash/all_flash.%s.production/%s", device->model, name);
	}

	if (download_file_from_zip(device->url, path, output, NULL) != 0) { //TODO: Enable &download_callback (NULL for now)
		throw SyringeBubble("Failed to download image. Check your network connection!");
	}
}

void FirmwareUploader::UploadImagePayload(char *type) {
	int size = 0;
	char path[255];
	unsigned char *payload = NULL;
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(path, '\0', 255);

	if (!strcmp(type, "iBEC") || !strcmp(type, "iBoot"))
		snprintf(path, 254, "payloads/cyanide/%s.armv7", "iBoot");
	else
		snprintf(path, 254, "payloads/cyanide/%s.armv7", "iBEC");

	FILE *fp = fopen(path, "rb");
	if (!fp)
		throw SyringeBubble("Unable to open payload");

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	payload = (unsigned char *)malloc(size * sizeof(unsigned char *));
	fread(payload, size, 1, fp);
	fclose(fp);
		
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to upload payload");
	}

	error = irecv_send_buffer(client, (unsigned char *)payload, size, 1);
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to upload payload");
	}
}

void FirmwareUploader::UploadRamdisk() {
	int size = 0;
	unsigned char *ramdisk = NULL;
	irecv_error_t error = IRECV_E_SUCCESS;

	error = irecv_send_buffer(client, (unsigned char *)ramdisk, size, 0);
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to upload ramdisk");
	}

	error = irecv_send_command(client, "setenv filesize 0x1000000");
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to execute ramdisk");
	}

	error = irecv_send_command(client, "ramdisk");
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to execute ramdisk");
	}

	error = irecv_send_command(client, "go kernel bootargs rd=md0 -v amfi_allow_any_signature=1");
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to set kernel bootargs");
	}

	UploadImage("DeviceTree");

	error = irecv_send_command(client, "go devicetree");
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to load device tree");
	}

	UploadImage("kernelcache");

	error = irecv_send_command(client, "go memory copy 0x41000000 0x44800000 0x1000000");
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to append kernelcache");
	}

	error = irecv_send_command(client, "go rdboot");
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to hook jump_to command");
	}

	error = irecv_send_command(client, "bootx");
	if (error != IRECV_E_SUCCESS) {
		throw SyringeBubble("Unable to boot kernelcache");
	}
}

void FirmwareUploader::UploadRamdiskFiles() {

}

