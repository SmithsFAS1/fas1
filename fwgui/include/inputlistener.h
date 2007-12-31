#pragma once

namespace FwGui
{
#define FWGUI_NUM_KEYS 14
	enum Key {
		FWKEY_A = 0,
		FWKEY_B,
		FWKEY_SELECT,
		FWKEY_START,
		FWKEY_RIGHT,
		FWKEY_LEFT,
		FWKEY_UP,
		FWKEY_DOWN,
		FWKEY_R,
		FWKEY_L,
		FWKEY_X,
		FWKEY_Y,
		FWKEY_TOUCH,
		FWKEY_LID,
		FWKEY_NONE = -1,
	};

	class InputListener
	{
	public:
		virtual ~InputListener() {};
		virtual void KeyDown(Key key) {};
		virtual void KeyUp(Key key) {};
		virtual void TouchDown(int x, int y) {};
		virtual void TouchDrag(int x, int y) {};
		virtual void TouchUp(int x, int y) {};
	};
}
