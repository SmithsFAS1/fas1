#pragma once
#include "fontfile.h"

namespace FwGui
{
	class Font
	{
	public:
		static Font* GUNSHIP_12;
		static Font* VERA_11;
	
		Font(const void* fontdata);
		int GetHeight() const;
		int GetStringWidth(const char* string) const;
		const Glyph* GetGlyph(int character) const;
		const u8* GetBitmapBuffer(const Glyph* glyph) const;
	
	private:
		const u8* fontdata;
		const FontFileHeader* header;
		const Glyph* glyphs;
	};
}
