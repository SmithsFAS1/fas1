#pragma once

#include "file.h"

class FileFactory
{
public:
	static File* OpenFile(const char* filename, bool write);
};
extern int carttype;
