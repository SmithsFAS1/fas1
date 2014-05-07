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
	carttype = 0;
	DetectFlashCart();
	int offset;
	int end;
	sscanf(filename, "%x/%n", &offset, &end);
	if(end == 0)
	{
		printf("\nUnknown offset\n");
		throw "Unknown offset";
	}
	if (carttype==1){
		if((offset & FLASHCART_ERASE_BLOCK_SIZE_MASK) != 0)
		{
			printf("\nUnsupported offset\n");
			throw "Unsupported offset";
		}
	}else if (carttype==2){
		if((offset & NOA_FLASHCART_ERASE_BLOCK_SIZE_MASK) != 0)
		{
			printf("\nUnsupported offset\n");
			throw "Unsupported offset";
		}
	}else if (carttype==3){
		if((offset & NOA256_FLASHCART_ERASE_BLOCK_SIZE_MASK) != 0)
		{
			printf("\nUnsupported offset\n");
			throw "Unsupported offset";
		}
	}
	if (carttype!=0)
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
			Close(carttype);
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
	carttype = 0;
	int type = CartTypeDetect();
	switch(type)
	{
	case 0x16 : printf ("  FA 32M\n");  break;
	case 0x17 : printf ("  FA 64M\n");  break;
	case 0x18 : printf ("  FA 128M\n"); break;
	case 0x2e : printf ("  Standard ROM\n");         break;
	case 0x96 : printf ("  Turbo FA 64M\n");  carttype=1; break;
	case 0x97 : printf ("  Turbo FA 128M\n"); carttype=1; break;
	case 0x98 : printf ("  Turbo FA 256M\n"); carttype=1; break;
	case 0xdc : printf ("  Hudson\n");               break;
	case 0xe2 : printf ("  Nintendo Flash Cart\n"); carttype=2;  break;
	case 0xb0 : printf ("  Nintendo 256M Flash Cart\n"); carttype=3;  break;
	default   : printf ("  Unknown\n");              break;
	}

	if(carttype==0)
	{
		printf("\nUnsupported flashcart (0x%x)\n", type);
	}
}

int FlashCartFile::Read(void* dest, int length)
{
	printf("\nReading from flash cart not supported.\n");
	throw "Reading from flash cart not supported.";
}

bool FlashCartFile::Write(void* source, int length, int carttype)
{
	if(state != FILESTATE_WRITE)
	{
		printf("\nIllegal state.\n");
		return 0;
	}
	u8* dataPtr = (u8*)source;
	int tempLength = length;
	if (carttype==1){
		if(bufferFill > 0 &&
			bufferFill + length > FLASHCART_WRITE_BLOCK_SIZE)
		{
			int bufferFree = FLASHCART_WRITE_BLOCK_SIZE - bufferFill;
			memcpy(buffer + bufferFill, dataPtr, bufferFree);
			tempLength -= bufferFree;
			dataPtr += bufferFree;
			DoWrite(buffer, FLASHCART_WRITE_BLOCK_SIZE, carttype);
			bufferFill = 0;
		}
		if(tempLength > FLASHCART_WRITE_BLOCK_SIZE) {
			int writeableLength = tempLength & ~FLASHCART_WRITE_BLOCK_SIZE_MASK;
			DoWrite(dataPtr, writeableLength, carttype);
			tempLength -= writeableLength;
			dataPtr += writeableLength;
		}
		memcpy(buffer + bufferFill, dataPtr, tempLength);
		bufferFill += tempLength;	
	}else if (carttype==2){
		if(bufferFill > 0 &&
			bufferFill + length > NOA_FLASHCART_WRITE_BLOCK_SIZE)
		{
			int bufferFree = NOA_FLASHCART_WRITE_BLOCK_SIZE - bufferFill;
			memcpy(noabuffer + bufferFill, dataPtr, bufferFree);
			tempLength -= bufferFree;
			dataPtr += bufferFree;
			DoWrite(noabuffer, NOA_FLASHCART_WRITE_BLOCK_SIZE, carttype);
			bufferFill = 0;
		}
		if(tempLength > NOA_FLASHCART_WRITE_BLOCK_SIZE) {
			int writeableLength = tempLength & ~NOA_FLASHCART_WRITE_BLOCK_MASK;
			DoWrite(dataPtr, writeableLength, carttype);
			tempLength -= writeableLength;
			dataPtr += writeableLength;
		}
		memcpy(noabuffer + bufferFill, dataPtr, tempLength);
		bufferFill += tempLength;	
	}else if (carttype==3){
		if(bufferFill > 0 &&
			bufferFill + length > NOA256_FLASHCART_WRITE_BLOCK_SIZE)
		{
			int bufferFree = NOA256_FLASHCART_WRITE_BLOCK_SIZE - bufferFill;
			memcpy(noa256buffer + bufferFill, dataPtr, bufferFree);
			tempLength -= bufferFree;
			dataPtr += bufferFree;
			DoWrite(noa256buffer, NOA256_FLASHCART_WRITE_BLOCK_SIZE, carttype);
			bufferFill = 0;
		}
		if(tempLength > NOA256_FLASHCART_WRITE_BLOCK_SIZE) {
			int writeableLength = tempLength & ~NOA_FLASHCART_WRITE_BLOCK_MASK;
			DoWrite(dataPtr, writeableLength, carttype);
			tempLength -= writeableLength;
			dataPtr += writeableLength;
		}
		memcpy(noa256buffer + bufferFill, dataPtr, tempLength);
		bufferFill += tempLength;	
	}
	return 1;
}

void FlashCartFile::Close(int carttype)
{
	if(state == FILESTATE_WRITE && bufferFill > 0)
	{
		if(carttype==1)
			DoWrite(buffer, FLASHCART_WRITE_BLOCK_SIZE, carttype);
		else if(carttype==2)	
			DoWrite(buffer, NOA_FLASHCART_WRITE_BLOCK_SIZE, carttype);
		else if(carttype==3)
			DoWrite(buffer, NOA256_FLASHCART_WRITE_BLOCK_SIZE, carttype);
	}
	state = FILESTATE_CLOSED;
}

void FlashCartFile::DoWrite(u8* source, int length, int carttype)
{
		while(filePtr + length > erasePtr)
		{
			if (carttype==1)
				EraseNextBlock();
			else if (carttype==2)
				EraseNextBlockNintendo();
			else if (carttype==3)
				EraseNextBlockNintendo256();
		}

		int blockCount = 0;
		if (carttype==1)
			blockCount = length / FLASHCART_WRITE_BLOCK_SIZE;
		else if (carttype==2)
			blockCount = length / NOA_FLASHCART_WRITE_BLOCK_SIZE;
		else if (carttype==3)
			blockCount = length / NOA256_FLASHCART_WRITE_BLOCK_SIZE;

		int result = 0;
		if (carttype==1){
			result = WriteTurboFACart((u32)source,(u32)filePtr,blockCount);
		}else if (carttype==2){
			result = WriteNintendoFlashCart((u32)source,(u32)filePtr,blockCount);
		}else if (carttype==3){
			result = WriteNintendoFlashCart256((u32)source,(u32)filePtr,blockCount);
		}
		
		if(!result)
		{
			printf("\nFailed to write flash at 0x%x\n", (u32)filePtr);
			state = FILESTATE_CLOSED;
		}

		if (carttype==1)
		{
			result = memcmp(source, filePtr, length);
			if(result != 0)
			{
				printf("\nVerify failed at 0x%x\n", (u32)filePtr);
				state = FILESTATE_CLOSED;
			}
		}

		filePtr += length;
}

void FlashCartFile::EraseNextBlock()
{
	int result = EraseTurboFABlocks((u32)erasePtr,1);
	if(!result)
	{
		printf("\nFailed to erase flash at 0x%x\n", (u32)erasePtr);
		state = FILESTATE_CLOSED;
	}

	erasePtr += FLASHCART_ERASE_BLOCK_SIZE;
}

void FlashCartFile::EraseNextBlockNintendo()
{
	int result = EraseNintendoFlashBlocks((u32)erasePtr,1);
	if(!result)
	{
		char e[1024];
		sprintf(e, "Failed to erase flash at 0x%x", (u32)erasePtr);
		printf("\nFailed to erase flash at 0x%x\n", (u32)erasePtr);
		throw e;
	}

	erasePtr += NOA_FLASHCART_ERASE_BLOCK_SIZE;

}

void FlashCartFile::EraseNextBlockNintendo256()
{
	int result = EraseNintendoFlashBlocks256((u32)erasePtr,1);
	if(!result)
	{
		char e[1024];
		sprintf(e, "Failed to erase flash at 0x%x", (u32)erasePtr);
		printf("\nFailed to erase flash at 0x%x\n", (u32)erasePtr);
		throw e;
	}

	erasePtr += NOA256_FLASHCART_ERASE_BLOCK_SIZE;

}
