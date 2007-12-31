#include "fwgui.h"
#include "textentrydialog.h"
#include "button.h"
#include "imagebutton.h"

#define NUM_KEYS 46
#define UNSHIFTED_CHARS "1234567890-=""qwertyuiop""asdfghjkl""zxcvbnm,./"";' []"
#define SHIFTED_CHARS "!@#$%^&*()_+""QWERTYUIOP""ASDFGHJKL""ZXCVBNM<>?"":~ {}"

namespace FwGui
{
	TextEntryDialog::TextEntryDialog(const char* title, const char* text, int maxLength)
	:	Dialog(title, NUM_KEYS + 5),
		ok(false),
		currentKey(0),
		shifted(false)
	{
		keys = new Button*[NUM_KEYS];
		int x = 10;
		int y = 78;
		AddKeys(x, y, 12);
		x += 9;
		y += 21;
		AddKeys(x, y, 10);
		backspaceButton = new ImageButton(x + 10*19, y, STDIMG_BACKSPACE);
		AddControl(backspaceButton, KEY_B);
		x += 9;
		y += 21;
		AddKeys(x, y, 9);
		enterButton = new Button(x + 9*19, y, 47, 20, "ENTER");
		AddControl(enterButton, KEY_START);
		x += 9;
		y += 21;
		shiftButton = new ImageButton(10, y, STDIMG_SHIFT);
		AddControl(shiftButton);
		AddKeys(x, y, 10);
		x -= 9;
		y += 21;
		AddKeys(x, y, 2);
		x += 38;
		keys[currentKey] = new Button(x, y, 114, 20, "");
		AddControl(keys[currentKey]);
		currentKey++;
		x += 115;
		AddKeys(x, y, 2);
	
		SetKeys();
	
		editBox = new EditBox(10, 39, 236, 24, text, maxLength);
		AddControl(editBox);
		
		closeButton = new ImageButton(232, 0, STDIMG_CLOSE);
		AddControl(closeButton);
	}
	
	TextEntryDialog::~TextEntryDialog()
	{
		for(int i = 0; i < NUM_KEYS; i++)
		{
			delete keys[i];
		}
	
		delete backspaceButton;
		delete shiftButton;
		delete enterButton;
		delete editBox;
		delete closeButton;
	}
	
	void TextEntryDialog::ControlClicked(Control* control)
	{
		if(control == enterButton)
		{
			ok = true;
		}
		else if(control == backspaceButton)
		{
			editBox->Backspace();
			Repaint();
		}
		else if(control == shiftButton)
		{
			shifted = !shifted;
			SetKeys();
			Repaint();
		}
		else if(control == editBox)
		{
			// do nothing
		}
		else if(control == closeButton)
		{
			editBox->SetText("");
			ok = true;
		}
		else
		{
			editBox->AddChar((char)(int)control->GetData());
			if(shifted)
			{
				shifted = false;
				SetKeys();
			}
			
			Repaint();
		}
	}
	
	void TextEntryDialog::AddKeys(int x, int y, int count)
	{
		for(int i = 0; i < count; i++)
		{
			keys[currentKey] = new Button(x, y, 18, 20, "");
			AddControl(keys[currentKey]);
			currentKey++;
			x += 19;
		}
	}
	
	void TextEntryDialog::SetKeys()
	{
		const char* currentChar = shifted ? SHIFTED_CHARS : UNSHIFTED_CHARS;
		int i = 0;
		while(*currentChar != '\0')
		{
			char str[2];
			str[0] = *currentChar;
			str[1] = '\0';
			keys[i]->SetText(str);
			keys[i]->SetData((void*)*currentChar);
			i++;
			currentChar++;
		}
	}
}
