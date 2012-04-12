/**
  * GreenPois0n Cynanide - common.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "lock.h"
#include "common.h"
#include "framebuffer.h"

void(*_free)(void* ptr) = NULL;
void*(*_malloc)(unsigned int size) = NULL;
int(*_printf)(const char *fmt, ...) = NULL;

void* gLoadaddr = NULL;
void* gBaseaddr = NULL;
void* gRomBaseaddr = NULL;
void* gBssBaseaddr = NULL;
void* gBootBaseaddr = NULL;

int cout_count = 0;

void* find_printf() {
	int i = 0;
	int j = 0;
	unsigned int sp;
	unsigned int* stack = &sp;
	void(*default_block_write)(void) = find_function("default_block_write", gBaseaddr, gBaseaddr);
	
	default_block_write();
	for(i = 0; i < 0x100; i += 4) {
		unsigned int value = *(stack - i);
		if((value & 0xFFF00000) == gBaseaddr) {
			for(j = 0; j < 0x100; j++) {
				unsigned short* instruction = (unsigned short*)(value + j);
				if(*instruction == 0xB40F) {
					void(*debug)(const char* fmt, ...) = (void*) value + (j+1);
					debug("stack[%d] = 0x%08x, value[%d] = 0x%04x\n", i, value, j, *instruction);
					return (void*) value + (j+1);
				}
			}
		}
	}
	return 0;
}

void* find_free() {
	return find_function("free", gBaseaddr, gBaseaddr);
}

void* find_malloc() {
	void* bytes = patch_find(gBaseaddr, 0x40000, "\x80\xB5\x00\xAF\x01\x21\x00\x22", 8);
	if (bytes==NULL) return NULL;
	return bytes+1;
}

int common_init() {

	// Setup global device and firmware variables here
	// Still need to add the framebuffer address here, or just look it up later on
	// We could also look up loadaddr later on maybe
	// m68ap = iPhone2g

	if(strstr((char*) (gBootBaseaddr + 0x200), "m68ap")) {
		gLoadaddr = 0xDEADBEEF; // Fix Me!!
		gRomBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBssBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBootBaseaddr = 0xDEADBEEF; // Fix Me!!
		
	// n88ap = iPhone3gs
	} else if(strstr((char*) (gBootBaseaddr + 0x200), "n88ap")) {	
		gLoadaddr = 0x41000000;
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x4FF00000;
		
	// n90ap, n92ap = iPhone4
	} else if(strstr((char*) (0x200), "n90ap")
		 || strstr((char*) (0x200), "n92ap")) {	
		gLoadaddr = 0x41000000;
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x5FF00000;
		
	// n81ap = iPod4g
		gLoadaddr = 0x41000000;
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x5FF00000;
		
	// n18ap = iPod3g
	} else if(strstr((char*) (0x200), "n18ap")) {	
		gLoadaddr = 0x41000000;
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x4FF00000;
		
	// n72ap = iPod2g
	} else if(strstr((char*) (0x200), "n72ap")) {	
		gLoadaddr = 0x09000000;
		gRomBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBssBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBootBaseaddr = 0xFF000000; // Check Me!!
		
	// k48ap = iPad1
	} else if(strstr((char*) (0x200), "k48ap")) {	
		gLoadaddr = 0x41000000;
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x5FF00000;
		
	// k93ap, k94ap, k95ap = iPad2
	} else if(strstr((char*) (0x200), "k93ap") ||
				strstr((char*) (0x200), "k94ap") ||
				strstr((char*) (0x200), "k95ap")) {	
		gLoadaddr = 0xDEADBEEF; // Fix Me!!
		gRomBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBssBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBootBaseaddr = 0xDEADBEEF; // Fix Me!!
		
	// k66ap = AppleTV
	} else if(strstr((char*) (0x200), "k66ap")) {	
		gLoadaddr = 0x41000000;
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x5FF00000;
		
	} else {	
		gLoadaddr = NULL;
		gRomBaseaddr = NULL;
		gBssBaseaddr = NULL;
		gBootBaseaddr = NULL;
		return -1;
	}

	if(strstr((char*) (gBssBaseaddr+0x200), "iBSS")) {
		gBaseaddr = gBssBaseaddr;
	} else if(strstr((char*) (gBootBaseaddr+0x200), "iBoot") || strstr((char*) (gBootBaseaddr+0x200), "iBEC")) {
		gBaseaddr = gBootBaseaddr;
	} else {
		gBaseaddr = NULL;
		return -1;
	}
//	gBaseaddr = gBssBaseaddr;

	// Find our essential functions so we can printf!!
	_printf = find_printf();
	if(_printf == NULL) {
		fb_print("Unable to find printf\n");
		return -1;
	} else {
		printf("Found printf at 0x%x\n", _printf);
	}	

	_malloc = find_malloc();
	if(_malloc == NULL) {
		puts("Unable to find malloc\n");
		return -1;
	} else {
		printf("Found malloc at 0x%x\n", _malloc);
	}

	_free = find_free();
	if(_free == NULL) {
		puts("Unable to find free\n");
		return -1;
	} else {
		printf("Found free at 0x%x\n", _free);
	}

	//printf("Data at gBssBaseaddr: %s\nData at gBootBaseaddr: %s\n", (gBssBaseaddr + 0x200), (gBootBaseaddr + 0x200));
	return 0;
}

void _puts(const char* message) {
	printf("%s", message);
	fb_print(message);
}

void hexdump(unsigned char* buf, unsigned int len) {
	int i, j;
	enter_critical_section();
	printf("0x%08x: ", buf);
	for (i = 0; i < len; i++) {
		if (i % 16 == 0 && i != 0) {
			for (j=i-16; j < i; j++) {
				unsigned char car = buf[j];
				if (car < 0x20 || car > 0x7f) car = '.';
				printf("%c", car);
			}
			printf("\n0x%08x: ", buf+i);
		}
		printf("%02x ", buf[i]);
	}

	int done = (i % 16);
	int remains = 16 - done;
	if (done > 0) {
		for (j = 0; j < remains; j++) {
			printf("   ");
		}
	}

	if ((i - done) >= 0) {
		if (done == 0 && i > 0) done = 16;
		for (j = (i - done); j < i; j++) {
			unsigned char car = buf[j];
			if (car < 0x20 || car > 0x7f) car = '.';
			printf("%c", car);
		}
	}

	printf("\n\n");
	exit_critical_section();
}

void printf_begin() {
	cout_count = 0;
}

void printf_filler() {
	int blanks = (0x201 - (cout_count % 0x200));
	if (blanks > 100 || blanks == 0) return;
	int i;
	for (i = 0; i < blanks; i++) {
		_printf("");
	}
	printf_begin();
}

int gpprintf(const char* fmt, ...) {
	int ret;
	//va_list args;
	enter_critical_section();
	
	//va_start(args, fmt);
	//ret = vprintf(fmt, args);
	//va_end(args);
	
	cout_count += ret;
	printf_filler();
	exit_critical_section();

	return ret;
}


void panic(const char* why) {
	printf("%s", why);
	while(1){}
}
