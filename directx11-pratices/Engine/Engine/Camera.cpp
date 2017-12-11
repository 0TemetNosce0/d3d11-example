#include "Camera.h"

Camera::Camera() : mPos(0.0f, 0.0f, 0.0f), mRot(0.0f, 0.0f, 0.0f)
{
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	mPos.x = x;
	mPos.y = y;
	mPos.z = z;
}

void Camera::SetRotation(float x, float y, float z)
{
	mRot.x = x;
	mRot.y = y;
	mRot.z = z;
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&mPos);
}

XMVECTOR Camera::GetRotationXM() const
{
	return XMLoadFloat3(&mRot);
}

XMMATRIX Camera::GetViewMatrixXM() const
{
	return XMLoadFloat4x4(&mView);
}

XMFLOAT3 Camera::GetPosition() const
{
	return mPos;
}

XMFLOAT3 Camera::GetRotation() const
{
	return mRot;
}

XMFLOAT4X4 Camera::GetViewMatrix() const
{
	return mView;
}


void Camera::Render()
{
	XMFLOAT4 pos(mPos.x, mPos.y, mPos.z, 0.0f);
	XMFLOAT4 up(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR vPos = XMLoadFloat4(&pos);
	XMVECTOR vUp = XMLoadFloat4(&up);

	XMFLOAT4 lookAt;
	// Calculate the rotation in radians.
	float radians = mRot.y * (XM_PI / 180);
	// Setup where the camera is looking.
	lookAt.x = sinf(radians) + mPos.x;
	lookAt.y = mPos.y;
	lookAt.z = cosf(radians) + mPos.z;
	XMVECTOR vLookAt = XMLoadFloat4(&lookAt);

	XMMATRIX viewMatrix = XMMatrixLookAtLH(vPos, vLookAt, vUp);
	XMStoreFloat4x4(&mView, viewMatrix);
}