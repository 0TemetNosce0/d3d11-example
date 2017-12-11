#include "RenderTexture.h"

RenderTexture::RenderTexture()
{
	mRenderTargetTexture = nullptr;
	mRenderTargetView = nullptr;
	mShaderResourceView = nullptr;
}

RenderTexture::RenderTexture(const RenderTexture& other)
{
}

RenderTexture::~RenderTexture()
{
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView() const
{
	return mShaderResourceView;
}

// Note that if you are rendering your screen, you should keep the aspect ratio of this render to texture the same as the aspect ratio of the screen or there will be some distortion.
void RenderTexture::Init(ID3D11Device* device, int texWidth, int texHeight)
{
	// Init a null texture, the size is satisfied with texWidth, texHeight.
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.Width = texWidth;
	texDesc.Height = texHeight;
	texDesc.CPUAccessFlags = 0;
	texDesc.ArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	HR(device->CreateTexture2D(&texDesc, NULL, &mRenderTargetTexture));
	
	// Create a render target view corresponding to the previous texture.
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	HR(device->CreateRenderTargetView(mRenderTargetTexture, &rtvDesc, &mRenderTargetView));

	// Create a shader resource view corresponding to the previous texture.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(device->CreateShaderResourceView(mRenderTargetTexture, &srvDesc, &mShaderResourceView));
}

void RenderTexture::Shutdown()
{
	ReleaseCOM(mShaderResourceView);
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mRenderTargetTexture);
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsView)
{
	// Bind the render target view and depth stencil view to the output render pipeline.
	context->OMSetRenderTargets(1, &mRenderTargetView, dsView);
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsView, float r, float g, float b, float a)
{
	float color[4] = { r, g, b, a };
	// Clear the back buffer.
	context->ClearRenderTargetView(mRenderTargetView, color);
	// Clear the depth stencil buffer.
	context->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}