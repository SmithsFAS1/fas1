#include "fwgui.h"
#include "dialog.h"
#include "theme.h"
#include <string.h>
#include <limits.h>

namespace FwGui
{
	Dialog::Dialog(const char* title, int numcontrols)
	:	repaint(true),
		title(strdup(title)),
		numcontrols(numcontrols),
		nextcontrol(0),
		controls(new Control*[numcontrols]),
		selectedControl(0)
	{
		for(int i = 0;i<14;i++)
			defaultControl[i] = -1;
	}
	
	Dialog::~Dialog()
	{
		free(title);
		delete controls;
	}
	
	void Dialog::Paint(Graphics* graphics)
	{
		if(backgroundImage == NULL)
		{
			graphics->SetFillColor(backgroundColor);
			graphics->FillRect(0, 0, 256, 192);
		}
		else
		{
			graphics->Blit(0, 0, 256, 192, backgroundImage);
		}
	
		graphics->SetFillColor(titleBackgroundColor);
		graphics->FillRect(0, 0, 256, 24);
		graphics->SetFont(Font::GUNSHIP_12);
		graphics->SetPenColor(titleTextColor);
		graphics->DrawString(6, 17, title);
	
		for(int i = 0; i < numcontrols; i++)
		{
			controls[i]->Paint(graphics);
		}
	
		controls[selectedControl]->PaintOutline(graphics);
	}
	
	void Dialog::AddControl(Control* control)
	{
		FWGUI_ASSERT(nextcontrol < numcontrols);
	
		controls[nextcontrol] = control;
		nextcontrol++;
	}
	
	void Dialog::AddControl(Control* control, int defaultButton)
	{
		FWGUI_ASSERT(nextcontrol < numcontrols);
	
		controls[nextcontrol] = control;
		
		FWGUI_ASSERT(defaultControl[ButtonToArray(defaultButton)] == -1);
		defaultControl[ButtonToArray(defaultButton)] = nextcontrol;
		
		nextcontrol++;
	}
	
	void Dialog::Select(Control* control)
	{
		for(int i = 0; i < numcontrols; i++)
		{
			if(controls[i] == control)
			{
				selectedControl = i;
			}
		}
		Repaint();
	}

	void Dialog::KeyA()
	{
		if(controls[selectedControl]->IsEnabled())
		{
			ControlClicked(controls[selectedControl]);
		}
	}
	
	void Dialog::KeyB()
	{
		int control = defaultControl[ButtonToArray(KEY_B)];
		if(control != -1)
		{	
			if(controls[control]->IsEnabled())
			{
				ControlClicked(controls[control]);
			}
		}
	}
	
	void Dialog::KeyStart()
	{
		int control = defaultControl[ButtonToArray(KEY_START)];
		if(control != -1)
		{	
			if(controls[control]->IsEnabled())
			{
				ControlClicked(controls[control]);
			}
		}
	}
	
	void Dialog::KeyUp()
	{
		int startX = controls[selectedControl]->x;
		int startY = controls[selectedControl]->y;
		
		int distance = INT_MAX;
		for(int i = 0; i < numcontrols; i++)
		{
			if(controls[i]->y + controls[i]->height < startY)
			{
				int dx = controls[i]->x - startX;
				int dy = controls[i]->y + controls[i]->height - startY;
				if(dx*dx + dy*dy < distance)
				{
					if(controls[i]->IsSelectable())
					{
						selectedControl = i;
						distance = dx*dx + dy*dy;
					}
				}
			}
		}
		Repaint();
	}
	
	void Dialog::KeyDown()
	{
		int startX = controls[selectedControl]->x;
		int startY = controls[selectedControl]->y + controls[selectedControl]->height;
		
		int distance = INT_MAX;
		for(int i = 0; i < numcontrols; i++)
		{
			if(controls[i]->y > startY)
			{
				int dx = controls[i]->x - startX;
				int dy = controls[i]->y - startY;
				if(dx*dx + dy*dy < distance)
				{
					if(controls[i]->IsSelectable())
					{
						selectedControl = i;
						distance = dx*dx + dy*dy;
					}
				}
			}
		}
		Repaint();
	}
	
	void Dialog::KeyLeft()
	{
		int startX = controls[selectedControl]->x;
		int startY = controls[selectedControl]->y;
		
		int distance = INT_MAX;
		for(int i = 0; i < numcontrols; i++)
		{
			if(controls[i]->x + controls[i]->width < startX)
			{
				int dx = controls[i]->x + controls[i]->width - startX;
				int dy = controls[i]->y - startY;
				if(dx*dx + dy*dy < distance)
				{
					if(controls[i]->IsSelectable())
					{
						selectedControl = i;
						distance = dx*dx + dy*dy;
					}
				}
			}
		}
		Repaint();
	}
	
	void Dialog::KeyRight()
	{
		int startX = controls[selectedControl]->x + controls[selectedControl]->width;
		int startY = controls[selectedControl]->y;
		
		int distance = INT_MAX;
		for(int i = 0; i < numcontrols; i++)
		{
			if(controls[i]->x > startX)
			{
				int dx = controls[i]->x - startX;
				int dy = controls[i]->y - startY;
				if(dx*dx + dy*dy < distance)
				{
					if(controls[i]->IsSelectable())
					{
						selectedControl = i;
						distance = dx*dx + dy*dy;
					}
				}
			}
		}
		Repaint();
	}
	
	void Dialog::TouchDown(int x, int y)
	{
		for(int i = 0; i < numcontrols; i++)
		{
			if(controls[i]->Hit(x, y))
			{
				if(controls[i]->IsSelectable())
				{
					selectedControl = i;
				}
				if(controls[i]->IsEnabled())
				{
					ControlClicked(controls[i]);
				}
				Repaint();
			}
		}
	}
	
	void Dialog::TouchHeld(int x, int y)
	{
	}
	
	void Dialog::TouchUp(int x, int y)
	{
	}
	
	int Dialog::ButtonToArray(int button)
	{
		int num = 0;
		for(num = 0;num<14;num++)
		{
			if(button & BIT(0))
				break;
			button = button>>1;
		}
		return num;
	}
}
