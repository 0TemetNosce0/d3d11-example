#ifndef _TEXTUREARRAY_H_
#define _TEXTUREARRAY_H_

#include <d3d11.h>

#include "Utils.h"
#include "Third/DDSTextureLoader.h"  // light-weight dds loader

class TextureArray
{
public:
	TextureArray();
	TextureArray(const TextureArray& other);
	~TextureArray();

	bool Init(ID3D11Device* device, WCHAR** textureNames, int texNum);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray() const;

private:
	ID3D11ShaderResourceView** mTextureArray;
};

#endif
