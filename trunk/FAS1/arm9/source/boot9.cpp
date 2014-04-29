#include <nds.h>
#include <malloc.h>
#include <string.h>
#include "bmp15.h"
#include "boot9.h"

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
	if(((PERSONAL_DATA *)0x023FFC80)->_user_data.gbaScreen) {
            REG_POWERCNT &= ~POWER_SWAP_LCDS;
    }
    else {
            REG_POWERCNT |= POWER_SWAP_LCDS;
    }
	gbaFrame();
/*	
	REG_IME = 0;
	ResetVideo();
	// http://nocash.emubase.de/gbatek.htm#dsbackwardscompatiblegbamode
	REG_EXMEMCNT |= 0x8080;     // ARM7 has access to GBA cart
	REG_POWERCNT = POWER_SWAP_LCDS | POWER_LCD | POWER_2D_A;
	REG_EXMEMCNT &= ~0x4000;    // set Async Main Memory mode
	// notify arm7
*/	IPC->mailData = 2;
	// halt indefinitly, since irqs are disabled
	swiWaitForIRQ();
}

void gbaFrame(void)
{
  u16* frame=NULL;
  cBMP15 frameBMP=createBMP15FromFile("/GBA/gbaframe.bmp");
  if(frameBMP.valid()&&frameBMP.width()==SCREEN_WIDTH&&frameBMP.height()==SCREEN_HEIGHT)
  {
    frame=(u16*)malloc(SCREEN_WIDTH*SCREEN_HEIGHT*2);
    memcpy(frame,frameBMP.buffer(),SCREEN_WIDTH*SCREEN_HEIGHT*2); //tricky thing
  }

  videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000, VRAM_C_SUB_BG_0x06200000, VRAM_D_LCD);

  // for the main screen
  BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(0) | BG_WRAP_OFF;
  BG3_XDX = 1 << 8; //scale x
  BG3_XDY = 0; //rotation x
  BG3_YDX = 0; //rotation y
  BG3_YDY = 1 << 8; //scale y
  BG3_CX = 0; //translation x
  BG3_CY = 0; //translation y

  if(frame)
  {
    dmaCopy(frame,(void*)BG_BMP_RAM(0),SCREEN_WIDTH*SCREEN_HEIGHT*2);
    dmaCopy(frame,(void*)BG_BMP_RAM(8),SCREEN_WIDTH*SCREEN_HEIGHT*2);
    free(frame);
  }
  else
  {
    memset((void*)BG_BMP_RAM(0),0,0x18000);
    memset((void*)BG_BMP_RAM(8),0,0x18000);
  }
}
