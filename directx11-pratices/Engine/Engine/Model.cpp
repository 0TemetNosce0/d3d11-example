#include "Model.h"

Model::Model()
{
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;

	mTextureArray = nullptr;
	mModelData = nullptr;
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

bool Model::Init(ID3D11Device* device, char* modelName, WCHAR** textureNames, int texNum)
{
	Check(LoadModel(modelName));

	// Calculate the normal, tangent, and binormal vectors for the model.
	CalculateModelVectors();

	Check(InitBuffers(device));

	mTextureArray = new TextureArray;
	Check(mTextureArray->Init(device, textureNames, texNum));

	return true;
}

void Model::Render(ID3D11DeviceContext* context)
{
	RenderBuffers(context);
}

void Model::Shutdown()
{
	if (mTextureArray)
	{
		mTextureArray->Shutdown();
		SafeDelete(mTextureArray);
	}

	if (mModelData)
	{
		delete[] mModelData;
		mModelData = nullptr;
	}

	ShutdownBuffers();
}

bool Model::InitBuffers(ID3D11Device* device)
{
	Vertex* vertices = new Vertex[mVertexCount];
	unsigned long* indices = new unsigned long[mIndexCount];

	for (int i = 0; i < mVertexCount; ++i)
	{
		vertices[i].pos = XMFLOAT3(mModelData[i].x, mModelData[i].y, mModelData[i].z);
		vertices[i].tex = XMFLOAT2(mModelData[i].u, mModelData[i].v);
		vertices[i].normal = XMFLOAT3(mModelData[i].nx, mModelData[i].ny, mModelData[i].nz);
		vertices[i].tangent = XMFLOAT3(mModelData[i].tx, mModelData[i].ty, mModelData[i].tz);
		vertices[i].binormal = XMFLOAT3(mModelData[i].bx, mModelData[i].by, mModelData[i].bz);
		indices[i] = i;
	}

	D3D11_BUFFER_DESC vbDesc;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = vertices;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	HR(device->CreateBuffer(&vbDesc, &vbData, &mVertexBuffer));

	D3D11_BUFFER_DESC ibDesc;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = indices;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	HR(device->CreateBuffer(&ibDesc, &ibData, &mIndexBuffer));

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void Model::RenderBuffers(ID3D11DeviceContext* context)
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::ShutdownBuffers()
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

ID3D11ShaderResourceView** Model::GetTextureArray() const
{
	return mTextureArray->GetTextureArray();
}

bool Model::LoadModel(char* modelName)
{
	std::ifstream fin(modelName, std::ios::in);
	if (fin.fail())
		return false;

	char igonre;
	fin.get(igonre);
	while (igonre != ':')
		fin.get(igonre);

	fin >> mVertexCount;
	mIndexCount = mVertexCount;

	mModelData = new ModelType[mVertexCount];
	fin.get(igonre);
	while (igonre != ':')
		fin.get(igonre);

	for (int i = 0; i < mVertexCount; ++i)
	{
		fin >> mModelData[i].x >> mModelData[i].y >> mModelData[i].z;
		fin >> mModelData[i].u >> mModelData[i].v;
		fin >> mModelData[i].nx >> mModelData[i].ny >> mModelData[i].nz;
	}

	fin.close();

	return true;
}

void Model::CalculateModelVectors()
{
	int faceCount = mVertexCount / 3;
	int index = 0;

	TempVertex v1, v2, v3;
	Vector tangent, binormal, normal;
	// Go through all the faces and calculate the tangent, binormal, and normal vectors.
	for (int i = 0; i < faceCount; ++i)
	{
		// Get the three vertices for this face form the model.
		v1.x = mModelData[index].x;
		v1.y = mModelData[index].y;
		v1.z = mModelData[index].z;
		v1.u = mModelData[index].u;
		v1.v = mModelData[index].v;
		v1.nx = mModelData[index].nx;
		v1.ny = mModelData[index].ny;
		v1.nz = mModelData[index].nz;
		++index;

		v2.x = mModelData[index].x;
		v2.y = mModelData[index].y;
		v2.z = mModelData[index].z;
		v2.u = mModelData[index].u;
		v2.v = mModelData[index].v;
		v2.nx = mModelData[index].nx;
		v2.ny = mModelData[index].ny;
		v2.nz = mModelData[index].nz;
		++index;

		v3.x = mModelData[index].x;
		v3.y = mModelData[index].y;
		v3.z = mModelData[index].z;
		v3.u = mModelData[index].u;
		v3.v = mModelData[index].v;
		v3.nx = mModelData[index].nx;
		v3.ny = mModelData[index].ny;
		v3.nz = mModelData[index].nz;
		++index;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(v1, v2, v3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		mModelData[index - 1].nx = normal.x;
		mModelData[index - 1].ny = normal.y;
		mModelData[index - 1].nz = normal.z;
		mModelData[index - 1].tx = tangent.x;
		mModelData[index - 1].ty = tangent.y;
		mModelData[index - 1].tz = tangent.z;
		mModelData[index - 1].bx = binormal.x;
		mModelData[index - 1].by = binormal.y;
		mModelData[index - 1].bz = binormal.z;

		mModelData[index - 2].nx = normal.x;
		mModelData[index - 2].ny = normal.y;
		mModelData[index - 2].nz = normal.z;
		mModelData[index - 2].tx = tangent.x;
		mModelData[index - 2].ty = tangent.y;
		mModelData[index - 2].tz = tangent.z;
		mModelData[index - 2].bx = binormal.x;
		mModelData[index - 2].by = binormal.y;
		mModelData[index - 2].bz = binormal.z;

		mModelData[index - 3].nx = normal.x;
		mModelData[index - 3].ny = normal.y;
		mModelData[index - 3].nz = normal.z;
		mModelData[index - 3].tx = tangent.x;
		mModelData[index - 3].ty = tangent.y;
		mModelData[index - 3].tz = tangent.z;
		mModelData[index - 3].bx = binormal.x;
		mModelData[index - 3].by = binormal.y;
		mModelData[index - 3].bz = binormal.z;
	}
}

void Model::CalculateTangentBinormal(TempVertex v1, TempVertex v2, TempVertex v3, Vector& tangent, Vector& binormal)
{
	// Calculate the two vectors for this face.
	float vector1[3], vector2[3];
	vector1[0] = v2.x - v1.x;
	vector1[1] = v2.y - v1.y;
	vector1[2] = v2.z - v1.z;

	vector2[0] = v3.x - v1.x;
	vector2[1] = v3.y - v1.y;
	vector2[2] = v3.z - v1.z;

	// Calculate the u, v texture space vectors.
	float uVector[2], vVector[2];
	uVector[0] = v2.u - v1.u;
	vVector[0] = v2.v - v1.v;

	uVector[1] = v3.u - v1.u;
	vVector[1] = v3.v - v1.v;

	// Calculate the denominator of the tangent/binormal equation.
	float den = 1.0f / (uVector[0] * vVector[1] - uVector[1] * vVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (vVector[1] * vector1[0] - vVector[0] * vector2[0]) * den;
	tangent.y = (vVector[1] * vector1[1] - vVector[0] * vector2[1]) * den;
	tangent.z = (vVector[1] * vector1[2] - vVector[0] * vector2[2]) * den;

	binormal.x = (uVector[0] * vector2[0] - uVector[1] * vector1[0]) * den;
	binormal.y = (uVector[0] * vector2[1] - uVector[1] * vector1[1]) * den;
	binormal.z = (uVector[0] * vector2[2] - uVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	float length = sqrt((binormal.x * binormal.x) + (binormal.y*binormal.y) + (binormal.z*binormal.z));

	// Normalize the normal and then store it.
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}

void Model::CalculateNormal(Vector tangent, Vector binormal, Vector& normal)
{
	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = tangent.y*binormal.z - tangent.z*binormal.y;
	normal.y = tangent.z*binormal.x - tangent.x*binormal.z;
	normal.z = tangent.x*binormal.y - tangent.y*binormal.x;

	// Calculate the length of the normal.
	float length = sqrt((normal.x * normal.x) + (normal.y*normal.y) + (normal.z*normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.y = normal.y / length;
}