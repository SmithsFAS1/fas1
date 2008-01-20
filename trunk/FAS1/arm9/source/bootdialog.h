#pragma once

#include <dialog.h>
#include <label.h>
#include <button.h>
#include <imagebutton.h>

#define NUM_BUTTONS 6
#define MAX_ITEMS 32

void WriteROM(const char* filename);
void BackupSRAM();
void WriteSRAM();
void vBlank();

struct BootItem
{
//	Filetype filetype;
	char title[256]; //longer for GBA ROM name

	char* address;
};	

class BootDialog : public FwGui::Dialog
{
public:
	BootDialog();
	virtual ~BootDialog();

	void ScanSlot1();
	void RefreshSlot1Buttons();
	void wait ( int seconds );
	virtual void ControlClicked(FwGui::Control* control);
	virtual void KeyUp();
	virtual void KeyDown();
	virtual void KeyLeft();
	virtual void KeyRight();
	virtual void KeyStart();

private:
	FwGui::ImageButton* up;
	FwGui::ImageButton* down;
	FwGui::Button* buttons[NUM_BUTTONS];
	BootItem items[MAX_ITEMS];
	int numItems;
	int scrollOffset;
};
