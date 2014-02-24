#include "stdafx.h"

class Robot
{
	public:
		Robot();
		int getLeftSpeed();
		int getRightSpeed();
		void setLeftSpeed(int lSpeed);
		void setRightSpeed(int rSpeed);
		void setSpeed(int l, int r);

	private:
		int leftSpeed;
		int rightSpeed;
};