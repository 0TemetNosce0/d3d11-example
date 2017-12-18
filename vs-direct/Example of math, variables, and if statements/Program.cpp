//#include <windows.h>    
//#include <d2d1.h>    
//#include <d2d1helper.h>    
//#include <dwrite.h>    
//#pragma comment(lib, "dwrite.lib")    
//#pragma comment(lib, "d2d1.lib")    
// //D2D demo
//static HINSTANCE g_hinst;
//static HWND g_hwnd;
//static bool g_flag_app_exit;
//
//static ID2D1Factory * g_factory;
//static ID2D1HwndRenderTarget * g_render_target;
//
//void OnSize(LPARAM lparam)
//{
//	if (g_render_target)
//		g_render_target->Resize(D2D1::SizeU(LOWORD(lparam), HIWORD(lparam)));
//}
//
//bool AppInit()
//{
//	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_factory);
//
//	RECT rc;
//	GetClientRect(g_hwnd, &rc);
//
//	g_factory->CreateHwndRenderTarget(
//		D2D1::RenderTargetProperties(),
//		D2D1::HwndRenderTargetProperties(g_hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
//		&g_render_target);
//
//	return true;
//}
//
//void OnPaint()
//{
//	if (!g_render_target)
//		return;
//
//	g_render_target->BeginDraw();
//	g_render_target->Clear(D2D1::ColorF(0.63, 0.84, 0.00));
//	g_render_target->EndDraw();
//}
//
//void AppExit()
//{
//	g_render_target->Release();
//	g_factory->Release();
//}
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//	switch (msg)
//	{
//	case WM_PAINT:
//		OnPaint();
//		break;
//
//	case WM_SIZE:
//		OnSize(lparam);
//		break;
//
//	case WM_DESTROY:
//		g_flag_app_exit = true;
//		break;
//
//	default:
//		return DefWindowProc(hwnd, msg, wparam, lparam);
//	}
//	return 0;
//}
//
//int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//	WNDCLASSEX wc;
//	MSG msg;
//
//	memset(&wc, 0, sizeof(wc));
//	wc.cbSize = sizeof(WNDCLASSEX);
//	wc.lpfnWndProc = WndProc;
//	wc.hInstance = hinst;
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//	wc.lpszClassName = L"WindowClass";
//	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
//
//	if (!RegisterClassEx(&wc))
//	{
//		MessageBox(NULL, L"123", L"Error!", MB_ICONEXCLAMATION | MB_OK);
//		return 0;
//	}
//
//	g_hinst = hinst;
//
//	g_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"WindowClass", L"Direct2D Demo", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
//		CW_USEDEFAULT,
//		CW_USEDEFAULT,
//		640,
//		480,
//		NULL, NULL, hinst, NULL);
//
//	if (g_hwnd == NULL)
//	{
//		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
//		return 0;
//	}
//
//	if (!AppInit())
//	{
//		MessageBox(NULL, L"Application Initialisation Failed !", L"Error", MB_ICONEXCLAMATION | MB_OK);
//		return 0;
//	}
//
//	while (!g_flag_app_exit)
//	{
//		if (PeekMessage(&msg, g_hwnd, 0, 0, PM_REMOVE))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//	AppExit();
//
//	return msg.wParam;
//}

//D3D demo
#include <d3d9.h>  
/*#include <d3dx9.h> */ 
#include <stdio.h>  
#pragma comment(lib,"d3d9.lib")  
//#pragma comment(lib,"d3dx9.lib")  
#pragma comment(lib,"winmm.lib")  

#define KeyDown(vk_code)    ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)  
#define Msg(info)           MessageBox(NULL,info,"message",MB_OK)  
#define ErrorMsg(info)      MessageBox(NULL,info,"error",MB_ICONSTOP)  
#define DXRELOBJ(pObject)   if(pObject){pObject->Release();}  

#define WND_CLASS L"wndclass"  
#define WND_TITLE L"D3DApp Demo"  
#define WND_WIDTH    400  
#define WND_HEIGHT   300  

HWND g_hwnd;
HINSTANCE g_hinst;

LPDIRECT3D9 g_d3d;
LPDIRECT3DDEVICE9 g_device;
LPDIRECT3DVERTEXBUFFER9 g_vb;

#define D3DFVF_CUSTOM (D3DFVF_XYZ|D3DFVF_DIFFUSE)  
struct CustomVertex
{
	float x, y, z;
	long color;
};

bool InitGeometry()
{
	CustomVertex triangle[] =
	{
		-1, -1, 0, 0xffff00,
		1, -1, 0, 0xff0000,
		0, 1, 0, 0x0000ff
	};

	if (FAILED(g_device->CreateVertexBuffer(sizeof(triangle), 0, D3DFVF_CUSTOM, D3DPOOL_DEFAULT, &g_vb, NULL)))
		return false;

	void * paddr;

	if (FAILED(g_vb->Lock(0, 0, (void**)&paddr, 0)))
		return false;

	memcpy(paddr, triangle, sizeof(triangle));

	g_vb->Unlock();

	return true;
}

bool InitD3D()
{
	if (NULL == (g_d3d = Direct3DCreate9(D3D_SDK_VERSION)))
		return false;

	D3DDISPLAYMODE d3dpm;
	if (FAILED(g_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dpm)))
		return false;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = g_hwnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = d3dpm.Format;

	if (FAILED(g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_device)))
		return false;

	g_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return true;
}

bool InitApp()
{
	if (!InitD3D() | !InitGeometry())
	{
		//ErrorMsg("Unable to Initialize D3D !");
		return false;
	}

	return true;
}

void Render()
{
	g_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_device->BeginScene();


	g_device->SetStreamSource(0, g_vb, 0, sizeof(CustomVertex));
	g_device->SetFVF(D3DFVF_CUSTOM);
	g_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 1);

	g_device->EndScene();
	g_device->Present(NULL, NULL, NULL, NULL);
}

void ShutdownApp()
{
	DXRELOBJ(g_vb);
	DXRELOBJ(g_device);
	DXRELOBJ(g_d3d);
}

LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprevinst, LPSTR cmdlint, INT show)
{
	g_hinst = hinst;

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_CLASSDC;
	wcex.lpfnWndProc = MsgProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(NULL, IDC_APPSTARTING);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = WND_CLASS;
	wcex.lpszMenuName = NULL;
	wcex.hIconSm = LoadIcon(NULL, IDC_APPSTARTING);

	if (!RegisterClassEx(&wcex))
	{
		//ErrorMsg(L"RegisterClassEx failed!");
		return 0;
	}

	g_hwnd = CreateWindow(WND_CLASS, WND_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT,
		NULL, NULL, hinst, NULL);

	if (!g_hwnd)
		return 0;

	ShowWindow(g_hwnd, SW_SHOWNORMAL);
	UpdateWindow(g_hwnd);

	MSG msg = { 0 };

	if (!InitApp())
		msg.message = WM_QUIT;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Render();
	}

	ShutdownApp();
	UnregisterClass(WND_CLASS, wcex.hInstance);

	return 0;
}