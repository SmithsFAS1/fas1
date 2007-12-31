#include "fwgui.h"
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "button.h"
#include "theme.h"

namespace FwGui
{
	Button::Button(int x, int y, int width, int height, const char* text)
	:	Control(x, y, width, height),
		text(strdup(text))
	{
		selectable = true;
	}
	
	Button::~Button()
	{
		free(text);
	}
	
	void Button::SetText(const char* text)
	{
		free(this->text);
		this->text = strdup(text);
	}
	
	void Button::Paint(Graphics* graphics)
	{
		graphics->SetFillColor(enabled ?
			enabledButtonColor :
			disabledButtonColor);
		graphics->FillRect(x, y, width, height);
		graphics->SetFont(Font::VERA_11);
		graphics->SetPenColor(buttonTextColor);
		graphics->DrawCenteredString(x, y + (height-11)/2 + 10, width, text);
	}
}
