//***************************************************************************************
// RenderStates.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "RenderStates.h"

ID3D11RasterizerState* RenderStates::WireframeRS = nullptr;
ID3D11RasterizerState* RenderStates::NoCullRS    = nullptr;
	
ID3D11BlendState*      RenderStates::NoRenderTargetWritesBS = nullptr;
ID3D11BlendState*      RenderStates::AlphaToCoverageBS = nullptr;
ID3D11BlendState*      RenderStates::TransparentBS     = nullptr;

ID3D11DepthStencilState* RenderStates::MarkDepthDSS = nullptr;
ID3D11DepthStencilState* RenderStates::DrawDepthDSS = nullptr;

void RenderStates::InitAll(ID3D11Device* device)
{
	//
	// WireframeRS
	//
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&wireframeDesc, &WireframeRS));

	//
	// NoCullRS
	//
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&noCullDesc, &NoCullRS));

	//
	// NoRenderTargetWritesBS
	//
	D3D11_BLEND_DESC noRenderTargetWritesDesc = { 0 };
	noRenderTargetWritesDesc.AlphaToCoverageEnable = false;
	noRenderTargetWritesDesc.IndependentBlendEnable = false;
	noRenderTargetWritesDesc.RenderTarget[0].BlendEnable = false;
	noRenderTargetWritesDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	noRenderTargetWritesDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	noRenderTargetWritesDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	noRenderTargetWritesDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	noRenderTargetWritesDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	noRenderTargetWritesDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	noRenderTargetWritesDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	noRenderTargetWritesDesc.RenderTarget[0].RenderTargetWriteMask = 0; // no write color info to render target

	HR(device->CreateBlendState(&noRenderTargetWritesDesc, &NoRenderTargetWritesBS));

	//
	// AlphaToCoverageBS
	//

	D3D11_BLEND_DESC alphaToCoverageDesc = { 0 };
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS));

	//
	// TransparentBS
	//

	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&transparentDesc, &TransparentBS));

	//
	// MarkDepthDSS
	//
	D3D11_DEPTH_STENCIL_DESC markDepthDesc = { 0 };
	markDepthDesc.DepthEnable = true;
	markDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	markDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	markDepthDesc.StencilEnable = true;
	markDepthDesc.StencilReadMask = 0xff;
	markDepthDesc.StencilWriteMask = 0xff;
	// FrontFace
	markDepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	markDepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	markDepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	markDepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	// BackFace
	markDepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	markDepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	markDepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	markDepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;

	HR(device->CreateDepthStencilState(&markDepthDesc, &MarkDepthDSS));

	//
	// DrawDepthDSS
	//
	D3D11_DEPTH_STENCIL_DESC drawDepthDesc = { 0 };
	drawDepthDesc.DepthEnable = true;
	drawDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	drawDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	drawDepthDesc.StencilEnable = true;
	drawDepthDesc.StencilReadMask = 0xff;
	drawDepthDesc.StencilWriteMask = 0xff;
	// FrontFace
	drawDepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	drawDepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawDepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawDepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	// BackFace
	drawDepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	drawDepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawDepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawDepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&drawDepthDesc, &MarkDepthDSS));

}

void RenderStates::DestroyAll()
{
	ReleaseCOM(WireframeRS);
	ReleaseCOM(NoCullRS);
	ReleaseCOM(NoRenderTargetWritesBS);
	ReleaseCOM(AlphaToCoverageBS);
	ReleaseCOM(TransparentBS);
	ReleaseCOM(MarkDepthDSS);
}