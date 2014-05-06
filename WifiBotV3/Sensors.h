#ifndef DEF_SENSORS
#define DEF_SENSORS
#include "stdafx.h"

ref class Sensors
{
	public:
		Sensors(){
			IR = gcnew array<int>(4);
		}

	public:
		int leftSpeed;
		int rightSpeed;
		int batterie;
		array<int>^ IR;
		int oct;
};

#endif