CROSS ?= arm-elf-
CC = $(CROSS)gcc
LD = $(CROSS)ld
SRC = ../../../..
#BIN2C = $(SRC)/tools/bin2c
LOADADDR = 0x42000000
TYPE = iBoot
MODEL = n92ap
FIRMWARE = 8B117
OBJCOPY = $(CROSS)objcopy
OBJECTS = entry.o main.o common.o commands.o task.o lock.o aes.o bdev.o image.o nvram.o filesystem.o kernel.o memory.o patch.o functions.o framebuffer.o breakpoint.o uart.o radio.o
CFLAGS = -I. -I./.. -I./../.. -I./$(SRC)/include -nostdlib -mlittle-endian -w
LDFLAGS = -Ttext=$(LOADADDR) -nostdlib -lc -lm -lgcc

%.o: $(SRC)/%.S
	@$(CC) -c $(<) -o $(@) $(INCLUDES) $(CFLAGS) 

%.o: $(SRC)/%.c
	@$(CC) -c $(<) -o $(@) $(INCLUDES) $(CFLAGS) 

all: payload

payload: payload.elf
	@cp payload $(SRC)/payloads/$(TYPE).$(MODEL).$(FIRMWARE)
	
payload.elf: $(OBJECTS)
	@$(CC) -o $(@) $(OBJECTS) $(LDFLAGS)
	@$(OBJCOPY) -O binary $(@) payload

clean:
	@rm -rf *.o *.elf payload
	
