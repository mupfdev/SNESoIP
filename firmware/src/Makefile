# -*-makefile-*-
.PHONY: all clean flash


include ../config.mk


all:
	make -C ../


clean:
	rm *.o net/*.o ../*.elf ../*.hex


flash: all
	fboot -d $(PORT) -b 115200 -H -p ../$(PRG).hex
#	fboot -d $(PORT) -b $(BAUD) -v -p ../$(PRG).hex
