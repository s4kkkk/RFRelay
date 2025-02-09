STMCUBE_MODULE_PATH = /usr/arm-none-eabi/STM32CubeF1

CC := /usr/bin/arm-none-eabi-gcc
OBJCOPY := /usr/bin/arm-none-eabi-objcopy
ST_FLASH := /usr/bin/st-flash

CFLAGS := -gdwarf-4 -g3 -std=c17 -Wall -mcpu=cortex-m3 -DSTM32F103xB
CFLAGS += -I$(STMCUBE_MODULE_PATH)/Drivers/CMSIS/Core/Include
CFLAGS += -I$(STMCUBE_MODULE_PATH)/Drivers/CMSIS/Device/ST/STM32F1xx/Include
CFLAGS += -I$(CURDIR)/include
CFLAGS += -Tstartup/STM32F103XB_FLASH.ld -nostdlib

.PHONY: all clean upload startup

all: startup

clean:
	rm -rf objs firmware.elf firmware.bin

upload: firmware.bin
	$(ST_FLASH) write $< 0x8000000

startup: startup/startup_stm32f103xb.s startup/system_stm32f1xx.c
	mkdir -p objs
	$(CC) -c -o objs/startup_stm32f103xb.o startup/startup_stm32f103xb.s
	$(CC) $(CFLAGS) -c -o objs/system_stm32f1xx.o startup/system_stm32f1xx.c

firmware.elf: all
	$(CC) $(CFLAGS) -o firmware.elf $(wildcard objs/*.o)

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@


include main/Makefile
include kernel/Makefile
