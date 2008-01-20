#pragma once

#include <dialog.h>
#include <label.h>
#include <button.h>
#include <imagebutton.h>

#define NUM_BUTTONS 6
#define MAX_ITEMS 32

void BackupSRAM();
void WriteSRAM(const char* filename);
void vBlank();
void set_ram_start(int i);
int fcSetSRamStart(int offset);

struct SaveItem
{
//	Filetype filetype;
	char title[256]; //longer for GBA ROM name

	char* address;
};	

class SaveDialog : public FwGui::Dialog
{
public:
	SaveDialog();
	virtual ~SaveDialog();

	void ScanSlot1();
	void RefreshSlot1Buttons();
	void wait ( int seconds );
	virtual void ControlClicked(FwGui::Control* control);
	virtual void KeyUp();
	virtual void KeyDown();
	virtual void KeyLeft();
	virtual void KeyRight();

private:
	FwGui::ImageButton* up;
	FwGui::ImageButton* down;
	FwGui::Button* buttons[NUM_BUTTONS];
	SaveItem items[MAX_ITEMS];
	int numItems;
	int scrollOffset;
};
