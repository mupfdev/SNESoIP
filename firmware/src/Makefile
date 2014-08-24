# -*-makefile-*-
.PHONY: all clean flash


include ../config.mk


all:
	make -C ../


clean:
	rm *.o net/*.o ../*.elf ../*.hex


flash: all
	fboot -d $(PORT) -b $(BAUD) -v -p ../$(PRG).hex
