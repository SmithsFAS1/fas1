#include <nds.h>

void ResetVideo()
{
	// turn off video
	videoSetMode(0);
	videoSetModeSub(0);
	
	// zero out video memory
	vramSetMainBanks(
		VRAM_A_LCD,
		VRAM_B_LCD,
		VRAM_C_LCD,
		VRAM_D_LCD);
	u32* ptr = (u32*)0x6800000;
	while(ptr < (u32*)0x687FFFF)
	{
		*ptr++ = 0;
	}
}

void BootDsGbaARM9()
{
	REG_IME = 0;
	ResetVideo();

	// This works the same way as PassMe. Arm9 will enter an endless loop and
	// Arm7 will jump to the beginning of gba cart. A loader on the cart will
	// copy everything to ram, change the arm9 loop address to point at the
	// new arm9 binary, and then finally jump to the new arm7 binary.

	REG_EXMEMCNT |= 0x8080;     // ARM7 has access to GBA cart
	*((vu32*)0x027FFE04) = (u32)0xE59FF018;  // ldr pc, 0x027FFE24
	*((vu32*)0x027FFE24) = (u32)0x027FFE04;  // Set ARM9 Loop address

	// notify arm7
	IPC->mailData = 1;

	swiSoftReset();
}

void BootGbaARM9()
{
	REG_IME = 0;
	ResetVideo();

	// http://nocash.emubase.de/gbatek.htm#dsbackwardscompatiblegbamode

	REG_EXMEMCNT |= 0x8080;     // ARM7 has access to GBA cart
	REG_POWERCNT = POWER_SWAP_LCDS | POWER_LCD | POWER_2D_A;
	REG_EXMEMCNT &= ~0x4000;    // set Async Main Memory mode

	// notify arm7
	IPC->mailData = 2;

	// halt indefinitly, since irqs are disabled
	swiWaitForIRQ();
}
