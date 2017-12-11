#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include <d3d11.h>
#include "Utils.h"

class RenderTexture
{
public:
	RenderTexture();
	RenderTexture(const RenderTexture& other);
	~RenderTexture();

	void Init(ID3D11Device* device, int texWidth, int texHeight);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsView);
	void ClearRenderTarget(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsView, float r, float g, float b, float a);
	ID3D11ShaderResourceView* GetShaderResourceView() const;

private:
	ID3D11Texture2D* mRenderTargetTexture;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11ShaderResourceView* mShaderResourceView;
};

#endif