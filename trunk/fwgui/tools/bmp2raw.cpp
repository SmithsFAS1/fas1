/*
 * bmp2raw - extracts the raw image data from a bmp file
 *
 * Author: Sten Larsson
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
using namespace std;

#pragma pack(1)
struct BitmapFileHeader { 
	unsigned short type; 
	unsigned int   size; 
	unsigned short reserved1; 
	unsigned short reserved2; 
	unsigned int   offset; 
};

#pragma pack(1)
struct BitmapInfoHeader {
	unsigned int   size; 
	unsigned int   width; 
	unsigned int   height; 
	unsigned short planes; 
	unsigned short bitCount; 
	unsigned int   compression; 
	unsigned int   sizeImage; 
	unsigned int   xPixelsPerMeter; 
	unsigned int   yPixelsPerMeter; 
	unsigned int   colorsUsed; 
	unsigned int   colorsImportant; 
};

template<typename T>
T clamp(T v)
{
	return v < 0 ? 0 : (v > 0xff ? 0xff : v);
}

int main(int argc, char *argv[])
{
	cout << "bmp2raw v1.0 by Sten Larsson" << endl;

	const char* base = "bmp2raw";

	if(argc != 3)
	{
		cout << "Usage: " << base << " input.bmp output.bin" << endl;
		return 1;
	}
	char* infilename = argv[1];
	char* outfilename = argv[2];

	FILE* bmpFile = fopen(infilename, "rb");
	if (bmpFile == NULL)
	{
		cerr << "Failed to open input file " << infilename << "." << endl;
		perror(base);
		return 1;
	}

	// first read header to figure out size of file
	BitmapFileHeader fileHeader;
	size_t count = fread(&fileHeader, sizeof(fileHeader), 1, bmpFile);
	if(count != 1)
	{
		cerr << "Unable to read file header." << endl;
		cerr << base << ": " << strerror(ferror(bmpFile)) << endl;
		return 1;
	}

	if (fileHeader.type != 19778) // "BM"
	{
		cerr << "Input file is not in bmp format." << endl;
		return 1;
	}

	// read entire file
	unsigned char* data = new unsigned char[fileHeader.size];
	rewind(bmpFile);
	count = fread(data, fileHeader.size, 1, bmpFile);
	if(count != 1)
	{
		cerr << "Failed to read file. (" << fileHeader.size << ")" << endl;
		cerr << base << ": " << strerror(ferror(bmpFile)) << endl;
		return 1;
	}

	fclose(bmpFile);

	// check that it's a supported bmp file
	BitmapInfoHeader* infoHeader = (BitmapInfoHeader*)(data + sizeof(BitmapFileHeader));
	if (infoHeader->compression != 0) {
		cerr << "Input file must be not be compressed. (" << infoHeader->compression << ")" << endl;
		return 1;
	}

	// allocate space for output data
	int outsize = infoHeader->width * infoHeader->height;
	unsigned char* outputData = new unsigned char[outsize];

	// every row in bmp image data is padded to be divisible by 4
	int stride = (infoHeader->width + ((4 - infoHeader->width % 4) % 4));

	// do the conversion
	unsigned char* inPtr;
	unsigned char* outPtr = outputData;
	for(unsigned int y = 0; y < infoHeader->height; y++)
	{
		// the bottom row comes first in bmp, but top row should be first
		// in output
		inPtr = data + fileHeader.offset + (infoHeader->height - y - 1) * stride;

		for(unsigned int x = 0; x < infoHeader->width; x++)
		{
			*outPtr++ = *inPtr++;
		}
	}

	FILE* rawFile = fopen(outfilename, "wb");
	if(rawFile == NULL)
	{
		cerr << "Failed to open output file " << outfilename << ".";
		perror(base);
		return 1;
	}

	count = fwrite(outputData, outsize, 1, rawFile);
	if(count != 1)
	{
		cerr << "Failed to write raw data." << endl;
		cerr << base << ": " << strerror(ferror(rawFile)) << endl;
		return 1;
	}

	fclose(rawFile);

	cout << outfilename << " saved successfully." << endl;

	return 0;
}
