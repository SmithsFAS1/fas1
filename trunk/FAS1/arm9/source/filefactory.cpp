#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filefactory.h"
#include "flashcartfile.h"

File* FileFactory::OpenFile(const char* filename, bool write)
{
	char dir[11];
	int offset = 0;
	printf("path: %s\n", filename);
	if(filename[0] == '/')
	{
		filename++;
	}
	sscanf(filename, "%10[^/]/%n", dir, &offset);
	if(offset == 0)
	{
		throw "Illegal path";
	}

	if(strcmp(dir, "rom") == 0)
	{
		return new FlashCartFile(filename + offset, write);
	}
	else
	{
		throw "Unknown path";
	}
}
