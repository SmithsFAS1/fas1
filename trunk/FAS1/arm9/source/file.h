#pragma once

enum FileState
{
	FILESTATE_READ,
	FILESTATE_WRITE,
	FILESTATE_CLOSED
};

class File
{
public:
	virtual ~File() {};

	virtual int Read(void* dest, int length) = 0;
	virtual void Write(void* source, int length) = 0;
	virtual void Close() = 0;
};
