#pragma once

#include "graphics.h"

namespace FwGui
{
	class Control
	{
	public:
		Control(int x, int y, int width, int height);
		virtual ~Control();
	
		void SetEnabled(bool enabled);
		bool IsEnabled();
		bool IsSelectable();
	
		void* GetData();
		void SetData(void* data);
	
		virtual void Paint(Graphics* graphics);
		virtual void PaintOutline(Graphics* graphics);
		virtual bool Hit(int testX, int testY);
	
	protected:
		int x;
		int y;
		int width;
		int height;
		bool enabled;
		bool selectable;
		void* data;
		
		friend class Dialog;
	};
}
