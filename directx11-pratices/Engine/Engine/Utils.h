#ifndef _UTILS_H_
#define _UTILS_H_

#define ReleaseCOM(x) { if (x) { x->Release(); x = nullptr; } }
#define SafeDelete(x) { delete x; x = nullptr; }

#if defined(DEBUG) | defined(_DEBUG)
#include "Third/dxerr.h"
#include "Third/DrimoonDebug.h"
#define HR(x) do{ HRESULT hr = (x); if(FAILED(hr)) DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true); }while(0)
#define Check(x) do{ bool result = (x); if(!result) return false; }while(0)
#else
#define HR(x) (x)
#define Check(x) (x)
#endif

#endif