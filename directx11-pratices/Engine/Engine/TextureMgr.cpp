#include "TextureMgr.h"

TextureMgr::TextureMgr()
{
}

TextureMgr::TextureMgr(const TextureMgr& other)
{
}

TextureMgr::~TextureMgr()
{
}

bool TextureMgr::Init(ID3D11Device* device, WCHAR** textureNames, int texNum)
{
	ID3D11Resource* texRes = nullptr;
	ID3D11ShaderResourceView* tmp = nullptr;
	for (int i = 0; i < texNum; ++i)
	{
		HR(DirectX::CreateDDSTextureFromFile(device, textureNames[i], &texRes, &tmp));
		mTextureMap.insert(std::make_pair(textureNames[i], tmp));

		delete texRes;
		texRes = nullptr;
	}
	
	delete tmp;
	tmp = nullptr;

	return true;
}

void TextureMgr::Shutdown()
{
	for (auto it = mTextureMap.begin(); it != mTextureMap.end(); ++it)
		ReleaseCOM(it->second);

	mTextureMap.clear();
}

ID3D11ShaderResourceView* TextureMgr::GetTexture(WCHAR* textureName)
{
	auto it = mTextureMap.find(textureName);
	return it->second;
}