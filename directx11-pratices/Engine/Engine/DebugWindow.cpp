#include "DebugWindow.h"

DebugWindow::DebugWindow()
{
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;
}

DebugWindow::DebugWindow(const DebugWindow& other)
{
}

DebugWindow::~DebugWindow()
{
}

int DebugWindow::GetIndexCount() const
{
	return mIndexCount;
}

bool DebugWindow::Init(ID3D11Device* device, int screenWidth, int screenHeight, int bmpWidth, int bmpHeight)
{
	// Init data.
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
	mBitmapWidth = bmpWidth;
	mBitmapHeight = bmpHeight;
	mPrevX = -1;
	mPrevY = -1;
	mVertexCount = 6;
	mIndexCount = 6;

	Check(InitBuffers(device));
	return true;
}

void DebugWindow::Shutdown()
{
	ShutdownBuffers();
}

bool DebugWindow::Render(ID3D11DeviceContext* context, int posX, int posY)
{
	// Re-build the dynamic vertex buffer for rendering to different location on the screen.
	Check(UpdateBuffers(context, posX, posY));

	RenderBuffers(context);
	return true;
}

bool DebugWindow::InitBuffers(ID3D11Device* device)
{
	Vertex* vertices = new Vertex[mVertexCount];
	memset(vertices, 0, sizeof(Vertex)*mVertexCount);

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = vertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(vbDesc));
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(device->CreateBuffer(&vbDesc, &vData, &mVertexBuffer));

	UINT* indices = new UINT[mIndexCount];
	for (int i = 0; i < mIndexCount; ++i)
		indices[i] = i;

	D3D11_SUBRESOURCE_DATA iData;
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC ibDesc;
	ZeroMemory(&ibDesc, sizeof(ibDesc));
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(UINT) * mIndexCount;
	HR(device->CreateBuffer(&ibDesc, &iData, &mIndexBuffer));

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void DebugWindow::ShutdownBuffers()
{
	ReleaseCOM(mIndexBuffer);
	ReleaseCOM(mVertexBuffer);
}

bool DebugWindow::UpdateBuffers(ID3D11DeviceContext* context, int posX, int posY)
{
	if (posX == mPrevX && posY == mPrevY)
		return true;

	mPrevX = posX;
	mPrevY = posY;

	float left = static_cast<float>(posX) - static_cast<float>(mScreenWidth / 2);
	float right = left + static_cast<float>(mBitmapWidth);
	float top = static_cast<float>(mScreenHeight / 2) - static_cast<float>(posY);
	float bottom = top - static_cast<float>(mBitmapHeight);

	Vertex* vertices = new Vertex[mVertexCount];
	vertices[0].pos = XMFLOAT3(left, top, 0.0f);
	vertices[0].tex = XMFLOAT2(0.0f, 0.0f);
	vertices[1].pos = XMFLOAT3(right, bottom, 0.0f);
	vertices[1].tex = XMFLOAT2(1.0f, 1.0f);
	vertices[2].pos = XMFLOAT3(left, bottom, 0.0f);
	vertices[2].tex = XMFLOAT2(0.0f, 1.0f);
	vertices[3].pos = XMFLOAT3(left, top, 0.0f);
	vertices[3].tex = XMFLOAT2(0.0f, 0.0f);
	vertices[4].pos = XMFLOAT3(right, top, 0.0f);
	vertices[4].tex = XMFLOAT2(1.0f, 0.0f);
	vertices[5].pos = XMFLOAT3(right, bottom, 0.0f);
	vertices[5].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HR(context->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	Vertex* verticesPtr = (Vertex*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, sizeof(Vertex)*mVertexCount);
	context->Unmap(mVertexBuffer, 0);

	delete[] vertices;
	vertices = nullptr;
	return true;
}

void DebugWindow::RenderBuffers(ID3D11DeviceContext* context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}