#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

#export TARGET		:=	$(shell basename $(CURDIR))
export TARGET		:=	FAS1Flasher
export TOPDIR		:=	$(CURDIR)


#---------------------------------------------------------------------------------
# path to tools - this can be deleted if you set the path in windows
#---------------------------------------------------------------------------------
export PATH		:=	$(DEVKITARM)/bin:$(PATH)

.PHONY: $(TARGET).arm7 $(TARGET).arm9 gbamenu/gbamenu_mb.gba loader/loader.bin

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).ds.gba

$(TARGET).ds.gba	: $(TARGET).nds gbamenu/gbamenu_mb.gba loader/loader.bin
	dsbuild $(TARGET).nds -o temp.ds.gba -l loader/loader.bin
	cat temp.ds.gba gbamenu/gbamenu_mb.gba > $@
	rm temp.ds.gba
	gbafix $@
	
	#FCS image of a slot-1 device appended to DS.GBA = runs in DeSuMe with virtual libfat
	padbin 512 $(TARGET).ds.gba
	cat $(TARGET).ds.gba fcs.img > $(TARGET)_fs.ds.gba
	dlditool fcsr.dldi $(TARGET)_fs.ds.gba
	
#---------------------------------------------------------------------------------
$(TARGET).nds	:	$(TARGET).arm7 $(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 $(TARGET).arm7 -9 $(TARGET).arm9

#---------------------------------------------------------------------------------
$(TARGET).arm7	: arm7/$(TARGET).elf
$(TARGET).arm9	: arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
gbamenu/gbamenu_mb.gba:
	$(MAKE) -C gbamenu

#---------------------------------------------------------------------------------
loader/loader.bin:
	$(MAKE) -C loader

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C gbamenu clean
	$(MAKE) -C loader clean
	rm -f $(TARGET).ds.gba $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9
