###
# GreenPois0n Anthrax - src/Makefile
# Copyright (C) 2010 Chronic-Dev Team
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
###
 
SDKVER = 5.0
PLATFORM = /Developer/Platforms/iPhoneOS.platform
SDK = $(PLATFORM)/Developer/SDKs/iPhoneOS$(SDKVER).sdk
CC = $(PLATFORM)/Developer/usr/bin/gcc-4.2
STRIP = $(PLATFORM)/Developer/usr/bin/strip
LDID = /usr/local/bin/ldid
CFLAGS = -arch armv6 -isysroot $(SDK) -w -Iincludes/

TARGETS = $(LAUNCHD_TARGET) $(ACCD_TARGET)

ACCD_TARGET = accd

# Setupp objects for launchd
LAUNCHD_TARGET = launchd
LAUNCHD_OBJECTS = launchd.o syscalls.o utils.o device.o firmware.o
LAUNCHD_CFLAGS = $(CFLAGS) -static -nostartfiles -nodefaultlibs -nostdlib
LAUNCHD_LDFLAGS = -Wl,-e,_main


PWD = $(shell pwd)

%.o: %.S
	@$(CC) -c $(<) -o $(@) $(LAUNCHD_CFLAGS)

%.o: %.c
	@$(CC) -c $(<) -o $(@) $(LAUNCHD_CFLAGS) 

all: $(TARGETS)

launchd: $(LAUNCHD_OBJECTS)
	@echo "Building launchd..."
	@$(CC) -o $(LAUNCHD_TARGET) $(LAUNCHD_OBJECTS) $(LAUNCHD_CFLAGS) $(LAUNCHD_LDFLAGS)
	@$(STRIP) $(LAUNCHD_TARGET)
	@ldid -S $(LAUNCHD_TARGET)

accd:
	@echo "Building accd..."
	@$(CC) -c -o watchdog.o watchdog.c $(CFLAGS) -F/Developer/SDKs/MacOSX10.7.sdk/System/Library/Frameworks/
	@$(CC) -c -o usbmux.o usbmux.c $(CFLAGS) -F/Developer/SDKs/MacOSX10.7.sdk/System/Library/Frameworks/
	@$(CC) -o $(ACCD_TARGET) accd.c watchdog.o usbmux.o $(CFLAGS) -framework IOKit -framework CoreFoundation
	@$(STRIP) $(ACCD_TARGET)
	@ldid -S $(ACCD_TARGET)

clean:
	@rm -rf *.o launchd accd

