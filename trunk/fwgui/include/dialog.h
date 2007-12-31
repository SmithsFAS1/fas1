#pragma once

#include "control.h"
#include "graphics.h"

namespace FwGui
{
	class Dialog
	{
	public:
		Dialog(const char* title, int numcontrols);
		virtual ~Dialog();
	
		void Paint(Graphics* graphics);
		void Repaint() { repaint = true; }
		bool NeedsRepaint() { return repaint; }
		void SetRepaint(bool repaint) { this->repaint = repaint; }
		void Select(Control* control);
	
		virtual void KeyA();
		virtual void KeyB();
		virtual void KeyStart();
		virtual void KeyUp();
		virtual void KeyDown();
		virtual void KeyLeft();
		virtual void KeyRight();
		virtual void TouchDown(int x, int y);
		virtual void TouchHeld(int x, int y);
		virtual void TouchUp(int x, int y);
		virtual void ControlClicked(Control* control) = 0;
	
	protected:
		void AddControl(Control* control);
		void AddControl(Control* control, int defaultButton);
		int ButtonToArray(int button);
	
		bool repaint;
		char* title;
		int numcontrols;
		int nextcontrol;
		Control** controls;
		int selectedControl;
		int defaultControl[14]; // 14 buttons in the KEYPAD_BITS enum
	};
}
