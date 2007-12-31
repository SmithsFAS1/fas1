#pragma once

namespace FwGui
{
	class Dialog;
	
	class Driver
	{
	public:
		Driver();
		~Driver();
		
		void SetActiveDialog(Dialog* dialog);
		
		void Tick();
	
	private:
		void InitGraphics();
	
		unsigned short* framebuffer;
		Dialog* dialog;
		int lastX;
		int lastY;
		bool firstTouch;
		bool needGraphicsInit;
	};
}
