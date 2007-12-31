#include <nds.h>

void BootDsGbaARM7()
{
	REG_IME = 0;

	// Bootloader start address
	*((vu32*)0x027FFE34) = (u32)0x08000000;
	
	// Jump to boot loader
	swiSoftReset();
}

void BootGbaARM7()
{
	REG_IME = 0;

	// http://nocash.emubase.de/gbatek.htm#dsbackwardscompatiblegbamode

	// enable sound amplifier & upper backlight
	SerialWaitBusy();
	REG_SPICNT = SPI_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE | SPI_CONTINUOUS | SPI_DEVICE_POWER;
	REG_SPIDATA = PM_CONTROL_REG;
	SerialWaitBusy();
	REG_SPICNT = SPI_ENABLE | SPI_BAUD_1MHZ | SPI_BYTE_MODE | SPI_DEVICE_POWER;
	REG_SPIDATA = PM_SOUND_AMP | PM_BACKLIGHT_TOP;
	SerialWaitBusy();
	REG_SPICNT = 0;

	while(REG_VCOUNT != 200);  // Wait for VBlank
	
	// enter GBA mode
	__asm volatile (
		"mov r2,#0x40\n"
		"swi 0x1f0000\n"
	:
	:
	: "r2"
	);
}
