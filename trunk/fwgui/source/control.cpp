#include "fwgui.h"
#include "control.h"
#include "theme.h"

namespace FwGui
{
	Control::Control(int x, int y, int width, int height)
	:	x(x),
		y(y),
		width(width),
		height(height),
		enabled(true),
		selectable(false),
		data(NULL)
	{
	}
	
	Control::~Control()
	{
	}
	
	void Control::SetEnabled(bool enabled)
	{
		this->enabled = enabled;
	}
	
	bool Control::IsEnabled()
	{
		return enabled;
	}
	
	bool Control::IsSelectable()
	{
		return selectable;
	}
	
	void* Control::GetData()
	{
		return data;
	}
	
	void Control::SetData(void* data)
	{
		this->data = data;
	}
	
	void Control::Paint(Graphics* graphics)
	{
	}
	
	void Control::PaintOutline(Graphics* graphics)
	{
		graphics->SetPenColor(selectedColor);
		graphics->DrawRect(x, y, width, height);
	}
	
	bool Control::Hit(int testX, int testY)
	{
		return
			(testX >= x && testX < x + width &&
			testY >= y && testY < y + height);
	}
}
