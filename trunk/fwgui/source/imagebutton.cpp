#include "fwgui.h"
#include "imagebutton.h"
#include "theme.h"

#include "up_bin.h"
#include "down_bin.h"
#include "close_bin.h"
#include "backspace_bin.h"
#include "shift_bin.h"

namespace FwGui
{
	static int widths[] = {20, 20, 24, 37, 26};
	static int heights[] = {20, 20, 24, 20, 20};
	static const u8* bitmaps[] = {up_bin, down_bin, close_bin, backspace_bin, shift_bin};
	
	ImageButton::ImageButton(int x, int y, int width, int height, const u8* bitmap)
	:	Control(x, y, width, height),
		bitmap(bitmap)
	{
		selectable = true;
	}
	
	ImageButton::ImageButton(int x, int y, StandardImage image)
	:	Control(x, y, widths[image], heights[image]),
		bitmap(bitmaps[image])
	{
		selectable = true;
	}
	
	ImageButton::~ImageButton()
	{
	}
	
	void ImageButton::Paint(Graphics* graphics)
	{
		graphics->SetFillColor(enabled ?
			enabledButtonColor :
			disabledButtonColor);
		graphics->FillRect(x, y, width, height);
		graphics->SetPenColor(buttonTextColor);
		graphics->BlitAlpha(x, y, width, height, bitmap);
	}
}
