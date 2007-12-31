#include "fwgui.h"
#include "driver.h"
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include "font.h"
#include "gunship_12_bin.h"
#include "vera_11_bin.h"
#include "dialog.h"
#include "theme.h"

#define TOUCH_DELTA 100

namespace FwGui
{
	Driver::Driver()
	:	framebuffer((u16*)BG_BMP_RAM(0)),
		dialog(NULL),
		lastX(INT_MAX),
		lastY(INT_MAX),
		firstTouch(true),
		needGraphicsInit(false),
		listener(NULL)
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
		SetListener(dialog);
		if(dialog != NULL)
		{
			needGraphicsInit = true;
			dialog->Repaint();
		}
	}
	
	void Driver::SetListener(InputListener* listener)
	{
		this->listener = listener;
	}
	
	bool Driver::Tick()
	{
		if(listener != NULL) {
			scanKeys();
			
			uint32_t down = keysDown();
			uint32_t up = keysUp();

			for(int i = 0; i < FWGUI_NUM_KEYS; i++) {
				if(down & BIT(i))
					listener->KeyDown((Key)i);
				if(up & BIT(i))
					listener->KeyUp((Key)i);
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
						
						listener->TouchDown(x, y);
					}
					else
					{
						listener->TouchDrag(x, y);
					}
				}
			}
			else if(keysUp() & KEY_TOUCH)
			{
				firstTouch = true;
				listener->TouchUp(lastX, lastY);
				lastX = INT_MAX;
				lastY = INT_MAX;
			}
		}
		
		if(dialog != NULL && dialog->NeedsRepaint())
		{
			u16* doublebuffer = new u16[256*192];
			Graphics* graphics = new Graphics(doublebuffer, 256, 192);
			if(backgroundImage == NULL)
			{
				graphics->SetFillColor(backgroundColor);
				graphics->FillRect(0, 0, 256, 192);
			}
			else
			{
				graphics->Blit(0, 0, 256, 192, (u16*)backgroundImage);
			}
			dialog->Paint(graphics);
			if(needGraphicsInit)
			{
				InitGraphics();
			}
			memcpy(framebuffer, doublebuffer, 256*192*2);
			if(needGraphicsInit)
			{
				InitGraphics();
				needGraphicsInit = false;
			}
			delete graphics;
			delete doublebuffer;
			dialog->SetRepaint(false);
		}

		return true;
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
		BLEND_Y = 0;
		SUB_BLEND_Y = 0;
	}
}
