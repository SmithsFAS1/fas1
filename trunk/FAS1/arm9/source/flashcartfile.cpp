#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flashcartfile.h"
#include "cartlib.h"

#define SHARP28F_BLOCKERASE 0x20
#define SHARP28F_CONFIRM    0xD0
#define SHARP28F_READARRAY  0xff
#define SHARP28F_WORDWRITE  0x10
#define INTEL28F_READARRAY  0xff


FlashCartFile::FlashCartFile(const char* filename, bool write)
:	bufferFill(0),
	filePtr(NULL),
	erasePtr(NULL),
	state(write ? FILESTATE_WRITE : FILESTATE_READ)
{
	nintendo = false;
	DetectFlashCart();
	int offset;
	int end;
	sscanf(filename, "%x/%n", &offset, &end);
	if(end == 0)
	{
		printf("\nUnknown offset\n");
		throw "Unknown offset";
	}
	if (!nintendo){
		if((offset & FLASHCART_ERASE_BLOCK_SIZE_MASK) != 0)
		{
			printf("\nUnsupported offset\n");
			throw "Unsupported offset";
		}
	}else{
		if((offset & NOA_FLASHCART_ERASE_BLOCK_SIZE_MASK) != 0)
		{
			printf("\nUnsupported offset\n");
			throw "Unsupported offset";
		}

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
			Close(nintendo);
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
	visoly = false;
	nintendo = false;
	int type = CartTypeDetect();
	switch(type)
	{
	case 0x16 : printf ("  FA 32M\n");  break;
	case 0x17 : printf ("  FA 64M\n");  break;
	case 0x18 : printf ("  FA 128M\n"); break;
	case 0x2e : printf ("  Standard ROM\n");         break;
	case 0x96 : printf ("  Turbo FA 64M\n");  visoly=1; break;
	case 0x97 : printf ("  Turbo FA 128M\n"); visoly=1; break;
	case 0x98 : printf ("  Turbo FA 256M\n"); visoly=1; break;
	case 0xdc : printf ("  Hudson\n");               break;
	case 0xe2 : printf ("  Nintendo Flash Cart\n"); nintendo=1;  break;
	default   : printf ("  Unknown\n");              break;
	}

	if(!visoly && !nintendo)
	{
		printf("\nUnsupported flashcart (0x%x)\n", type);
	}
}

int FlashCartFile::Read(void* dest, int length)
{
	printf("\nReading from flash cart not supported.\n");
	throw "Reading from flash cart not supported.";
}

void FlashCartFile::Write(void* source, int length, bool nin)
{
	if(state != FILESTATE_WRITE)
	{
		printf("\nIllegal state.\n");
		throw "Illegal state.";
	}

	if (!nin){
		u8* dataPtr = (u8*)source;
		int tempLength = length;
		if(bufferFill > 0 &&
			bufferFill + length > FLASHCART_WRITE_BLOCK_SIZE)
		{
			int bufferFree = FLASHCART_WRITE_BLOCK_SIZE - bufferFill;
			memcpy(buffer + bufferFill, dataPtr, bufferFree);
			tempLength -= bufferFree;
			dataPtr += bufferFree;
			DoWrite(buffer, FLASHCART_WRITE_BLOCK_SIZE, nin);
			bufferFill = 0;
		}
		if(tempLength > FLASHCART_WRITE_BLOCK_SIZE) {
			int writeableLength = tempLength & ~FLASHCART_WRITE_BLOCK_SIZE_MASK;
			DoWrite(dataPtr, writeableLength, nin);
			tempLength -= writeableLength;
			dataPtr += writeableLength;
		}
		memcpy(buffer + bufferFill, dataPtr, tempLength);
		bufferFill += tempLength;	
	}else{
		u8* dataPtr = (u8*)source;
		int tempLength = length;
		if(bufferFill > 0 &&
			bufferFill + length > NOA_FLASHCART_WRITE_BLOCK_SIZE)
		{
			int bufferFree = NOA_FLASHCART_WRITE_BLOCK_SIZE - bufferFill;
			memcpy(noabuffer + bufferFill, dataPtr, bufferFree);
			tempLength -= bufferFree;
			dataPtr += bufferFree;
			DoWrite(noabuffer, NOA_FLASHCART_WRITE_BLOCK_SIZE, nin);
			bufferFill = 0;
		}
		if(tempLength > NOA_FLASHCART_WRITE_BLOCK_SIZE) {
			int writeableLength = tempLength & ~NOA_FLASHCART_WRITE_BLOCK_MASK;
			DoWrite(dataPtr, writeableLength, nin);
			tempLength -= writeableLength;
			dataPtr += writeableLength;
		}
		memcpy(noabuffer + bufferFill, dataPtr, tempLength);
		bufferFill += tempLength;	
	}
}

void FlashCartFile::Close(bool nin)
{
	if(state == FILESTATE_WRITE && bufferFill > 0)
	{
		(!nin) ? DoWrite(buffer, FLASHCART_WRITE_BLOCK_SIZE, nin) : DoWrite(noabuffer, NOA_FLASHCART_WRITE_BLOCK_SIZE, nin);
	}
	state = FILESTATE_CLOSED;
}

void FlashCartFile::DoWrite(u8* source, int length, bool nin)
{
		while(filePtr + length > erasePtr)
		{
			//printf("\nfp:0x%x|l:%i|ep:0x%x\n",filePtr,length,erasePtr);
			(!nin) ? EraseNextBlock() : EraseNextBlockNintendo();
		}

		int blockCount = (!nin) ? length / FLASHCART_WRITE_BLOCK_SIZE : length / NOA_FLASHCART_WRITE_BLOCK_SIZE;

		int result = (!nin) ? WriteTurboFACart((u32)source,(u32)filePtr,blockCount) : WriteNintendoFlashCart((u32)source,(u32)filePtr,blockCount);
		if(!result)
		{
			char e[1024];
			printf("\nFailed to write flash at 0x%x\n", (u32)filePtr);
			throw e;
		}

		if (!nin)
		{
			result = memcmp(source, filePtr, length);
			if(result != 0)
			{
				char e[1024];
				printf("\nVerify failed at 0x%x\n", (u32)filePtr);
				throw e;
			}
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
		printf("\nFailed to erase flash at 0x%x\n", (u32)erasePtr);
		throw e;
	}

	erasePtr += FLASHCART_ERASE_BLOCK_SIZE;
}

void FlashCartFile::EraseNextBlockNintendo()
{
	int result = EraseNintendoFlashBlocks(
		(u32)erasePtr,
		1);
	if(!result)
	{
		char e[1024];
		sprintf(e, "Failed to erase flash at 0x%x", (u32)erasePtr);
		printf("\nFailed to erase flash at 0x%x\n", (u32)erasePtr);
		throw e;
	}

	erasePtr += NOA_FLASHCART_ERASE_BLOCK_SIZE;
	//printf("EraseNintendo done? ep-0x%x\n", erasePtr);

}
