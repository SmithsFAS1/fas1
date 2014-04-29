#pragma once

#include "file.h"

#define FLASHCART_DEFAULT_OFFSET 0x400000
#define FLASHCART_ERASE_BLOCK_SIZE 0x40000 //256KB erasing chunks - Visoly
#define FLASHCART_ERASE_BLOCK_SIZE_MASK 0x3FFFF
#define FLASHCART_WRITE_BLOCK_SIZE 0x40 //64x blocks - visoly
#define FLASHCART_WRITE_BLOCK_SIZE_MASK 0x3F

#define NOA_FLASHCART_WRITE_BLOCK_SIZE 0x2 //2x blocks - NOA
#define NOA_FLASHCART_WRITE_BLOCK_MASK 0x1
#define NOA_FLASHCART_ERASE_BLOCK_SIZE 0x10000 //64KB erasing chunks - NOA
#define NOA_FLASHCART_ERASE_BLOCK_SIZE_MASK 0xffff

class FlashCartFile : public File
{
public:
	FlashCartFile(const char* filename, bool write);
	virtual ~FlashCartFile();

	void RomFlash(const char* filename, bool write);

	virtual int Read(void* dest, int length);
	virtual void Write(void* source, int length, bool nin);
	virtual void Close(bool nin);
	u8* filePtr;

private:
	void DetectFlashCart();
	void DoWrite(u8* source, int length, bool nin);
	void EraseNextBlock();
	void EraseNextBlockNintendo();

	u8 noabuffer[NOA_FLASHCART_WRITE_BLOCK_SIZE];

	u8 buffer[FLASHCART_WRITE_BLOCK_SIZE];
	int bufferFill;
	u8* erasePtr;
	FileState state;
};

extern bool nintendo;
extern bool visoly;

