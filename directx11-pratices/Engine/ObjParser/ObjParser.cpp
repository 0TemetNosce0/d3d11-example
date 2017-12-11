#include "ObjParser.h"

ObjParser::ObjParser() : 
	mInFile(""), mOutFile(""),
	mVertexCount(0), mTexcoordCount(0), mNormalCount(0), mFaceCount(0)
{
	mErr.open("parser-error-log.txt");
}

ObjParser::ObjParser(const ObjParser& other)
{
}

ObjParser::~ObjParser()
{
	mErr.close();
}

bool ObjParser::Parse(char* fileName)
{
	SetFileName(fileName);
	if (!ReadFileCounts())
		return false;
	if (!LoadDataStructures())
		return false;

	return true;
}

void ObjParser::SetFileName(char* fileName)
{
	std::stringstream ss;
	ss << fileName;
	ss >> mInFile;
	
	mOutFile = mInFile;
	int size = mOutFile.size();
	mOutFile[size - 3] = 't';
	mOutFile[size - 2] = 'x';
	mOutFile[size - 1] = 't';
}

bool ObjParser::ReadFileCounts()
{
	char input;
	std::ifstream fin;
	fin.open(mInFile);
	if (fin.fail())
	{
		mErr << "Open input file : Failed." << std::endl;
		return false;
	}

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			switch (input)
			{
				case ' ':
				{
					mVertexCount++;
					break;
				}
				case 't':
				{
					mTexcoordCount++;
					break;
				}
				case 'n':
				{
					mNormalCount++;
					break;
				}
			}
		}
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
				mFaceCount++;
		}

		// otherwise read in the remainder of the line.
		while (input != '\n')
			fin.get(input);

		// start reading the beginning of the next line.
		fin.get(input);
	}
	fin.close();

	return true;
}

bool ObjParser::LoadDataStructures()
{
	VertexType* vertices = new VertexType[mVertexCount];
	VertexType* texcoords = new VertexType[mTexcoordCount];
	VertexType* normals = new VertexType[mNormalCount];
	FaceType* faces = new FaceType[mFaceCount];

	// After get enough memory, clear these counts to 0, use it as index
	mVertexCount = 0;
	mTexcoordCount = 0;
	mNormalCount = 0;
	mFaceCount = 0;

	std::ifstream fin;
	fin.open(mInFile);
	if (fin.fail())
	{
		mErr << "Open input file : Failed." << std::endl;
		return false;
	}
	char input;
	char ignore;
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			switch (input)
			{
				case ' ':
				{
					fin >> vertices[mVertexCount].x >> vertices[mVertexCount].y >> vertices[mVertexCount].z;
					vertices[mVertexCount].z *= -1.0f;  // RH->LH
					mVertexCount++;
					break;
				}
				case 't':
				{
					fin >> texcoords[mTexcoordCount].x >> texcoords[mTexcoordCount].y;
					texcoords[mTexcoordCount].y = 1.0f - texcoords[mTexcoordCount].y;  // RH->LH
					mTexcoordCount++;
					break;
				}
				case 'n':
				{
					fin >> normals[mNormalCount].x >> normals[mNormalCount].y >> normals[mNormalCount].z;
					normals[mNormalCount].z *= -1.0f; // RH->LH
					mNormalCount++;
					break;
				}
			}
		}
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> faces[mFaceCount].vIndex3 >> ignore >> faces[mFaceCount].tIndex3 >> ignore >> faces[mFaceCount].nIndex3;
				fin >> faces[mFaceCount].vIndex2 >> ignore >> faces[mFaceCount].tIndex2 >> ignore >> faces[mFaceCount].nIndex2;
				fin >> faces[mFaceCount].vIndex1 >> ignore >> faces[mFaceCount].tIndex1 >> ignore >> faces[mFaceCount].nIndex1;
				mFaceCount++;
			}
		}

		while (input != '\n')
			fin.get(input);

		fin.get(input);
	}
	fin.close();
	std::ofstream fout(mOutFile, std::ios::out);
	if (!fout)
	{
		mErr << "Find output file : Failed." << std::endl;
		return false;
	}

	// Write out the file header that our model format uses.
	fout << "Vertex Count: " << (mFaceCount * 3) << std::endl;
	fout << std::endl;
	fout << "Data:" << std::endl;
	fout << std::endl;

	int vIndex, tIndex, nIndex;
	for (int i = 0; i<mFaceCount; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << std::endl;
	}

	fout.close();
	mErr << "Successed in parsing " << mInFile << std::endl;

	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}
	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}
	if (normals)
	{
		delete[] normals;
		normals = 0;
	}
	if (faces)
	{
		delete[] faces;
		faces = 0;
	}

	return true;
}

