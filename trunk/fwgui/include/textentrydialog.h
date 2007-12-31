#pragma once

#include <dialog.h>
#include <editbox.h>

namespace FwGui
{
	class Button;
	class ImageButton;
	
	class TextEntryDialog : public Dialog
	{
	public:
		TextEntryDialog(const char* title, const char* text, int maxLength);
		virtual ~TextEntryDialog();
	
		virtual void ControlClicked(Control* control);
	
		bool ok;
		
		const char* GetText() { return editBox->GetText(); }
		void SetText(const char* text) { editBox->SetText(text); Repaint(); }
	
	private:
		void AddKeys(int x, int y, int count);
		void SetKeys();
	
		Button** keys;
		ImageButton* backspaceButton;
		ImageButton* shiftButton;
		Button* enterButton;
		EditBox* editBox;
		ImageButton* closeButton;
		int currentKey;
		bool shifted;
	};
}
