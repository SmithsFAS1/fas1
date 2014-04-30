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

//#include <dswifi9.h>
#include <driver.h>

#include "cartlib.h"
#include "bootdialog.h"
#include "savedialog.h"
#include "filefactory.h"
#include "boot9.h"

BootDialog* dialog = NULL;
SaveDialog* dialog2 = NULL;
bool nintendo = 0;
bool visoly = 0;

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

	fatInitDefault(); // initialize FAT
	
	FwGui::Driver gui;
	
	printf("FlashAdvance Slot1 Flasher v1.6b\n");
	printf("-----------\n");
	printf("Press SELECT to back up Bank 1\n");
	printf("Press L+R+START to boot Slot-2\n");
	printf("Press Y to switch menus\n");
	printf("-----------\n");

	try
	{
		// map gba cartridge to arm9
		REG_EXMEMCNT &= ~0x80;
		VisolySetFlashBaseAddress(0);
		int di = 0; //ugly active gui checking, even = files, odd = saves
		dialog = new BootDialog();
		dialog2 = new SaveDialog();
		gui.SetActiveDialog(dialog);
			swiWaitForVBlank();
			gui.Tick();
		while(true)
		{
			gui.Tick();
			if((keysHeld() & KEY_START) && (keysHeld() & KEY_L) && (keysHeld() & KEY_R))
			{
				VisolySetFlashBaseAddress(0);
				BootGbaARM9();
			}
			if(keysDown() & KEY_Y)
			{
				//Dialog Switch
				if (di%2 == 0) { 
				gui.SetActiveDialog(dialog); 
				di++;
				dialog->ScanSlot1();
				dialog->RefreshSlot1Buttons();
				dialog->Repaint();
				}
				else { 
				gui.SetActiveDialog(dialog2); 
				di++;
				dialog2->ScanSlot1();
				dialog2->RefreshSlot1Buttons();
				dialog2->Repaint();
				}
			}
			if(keysUp() & KEY_SELECT)	 //Back up SRAM Bank 1 with SELECT - Smiths
			{
				BackupSRAM();
			}
			swiWaitForVBlank();
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

	int fileposition = 0;
	long endof = 0;
	int i = 0;
	File* file = FileFactory::OpenFile("/rom/000000", true); 	//use FileFactory to get File Information for writing to block 0, should set nintendo flag

	if (nintendo || visoly)
	{
		unsigned short lastReceivedBlock = 0;
		unsigned int bytesReceived = 0;
		int blocksize = 8;
		int length = blocksize;
		printf("Written: \e[s         0 k");
		char* buffer[blocksize];
		while(endof <= lSize) //do Until it's written it all
		{
			if (i >= 262144 && endof<16777168) {
				printf("\e[u\e[0K%10u k %u|%u", bytesReceived >> 10,endof,lSize);
				i = 0;
			}
			fread((u8*)buffer,1,8,rom); //read a byte of rom to buffer
			lastReceivedBlock = (lastReceivedBlock + 1) & 0xFFFF;
			
			if ((file->Write(buffer, blocksize, nintendo)) == 0) { endof = lSize+200; } //if any errors in file writing, dirty abort
			bytesReceived += blocksize; //increase
			fileposition += blocksize;
			endof += blocksize;
			i += blocksize;
			if (!(endof>=lSize))
				fseek (rom, fileposition, SEEK_SET); //set starting point of rom to next byte
			//if (endof>=16777168) printf("\n>16M:%lu|t:%lu",endof,lSize);
		}
		file->Close(nintendo);
		delete file;
		fclose(rom);
		(endof>=lSize+200) ? printf("\nError Flashing File.\n") : printf("\nFile flashed successfully.\n");
		}
	}
}

//Get that first 64KB! - Smiths
void BackupSRAM() {
	DIR_ITER* dir;
	dir = diropen ("/Saves/"); //Saves dir exist?
	if (!dir) { printf("\nCreating SAVES directory\n"); mkdir ("/Saves", 1); }
	FILE * savedata = fopen ("/saves/bank1.sav", "wb");
	printf("\nBacking up Bank 1\n");
	u8* start = SRAM_START; //Beginning of SRAM
	int bytes = 0; int i = 0;
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
		i += 8;
		if (i >= 4096) {printf("\e[u\e[0K%5u k", bytes); i=0;}
	}
	fclose (savedata);
	printf("\nDone!\n");
	printf("File: /Saves/bank1.sav on Slot1\n");	
}