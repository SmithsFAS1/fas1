#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flashcartfile.h"
#include "cartlib.h"

FlashCartFile::FlashCartFile(const char* filename, bool write)
:	bufferFill(0),
	filePtr(NULL),
	erasePtr(NULL),
	state(write ? FILESTATE_WRITE : FILESTATE_READ)
{
	DetectFlashCart();
	int offset;
	int end;
	sscanf(filename, "%x/%n", &offset, &end);
	if(end == 0)
	{
		throw "Unknown offset";
	}

	if((offset & FLASHCART_ERASE_BLOCK_SIZE_MASK) != 0)
	{
		throw "Unsupported offset";
	}

	printf("Writing at offset 0x%x\n", offset);
	filePtr = erasePtr = (u8*)0x08000000 + offset;
	bufferFill = 0;
}

FlashCartFile::~FlashCartFile()
{
	if(state != FILESTATE_CLOSED)
	{
		try
		{
			Close();
		}
		catch(...)
		{
		}
	}
}

void FlashCartFile::DetectFlashCart()
{
	SetVisolyFlashRWMode();
	
	printf("Detecting flash type:\n");
	bool isVisolyTurbo = false;
	int type = CartTypeDetect();
	switch(type)
	{
	case 0x16 : printf ("  FA 32M\n");  break;
	case 0x17 : printf ("  FA 64M\n");  break;
	case 0x18 : printf ("  FA 128M\n"); break;
	case 0x2e : printf ("  Standard ROM\n");         break;
	case 0x96 : printf ("  Turbo FA 64M\n");  isVisolyTurbo=1; break;
	case 0x97 : printf ("  Turbo FA 128M\n"); isVisolyTurbo=1; break;
	case 0x98 : printf ("  Turbo FA 256M\n"); isVisolyTurbo=1; break;
	case 0xdc : printf ("  Hudson\n");               break;
	case 0xe2 : printf ("  Nintendo Flash Cart\n");  break;
	default   : printf ("  Unknown\n");              break;
	}

	if(!isVisolyTurbo)
	{
		char e[1024];
		sprintf(e, "Unsupported flashcart (0x%x)", type);
		throw e;
	}
}

int FlashCartFile::Read(void* dest, int length)
{
	throw "Reading from flash cart not supported.";
}

void FlashCartFile::Write(void* source, int length)
{
	if(state != FILESTATE_WRITE)
	{
		throw "Illegal state.";
	}

	u8* dataPtr = (u8*)source;
	int tempLength = length;
	if(bufferFill > 0 &&
		bufferFill + length > FLASHCART_WRITE_BLOCK_SIZE)
	{
		int bufferFree = FLASHCART_WRITE_BLOCK_SIZE - bufferFill;
		memcpy(buffer + bufferFill, dataPtr, bufferFree);
		tempLength -= bufferFree;
		dataPtr += bufferFree;
		DoWrite(buffer, FLASHCART_WRITE_BLOCK_SIZE);
		bufferFill = 0;
	}
	if(tempLength > FLASHCART_WRITE_BLOCK_SIZE) {
		int writeableLength = tempLength & ~FLASHCART_WRITE_BLOCK_SIZE_MASK;
		DoWrite(dataPtr, writeableLength);
		tempLength -= writeableLength;
		dataPtr += writeableLength;
	}

	memcpy(buffer + bufferFill, dataPtr, tempLength);
	bufferFill += tempLength;	
}

void FlashCartFile::Close()
{
	if(state == FILESTATE_WRITE && bufferFill > 0)
	{
		DoWrite(buffer, FLASHCART_WRITE_BLOCK_SIZE);
	}

	state = FILESTATE_CLOSED;
}

void FlashCartFile::DoWrite(u8* source, int length)
{
	while(filePtr + length > erasePtr)
	{
		EraseNextBlock();
	}

	int blockCount = length / FLASHCART_WRITE_BLOCK_SIZE;

	int result = WriteTurboFACart(
		(u32)source,
		(u32)filePtr,
		blockCount);
	if(!result)
	{
		char e[1024];
		sprintf(e, "Failed to write flash at 0x%x", (u32)filePtr);
		throw e;
	}

	result = memcmp(source, filePtr, length);
	if(result != 0)
	{
		char e[1024];
		sprintf(e, "Verify failed at 0x%x", (u32)filePtr);
		throw e;
	}

	filePtr += length;
}

void FlashCartFile::EraseNextBlock()
{
	int result = EraseTurboFABlocks(
		(u32)erasePtr,
		1);
	if(!result)
	{
		char e[1024];
		sprintf(e, "Failed to erase flash at 0x%x", (u32)erasePtr);
		throw e;
	}

	erasePtr += FLASHCART_ERASE_BLOCK_SIZE;
}
