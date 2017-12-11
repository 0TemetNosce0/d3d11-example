#include <iostream>

#include "ObjParser.h"

int main(int argc, char* argv[])
{
	ObjParser* pObjParser = new ObjParser();

	bool result = pObjParser->Parse(argv[1]);
	if (!result)
		return -1;

	return 0;
}