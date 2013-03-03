// Ŭnicode please
#pragma once

#include "sora/arch.h"
#include <limits>
#include <cstdlib>
#include <cmath>

namespace sora {;
const float kPi = static_cast<float>(M_PI);
const float kPiOver2 = static_cast<float>(M_PI_2);
const float kPiOver4 = static_cast<float>(M_PI_4);
const float kTwoPi = static_cast<float>(M_PI * 2);

template<typename T>
bool IsNaN(T value) 
{
	if (value != value) {
		return true;
	} else {
		return false;
	}
}

template<typename T>
bool IsInf(T num) 
{
	if(std::numeric_limits<T>::has_infinity == false)	{
		return false;
	}
	if(num == std::numeric_limits<T>::infinity())	{
		return true;
	}
	return false;
}

template<typename T>
bool IsValudNum(T num) 
{
	if(IsNaN(num) == false) {
		return false;
	}
	if(IsInf(num) == false) {
		return false;
	}
	return true;
}

template<typename T>
bool IsPower(T base, T num) 
{
	T curr = 1;
	while(curr < num) {
		curr *= base;
		if(curr == num) {
			return true;
		}
	}
	return false;
}

template<typename T>
T CeilPower(T base, T num) 
{
	T curr = 1;
	while(curr < num) {
		curr *= base;
	}
	return curr;
}

float DegToRad(float degree);
float RadToDeg(float radian);

bool EqualAbsError(float a, float b, float maxError);	
bool EqualRelError(float a, float b, float maxError);	
bool EqualUlps(float a, float b, int maxUlps);

}	//namespace sora