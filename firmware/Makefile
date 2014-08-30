# -*-makefile-*-
.PHONY: all clean flash


include config.mk


all: $(PRG).hex


$(PRG).hex: $(PRG).elf
	avr-objcopy -j .text -j .data -O ihex $(PRG).elf $(PRG).hex
	avr-size --target=ihex $(PRG).hex

$(PRG).elf: $(OBJ)
	avr-gcc $+ -o $(PRG).elf -mmcu=$(MCU)
	avr-size -A $(PRG).elf

%.o: %.c
	avr-gcc $< \
	-Os \
	-fno-tree-scev-cprop \
	-std=c99 \
	-mcall-prologues \
	-Wall \
	-Werror \
	-Wextra \
	-DBAUD=$(BAUD) \
	-DDEBUG=$(DEBUG) \
	-DF_CPU=$(F_CPU) \
	-g -mmcu=$(MCU) -c -o $@


clean:
	rm src/*.o src/net/*.o *.elf *.hex


flash: $(PRG).hex
	fboot -d $(PORT) -b $(BAUD) -H -v -p $(PRG).hex
