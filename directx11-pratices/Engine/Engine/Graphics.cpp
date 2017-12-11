#include "Graphics.h"

Graphics::Graphics()
{
	mD3D = nullptr;
	mText = nullptr;
	mModel = nullptr;
	mModelList = nullptr;
	mLight = nullptr;
	mCamera = nullptr;
	mFrustum = nullptr;
	mFogShader = nullptr;
	mLightShader = nullptr;
	mTextureShader = nullptr;
	mClipPlaneShader = nullptr;
	mRenderTexture = nullptr;
	mDebugWindow = nullptr;
}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Init(HWND hwnd, int width, int height)
{
	mD3D = new D3D;
	Check(mD3D->Init(hwnd, width, height, GRAPHICS_VSYNC_ENABLE, GRAPHICS_FULL_SCREEN, GRAPHICS_SCREEN_DEPTH, GRAPHICS_SCREEN_NEAR));

	mCamera = new Camera;
	mCamera->SetPosition(0.0f, 0.0f, -1.0f);
	mCamera->Render();

	mFrustum = new Frustum;

	mLight = new Light;
	mLight->SetAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
	mLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	mLight->SetDiffuseDir(0.0f, 0.0f, 1.0f);
	mLight->SetSpecularPower(32.0f);
	mLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);

	mModel = new Model;
	WCHAR* textureNames[3] = { L"Textures/stone01.dds" , L"Textures/specularMap.dds", L"Textures/bumpMap02.dds" };
	Check(mModel->Init(mD3D->GetDevice(), "Models/cube.txt", textureNames, 3));

	mModelList = new ModelList;
	Check(mModelList->Init(1));

	mText = new Text;
	mText->Init(mD3D->GetDevice(), mD3D->GetDeviceContext(), hwnd, width, height, "Fonts/fontdata.txt", L"Fonts/font.dds", 32, mCamera->GetViewMatrix());

	mFogShader = new FogShader;
	Check(mFogShader->Init(hwnd, mD3D->GetDevice()));

	mClipPlaneShader = new ClipPlaneShader;
	Check(mClipPlaneShader->Init(hwnd, mD3D->GetDevice()));

	mLightShader = new LightShader;
	Check(mLightShader->Init(hwnd, mD3D->GetDevice()));

	mTextureShader = new TextureShader;
	Check(mTextureShader->Init(hwnd, mD3D->GetDevice()));

	mRenderTexture = new RenderTexture;
	mRenderTexture->Init(mD3D->GetDevice(), width, height);

	mDebugWindow = new DebugWindow;
	Check(mDebugWindow->Init(mD3D->GetDevice(), width, height, 300, 225));

	return true;
}

void Graphics::Shutdown()
{
	if (mDebugWindow)
	{
		mDebugWindow->Shutdown();
		SafeDelete(mDebugWindow);
	}

	if (mRenderTexture)
	{
		mRenderTexture->Shutdown();
		SafeDelete(mRenderTexture);
	}

	if (mFogShader)
	{
		mFogShader->Shutdown();
		SafeDelete(mFogShader);
	}

	if (mClipPlaneShader)
	{
		mClipPlaneShader->Shutdown();
		SafeDelete(mClipPlaneShader);
	}

	if (mLightShader)
	{
		mLightShader->Shutdown();
		SafeDelete(mLightShader);
	}

	if (mTextureShader)
	{
		mTextureShader->Shutdown();
		SafeDelete(mTextureShader);
	}

	if (mModelList)
	{
		mModelList->Shutdown();
		SafeDelete(mModelList);
	}

	if (mModel)
	{
		mModel->Shutdown();
		SafeDelete(mModel);
	}

	if (mText)
	{
		mText->Shutdown();
		SafeDelete(mText);
	}

	SafeDelete(mLight);
	SafeDelete(mFrustum);
	SafeDelete(mCamera);

	if (mD3D)
	{
		mD3D->Shutdown();
		SafeDelete(mD3D);
	}
}

bool Graphics::Frame(float dt, int fps, int cpuRate, float rotY, float posZ)
{
	mText->SetFps(fps);
	mText->SetCpuRate(cpuRate);

	mCamera->SetPosition(0.0f, 0.0f, -10.0f + posZ);
	mCamera->SetRotation(0.0f, rotY, 0.0f);

	// After call Frame func, re-calculate the new view matrix.
	mCamera->Render();

	return true;
}

bool Graphics::Render()
{
	// Base 3D matrix
	XMFLOAT4X4 world = mD3D->GetWorldMatrix();
	XMFLOAT4X4 view = mCamera->GetViewMatrix();
	XMFLOAT4X4 proj = mD3D->GetProjMatrix();
	XMFLOAT4X4 ortho = mD3D->GetOrthoMatrix();

	Check(RenderToTexture());
	// Construct frustum ready to clip outside object.
	// mFrustum->ConstructFrustum(GRAPHICS_SCREEN_DEPTH, proj, view);

	////////////////////////////////////////////////////////////////////////
	// 3D Model Rendering
	////////////////////////////////////////////////////////////////////////
//	float fogColor = 0.7f;
//	mD3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);
	mD3D->BeginScene(0.8f, 0.8f, 1.0f, 1.0f);
	Check(RenderScene());
	mDebugWindow->Render(mD3D->GetDeviceContext(), 724, 0);
	mTextureShader->Render(mD3D->GetDeviceContext(), mDebugWindow->GetIndexCount(), world, mText->GetBaseViewMatrix(), ortho, mRenderTexture->GetShaderResourceView());
	////////////////////////////////////////////////////////////////////////
	// 2D Font Rendering
	////////////////////////////////////////////////////////////////////////
	// Before rendering 2D objects, disable depth test and turn on blending.
	mD3D->TurnDepthOff();
	mD3D->TurnBlendOn();

	// Render fps and using rate of cpu.
	mText->ShowPerformance(mD3D->GetDeviceContext(), world, ortho);

	// Render primary card name.
	char cardName[128];
	int cardMemory = 0;
	mD3D->GetVideoCardInfo(cardName, cardMemory);
	Check(mText->Render(mD3D->GetDeviceContext(), cardName, 20, 60, XMFLOAT3(1.0f, 0.0f, 0.0f), world, ortho));
	
	// Show the counts of rendered spheres.
	//char renderCountsStr[32] = "Render Counts:";
	//char tmp[8];
	//_itoa_s(renderCount, tmp, 10);
	//strcat_s(renderCountsStr, tmp);
	//mText->Render(mD3D->GetDeviceContext(), renderCountsStr, 20, 80, XMFLOAT3(1.0f, 0.0f, 0.0f), world, ortho);

	// After rendering 2D objects, restore render states.
	mD3D->TurnBlendOff();
	mD3D->TurnDepthOn();

	// Swap chain present.
	mD3D->EndScene();

	return true;
}

bool Graphics::RenderScene()
{
	static float angle = 0.0f;
	angle += XM_PI * 0.0025f;
	if (angle >= 6.28f)
		angle = 0.0f;

	XMMATRIX rotate = XMMatrixRotationX(angle) * XMMatrixRotationY(angle) * XMMatrixRotationZ(angle);

	// Base 3D matrix
	XMFLOAT4X4 world = mD3D->GetWorldMatrix();
	XMFLOAT4X4 view = mCamera->GetViewMatrix();
	XMFLOAT4X4 proj = mD3D->GetProjMatrix();

	XMMATRIX scale = XMMatrixScaling(5.0f, 5.0f, 5.0f);

	if(GetAsyncKeyState('1') & 0x8000)
		XMStoreFloat4x4(&world, XMLoadFloat4x4(&world) * scale * rotate);
	else
		XMStoreFloat4x4(&world, XMLoadFloat4x4(&world) * scale);



	ID3D11DeviceContext* context = mD3D->GetDeviceContext();
	// IA Set VB, IB, Primitive's Type
	mModel->Render(context);
	ID3D11ShaderResourceView** texArray = mModel->GetTextureArray();
	//Check(mFogShader->Render(context, mModel->GetIndexCount(), world, view, proj, texArray[0], 0.0f, 30.0f));
//	mLightShader->Render(context, mModel->GetIndexCount(), world, view, proj, texArray, mLight->GetAmbientColor(), mLight->GetDiffuseColor(), mLight->GetDiffuseDir(), mLight->GetSpecularPower(), mLight->GetSpecularColor(), mCamera->GetPosition());
	mClipPlaneShader->Render(mD3D->GetDeviceContext(), mDebugWindow->GetIndexCount(), world, view, proj, XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f), texArray[0]);

	
	return true;
}

bool Graphics::RenderToTexture()
{
	mRenderTexture->SetRenderTarget(mD3D->GetDeviceContext(), mD3D->GetDepthStencilView());
	mRenderTexture->ClearRenderTarget(mD3D->GetDeviceContext(), mD3D->GetDepthStencilView(), 0.87f, 0.95f, 0.91f, 1.0f);
	Check(RenderScene());
	mD3D->SetBackBufferRenderTarget();
	return true;
}