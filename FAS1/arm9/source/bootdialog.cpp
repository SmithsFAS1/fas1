#include <nds.h>
#include <stdio.h>
#include <string.h>
#include "bootdialog.h"
#include "cartlib.h"
#include "boot9.h"
#include <fat.h> //Fat Support
#include <sys/dir.h> //Read Dir Support
#include "filefactory.h"
#include <time.h>
#include <math.h>

#define min(x, y) ((x) < (y) ? (x) : (y))

using namespace FwGui;

BootDialog::BootDialog()
:	Dialog("ROM FLASH (Y SAVES)", NUM_BUTTONS + 2),
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

BootDialog::~BootDialog()
{
	delete up;
	delete down;

	for(int i = 0; i < NUM_BUTTONS; i++)
	{
		delete buttons[i];
	}
}

void BootDialog::KeyUp()
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

void BootDialog::KeyDown()
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

void BootDialog::KeyLeft()
{
}

void BootDialog::KeyRight()
{
}

void BootDialog::KeyStart()
{
	VisolySetFlashBaseAddress(0);
	BootGbaARM9();
}

void BootDialog::ScanSlot1()
{
	struct stat st;
	int i = 0;
	char filename[256]; // to hold a full filename and string terminator
	DIR_ITER* dir;
	dir = diropen ("/GBA/"); //GBA directory on Slot-1
	char * extension;
	
	if (dir == NULL) {
		iprintf ("/GBA directory not found!\n");
	} else {
		while (dirnext(dir, filename, &st) == 0) {
			extension = strrchr(filename, '.');
			if (strcasecmp(extension, ".gba") == 0) { //.GBA files found, add to list	
				strncpy(items[i].title, filename, 256);
				i++;
			}
			else if (strcasecmp(extension, ".bin") == 0) { //.bin files found, add to list (PogoShell)
				strncpy(items[i].title, filename, 256);
				i++;
			}
		}
	}
		numItems = i;

	for(; i < MAX_ITEMS; i++)
	{
		memset(items + i, 0, sizeof(BootItem));
	}

}

void BootDialog::RefreshSlot1Buttons()
{
	up->SetEnabled(scrollOffset > 0);
	down->SetEnabled(scrollOffset + NUM_BUTTONS < numItems);

	for(int i = 0; i < NUM_BUTTONS; i++)
	{
		Button* button = buttons[i];
		BootItem* item = &items[i + scrollOffset];
	
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

void BootDialog::ControlClicked(Control* control)
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
		BootItem* item = (BootItem*)control->GetData();

		if(item != NULL)
		{
			char theROM[266]; // to hold a full filename and string terminator
			strcpy(theROM,"/GBA/");
			strcat(theROM,item->title);
			
			int o = 0;
			//File Size Get!
			FILE * rom = fopen (theROM,"rb"); //open rom
			long lSize;	
			fseek (rom , 0 , SEEK_END);
			lSize = ftell (rom);
			double b = double(lSize)/1000000;
			rewind (rom);
			fclose(rom);

			printf("\nPress Start to write ROM:\n");
			printf("%s\n", item->title);

			//18 seconds per meg 8000000 1048576
			double flashtime = ((b * 18)/60);
			printf("%.2f megs, about %.1f minutes\n", b, flashtime);
			printf("Press \"B\" to Cancel\n");
			printf("Timeout: \e[s    0 seconds");


			u16 time_left = 600; // 10 second countdown
			s8 done = 0;

			while(!done)
			{
				printf("\e[u\e[0K%5u seconds", (time_left/60)+1);
				vBlank();
				scanKeys();
				if(keysDown() & KEY_START) {
					printf("\n");
					WriteROM(theROM);
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
		}
	}
}

void vBlank() { while(REG_VCOUNT != 160); }
