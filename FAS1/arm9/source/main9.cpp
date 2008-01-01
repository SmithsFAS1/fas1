#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

//Fat support - Smiths
#include <fat.h>
#include <sys/dir.h>
//SRAM Stuff again
#define SRAM_START ((u8*)0x0A000000)
#define SRAM_END ((u8*)0x0A03FFFF) //256KB

#include <sys/stat.h>

#include <dswifi9.h>
#include <driver.h>

#include "cartlib.h"
#include "bootdialog.h"
#include "filefactory.h"

BootDialog* dialog = NULL;

// some functions needed by wifi lib
extern "C" {
	void * sgIP_malloc(int size)
	{
		return malloc(size);
	}

	void sgIP_free(void *ptr)
	{
		free(ptr);
	}

	void sgIP_dbgprint(char *txt, ...)
	{
		va_list args;
		va_start(args,txt);
		vprintf(txt,args);
	}

	// wifi timer function, to update internals of sgIP
	void Timer_50ms(void) {
		Wifi_Timer(50);
	}

	// notification function to send fifo message to arm7
	void arm9_synctoarm7()
	{
		IPC_SendSync(0);
	}

	// interrupt handler to receive fifo messages from arm7
	void arm9_ipc_sync()
	{
		Wifi_Sync();
	}
}


// function used by our own server for timeouts
void ResetTimer()
{
	// make a 1 Hz timer
	TIMER0_CR = 0;
	TIMER1_CR = 0;
	TIMER0_DATA = 0xffff - 0x7fff;
	TIMER1_DATA = 0;
	TIMER0_CR = TIMER_DIV_1024;
	TIMER1_CR = TIMER_CASCADE;
}

// function used by our own server for timeouts
int GetTimer()
{
	return TIMER1_DATA;
}

void WaitForKeyPress()
{
	scanKeys();
	do
	{
		swiWaitForVBlank();
		scanKeys();
	}
	while(keysDown() == 0);
}

int main()
{
	IPC->mailData=0;
	IPC->mailSize=0;

	consoleDemoInit();
	irqInit();
	irqEnable(IRQ_VBLANK); // needed by swiWaitForVBlank()

	fatInitDefault(); // initialize FAT - Smiths
	
	FwGui::Driver gui;
	
	printf("Slot1 FlashAdvance Flasher v1.0\n");
	printf("-----------\n");
	printf("Press SELECT to back up Bank 1\n");
	printf("Press Y to write SRAM.SAV\n to Bank 1\n");
	printf("Choose ROM to flash below\n");
	printf("-----------\n");

	try
	{
		// map gba cartridge to arm9
		REG_EXMEMCNT &= ~0x80;
		VisolySetFlashBaseAddress(0);

		dialog = new BootDialog();
		gui.SetActiveDialog(dialog);
			swiWaitForVBlank();
			gui.Tick();
		while(true)
		{
			gui.Tick();
			if(keysDown() & KEY_X)
			{
				//Nada for X
			}
			if(keysUp() & KEY_Y)
			{
				WriteSRAM();
			}
			//Back up SRAM Bank 1 with SELECT - Smiths
			if(keysDown() & KEY_SELECT)
			{
				BackupSRAM();
			}
			if(keysDown() & KEY_START)
			{
				//Anything for Start?
			}
			swiWaitForVBlank();

			dialog->ScanSlot1();
			dialog->RefreshSlot1Buttons();
			dialog->Repaint();
		}
		

	}
	catch(const char* exception)
	{
		printf("\n*** Exception\n%s\n", exception);
	}

	while(true)
	{
		swiWaitForVBlank();
	}
}

//Get that first 64KB! - Smiths
void BackupSRAM() {

FILE * savedata = fopen ("/bank1.sav", "wb");
printf("\nBacking up Bank 1\n");
u8* start = SRAM_START; //Beginning of SRAM
int bytes = 0;
u8* bank1 = SRAM_START+65535; //64KB = bank1 (rest need bank switching I believe)
u8* end = SRAM_END; //unused, but eventually (hopefully) map to end of SRAM for proper 256KB dump
char strbuffer[8]; //only likes to work in blocks of 8 (1 byte at a time!)

printf("Written: \e[s    0 k");

while (start < bank1 - 1) {
	VisolyModePreamble (); //Don't know if needed, FLinker has it though
	memcpy(strbuffer, start, sizeof(strbuffer));
	fwrite((u8*)strbuffer, 1, sizeof(strbuffer), savedata); //1 byte at a time, whee!
	start += 8; //next byte, please
	bytes += 8;
	printf("\e[u\e[0K%5u k", bytes);

	}

fclose (savedata);
printf("\nDone!\n");
printf("File: bank1.sav in root of Slot1\n");

}

//Write Bank1 SRAM - Smiths
void WriteSRAM() {
	FILE * savedata = fopen ("/sram.sav", "rb");
	if (!savedata) { printf("\nFile \"SRAM.SAV\" not found!"); } 
	else {
		u8* start = SRAM_START; //Beginning of SRAM
		int bytes = 0;
		u8* bank1 = SRAM_START+65535; //64KB = bank1 (rest need bank switching I believe)
		char strbuffer[8]; //only likes to work in blocks of 8 (1 byte at a time!)
		printf("\nPress Start to write SRAM\n");
		printf("Press \"B\" to Cancel\n");
		printf("Timeout: \e[s    0 seconds");
		int o = 0;
		u16 time_left = 600; // 10 second countdown
		s8 done = 0;
	
		while(!done)
			{
				printf("\e[u\e[0K%5u seconds", (time_left/60)+1);
				vBlank();
				scanKeys();
				if(keysDown() & KEY_START) {
				printf("\nWriting Bank 1\n");
					printf("Written: \e[s    0 k");
					while (start < bank1 - 1) {
						VisolyModePreamble (); //Don't know if needed, FLinker has it though
						fread((u8*)strbuffer,1,8,savedata); //write a byte of save to memory
						memcpy(start, strbuffer, sizeof(strbuffer));
						start += 8; //next byte, please
						bytes += 8;
						printf("\e[u\e[0K%5u k", bytes);
					}
					o = 0;
					done = 1;
				}
				if(keysDown() & KEY_B) {
					printf("\e[u\e[0K%5s\n", "Aborted!");
					o = 0;
					done = 1;
				}
				time_left--;
	
				if(time_left == 0) {
				done = 1;
				o = 1;
				} 
			}
			if (o) {
				printf("\e[u\e[0K%5s\n", "Timed Out!");
			}

		fclose (savedata);
		printf("\nDone!\n");
	}
}

void WriteROM(const char* filename) {
	FILE * rom = fopen (filename,"rb"); //open rom
	if (!rom) {
	printf("File %s not found!\n", filename);
	} else {
	
	//File Size Get!
	long lSize;	
	fseek (rom , 0 , SEEK_END);
	lSize = ftell (rom);
	rewind (rom);
	//printf("%s\nsize: %i bytes\n",filename,lSize);
	
	int fileposition = 0;
	u32 i = 0;
	int endof = 0;

	//use FileFactory to get File Information for writing to block 0
	File* file = FileFactory::OpenFile("/rom/000000", true);
	unsigned short lastReceivedBlock = 0;
	unsigned int bytesReceived = 0;
	int blocksize = 8;
	int length = blocksize;
	char* buffer[8];

	printf("Written: \e[s    0 k");
	while(endof < lSize) //do Until it's written it all
	{
		fread((u8*)buffer,1,8,rom); //read a byte of rom to buffer
		lastReceivedBlock = (lastReceivedBlock + 1) & 0xFFFF;
		file->Write(buffer, 8); //use FileFactory write command to write that byte
		printf("\e[u\e[0K%5u k", bytesReceived >> 10);
		bytesReceived += length; //increase
		fileposition += 8;
		endof += 8;
		fseek (rom, fileposition, SEEK_SET); //set starting point of rom to next byte
	}
	file->Close();
	delete file;
	fclose(rom);
	printf("\nFile flashed successfully.\n");

	}
}

