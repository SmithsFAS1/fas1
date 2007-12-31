#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

#include "fontfile.h"

using namespace std;

#define START_CHAR 32
#define END_CHAR 126

void usage(char* base)
{
	cout << "Usage: " << base << " [options] input.ttf output.bin" << endl;
	cout << "Options:" << endl;
	cout << "\t--size px     Fontsize. (default 12)" << endl;
	cout << "\t--start char  First character to encode. (default 32)" << endl;
	cout << "\t--end char    Last character to encode. (default 126)" << endl;
	cout << "\t--auto        Force use of autohinter." << endl;
}

int main(int argc, char *argv[])
{
	cout << "fontconvert v1.0 by Sten Larsson" << endl;

	char* base = basename(argv[0]);

	int size = 0;
	int startChar = 32;
	int endChar = 126;
	bool autohinter = false;

	int arg = 1;
	while(arg < argc && argv[arg][0] == '-')
	{
		if(strcmp(argv[arg], "--help") == 0)
		{
			usage(base);
			return 0;
		}
		else if(strcmp(argv[arg], "--size") == 0)
		{
			if(arg + 1 >= argc)
			{
				usage(base);
				return 1;
			}
			size = atoi(argv[arg+1]);
			arg++;
		}
		else if(strcmp(argv[arg], "--start") == 0)
		{
			if(arg + 1 >= argc)
			{
				usage(base);
				return 1;
			}
			startChar = atoi(argv[arg+1]);
			arg++;
		}
		else if(strcmp(argv[arg], "--end") == 0)
		{
			if(arg + 1 >= argc)
			{
				usage(base);
				return 1;
			}
			endChar = atoi(argv[arg+1]);
			arg++;
		}
		else if(strcmp(argv[arg], "--auto") == 0)
		{
			autohinter = true;
		}

		arg++;
	}

	if(arg + 2 != argc)
	{
		cerr << "Files not specified." << endl;
		return 1;
	}
	char* infilename = argv[arg];
	char* outfilename = argv[arg+1];

	if(size == 0)
	{
		cerr << "Font size not specified or invalid." << endl;
		return 1;
	}

	FT_Library library;
	int error;
	error = FT_Init_FreeType(&library);
	if(error)
	{
		cerr << "Failed to initialize FreeType." << endl;
		return 1;
	}

	FT_Face face;
	error = FT_New_Face(
		library,
		infilename,
		0,
		&face);
	if(error == FT_Err_Unknown_File_Format)
	{
		cerr << "Unsupported input font." << endl;
		return 1;
	}
	else if(error)
	{
		cerr << "Failed to load input font." << endl;
		return 1;
	}

	error = FT_Set_Pixel_Sizes(
		face,           // handle to face object
		0,              // pixel_width
		atoi(argv[2])); // pixel_height
	if(error)
	{
		cerr << "Failed to set pixel size." << endl;
		return 1;
	}

	// don't know how much we need...
	unsigned char* data = new unsigned char[10*1024*1024];
	FontFileHeader* header = (FontFileHeader*)data;
	Glyph* glyphs = (Glyph*)(data + sizeof(FontFileHeader));
	unsigned char* dataEndPtr = data + sizeof(Glyph) * (END_CHAR - START_CHAR + 1);

	Glyph* currentGlyph = glyphs;
	for(int i = startChar; i <= endChar; i++)
	{
		FT_Int32 flags = FT_LOAD_RENDER;
		if(autohinter)
		{
			flags |= FT_LOAD_FORCE_AUTOHINT;
		}
		error = FT_Load_Char(face, i, flags);
		if(error)
		{
			cerr << "Failed to render character " << i << "." << endl;
			return 1;
		}

		currentGlyph->width = face->glyph->bitmap.width;
		currentGlyph->height = face->glyph->bitmap.rows;
		currentGlyph->left = face->glyph->bitmap_left;
		currentGlyph->top = -face->glyph->bitmap_top;
		currentGlyph->advancex = face->glyph->advance.x >> 6;
		currentGlyph->advancey = face->glyph->advance.y >> 6;
		currentGlyph->offset = (unsigned int)(dataEndPtr - data);
		unsigned char* destptr = dataEndPtr;
		for(unsigned int y = 0; y < currentGlyph->height; y++)
		{
			unsigned char* srcptr =
				face->glyph->bitmap.buffer +
				y * face->glyph->bitmap.pitch;

			for(unsigned int x = 0; x < currentGlyph->width; x++)
			{
				*destptr++ = *srcptr++;
			}
		}

		currentGlyph++;
		dataEndPtr = destptr;
	}

	// set up header
	header->type = FONT_SIGNATURE;
	header->filesize = (unsigned int)(dataEndPtr - data);
	header->startChar = startChar;
	header->endChar = endChar;
	header->height = face->size->metrics.height >> 6;

	FILE* rawFile = fopen(outfilename, "wb");
	if(rawFile == NULL)
	{
		cerr << "Failed to open output file " << outfilename << "." << endl;
		perror(base);
		return 1;
	}

	int count = fwrite(data, header->filesize, 1, rawFile);
	if(count != 1)
	{
		cerr << "Failed to write font data." << endl;
		cerr << base << ": " << strerror(ferror(rawFile)) << endl;
		return 1;
	}

	fclose(rawFile);

	cout << outfilename << " saved successfully." << endl;

	return 0;
}
