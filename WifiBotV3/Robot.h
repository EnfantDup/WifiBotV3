#ifndef DEF_ROBOT
#define DEF_ROBOT
#include "stdafx.h"

ref class Robot
{
	public:
		Robot(System::Threading::Mutex^ mutex);
		int getLeftSpeed();
		int getRightSpeed();
		bool getSimulateur();
		System::String^ getIp();
		void setLeftSpeed(int lSpeed);
		void setRightSpeed(int rSpeed);
		void setSpeed(int s);
		void proceedSpeed();
		void setKeyUp(bool up);
		void setKeyDown(bool down);
		void setKeyLeft(bool left);
		void setKeyRight(bool right);
		void setSimulateur(bool b);
		void setIp(System::String^ ip);

	private:
		System::Threading::Mutex^ mutex;
		bool keyUp;
		bool keyDown;
		bool keyLeft;
		bool keyRight;
		bool simulateur;
		int speed;
		int leftSpeed;
		int rightSpeed;
		System::String^ ip;
};

#endif