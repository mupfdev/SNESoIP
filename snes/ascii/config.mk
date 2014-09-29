# -*-makefile-*-


EMULATOR    = /usr/bin/snes9x
# The name of the output:
TARGET     := $(shell basename $(CURDIR))
# The directory where object files & intermediate files will be placed:
#BUILD      :=
#A list of directories containing source code:
SOURCES    := . src/
# A list of directories containing extra header files:
#INCLUDES   :=
# A list of directories containing resources (*.bmp, *.it, ...):
RESOURCES  := res/
#
FONTFILE   := font.bmp
