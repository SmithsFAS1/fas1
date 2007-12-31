#include "fwgui.h"
#include "graphics.h"

#define RGB1555(r, g, b) ((r)|((g)<<5)|((b)<<10)|(1<<15))

namespace FwGui
{
	Graphics::Graphics(u16* screenBuffer, int screenWidth, int screenHeight)
	:	screenBuffer(screenBuffer),
		screenWidth(screenWidth),
		screenHeight(screenHeight),
		font(NULL),
		penColor(Color(0,0,0)),
		fillColor(Color(0,0,0))
	{
	}
	
	void Graphics::SetPenColor(const Color& color)
	{
		penColor = color;
	}
	
	void Graphics::SetFillColor(const Color& color)
	{
		fillColor = color;
	}
	
	void Graphics::SetFont(const Font* font)
	{
		this->font = font;
	}
	
	void Graphics::DrawString(int x, int y, const char* text)
	{
		FWGUI_ASSERT(font != NULL);
	
		int currentX = x;
		int currentY = y;
		const char* ptr = text;
		while(*ptr != '\0')
		{
			switch(*ptr)
			{
			case '\n':
				currentX = x;
				currentY += font->GetHeight();
				break;
	
			default:
				const Glyph* glyph = font->GetGlyph(*ptr);
				if(glyph != NULL)
				{
					BlitAlpha(
						currentX + glyph->left,
						currentY + glyph->top,
						glyph->width,
						glyph->height,
						font->GetBitmapBuffer(glyph));
					currentX += glyph->advancex;
					currentY += glyph->advancey;
				}
				break;
			}
	
			ptr++;
		}
	}
	
	void Graphics::DrawCenteredString(int x, int y, int width, const char* text)
	{
		FWGUI_ASSERT(font != NULL);
	
		int textWidth = font->GetStringWidth(text);
		DrawString(x + (width - textWidth)/2, y, text);
	}
	
	int getLowerLimit(int start)
	{
		return start < 0 ? - start : 0;
	}
	
	int getUpperLimit(int start, int size, int screenSize)
	{
		return start + size >= screenSize ? screenSize - start : size;
	}
	
	void Graphics::DrawRect(int x, int y, int width, int height)
	{
		int r = penColor.r >> 3;
		int g = penColor.g >> 3;
		int b = penColor.b >> 3;
		u16 color = RGB1555(r,g,b);
	
		int beginCol = getLowerLimit(x);
		int beginRow = getLowerLimit(y);
		int endCol = getUpperLimit(x, width, screenWidth);
		int endRow = getUpperLimit(y, height, screenHeight);
		for(int row = beginRow; row < endRow; row++)
		{
			int cx = x + beginCol;
			int cy = y + row;
			*(screenBuffer + cx + cy * screenWidth) = color;
		}
		for(int row = beginRow; row < endRow; row++)
		{
			int cx = x + endCol - 1;
			int cy = y + row;
			*(screenBuffer + cx + cy * screenWidth) = color;
		}
		for(int col = beginCol; col < endCol; col++)
		{
			int cx = x + col;
			int cy = y + beginRow;
			*(screenBuffer + cx + cy * screenWidth) = color;
		}
		for(int col = beginCol; col < endCol; col++)
		{
			int cx = x + col;
			int cy = y + endRow - 1;
			*(screenBuffer + cx + cy * screenWidth) = color;
		}
	}
	
	void Graphics::FillRect(int x, int y, int width, int height)
	{
		int beginCol = getLowerLimit(x);
		int beginRow = getLowerLimit(y);
		int endCol = getUpperLimit(x, width, screenWidth);
		int endRow = getUpperLimit(y, height, screenHeight);
		for(int row = beginRow; row < endRow; row++)
		{
			for(int col = beginCol; col < endCol; col++)
			{
				int cx = x + col;
				int cy = y + row;
	
				u16* ptr = screenBuffer + cx + cy * screenWidth;
				u16 pixel = *ptr;
	
				unsigned int b = (pixel >> 10) & 0x1F;
				unsigned int g = (pixel >>  5) & 0x1F;
				unsigned int r = (pixel >>  0) & 0x1F;
	
				unsigned int intensity = fillColor.a >> 3;
				b = (b * (32 - intensity) + (fillColor.b >> 3) * intensity) >> 5;
				g = (g * (32 - intensity) + (fillColor.g >> 3) * intensity) >> 5;
				r = (r * (32 - intensity) + (fillColor.r >> 3) * intensity) >> 5;
	
				*ptr = RGB1555(r, g, b);
			}
		}
	}
	
	void Graphics::BlitAlpha(int x, int y, int width, int height, const u8* bitmap)
	{
		int beginCol = getLowerLimit(x);
		int beginRow = getLowerLimit(y);
		int endCol = getUpperLimit(x, width, screenWidth);
		int endRow = getUpperLimit(y, height, screenHeight);
	
		for(int row = beginRow; row < endRow; row++)
		{
			for(int col = beginCol; col < endCol; col++)
			{
				int cx = x + col;
				int cy = y + row;
	
				u16* ptr = screenBuffer + cx + cy * screenWidth;
	
				u16 pixel = *ptr;
				unsigned int b = (pixel >> 10) & 0x1F;
				unsigned int g = (pixel >>  5) & 0x1F;
				unsigned int r = (pixel >>  0) & 0x1F;
	
				unsigned int intensity = bitmap[col + row*width] >> 3;
				b = (b * (32 - intensity) + (penColor.b >> 3) * intensity) >> 5;
				g = (g * (32 - intensity) + (penColor.g >> 3) * intensity) >> 5;
				r = (r * (32 - intensity) + (penColor.r >> 3) * intensity) >> 5;
	
				*ptr = RGB1555(r, g, b);
			}
		}
	}
	
	void Graphics::Blit(int x, int y, int width, int height, const u16* bitmap)
	{
		int beginCol = getLowerLimit(x);
		int beginRow = getLowerLimit(y);
		int endCol = getUpperLimit(x, width, screenWidth);
		int endRow = getUpperLimit(y, height, screenHeight);
	
		for(int row = beginRow; row < endRow; row++)
		{
			for(int col = beginCol; col < endCol; col++)
			{
				int cx = x + col;
				int cy = y + row;
	
				u16* ptr = screenBuffer + cx + cy * screenWidth;
	
				*ptr = *bitmap++;
			}
		}
	}
}
