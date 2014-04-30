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
	virtual bool Write(void* source, int length, bool nintendo) = 0;
	virtual void Close(bool nintendo) = 0;
};
