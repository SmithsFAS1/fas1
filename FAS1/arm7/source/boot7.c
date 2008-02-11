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
	uint8 current, backlight;
    if(((PERSONAL_DATA *)0x023FFC80)->_user_data.gbaScreen) {
            backlight = ~PM_BACKLIGHT_TOP;
    }
    else {
            backlight = ~PM_BACKLIGHT_BOTTOM;
    }
    // Reset the clock if needed
    rtcReset();
    //enable sound
    powerON(POWER_SOUND);
    SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);
    REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
    REG_SPIDATA = 0x80;
    SerialWaitBusy();
    REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz ;
    REG_SPIDATA = 0;
    SerialWaitBusy();
    current = REG_SPIDATA & 0xff;
    current = current & backlight;
    SerialWaitBusy();
    REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHZ | SPI_CONTINUOUS;
    REG_SPIDATA = 0;
    SerialWaitBusy();
    REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHZ;
    REG_SPIDATA = current;
    SerialWaitBusy();

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
