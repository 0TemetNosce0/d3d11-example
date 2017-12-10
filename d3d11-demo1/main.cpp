#include <Windows.h>
#include <string>
#include <D3D11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

using namespace DirectX;///

using namespace std;

//��Ҫ��ȫ�ֱ���
//�ں���ѧϰ�����У���Щȫ�ֽ���Ϊ�����ĳ�Ա��������
HINSTANCE	g_hInstance(NULL);
HWND		g_hWnd(NULL);

wstring		g_clsName(L"d3d11");
wstring		g_wndTitle(L"D3D11��ʼ��");

UINT		g_winWidth(640);
UINT		g_winHeight(480);

//D3D���ȫ�ֱ���
ID3D11Device		*g_device(NULL);
ID3D11DeviceContext	*g_deviceContext(NULL);
IDXGISwapChain		*g_swapChain(NULL);

ID3D11DepthStencilView	*g_depthStencilView(NULL);
ID3D11RenderTargetView	*g_renderTargetView(NULL);

//��ʼ��
BOOL InitWin32();
//��ʼ��D3D
BOOL InitD3D();
void Render();
//��ѭ��
int	 Run();
//������Դ
void Release();

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//�������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	g_hInstance = hInstance;

	if (!InitWin32())
		return -1;
	if (!InitD3D())
		return -1;

	return Run();
}

BOOL InitWin32()
{
	//����������
	WNDCLASS wndcls;
	wndcls.cbClsExtra = 0;
	wndcls.cbWndExtra = 0;
	wndcls.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndcls.hInstance = g_hInstance;
	wndcls.lpfnWndProc = WinProc;
	wndcls.lpszClassName = g_clsName.c_str();
	wndcls.lpszMenuName = NULL;
	wndcls.style = CS_HREDRAW | CS_VREDRAW;

	//ע�ᴰ����
	if (!RegisterClass(&wndcls))
	{
		MessageBox(NULL, L"����ע��ʧ��!", L"����", MB_OK);
		return FALSE;
	}

	//��������
	g_hWnd = CreateWindow(g_clsName.c_str(),
		g_wndTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		g_winWidth, g_winHeight,
		NULL,
		NULL,
		g_hInstance,
		NULL);
	if (!g_hWnd)
	{
		MessageBox(NULL, L"���ڴ���ʧ��!", L"����", MB_OK);
		return FALSE;
	}

	//��ʾ�����´���
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);

	//Win32��ʼ�����
	return TRUE;
}

BOOL InitD3D()
{
	//�����豸��������
	D3D_FEATURE_LEVEL features[3] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3 };
	D3D_FEATURE_LEVEL myFeatureLevel;
	HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, 0, features, 3, D3D11_SDK_VERSION, &g_device, &myFeatureLevel, &g_deviceContext);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"����d3d11�豸ʧ�ܣ�", L"����", MB_OK);
		return FALSE;
	}

	//���4x�����ȼ�
	UINT msaa4xQuality(0);
	g_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaa4xQuality);

	//����������
	//������������
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferDesc.Width = g_winWidth;					//����
	swapChainDesc.BufferDesc.Height = g_winHeight;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;			//ˢ����
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;					//�㶨��������������ָ������
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	//ͬ��
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;						//���ݸ�ʽ��һ��ΪRGBA��Ԫɫ��ʽ
	swapChainDesc.BufferCount = 1;														//�󻺳���������1���㹻
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;						//Usage���ܺ����
	swapChainDesc.Flags = 0;
	swapChainDesc.OutputWindow = g_hWnd;							//�����ھ��
	swapChainDesc.SampleDesc.Count = 4;								//���ز���
	swapChainDesc.SampleDesc.Quality = msaa4xQuality - 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			//������������������DISCARD
	swapChainDesc.Windowed = true;									//����ģʽ

																	//��ȡIDXGIFactory�Դ���������
	IDXGIDevice *dxgiDevice(NULL);
	g_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	IDXGIAdapter *dxgiAdapter(NULL);
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
	IDXGIFactory *dxgiFactory(NULL);
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));
	hr = dxgiFactory->CreateSwapChain(g_device, &swapChainDesc, &g_swapChain);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"����d3d11������ʧ�ܣ�", L"����", MB_OK);
		return FALSE;
	}
	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();

	//������ͼ
	//��Ҫ�Ȼ����Դ
	ID3D11Texture2D *backBuffer(NULL);
	g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	hr = g_device->CreateRenderTargetView(backBuffer, NULL, &g_renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"����RenderTargetViewʧ�ܣ�", L"����", MB_OK);
		return FALSE;
	}
	backBuffer->Release();

	//������Ȼ�������ͼ
	//Ҫ�ȴ�����Ӧ������
	//����������Texture2D��Ҫ�ȸ�������
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = g_winWidth;
	desc.Height = g_winHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 4;
	desc.SampleDesc.Quality = msaa4xQuality - 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	ID3D11Texture2D *depthStencilBuffer(NULL);
	hr = g_device->CreateTexture2D(&desc, NULL, &depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"����d3d11��Ȼ�����ʧ�ܣ�", L"����", MB_OK);
		return FALSE;
	}
	hr = g_device->CreateDepthStencilView(depthStencilBuffer, NULL, &g_depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"����DepthStencilViewʧ�ܣ�", L"����", MB_OK);
		return FALSE;
	}

	//������ͼҪ�󶨵�������Ӧ�׶�
	g_deviceContext->OMSetRenderTargets(1, &g_renderTargetView, g_depthStencilView);
	depthStencilBuffer->Release();

	//��������ӿ�
	D3D11_VIEWPORT viewPort = { 0 };
	viewPort.Width = static_cast<FLOAT>(g_winWidth);
	viewPort.Height = static_cast<FLOAT>(g_winHeight);
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	g_deviceContext->RSSetViewports(1, &viewPort);

	return TRUE;
}

void Render()
{
	//��Ⱦһ����ɫ�Ĵ���
	XMVECTORF32 color = { 0.f, 1.f, 0.f, 1.6f };
	//ÿ֡��ʼ��Ⱦʱ�������RenderTargetView����Ȼ�������Ȼ�����ȫ����Ⱦ������
	g_deviceContext->ClearRenderTargetView(g_renderTargetView, /*Colors::MidnightBlue*/reinterpret_cast<float*>(&color));
	g_deviceContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	//ȫ���ĳ������ƹ��������������.....

	//�����ʾ
	g_swapChain->Present(0, 0);
}

int Run()
{
	MSG msg = { 0 };
	//����Ϣѭ����Ҳ����Ϸ���е���ѭ��
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//����������������:��Ⱦ
			Render();
		}
	}

	Release();

	return msg.wParam;
}

void Release()//�ƺ�
{
	g_depthStencilView->Release();
	g_renderTargetView->Release();
	g_swapChain->Release();
	g_deviceContext->Release();
	g_device->Release();
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}