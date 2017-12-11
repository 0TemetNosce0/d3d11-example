#include "DrimoonDebug.h"

void PrintMatrix(DirectX::XMFLOAT4X4 m, WCHAR* info)
{
	std::wstringstream ss;
	ss << info << std::endl;
	ss << m._11 << " " << m._12 << " " << m._13 << " " << m._14 << std::endl;
	ss << m._21 << " " << m._22 << " " << m._23 << " " << m._24 << std::endl;
	ss << m._31 << " " << m._32 << " " << m._33 << " " << m._34 << std::endl;
	ss << m._41 << " " << m._42 << " " << m._43 << " " << m._44 << std::endl;
	std::wstring tmp;
	tmp = ss.str();

	OutputDebugStringW(tmp.c_str());
}

void PrintVector(DirectX::XMFLOAT4 v, WCHAR* info)
{
	std::wstringstream ss;
	ss << info << std::endl;
	ss << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;
	std::wstring tmp = ss.str();

	OutputDebugStringW(tmp.c_str());
}

void PrintVector(DirectX::XMFLOAT3 v, WCHAR* info)
{
	std::wstringstream ss;
	ss << info << std::endl;
	ss << v.x << " " << v.y << " " << v.z << std::endl;
	std::wstring tmp = ss.str();

	OutputDebugStringW(tmp.c_str());
}