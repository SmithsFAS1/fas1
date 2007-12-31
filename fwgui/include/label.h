#pragma once

#include "control.h"

namespace FwGui
{
	class Label : public Control
	{
	public:
		Label(int x, int y, int width, int height, const char* text);
		virtual ~Label();
	
		void SetText(const char* text);
	
		virtual void Paint(Graphics* graphics);
	
	private:
		char* text;
	};
}
