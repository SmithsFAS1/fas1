#pragma once

#include "file.h"

#define FLASHCART_DEFAULT_OFFSET 0x400000
#define FLASHCART_ERASE_BLOCK_SIZE 0x40000
#define FLASHCART_ERASE_BLOCK_SIZE_MASK 0x3FFFF
#define FLASHCART_WRITE_BLOCK_SIZE 0x40
#define FLASHCART_WRITE_BLOCK_SIZE_MASK 0x3F

class FlashCartFile : public File
{
public:
	FlashCartFile(const char* filename, bool write);
	virtual ~FlashCartFile();

	void RomFlash(const char* filename, bool write);

	virtual int Read(void* dest, int length);
	virtual void Write(void* source, int length);
	virtual void Close();
	u8* filePtr;

private:
	void DetectFlashCart();
	void DoWrite(u8* source, int length);
	void EraseNextBlock();

	u8 buffer[FLASHCART_WRITE_BLOCK_SIZE];
	int bufferFill;
	u8* erasePtr;
	FileState state;
};
