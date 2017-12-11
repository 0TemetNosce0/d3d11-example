#ifndef _MODEL_H_
#define _MODEL_H_

#include <d3d11.h>
#include <cmath>
#include <fstream>
#include <DirectXMath.h>
using namespace DirectX;

#include "Utils.h"
#include "TextureArray.h"

class Model
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	// our own 3D Model format file( described in Model/Cube.txt )
	struct ModelType
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	// The following two structures will be used for calculating the tangent and binormal.
	struct TempVertex
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	};

	struct Vector
	{
		float x, y, z;
	};


public:
	Model();
	Model(const Model& other);
	~Model();

	bool Init(ID3D11Device* device, char* modelName, WCHAR** textureNames, int texNum);
	void Render(ID3D11DeviceContext* context);
	void Shutdown();

	int GetVertexCount() const { return mVertexCount; }
	int GetIndexCount() const { return mIndexCount; }
	ID3D11ShaderResourceView** GetTextureArray() const;

private:
	bool InitBuffers(ID3D11Device* device);
	void RenderBuffers(ID3D11DeviceContext* context);
	void ShutdownBuffers();

	bool LoadModel(char*);

	// calculate the tangent and binormal vectors for the model.
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertex v1, TempVertex v2, TempVertex v3, Vector& tangent, Vector& binormal);
	void CalculateNormal(Vector tangent, Vector binormal, Vector& normal);

private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	int mVertexCount;
	int mIndexCount;

	TextureArray* mTextureArray;
	ModelType* mModelData;
};

#endif
