# -*-makefile-*-
.PHONY: all clean flash fuses program

F_CPU   = 8000000UL
ISP     = usbasp
MCU     = attiny25
OBJ     = firmware.o io.o avr313/gpr.o avr313/kb.o
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
	rm *.o avr313/*.o *.elf *.hex

flash: $(PRG).hex
	avrdude -c $(ISP) -p $(TARGET) $(PORT) -U flash:w:$(PRG).hex
