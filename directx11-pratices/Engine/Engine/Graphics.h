#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <windows.h>
#include "D3D.h"
#include "Text.h"
#include "Model.h"
#include "ModelList.h"
#include "Timer.h"
#include "Light.h"
#include "Camera.h"
#include "Frustum.h"
#include "FogShader.h"
#include "LightShader.h"
#include "TextureShader.h"
#include "ClipPlaneShader.h"
#include "RenderTexture.h"
#include "DebugWindow.h"

const int GRAPHICS_SCREEN_WIDTH = 1024;
const int GRAPHICS_SCREEN_HEIGHT = 768;
const bool GRAPHICS_FULL_SCREEN = false;
const bool GRAPHICS_VSYNC_ENABLE = true;
const float GRAPHICS_SCREEN_DEPTH = 1000.0f;
const float GRAPHICS_SCREEN_NEAR = 1.0f;

class Graphics
{
public:
	Graphics();
	Graphics(const Graphics& other);
	~Graphics();

	bool Init(HWND hwnd, int width, int height);
	bool Frame(float dt, int fps, int cpuRate, float rotY, float posZ);
	bool Render();
	void Shutdown();

private:
	bool RenderScene();
	bool RenderToTexture();

private:
	D3D*    mD3D;
	Timer*  mTimer;
	Light*  mLight;
	Camera* mCamera;
	Frustum* mFrustum;
	FogShader* mFogShader;
	LightShader* mLightShader;
	ClipPlaneShader* mClipPlaneShader;
	TextureShader* mTextureShader;
	RenderTexture* mRenderTexture;
	DebugWindow* mDebugWindow;

	Text*   mText;
	Model*  mModel;
	ModelList* mModelList;
};
#endif
