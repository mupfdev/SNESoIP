# -*-makefile-*-
.PHONY: all clean fuses flash program
#
# Makefile for Peter Dannegger's bootloader, to be used with the GNU
# toolchest (as opposed to Atmel's Assembler).
#
# Time-stamp: <2010-02-01 12:20:02 hcz>

####### user presets ##########################
# adjust the following definitions to match your target:
##############################################

# ISP settings
ISP     = usbasp
PORT    = usb
TARGET  = m168
# Use the following TARGET if you're using an ATmega168P.
# Nothing else needs to be changed really. For futher details, read:
# http://www.atmel.com/Images/doc8035.pdf
#TARGET = m168p

# MCU name
# One of:
# - at90s2313, at90s2323,
#     at90s2333, at90s2343, at90s4414, at90s4433,
#     at90s4434, at90s8515, at90c8534, at90s8535, at86rf401,
#     attiny2313, attiny24, attiny44,
#     attiny84, attiny25, attiny45, attiny85
# - atmega103, atmega603, at43usb320,
#     at43usb355, at76c711
# - atmega48, atmega8, atmega83,
#     atmega85, atmega88, atmega8515, atmega8535, atmega8hva, at90pwm1,
#     at90pwm2, at90pwm3
# - atmega16, atmega161, atmega162,
#     atmega163, atmega164p, atmega165, atmega165p, atmega168,
#     atmega169, atmega169p, atmega32, atmega323, atmega324p, atmega325,
#     atmega325p, atmega329, atmega329p, atmega3250, atmega3250p,
#     atmega3290, atmega3290p, atmega406, atmega64, atmega640,
#     atmega644, atmega644p, atmega128, atmega1280, atmega1281,
#     atmega645, atmega649, atmega6450, atmega6490, atmega16hva,
#     at90can32, at90can64, at90can128, at90usb82, at90usb162,
#     at90usb646, at90usb647, at90usb1286, at90usb1287, at94k
# - atmega2560, atmega2561
#
# Examples (select one of them or add your own):
# MCU = atmega64
# MCU = attiny85
# MCU = atmega2560
# MCU = atmega1281
MCU = atmega168

# Name of the Atmel defs file for the actual MCU.
#
# They are part of AVR Studio (located in Windows at
# \Programs\Atmel\AVR Tools\AvrAssembler2\Appnotes\*.inc).
#
# You may choose to download AVR000.zip at
# http://attiny.com/definitions.htm instead.  If you do so, just click
# on the HERE in "Click HERE to download all the AVR definitions
# files" and unzip it in the same directory as this Makefile.
#
# Examples (select one of them or add your own):
# ATMEL_INC = m168def.inc
# ATMEL_INC=m64def.inc
# ATMEL_INC=tn85def.inc
# ATMEL_INC = m2560def.inc
# ATMEL_INC = m1281def.inc
ATMEL_INC = m168def.inc

# Processor frequency.  The value is not critical:
#F_CPU = 14745600
F_CPU = 16000000

#     AVR Studio 4.10 requires dwarf-2.
#     gdb runs better with stabs
#DEBUG = dwarf-2
DEBUG = stabs+

# Define the Tx and Rx lines here.  Set both groups to the same for
# one wire mode:
STX_PORT = PORTD
STX = PD1

SRX_PORT = PORTD
SRX = PD0

####### End user presets ######################

SHELL=/bin/bash

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CFLAGS += -I . -I ./added -I ./converted -I/usr/local/avr/include
CFLAGS += -ffreestanding
CFLAGS += -g$(DEBUG)
CFLAGS += -L,-g$(DEBUG)
CFLAGS += -DRAM_START=$(SRAM_START) -DSRAM_SIZE=$(SRAM_SIZE)
CFLAGS += -DSTX_PORT=$(STX_PORT) -DSTX=$(STX)
CFLAGS += -DSRX_PORT=$(SRX_PORT) -DSRX=$(SRX)

# The following files were imported by a gawk script without user
# intervention (in order to ease keeping up with future releases of
# the original bootloader):
AUTO_CONVERTED_FILES = \
  converted/progtiny.inc \
  converted/uart.inc \
  converted/password.inc \
  converted/progmega.inc \
  converted/watchdog.inc \
  converted/bootload.asm \
  converted/abaud.inc \
  converted/command.inc \
  converted/protocol.h \
  converted/apicall.inc \
  converted/verify.inc \
  converted/message.inc

# The following files must be worked on manually:
MANUALLY_ADDED_FILES = \
  added/fastload.inc \
  added/fastload.h \
  added/mangled_case.h \
  added/bootload.S \
  added/compat.h \
  added/fastload.h

ADDITIONAL_DEPENDENCIES = atmel_def.h

ASMSRC = $(AUTO_CONVERTED_FILES) $(MANUALLY_ADDED_FILES)

include atmel_def.mak

ifdef BOOTRST
STUB_OFFSET = 510
LOADER_START = ( $(FLASHEND) * 2 ) - 510
endif

all: bootload.hex

bootload.hex: bootload.elf

bootload.elf : bootload.o stub.o
ifndef BOOTRST
	vars="$$(./get_text_addrs.sh $(FLASHEND))"; \
	arch="$$(./get_avr_arch.sh -mmcu=$(MCU) bootload.o)"; \
	echo "arch=$$arch";\
	echo "$$vars"; \
	eval "$$vars"; \
	sed -e "s/@LOADER_START@/$$LOADER_START/g" \
	    -e s"/@ARCH@/$$arch/" \
	    -e s'/@RAM_START@/$(SRAM_START)/g' \
	    -e s'/@RAM_SIZE@/$(SRAM_SIZE)/g' \
	    -e "s/@STUB_OFFSET@/$$STUB_OFFSET/g" \
	    bootload.template.x > bootload.x; \
	avr-ld -N -E -T bootload.x -Map=$(patsubst %.elf,%,$@).map \
	  --cref $+ -o $@ --defsym Application="$$LOADER_START-2"
else
	vars="$$(./get_bootsection_addrs.sh $(FLASHEND) $(FIRSTBOOTSTART) \
                $(SECONDBOOTSTART) $(THIRDBOOTSTART) $(FORTHBOOTSTART))"; \
	arch="$$(./get_avr_arch.sh -mmcu=$(MCU) bootload.o)"; \
	echo "arch=$$arch";\
	echo "$$vars"; \
	eval "$$vars"; \
	sed -e "s/@LOADER_START@/$$LOADER_START/g" \
	    -e s"/@ARCH@/$$arch/" \
	    -e s'/@RAM_START@/$(SRAM_START)/g' \
	    -e s'/@RAM_SIZE@/$(SRAM_SIZE)/g' \
	    -e "s/@STUB_OFFSET@/$$STUB_OFFSET/g" \
	    bootload.template.x > bootload.x; \
	avr-ld -N -E -T bootload.x -Map=$(patsubst %.elf,%,$@).map \
	  --cref $+ -o $@ --defsym Application=0
endif

atmel_def.h: $(ATMEL_INC) Makefile
#        We use gawk instead of egrep here due to problems with
#        WinAVR's egrep (which I didn't dive into):
	./_conv.awk $< | gawk '/PAGESIZE|SIGNATURE_|SRAM_|FLASHEND|BOOT/' > $@

atmel_def.mak: atmel_def.h
	gawk '{ printf "%s = %s\n", $$2, $$3 }' $< > $@


bootload.o: $(ASMSRC) $(ADDITIONAL_DEPENDENCIES)
	avr-gcc -c -Wa,-adhlns=bootload.lst $(CFLAGS) added/bootload.S -o $@

stub.o: added/stub.S
	avr-gcc -c -Wa,-adhlns=stub.lst $(CFLAGS) $< -o $@

%.hex: %.elf
	avr-objcopy -O ihex $< $@
	@echo
	@echo " "All done.
	@echo " "Now run \'make fuses\' and \'make flash\'
	@echo " "or simply \'make program\' to burn the bootloader onto your SNESoIP.
	@echo

clean:
	rm -f atmel_def.h bootload.x *.defs *.o *.gas *.mak *.lst *.02x *.map *.elf *.hex


###
#  +----------------+---------+-------+
#  | Ext. fuse byte | Bit no. | Value |  efuse = 11110111 = 0x04
#  +----------------+---------+-------+  hfuse = 11011111 = 0xdf
#  | -              |    7    |   0   |  lfuse = 11111100 = 0xff
#  +----------------+---------+-------+
#  | -              |    6    |   0   |  efuse:
#  +----------------+---------+-------+  Unused bits programmed to avoid
#  | -              |    5    |   0   |  avrdude verification bug (#21954).
#  +----------------+---------+-------+  Boot size: 256 words / 512 bytes
#  | -              |    4    |   0   |
#  +----------------+---------+-------+  hfuse: default
#  | -              |    3    |   0   |
#  +----------------+---------+-------+  lfuse:
#  | BOOTSZ1        |    2    |   1   |  Ext. Crystal Osc.; Freq. 8.0- MHz
#  +----------------+----.----+-------+  Start-up time;
#  | BOOTSZ0        |    1    |   0   |  PWRDWN/RESET: 16K CK/14 CK + 65 ms
#  +----------------+---------+-------+
#  | BOOTRST        |    0    |   0   |
#  +----------------+---------+-------+
#  +----------------+---------+-------+ +----------------+---------+-------+
#  | High fuse byte  | Bit no. | Value || Low fuse byte  | Bit no. | Value |
#  +----------------+---------+-------+ +----------------+---------+-------+
#  | RSTDISBL       |    7    |   1   | | CKDIV8         |    7    |   1   |
#  +----------------+---------+-------+ +----------------+---------+-------+
#  | DWEN           |    6    |   1   | | CKOUT          |    6    |   1   |
#  +----------------+---------+-------+ +----------------+---------+-------+
#  | SPIEN          |    5    |   0   | | SUT1           |    5    |   1   |
#  +----------------+---------+-------+ +----------------+---------+-------+
#  | WDTON          |    4    |   1   | | SUT0           |    4    |   1   |
#  +----------------+---------+-------+ +----------------+---------+-------+
#  | EESAVE         |    3    |   1   | | CKSEL3         |    3    |   1   |
#  +----------------+---------+-------+ +----------------+---------+-------+
#  | BODLEVEL2      |    2    |   1   | | CKSEL2         |    2    |   1   |
#  +----------------+---------+-------+ +----------------+---------+-------+
#  | BODLEVEL1      |    1    |   1   | | CKSEL1         |    1    |   1   |
#  +----------------+---------+-------+ +----------------+---------+-------+
#  | BODLEVEL0      |    0    |   1   | | CKSEL0         |    0    |   1   |
#  +----------------+---------+-------+ +----------------+---------+-------+
fuses:
	avrdude -c ${ISP} -p ${TARGET} ${PORT} -u -U efuse:w:0x04:m -U hfuse:w:0xdf:m -U lfuse:w:0xff:m


flash: bootload.hex
	avrdude -c $(ISP) -p $(TARGET) $(PORT) -U flash:w:bootload.hex


program: fuses flash
