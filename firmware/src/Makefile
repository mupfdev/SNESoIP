# -*-makefile-*-
.PHONY: all clean flash


include ../config.mk


all:
	make -C ../


clean:
	rm *.o net/*.o ../*.elf ../*.hex


flash: all
	fboot -d $(PORT) -b 115200 -v -p ../$(PRG).hex
