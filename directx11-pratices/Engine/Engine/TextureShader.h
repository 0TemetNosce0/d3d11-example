#ifndef _TEXTURESHADER_H_
#define _TEXTURESHADER_H_

#include <d3d11.h>
#include <fstream>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "Utils.h"

#pragma comment(lib, "D3Dcompiler.lib")

class TextureShader
{
private:
	struct MatrixBuffer
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projMatrix;
	};

public:
	TextureShader();
	TextureShader(const TextureShader& other);
	~TextureShader();

	bool Init(HWND hwnd, ID3D11Device* device);
	bool Render(ID3D11DeviceContext* context, int indexCount, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj, ID3D11ShaderResourceView* texture);
	void Shutdown();

private:
	bool InitShader(HWND hwnd, ID3D11Device* device, WCHAR* vsPath, WCHAR* psPath);
	void RenderShader(ID3D11DeviceContext* context, int indexCount);
	void ShutdownShader();

	void OutputShaderErrorMessage(HWND hwnd, ID3D10Blob* blob, WCHAR* path);

	bool SetShaderParameters(ID3D11DeviceContext* context, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj, ID3D11ShaderResourceView* texture);

private:
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11SamplerState* mSampleState;
};

#endif
