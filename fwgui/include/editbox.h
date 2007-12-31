#pragma once

#include "control.h"

namespace FwGui
{
	class EditBox : public Control
	{
	public:
		EditBox(int x, int y, int width, int height, const char* text, int maxLength);
		virtual ~EditBox();
	
		const char* GetText();
		void SetText(const char* text);
	
		void AddChar(char c);
		void Backspace();
	
		virtual void Paint(Graphics* graphics);
	
	private:
		int insertPos;
		int maxLength;
		char* text;
	};
}
