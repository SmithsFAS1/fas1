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
	virtual bool Write(void* source, int length, int carttype) = 0;
	virtual void Close(int carttype) = 0;
};
