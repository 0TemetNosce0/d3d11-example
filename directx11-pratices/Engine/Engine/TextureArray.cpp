#include "TextureArray.h"

TextureArray::TextureArray()
{
	mTextureArray = nullptr;
}

TextureArray::TextureArray(const TextureArray& other)
{
}

TextureArray::~TextureArray()
{
}

bool TextureArray::Init(ID3D11Device* device, WCHAR** textureNames, int texNum)
{
	mTextureArray = new ID3D11ShaderResourceView*[texNum];
	for (int i = 0; i < texNum; ++i)
		mTextureArray[i] = nullptr;

	ID3D11Resource* res = nullptr;
	for(int i=0; i<texNum; ++i)
		HR(DirectX::CreateDDSTextureFromFile(device, textureNames[i], &res, &mTextureArray[i]));

	ReleaseCOM(res);

	return true;
}

void TextureArray::Shutdown()
{
	int size = sizeof(mTextureArray) / sizeof(ID3D11ShaderResourceView*);
	for (int i = 0; i < size; ++i)
	{
		ReleaseCOM(mTextureArray[i]);
	}	
}

ID3D11ShaderResourceView** TextureArray::GetTextureArray() const
{
	return mTextureArray;
}