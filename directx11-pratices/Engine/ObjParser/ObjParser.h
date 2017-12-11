#ifndef _OBJPARSER_H_
#define _OBJPARSER_H_

#include <string>
#include <fstream>
#include <sstream>

class ObjParser
{
private:
	struct VertexType
	{
		float x, y, z;
	};

	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

public:
	ObjParser();
	ObjParser(const ObjParser& other);
	~ObjParser();

	bool Parse(char* fileName);

private:
	void SetFileName(char* fileName);
	bool ReadFileCounts();
	bool LoadDataStructures();

	std::ofstream mErr;
	std::string mInFile;
	std::string mOutFile;
	int mVertexCount;
	int mTexcoordCount;
	int mNormalCount;
	int mFaceCount;
};
#endif