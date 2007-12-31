#include "fwgui.h"
#include "label.h"
#include "theme.h"
#include <string.h>

namespace FwGui
{
	Label::Label(int x, int y, int width, int height, const char* text)
	:	Control(x, y, width, height),
		text(strdup(text))
	{
	}
	
	Label::~Label()
	{
		free(text);
	}
	
	void Label::SetText(const char* text)
	{
		free(this->text);
		this->text = strdup(text);
	}
	
	void Label::Paint(Graphics* graphics)
	{
		graphics->SetFont(Font::VERA_11);
		graphics->SetPenColor(labelTextColor);
		graphics->DrawString(x, y + Font::VERA_11->GetHeight(), text);
	}
}
