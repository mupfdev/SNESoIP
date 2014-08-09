# -*-makefile-*-
.PHONY: all clean flash fuses program


# Basic configuration.
CID     = 0x00
P2CID   = 0x00
SERVER  = snesoip.de

PORT    = /dev/ttyUSB0

# Nothing below this line need to be changed.
F_CPU   = 16000000UL
MCU     = atmega168
OBJ     = firmware.o io.o \
	net/dhcp_client.o net/dnslkup.o net/enc28j60.o net/ip_arp_udp_tcp.o \
	uart/uart.o
PRG     = firmware


all: $(PRG).hex


$(PRG).hex: $(PRG).elf
	avr-objcopy -j .text -j .data -O ihex $(PRG).elf $(PRG).hex
	avr-size --target=ihex $(PRG).hex

$(PRG).elf: $(OBJ)
	avr-gcc $+ -o $(PRG).elf -mmcu=$(MCU)
	avr-size -A $(PRG).elf

%.o: %.c
	avr-gcc $< -Os -DF_CPU=$(F_CPU) \
	-DCID=$(CID) -DP2CID=$(P2CID) -DServerVHost=\"$(SERVER)\" \
	-g -mmcu=$(MCU) -c -o $@


clean:
	rm *.o net/*.o uart/*.o *.elf *.hex

flash: $(PRG).hex
	fboot -d $(PORT) -b 115200 -v -p $(PRG).hex
