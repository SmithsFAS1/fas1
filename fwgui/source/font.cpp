#include "fwgui.h"
#include "font.h"

namespace FwGui
{
	Font* Font::GUNSHIP_12;
	Font* Font::VERA_11;
	
	Font::Font(const void* fontdata)
	:	fontdata((const u8*)fontdata),
		header((const FontFileHeader*)fontdata),
		glyphs((const Glyph*)((const u8*)fontdata + sizeof(FontFileHeader)))
	{
		FWGUI_ASSERT(header->type == FONT_SIGNATURE);
	}
	
	int Font::GetHeight() const
	{
		return header->height;
	}
	
	int Font::GetStringWidth(const char* string) const
	{
		int width = 0;
		const char* ptr = string;
		while(*ptr != '\0')
		{
			width += GetGlyph(*ptr)->advancex;
			ptr++;
		}
	
		return width;
	}
	
	const Glyph* Font::GetGlyph(int character) const
	{
		if(character < header->startChar || character > header->endChar)
		{
			return NULL;
		}
	
		return glyphs + (character - header->startChar);
	}
	
	const u8* Font::GetBitmapBuffer(const Glyph* glyph) const
	{
		return fontdata + glyph->offset;
	}
}
