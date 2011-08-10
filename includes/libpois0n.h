#ifndef _LIBPOIS0N_H_
#define _LIBPOIS0N_H_
#include "exploit.h"

void pois0n_init(iDeviceTarget device, iDeviceFirmware firmware);
int pois0n_get_exploits();
char *pois0n_get_name_for_exploit(int num);
void pois0n_load_exploit(int num);
int pois0n_is_ready();
int pois0n_inject(int arg);
void pois0n_exit();
void pois0n_set_error(char *error);
char *pois0n_get_error();

#endif
