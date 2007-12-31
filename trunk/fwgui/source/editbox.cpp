#include "fwgui.h"
#include "editbox.h"
#include "theme.h"
#include <string.h>

namespace FwGui
{
	EditBox::EditBox(int x, int y, int width, int height, const char* text, int maxLength)
	:	Control(x, y, width, height),
		maxLength(maxLength)
	{
		this->text = (char*)malloc(maxLength+1);
		memset(this->text, 0, maxLength+1);
		strncpy(this->text, text, maxLength);
		insertPos = strlen(this->text);
	}
	
	EditBox::~EditBox()
	{
		free(text);
	}
	
	const char* EditBox::GetText()
	{
		return text;
	}
	
	void EditBox::SetText(const char* text)
	{
		strncpy(this->text, text, maxLength);
		insertPos = strlen(this->text);
	}
	
	void EditBox::AddChar(char c)
	{
		if(insertPos == maxLength)
		{
			return;
		}
	
		text[insertPos] = c;
		insertPos++;
	}
	
	void EditBox::Backspace()
	{
		if(insertPos == 0)
		{
			return;
		}
	
		insertPos--;
		text[insertPos] = '\0';
	}
	
	void EditBox::Paint(Graphics* graphics)
	{
		graphics->SetFillColor(enabled ?
			enabledEditBoxColor :
			disabledEditBoxColor);
		graphics->FillRect(x, y, width, height);
		graphics->SetPenColor(editBoxOutlineColor);
		graphics->DrawRect(x, y, width, height);
		
		// determine portion of text to draw
		unsigned int textPos = 0;
		if(Font::VERA_11->GetStringWidth(text) + 10 > width) // if we can't fit the whole string
		{
			for(textPos = 0;textPos < strlen(text);textPos++)	// loop through string until it's short enough to be displayed
				if(Font::VERA_11->GetStringWidth(&text[textPos]) + 10 < width)
					break;
		}
		
		// draw text	
		graphics->SetFont(Font::VERA_11);
		graphics->SetPenColor(editBoxTextColor);
		graphics->DrawString(x + 5, y + (height-11)/2 + 10, &text[textPos]);
	
		// draw marker
		graphics->DrawRect(
			x + 5 + Font::VERA_11->GetStringWidth(&text[textPos]) + 1,
			y + 3,
			1,
			height - 6);
	}
}
