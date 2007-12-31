#pragma once

#include "control.h"

namespace FwGui
{
	class Button : public Control
	{
	public:
		Button(int x, int y, int width, int height, const char* text);
		virtual ~Button();
	
		void SetText(const char* text);
	
		virtual void Paint(Graphics* graphics);
	
	private:
		char* text;
	};
}
