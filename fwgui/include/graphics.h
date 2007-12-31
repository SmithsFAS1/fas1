#pragma once
#include "font.h"

namespace FwGui
{
	struct Color
	{
		Color(u8 r, u8 g, u8 b, u8 a = 0xFF) : r(r), g(g), b(b), a(a) {}
		u8 r, g, b, a;
	};
	
	class Graphics
	{
	public:
		Graphics(u16* imagebuffer, int screenWidth, int screenHeight);
	
		void SetPenColor(const Color& color);
		void SetFillColor(const Color& color);
		void SetFont(const Font* font);
		void DrawString(int x, int y, const char* text);
		void DrawCenteredString(int x, int y, int width, const char* text);
		void DrawRect(int x, int y, int width, int height);
		void FillRect(int x, int y, int width, int height);
		void BlitAlpha(int x, int y, int width, int height, const u8* bitmap);
		void Blit(int x, int y, int width, int height, const u16* bitmap);
	
	private:
		u16* screenBuffer;
		int screenWidth;
		int screenHeight;
		const Font* font;
		Color penColor;
		Color fillColor;
	};
}
