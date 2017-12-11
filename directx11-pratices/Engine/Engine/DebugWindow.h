#ifndef _DEBUGWINDOW_H_
#define _DEBUGWINDOW_H_

#include <d3d11.h>
#include <DirectXMath.h>

#include "Utils.h"

using namespace DirectX;

// Just another name of bitmap class.
class DebugWindow
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
	};

public:
	DebugWindow();
	DebugWindow(const DebugWindow& other);
	~DebugWindow();

	bool Init(ID3D11Device* device, int screenWidth, int screenHeight, int bmpWidth, int bmpHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int posX, int posY);

	int GetIndexCount() const;

private:
	bool InitBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* context);
	bool UpdateBuffers(ID3D11DeviceContext* context, int posX, int posY);

private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	int mVertexCount, mIndexCount;
	int mScreenWidth, mScreenHeight;
	int mBitmapWidth, mBitmapHeight;
	int mPrevX, mPrevY;
};

#endif