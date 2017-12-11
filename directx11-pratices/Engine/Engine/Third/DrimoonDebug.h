#ifndef DRIMOON_DEBUG
#define DRIMOON_DEBUG
#include <windows.h>
#include <sstream>
#include <DirectXMath.h>

void PrintMatrix(DirectX::XMFLOAT4X4 m, WCHAR* info);
void PrintVector(DirectX::XMFLOAT4 v, WCHAR* info);
void PrintVector(DirectX::XMFLOAT3 v, WCHAR* info);
#endif

