#include "fwgui.h"
#include "driver.h"
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include "font.h"
#include "gunship_12_bin.h"
#include "vera_11_bin.h"
#include "dialog.h"

#define TOUCH_DELTA 100

namespace FwGui
{
	Driver::Driver()
	:	framebuffer((u16*)BG_BMP_RAM(0)),
		dialog(NULL),
		lastX(INT_MAX),
		lastY(INT_MAX),
		firstTouch(true),
		needGraphicsInit(false)
	{
		Font::GUNSHIP_12 = new Font(gunship_12_bin);
		Font::VERA_11 = new Font(vera_11_bin);
	}
	
	Driver::~Driver()
	{
		delete Font::GUNSHIP_12;
		delete Font::VERA_11;
	}
	
	void Driver::SetActiveDialog(Dialog* dialog)
	{
		this->dialog = dialog;
		if(dialog != NULL)
		{
			needGraphicsInit = true;
			dialog->Repaint();
		}
	}
	
	void Driver::Tick()
	{
		if(dialog == NULL)
		{
			return;
		}
		
		scanKeys();
		
		if(keysDown() & KEY_A)
		{
			dialog->KeyA();
		}
	
		if(keysDown() & KEY_B)
		{
			dialog->KeyB();
		}
		
		if(keysDown() & KEY_START)
		{
			dialog->KeyStart();
		}
	
		if(keysDown() & KEY_UP)
		{
			dialog->KeyUp();
		}
	
		if(keysDown() & KEY_DOWN)
		{
			dialog->KeyDown();
		}
	
		if(keysDown() & KEY_LEFT)
		{
			dialog->KeyLeft();
		}
	
		if(keysDown() & KEY_RIGHT)
		{
			dialog->KeyRight();
		}
	
		if(keysHeld() & KEY_TOUCH)
		{
			touchPosition touchXY = touchReadXY();
			int x = touchXY.px;
			int y = touchXY.py;
			int dx = x - lastX;
			int dy = y - lastY;
			lastX = x;
			lastY = y;
	
			if(dx*dx + dy*dy < TOUCH_DELTA)
			{
				if(firstTouch)
				{
					firstTouch = false;
					
					dialog->TouchDown(x, y);
				}
				else
				{
					dialog->TouchHeld(x, y);
				}
			}
		}
		else if(keysUp() & KEY_TOUCH)
		{
			firstTouch = true;
			dialog->TouchUp(lastX, lastY);
			lastX = INT_MAX;
			lastY = INT_MAX;
		}
		
		if(dialog->NeedsRepaint())
		{
			u16* doublebuffer = new u16[256*192];
			Graphics* graphics = new Graphics(doublebuffer, 256, 192);
			dialog->Paint(graphics);
			if(needGraphicsInit)
			{
				InitGraphics();
				needGraphicsInit = false;
			}
			memcpy(framebuffer, doublebuffer, 256*192*2);
			delete graphics;
			delete doublebuffer;
			dialog->SetRepaint(false);
		}
	}
	
	void Driver::InitGraphics()
	{
		swiWaitForVBlank();
		lcdMainOnBottom();
		vramSetBankA(VRAM_A_MAIN_BG);
		videoSetMode(MODE_3_2D | DISPLAY_BG3_ACTIVE);
		BG3_CR = BG_BMP16_256x256;
		BG3_XDX = 1<<8;
		BG3_XDY = 0;
		BG3_YDX = 0;
		BG3_YDY = 1<<8;
		BG3_CY = 0;
		BG3_CX = 0;
	
	}
}
