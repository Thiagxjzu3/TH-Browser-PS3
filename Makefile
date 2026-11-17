
TARGET := thbrowser
APPID := THBR00001
TITLE := TH Browser

OFILES := source/main.o source/rsxutil.o

CFLAGS := -Iinclude -I$(PS3DEV)/ppu/include -std=gnu99
LDFLAGS :=

LIBS := -lgcm_sys -lrsx -lsysutil -llv2

LD := ppu-gcc

BUILDDIR := build
PKGFILES := data

include $(PSL1GHT)/ppu_rules

pkg: $(TARGET).pkg

clean:
	rm -rf $(TARGET).elf $(TARGET).self $(TARGET).pkg $(BUILDDIR)
