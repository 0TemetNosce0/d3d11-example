#include "Frustum.h"

Frustum::Frustum()
{
}

Frustum::Frustum(const Frustum& other)
{
}

Frustum::~Frustum()
{
}

void Frustum::ConstructFrustum(float screenDepth, XMFLOAT4X4 proj, XMFLOAT4X4 view)
{
	// Update proj matrix's far plane = screenDepth
	// nz means near plane's depth of frustum
	float nz = -(proj._43 / proj._33);
	float r = screenDepth / (screenDepth - nz);
	proj._33 = r;
	proj._43 = -r*nz;

	// frustum matrix
	XMMATRIX f = XMLoadFloat4x4(&view) * XMLoadFloat4x4(&proj);
	XMFLOAT4X4 M;
	XMStoreFloat4x4(&M, f);

	// near plane
	mPlanes[0].x = M._14 + M._13;
	mPlanes[0].y = M._24 + M._23;
	mPlanes[0].z = M._34 + M._33;
	mPlanes[0].w = M._44 + M._43;

	// far plane
	mPlanes[1].x = M._14 - M._13;
	mPlanes[1].y = M._24 - M._23;
	mPlanes[1].z = M._34 - M._33;
	mPlanes[1].w = M._44 - M._43;

	// left plane
	mPlanes[2].x = M._14 + M._11;
	mPlanes[2].y = M._24 + M._21;
	mPlanes[2].z = M._34 + M._31;
	mPlanes[2].w = M._44 + M._41;

	// right plane
	mPlanes[3].x = M._14 - M._11;
	mPlanes[3].y = M._24 - M._21;
	mPlanes[3].z = M._34 - M._31;
	mPlanes[3].w = M._44 - M._41;

	// top plane
	mPlanes[4].x = M._14 - M._12;
	mPlanes[4].y = M._24 - M._22;
	mPlanes[4].z = M._34 - M._32;
	mPlanes[4].w = M._44 - M._42;

	// bottom plane
	mPlanes[5].x = M._14 + M._12;
	mPlanes[5].y = M._24 + M._22;
	mPlanes[5].z = M._34 + M._32;
	mPlanes[5].w = M._44 + M._42;

	// Normalize the plane equations.
	for (int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&mPlanes[i]));
		XMStoreFloat4(&mPlanes[i], v);
	}
}

bool Frustum::CheckPoint(XMFLOAT3 point)
{
	XMVECTOR v = XMLoadFloat3(&point);
	XMFLOAT3 tmp;
	float dot;
	for (int i = 0; i < 6; ++i)
	{
		XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v);
		XMStoreFloat3(&tmp, v);
		dot = tmp.x;
		if (dot < 0.0f)
			return false;
	}
	return true;
}

bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	XMFLOAT3 tmp;
	float dot;
	for (int i = 0; i < 6; ++i)
	{
		tmp = XMFLOAT3(xCenter - radius, yCenter - radius, zCenter - radius);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + radius, yCenter - radius, zCenter - radius);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - radius, yCenter + radius, zCenter - radius);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + radius, yCenter + radius, zCenter - radius);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - radius, yCenter - radius, zCenter + radius);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + radius, yCenter - radius, zCenter + radius);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - radius, yCenter + radius, zCenter + radius);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + radius, yCenter + radius, zCenter + radius);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		return false;
	}
	return true;
}

bool Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	XMFLOAT4 tmp;
	tmp.x = xCenter;
	tmp.y = yCenter;
	tmp.z = zCenter;
	tmp.w = 1.0f;
	for (int i = 0; i < 6; ++i)
	{
		float dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w + radius;
		if (dot < 0.0f)
			return false;
	}
	return true;
}

bool Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	XMFLOAT3 tmp;
	float dot;
	for (int i = 0; i < 6; ++i)
	{
		tmp = XMFLOAT3(xCenter - xSize, yCenter - ySize, zCenter - zSize);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + xSize, yCenter - ySize, zCenter - zSize);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - xSize, yCenter + ySize, zCenter - zSize);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + xSize, yCenter + ySize, zCenter - zSize);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - xSize, yCenter - ySize, zCenter + zSize);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + xSize, yCenter - ySize, zCenter + zSize);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - xSize, yCenter + ySize, zCenter + zSize);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + xSize, yCenter + ySize, zCenter + zSize);
		dot = mPlanes[i].x*tmp.x + mPlanes[i].y*tmp.y + mPlanes[i].z*tmp.z + mPlanes[i].w;
		if (dot >= 0.0f)
			continue;

		return false;
	}
	return true;
}
