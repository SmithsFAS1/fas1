#pragma once

#define FONT_SIGNATURE 0x544E4F46

namespace FwGui
{
	struct FontFileHeader
	{
		unsigned int type;
		unsigned int filesize;
		int startChar;
		int endChar;
		int height;
	};
	
	struct Glyph {
		unsigned int width;
		unsigned int height;
		int left;
		int top;
		int advancex;
		int advancey;
		unsigned int offset;
	};
}
