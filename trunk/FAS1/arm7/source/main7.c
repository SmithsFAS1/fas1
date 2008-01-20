#include <nds.h>

#include <nds/bios.h>
#include <nds/arm7/touch.h>
#include <nds/arm7/clock.h>

#include <nds.h>
#include "boot7.h"

// stuff below from libnds default arm7

 
//---------------------------------------------------------------------------------
void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format) {
//---------------------------------------------------------------------------------
	SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
	SCHANNEL_SOURCE(channel) = (u32)data;
	SCHANNEL_LENGTH(channel) = bytes >> 2 ;
	SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
}


//---------------------------------------------------------------------------------
s32 getFreeSoundChannel() {
//---------------------------------------------------------------------------------
	int i;
	for (i=0; i<16; i++) {
		if ( (SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0 ) return i;
	}
	return -1;
}

int vcount;
touchPosition first,tempPos;

//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	static int lastbut = -1;
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0;

	but = REG_KEYXY;

	if (!( (but ^ lastbut) & (1<<6))) {
 
		tempPos = touchReadXY();

		if ( tempPos.x == 0 || tempPos.y == 0 ) {
			but |= (1 <<6);
			lastbut = but;
		} else {
			x = tempPos.x;
			y = tempPos.y;
			xpx = tempPos.px;
			ypx = tempPos.py;
			z1 = tempPos.z1;
			z2 = tempPos.z2;
		}
		
	} else {
		lastbut = but;
		but |= (1 <<6);
	}

	if ( vcount == 80 ) {
		first = tempPos;
	} else {
		if (	abs( xpx - first.px) > 10 || abs( ypx - first.py) > 10 ||
				(but & ( 1<<6)) ) {

			but |= (1 <<6);
			lastbut = but;

		} else { 	
			IPC->mailBusy = 1;
			IPC->touchX			= x;
			IPC->touchY			= y;
			IPC->touchXpx		= xpx;
			IPC->touchYpx		= ypx;
			IPC->touchZ1		= z1;
			IPC->touchZ2		= z2;
			IPC->mailBusy = 0;
		}
	}
	IPC->buttons		= but;
	vcount ^= (80 ^ 130);
	SetYtrigger(vcount);

}

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------

	u32 i;


	//sound code  :)
	TransferSound *snd = IPC->soundData;
	IPC->soundData = 0;

	if (0 != snd) {

		for (i=0; i<snd->count; i++) {
			s32 chan = getFreeSoundChannel();

			if (chan >= 0) {
				startSound(snd->data[i].rate, snd->data[i].data, snd->data[i].len, chan, snd->data[i].vol, snd->data[i].pan, snd->data[i].format);
			}
		}
	}

}

int main(int argc, char ** argv)
{
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR; // enable & prepare fifo asap
	IPC->mailData=0;
	IPC->mailSize=0;

	// reset the clock if needed
	rtcReset();
 
	// enable sound
	powerON(POWER_SOUND);
	SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7f);
	IPC->soundData = 0;
	IPC->mailBusy = 0;

	// setup irq
	irqInit();
	irqSet(IRQ_VBLANK, VblankHandler);
	SetYtrigger(80);
	vcount = 80;
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqEnable(IRQ_VBLANK | IRQ_VCOUNT);

	// keep the ARM7 out of main RAM
	while(true)
	{
		swiWaitForVBlank();

		if (IPC->mailData == 1)
		{
			irqDisable(IRQ_ALL);
			BootDsGbaARM7();
		}
		else if (IPC->mailData == 2)
		{
			irqDisable(IRQ_ALL);
			BootGbaARM7();
		}
	}
}
