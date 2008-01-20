#include <nds.h>
#include <stdio.h>
#include <string.h>
#include "savedialog.h"
#include "cartlib.h"
#include "boot9.h"
#include <fat.h> //Fat Support
#include <sys/dir.h> //Read Dir Support
#include "filefactory.h"
#include <time.h>
#include <math.h>
#define SRAM_START ((u8*)0x0A000000)
#define SRAM_END ((u8*)0x0A03FFFF) //256KB


#define min(x, y) ((x) < (y) ? (x) : (y))

using namespace FwGui;

SaveDialog::SaveDialog()
:	Dialog("SAVE FLASH ([Y] ROM)", NUM_BUTTONS + 2),
	numItems(0),
	scrollOffset(0)
{
	memset(items, 0, sizeof(items));

	for(int i = 0; i < NUM_BUTTONS; i++)
	{
		buttons[i] = new Button(4, 27*(i+1)+1, 222, 24, "");
		AddControl(buttons[i]);
	}

	up = new ImageButton(231, 28, STDIMG_UP);
	AddControl(up);
	down = new ImageButton(231, 167, STDIMG_DOWN);
	AddControl(down);

	ScanSlot1();
	RefreshSlot1Buttons();

}

SaveDialog::~SaveDialog()
{
	delete up;
	delete down;

	for(int i = 0; i < NUM_BUTTONS; i++)
	{
		delete buttons[i];
	}
}

void SaveDialog::KeyUp()
{
	if(selectedControl == 0)
	{
		if(scrollOffset > 0)
		{
			scrollOffset--;
			RefreshSlot1Buttons();
			Repaint();
		}
	}
	else
	{
		selectedControl--;
		Repaint();
	}
}

void SaveDialog::KeyDown()
{
	if(selectedControl == min(numItems, NUM_BUTTONS) - 1)
	{
		if(scrollOffset + NUM_BUTTONS < numItems)
		{
			scrollOffset++;
			RefreshSlot1Buttons();
			Repaint();
		}
	}
	else
	{
		selectedControl++;
		Repaint();
	}
}

void SaveDialog::KeyLeft()
{
}

void SaveDialog::KeyRight()
{
}

void SaveDialog::ScanSlot1()
{
	struct stat st;
	int i = 0;
	char filename[256]; // to hold a full filename and string terminator
	DIR_ITER* dir;
	dir = diropen ("/SAVES/"); //GBA directory on Slot-1
	char * extension;
	
	if (dir == NULL) {
		iprintf ("/SAVES directory not found!\n");
	} else {
		while (dirnext(dir, filename, &st) == 0) {
			extension = strrchr(filename, '.');
			if (strcasecmp(extension, ".sav") == 0) { //.GBA files found, add to list	
				strncpy(items[i].title, filename, 256);
				i++;
			}
		}
	}
		numItems = i;

	for(; i < MAX_ITEMS; i++)
	{
		memset(items + i, 0, sizeof(SaveItem));
	}

}

void SaveDialog::RefreshSlot1Buttons()
{
	up->SetEnabled(scrollOffset > 0);
	down->SetEnabled(scrollOffset + NUM_BUTTONS < numItems);

	for(int i = 0; i < NUM_BUTTONS; i++)
	{
		Button* button = buttons[i];
		SaveItem* item = &items[i + scrollOffset];
	
		if(items[i].title != "")
		{
			char buf[100];
			sprintf(buf,"%s",items[i].title);
			button->SetText(buf);
			button->SetEnabled(true);
			button->SetData(item);
		}
		else
		{
			button->SetText("");
			button->SetEnabled(false);
			button->SetData(NULL);
		}
	}
}

void SaveDialog::ControlClicked(Control* control)
{
	if(control == up)
	{
		scrollOffset--;
		RefreshSlot1Buttons();
		Repaint();
	}
	else if(control == down)
	{
		scrollOffset++;
		RefreshSlot1Buttons();
		Repaint();
	}
	else
	{
		SaveItem* item = (SaveItem*)control->GetData();
		DIR_ITER* dir;
		dir = diropen ("/SAVES/"); //GBA directory on Slot-1
		if (dir) {
		char thesave[300]; // to hold a full filename and string terminator
		strcpy(thesave,"/Saves/");
		strcat(thesave,item->title);
		WriteSRAM(thesave);
		}
}
}

void WriteSRAM(const char* filename)
{
			FILE * savedata = fopen (filename, "rb");
			if (!savedata) { printf("\nFile \"%s\" not found!", filename); } 
			else {
				u8* start = SRAM_START; //Beginning of SRAM
				int bytes = 0;
				u8* bank1 = SRAM_START+65535; //64KB = bank1 (rest need bank switching I believe)
				char strbuffer[8]; //only likes to work in blocks of 8 (1 byte at a time!)
				printf("\nPress Start to write %s\n", filename);
				printf("Press \"B\" to Cancel\n");
				printf("Timeout: \e[s    0 seconds");
				int o = 0;
				int p = 0;
				u16 time_left = 600; // 10 second countdown
				s8 done = 0;
		
				while(!done)
				{
					printf("\e[u\e[0K%5u seconds", (time_left/60)+1);
					vBlank();
					scanKeys();
					if(keysUp() & KEY_START) {
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
					p = 0;
					}
					if(keysUp() & KEY_B) {
						printf("\e[u\e[0K%5s\n", "Aborted!");
						o = 0;
						p = 1;
						done = 1;
					}
					time_left--;
	
					if(time_left == 0) {
						done = 1;
						o = 1;
						p = 1;
					} 
				}
			if (o) {
				printf("\e[u\e[0K%5s\n", "Timed Out!");
			}

			fclose (savedata);
			if ((!p) && done) {
				printf("\nDone!\n");
				}
			}
}

//void vBlank() { while(REG_VCOUNT != 160); }
