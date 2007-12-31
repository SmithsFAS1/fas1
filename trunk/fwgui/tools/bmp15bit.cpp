/*
 * bmp15bit - converts a 24-bit BMP image to a raw 15-bit (plus one alpha bit) image
 *            suitable for Gameboy Advance and Nintendo DS
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

void usage(const char* base)
{
	cout << "Usage: " << base << " [options] input.bmp output.bin" << endl;
	cout << "Options:" << endl;
	cout << "\t--dither  Perform Floyd-Steinberg dithering on output." << endl;
}

int main(int argc, char *argv[])
{
	cout << "bmp15bit v1.0 by Sten Larsson" << endl;

	const char* base = "bmp15bit";
	
	bool dither = false;
	int arg = 1;
	while(arg < argc && argv[arg][0] == '-')
	{
		if(strcmp(argv[arg], "--help") == 0)
		{
			usage(base);
			return 0;
		}
		else if(strcmp(argv[arg], "--dither") == 0)
		{
			dither = true;
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
	if (infoHeader->bitCount != 24) {
		cerr << "Input file must be 24 bits per pixel. (" << infoHeader->bitCount << ")" << endl;
		return 1;
	}
	if (infoHeader->compression != 0) {
		cerr << "Input file must be not be compressed. (" << infoHeader->compression << ")" << endl;
		return 1;
	}

	// allocate space for output data
	int outsize = infoHeader->width * infoHeader->height * 2;
	unsigned char* outputData = new unsigned char[outsize];

	// every row in bmp image data is padded to be divisible by 4
	int stride = (infoHeader->width + ((4 - infoHeader->width % 4) % 4)) * 3;

	// set up temporary storage for dithering calculation
	size_t errorsRowLength = infoHeader->width * 3 + 6;
	double* errorsCurrentRow = new double[errorsRowLength];
	double* errorsNextRow = new double[errorsRowLength];
	memset(errorsCurrentRow, 0, errorsRowLength*8);
	memset(errorsNextRow, 0, errorsRowLength*8);

	// do the conversion and dithering
	unsigned char* inPtr;
	unsigned char* outPtr = outputData;
	for(unsigned int y = 0; y < infoHeader->height; y++)
	{
		// the bottom row comes first in bmp, but top row should be first
		// in output
		inPtr = data + fileHeader.offset + (infoHeader->height - y - 1) * stride;

		// the + 3 is so that we do not need to check the range
		double* errPtrCurrentRow = errorsCurrentRow + 3;
		double* errPtrNextRow = errorsNextRow + 3;	

		for(unsigned int x = 0; x < infoHeader->width; x++)
		{
			// load pixel, take error in account, and lose three bits precision
			unsigned int b = clamp(inPtr[0] + (int)errPtrCurrentRow[0]) >> 3;
			unsigned int g = clamp(inPtr[1] + (int)errPtrCurrentRow[1]) >> 3;
			unsigned int r = clamp(inPtr[2] + (int)errPtrCurrentRow[2]) >> 3;

			// calculate the dither errors
			// (if we don't calculate them dithering is effectivly disabled)
			if(dither)
			{
				// calculate error
				double errB = inPtr[0] - b * 255.0/31.0;
				double errG = inPtr[1] - g * 255.0/31.0;
				double errR = inPtr[2] - r * 255.0/31.0;

				// distribute the errors (the dithering filter)
				errPtrCurrentRow[3] += 7 * errB / 16;
				errPtrCurrentRow[4] += 7 * errG / 16;
				errPtrCurrentRow[5] += 7 * errR / 16;
				errPtrNextRow[-3] +=   3 * errB / 16;
				errPtrNextRow[-2] +=   3 * errG / 16;
				errPtrNextRow[-1] +=   3 * errR / 16;
				errPtrNextRow[ 0] +=   5 * errB / 16;
				errPtrNextRow[ 1] +=   5 * errG / 16;
				errPtrNextRow[ 2] +=   5 * errR / 16;
				errPtrNextRow[ 3] +=   1 * errB / 16;
				errPtrNextRow[ 4] +=   1 * errG / 16;
				errPtrNextRow[ 5] +=   1 * errR / 16;
			}

			// compose output pixel with alpha = 1
			*((short*)outPtr) =
				(1 << 15) |
				(b << 10) |
				(g << 5) |
				(r << 0);

			// go to next pixel
			inPtr += 3;
			outPtr += 2;
			errPtrCurrentRow += 3;
			errPtrNextRow += 3;
		}

		// swap row buffers
		double* tmp = errorsCurrentRow;
		errorsCurrentRow = errorsNextRow;
		errorsNextRow = tmp;
		memset(errorsNextRow, 0, errorsRowLength*8);
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
