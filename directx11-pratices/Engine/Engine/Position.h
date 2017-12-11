#ifndef _POSITION_H_
#define _POSITION_H_

#define POSITION_SPEED_Ratio 0.03f

#include <cmath>

class Position
{
public:
	Position();
	Position(const Position& other);
	~Position();

	void SetFrameTime(float frameTime);
	float GetRotation() const;
	float GetPosition() const;

	void GoAhead(bool keydown);
	void GoBack(bool keydown);
	void TurnLeft(bool keydown);
	void TurnRight(bool keydown);

private:
	float mFrameTime;
	float mRotY;
	float mPosZ;
	float mLeftTurnSpeed, mRightTurnSpeed;
	float mGoAheadSpeed, mGoBackSpeed;
};

#endif