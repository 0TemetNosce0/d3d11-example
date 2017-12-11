#include "Position.h"

Position::Position()
{
	mFrameTime = 0.0f;
	mRotY = 0.0f;
	mLeftTurnSpeed = 0.0f;
	mRightTurnSpeed = 0.0f;
}

Position::Position(const Position& other)
{
}

Position::~Position()
{
}

void Position::SetFrameTime(float frameTime)
{
	mFrameTime = frameTime;
}

float Position::GetRotation() const
{
	return mRotY;
}

float Position::GetPosition() const
{
	return mPosZ;
}

void Position::GoAhead(bool keydown)
{
	if (keydown)
	{
		mGoAheadSpeed += mFrameTime*0.01f;
		float max = mFrameTime * POSITION_SPEED_Ratio;
		if (mGoAheadSpeed > max)
			mGoAheadSpeed = max;
	}
	else
	{
		mGoAheadSpeed -= mFrameTime*0.005f;
		if (mGoAheadSpeed < 0.0f)
			mGoAheadSpeed = 0.0f;
	}

	mPosZ += mGoAheadSpeed;
	if (mPosZ > 5.0f)
		mPosZ = 5.0f;
}

void Position::GoBack(bool keydown)
{
	if (keydown)
	{
		mGoBackSpeed += mFrameTime*0.01f;
		float max = mFrameTime * POSITION_SPEED_Ratio;
		if (mGoBackSpeed > max)
			mGoBackSpeed = max;
	}
	else
	{
		mGoBackSpeed -= mFrameTime*0.005f;
		if (mGoBackSpeed < 0.0f)
			mGoBackSpeed = 0.0f;
	}

	mPosZ -= mGoBackSpeed;
	if (mPosZ < -50.0f)
		mPosZ = -50.0f;
}

void Position::TurnLeft(bool keydown)
{
	if (keydown)
	{
		mLeftTurnSpeed += mFrameTime * 0.01f;
		float max = mFrameTime * POSITION_SPEED_Ratio;
		if (mLeftTurnSpeed > max)
			mLeftTurnSpeed = max;
	}
	else
	{
		mLeftTurnSpeed -= mFrameTime * 0.005f;
		if (mLeftTurnSpeed < 0.0f)
			mLeftTurnSpeed = 0.0f;
	}

	mRotY += mLeftTurnSpeed;
	if (mRotY > 360.0f)
		mRotY -= 360.0f;
}

void Position::TurnRight(bool keydown)
{
	if (keydown)
	{
		mRightTurnSpeed += mFrameTime * 0.01f;
		float max = mFrameTime * POSITION_SPEED_Ratio;
		if (mRightTurnSpeed > max)
			mRightTurnSpeed = max;
	}
	else
	{
		mRightTurnSpeed -= mFrameTime * 0.005f;
		if (mRightTurnSpeed < 0.0f)
			mRightTurnSpeed = 0.0f;
	}

	mRotY -= mRightTurnSpeed;
	if (mRotY < 0.0f)
		mRotY += 360.0f;
}