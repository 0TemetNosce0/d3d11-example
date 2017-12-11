#ifndef _TEXTUREMGR_H_
#define _TEXTUREMGR_H_

#include <d3d11.h>
#include <map>

#include "Utils.h"
#include "Third/DDSTextureLoader.h"  // light-weight dds loader

class TextureMgr
{
public:
	TextureMgr();
	TextureMgr(const TextureMgr& other);
	~TextureMgr();

	bool Init(ID3D11Device* device, WCHAR** textureNames, int texNum);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture(WCHAR* textureName);

private:
	std::map<WCHAR*, ID3D11ShaderResourceView*> mTextureMap;
};

#endif