# -*-makefile-*-
.PHONY: all clean flash


include ../config.mk


all:
	make -C ../


clean:
	rm *.o net/*.o ../*.elf ../*.hex


flash: all
	fboot -d $(PORT) -b 57600 -H -p ../$(PRG).hex
#	fboot -d $(PORT) -b 57600 -H -v -p ../$(PRG).hex
