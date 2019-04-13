# -*-makefile-*-
.PHONY: all audiofiles clean font res run sprites


ifeq ($(strip $(DEVKIT65XX)),)
  $(error "Please set DEVKIT65XX in your environment. export DEVKIT65XX=<path to>devkit65XX")
endif

include $(DEVKIT65XX)/snes_rules
include config.mk


# Options for code generation:
CFLAGS += $(INCLUDE)


# A list of directories containing libraries, this must be the top level
# containing include and lib:
LIBDIRS    := $(PVSNESLIB)
#LIBOBJS   +:=

OUTPUT     := $(CURDIR)/$(TARGET)
CFILES     := $(foreach dir,$(SOURCES),$(wildcard $(dir)*.c))
SFILES     := $(foreach dir,$(SOURCES),$(wildcard $(dir)*.asm))
AUDIOFILES := $(foreach dir, $(RESOURCES), $(wildcard $(dir)*.it))
BMPFILES   := $(foreach dir, $(RESOURCES), $(wildcard $(dir)*.bmp))

ifneq ($(AUDIOFILES),)
  ifeq ($(wildcard soundbank.asm),)
    SFILES := soundbank.asm $(SFILES)
  endif
endif


# -- #
OFILES       := $(BINFILES:.bin=.obj) $(CFILES:.c=.obj) $(SFILES:.asm=.obj)
INCLUDE      := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                -I$(CURDIR)/$(BUILD)

SMCONVFLAGS  := -l -s -o soundbank -v -b 5
GTITLE       := -ht"$(TARGET)"


# -- #
all: res $(OUTPUT).sfc
	$(SNTOOLS) -hi! $(GTITLE) $(TARGET).sfc


clean:
	rm -f $(OFILES) $(TARGET).sfc $(TARGET).sym $(BMPFILES:.bmp=.pal) $(BMPFILES:.bmp=.pic) $(FONTFILE:.bmp=.pic) soundbank.*


run:
	$(EMULATOR) $(OUTPUT).sfc


# -- #
soundbank.asm: $(AUDIOFILES)
	$(SMCONV) $(SMCONVFLAGS) $^


audiofiles: soundbank.asm


font: $(FONTFILE)
	$(GFXCONV) -n -gs8 -po2 -pc16 -pe1 -mR! -m! -p! $<


sprites32x32: $(BMPFILES)
	@$(foreach file, $(BMPFILES), $(GFXCONV) -gs32 -pc16 -po16 -n $(file);)


res: audiofiles font sprites32x32


$(OUTPUT).sfc: $(OFILES)
