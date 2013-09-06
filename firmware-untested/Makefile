# -*-makefile-*-
.PHONY: all clean flash fuses program

F_CPU   = 16000000UL
ISP     = usbasp
MCU     = atmega8
OBJ     = firmware.o config.o io.o \
	net/dhcp_client.o net/dnslkup.o net/enc28j60.o net/ip_arp_udp_tcp.o
PORT    = usb
PRG     = firmware
TARGET  = m8


all: $(PRG).hex


$(PRG).hex: $(PRG).elf
	avr-objcopy -j .text -j .data -O ihex $(PRG).elf $(PRG).hex
	avr-size --target=ihex $(PRG).hex

$(PRG).elf: $(OBJ)
	avr-gcc $+ -o $(PRG).elf -mmcu=$(MCU)
	avr-size -A $(PRG).elf

%.o: %.c
	avr-gcc $< -Os -DF_CPU=$(F_CPU) -g -mmcu=$(MCU) -c -o $@


clean:
	rm *.o net/*.o *.elf *.hex

flash: $(PRG).hex
	avrdude -c $(ISP) -p $(TARGET) $(PORT) -U flash:w:$(PRG).hex

fuses:
	avrdude -c ${ISP} -p ${TARGET} ${PORT} -u -U hfuse:w:0xc9:m -U lfuse:w:0xef:m

program: fuses flash
