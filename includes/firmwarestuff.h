#ifndef _FIRMWARESTUFF_H_
#define _FIRMWARESTUFF_H_
#include "libirecovery.h"
#include "libpartial.h"
#include "exploit.h"
#include "payloads.h"

char pois0n_error[255];
irecv_client_t client = NULL;
irecv_device_t device = NULL;

iDeviceTarget pois0n_device;
iDeviceFirmware pois0n_firmware;

int upload_dfu_image(const char* type);
int upload_ibss();
int upload_ibss_payload();
int execute_ibss_payload(int arg);
int boot_iboot();
int boot_tethered();
int boot_ramdisk();
int upload_ibec_payload();
int upload_kernelcache();
int upload_ramdisk();
int upload_devicetree();
int upload_iboot();
int upload_firmware_image(const char* type);
int upload_firmware_payload(const char* type);
int recovery_callback(irecv_client_t client, const irecv_event_t* event);
void download_callback(ZipInfo* info, CDFile* file, size_t progress);
int send_command(char* command);
int fetch_image(const char* path, const char* output);
int fetch_dfu_image(const char* type, const char* output);
int fetch_firmware_image(const char* type, const char* output);


#endif

