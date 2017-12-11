//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cube.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a spinning cube in wireframe mode.  Demonstrates vertex and 
//       index buffers, world and view transformations, render states and
//       drawing commands.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

IDirect3DVertexBuffer9* VB = 0;
IDirect3DIndexBuffer9*  IB = 0;

//
// Classes and Structures
//

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z)
	{
		_x = x;  _y = y;  _z = z;
	}
	float _x, _y, _z;
	static const DWORD FVF;
	Vertex operator *(int factor){
		_x *= factor;
		_y *= factor;
		_z *= factor;
		return Vertex(_x,_y,_z);
	}
};
const DWORD Vertex::FVF = D3DFVF_XYZ;

//
// Framework Functions
//
bool Setup()
{
	//
	// Create vertex and index buffers.
	//

	Device->CreateVertexBuffer(
		5 * sizeof(Vertex), 
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0);

	Device->CreateIndexBuffer(
		18 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&IB,
		0);

	//
	// Fill the buffers with the cube data.
	//

	// define unique vertices:
	Vertex* vertices;
	VB->Lock(0, 0, (void**)&vertices, 0);

	// vertices of pyramid
	vertices[0] = Vertex( 0.0f,  1.0f,  0.0f)*10;
	vertices[1] = Vertex( 1.0f, -1.0f, -1.0f)*10;
	vertices[2] = Vertex( 1.0f, -1.0f,  1.0f)*10;
	vertices[3] = Vertex(-1.0f, -1.0f,  1.0f)*10;
	vertices[4] = Vertex(-1.0f, -1.0f, -1.0f)*10;

	VB->Unlock();

	// define the triangles of the pyramid:
	WORD* indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);

	indices[0]  = 0; indices[1]  = 1; indices[2]  = 4;

	indices[3]  = 0; indices[4]  = 3; indices[5]  = 2;

	indices[6]  = 0; indices[7]  = 4; indices[8]  = 3;

	indices[9]  = 0; indices[10] = 2; indices[11] = 1;

	indices[12] = 3; indices[13] = 4; indices[14] = 1;

	indices[15] = 3; indices[16] = 1; indices[17] = 2;


	IB->Unlock();

	//
	// Position and aim the camera.
	//

	D3DXVECTOR3 position(0.0f, 5.0f, -30.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);

    Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set the projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	//
	// Switch to wireframe mode.
	//

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// NO cull
	Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE );
	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
}

bool Display(float timeDelta)
{

	if( Device )
	{
		//
		// spin the cube:
		//
		D3DXMATRIX Rx, Ry;

		// rotate 45 degrees on x-axis
		D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);

		// incremement y-rotation angle each frame
		static float y = 0.0f;
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;

		// reset angle to zero when angle reaches 2*PI
		if( y >= 6.28f )
			y = 0.0f;

		// combine x- and y-axis rotation transformations.
		D3DXMATRIX p = Rx * Ry;

		Device->SetTransform(D3DTS_WORLD, &p);

		//
		// draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		Device->SetIndices(IB);
		Device->SetFVF(Vertex::FVF);

		// Draw cube.
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 5, 0, 6);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}


//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}