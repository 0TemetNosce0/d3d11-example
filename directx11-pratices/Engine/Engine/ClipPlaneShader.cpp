#include "ClipPlaneShader.h"

ClipPlaneShader::ClipPlaneShader()
{
	mVertexShader = nullptr;
	mPixelShader = nullptr;
	mLayout = nullptr;

	mClipBuffer = nullptr;
	mMatrixBuffer = nullptr;
	mSampleState = nullptr;
}

ClipPlaneShader::ClipPlaneShader(const ClipPlaneShader& other)
{
}

ClipPlaneShader::~ClipPlaneShader()
{
}

bool ClipPlaneShader::Init(HWND hwnd, ID3D11Device* device)
{
	Check(InitShader(hwnd, device, L"Shaders/clip.vs", L"Shaders/clip.ps"));

	return true;
}

bool ClipPlaneShader::Render(ID3D11DeviceContext* context, int indexCount,
	XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj, XMFLOAT4 clipPlane, ID3D11ShaderResourceView* texture)
{
	Check(SetShaderParameters(context, world, view, proj, clipPlane, texture));

	RenderShader(context, indexCount);

	return true;
}

void ClipPlaneShader::Shutdown()
{
	ShutdownShader();
}

bool ClipPlaneShader::InitShader(HWND hwnd, ID3D11Device* device, WCHAR* vsPath, WCHAR* psPath)
{
	HRESULT result;

	ID3D10Blob* errorMsg = nullptr;
	ID3D10Blob* vsBuffer = nullptr;

	result = D3DCompileFromFile(vsPath, NULL, NULL, "VS", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vsBuffer, &errorMsg);

	if (FAILED(result))
	{
		if (errorMsg)
			OutputShaderErrorMessage(hwnd, errorMsg, vsPath);
		else
			MessageBox(hwnd, vsPath, L"Missing Shader File.", MB_OK);

		return false;
	}

	ID3D10Blob* psBuffer = nullptr;
	result = D3DCompileFromFile(psPath, NULL, NULL, "PS", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &psBuffer, &errorMsg);
	if (FAILED(result))
	{
		if (errorMsg)
			OutputShaderErrorMessage(hwnd, errorMsg, psPath);
		else
			MessageBox(hwnd, psPath, L"Missing Shader File.", MB_OK);

		return false;
	}

	HR(device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &mVertexShader));
	HR(device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &mPixelShader));

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	HR(device->CreateInputLayout(polygonLayout, numElements, vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(), &mLayout));

	ReleaseCOM(vsBuffer);
	ReleaseCOM(psBuffer);

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer));

	D3D11_BUFFER_DESC clipBufferDesc;
	clipBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	clipBufferDesc.ByteWidth = sizeof(ClipPlaneBuffer);
	clipBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	clipBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	clipBufferDesc.MiscFlags = 0;
	clipBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(&clipBufferDesc, NULL, &mClipBuffer));

	D3D11_SAMPLER_DESC samplerDesc;
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(device->CreateSamplerState(&samplerDesc, &mSampleState));

	return true;
}

bool ClipPlaneShader::SetShaderParameters(ID3D11DeviceContext* context, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj, XMFLOAT4 clipPlane, ID3D11ShaderResourceView* texture)
{
	XMMATRIX w = XMMatrixTranspose(XMLoadFloat4x4(&world));
	XMMATRIX v = XMMatrixTranspose(XMLoadFloat4x4(&view));
	XMMATRIX p = XMMatrixTranspose(XMLoadFloat4x4(&proj));

	D3D11_MAPPED_SUBRESOURCE mappedRes;
	HR(context->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
	MatrixBuffer* mdataPtr = (MatrixBuffer*)mappedRes.pData;
	XMStoreFloat4x4(&(mdataPtr->worldMatrix), w);
	XMStoreFloat4x4(&(mdataPtr->viewMatrix), v);
	XMStoreFloat4x4(&(mdataPtr->projMatrix), p);
	context->Unmap(mMatrixBuffer, 0);
	context->VSSetConstantBuffers(0, 1, &mMatrixBuffer);

	HR(context->Map(mClipBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
	ClipPlaneBuffer* cdataPtr = (ClipPlaneBuffer*)mappedRes.pData;
	XMStoreFloat4(&(cdataPtr->clipPlane), XMLoadFloat4(&clipPlane));
	context->Unmap(mClipBuffer, 0);
	context->VSSetConstantBuffers(1, 1, &mClipBuffer);

	context->PSSetShaderResources(0, 1, &texture);

	return true;
}

void ClipPlaneShader::RenderShader(ID3D11DeviceContext* context, int indexCount)
{
	context->IASetInputLayout(mLayout);

	context->VSSetShader(mVertexShader, NULL, 0);
	context->PSSetShader(mPixelShader, NULL, 0);
	context->PSSetSamplers(0, 1, &mSampleState);

	context->DrawIndexed(indexCount, 0, 0);
}

void ClipPlaneShader::ShutdownShader()
{
	ReleaseCOM(mClipBuffer);
	ReleaseCOM(mMatrixBuffer);
	ReleaseCOM(mLayout);
	ReleaseCOM(mPixelShader);
	ReleaseCOM(mVertexShader);
}

void ClipPlaneShader::OutputShaderErrorMessage(HWND hwnd, ID3D10Blob* error, WCHAR* path)
{
	char* errorInfo = static_cast<char*>(error->GetBufferPointer());
	SIZE_T size = error->GetBufferSize();

	std::ofstream fout("texture_shader_compile_log.txt", std::ios::out);
	for (unsigned long i = 0; i < size; ++i)
		fout << errorInfo[i];
	fout.close();

	ReleaseCOM(error);
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", path, MB_OK);
}