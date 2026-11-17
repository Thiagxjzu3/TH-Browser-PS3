
TARGET := thbrowser
APPID := THBR00001
TITLE := TH Browser

OFILES := source/main.o

CFLAGS := -I$(PS3DEV)/ppu/include -std=gnu99
LDFLAGS :=

# Force the PlayStation 3 linker.
LD := ppu-gcc

BUILDDIR := build
PKGFILES := data

include $(PSL1GHT)/ppu_rules

pkg: $(TARGET).pkg

clean:
	rm -rf $(TARGET).elf $(TARGET).self $(TARGET).pkg $(BUILDDIR)
