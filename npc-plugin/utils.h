#ifndef UTILS_H
#define UTILS_H
#include "math.h"
#pragma pack(1)
typedef struct _VECTOR
{
	float X;
	float Y;
	float Z;
	_VECTOR()
	{
		this->X = 0.0, this->Y = 0.0, this->Z = 0.0;
	}
	_VECTOR(float x, float y, float z)
	{
		this->X = x;
		this->Y = y;
		this->Z = z;
	}
	_VECTOR operator+(const _VECTOR& vector)
	{
		_VECTOR result;
		result.X = this->X + vector.X;
		result.Y = this->Y + vector.Y;
		result.Z = this->Z + vector.Z;
		return result;
	}
	_VECTOR operator+=(const _VECTOR& vector)
	{
		this->X += vector.X;
		this->Y += vector.Y;
		this->Z += vector.Z;
		return *this;
	}
	_VECTOR operator-(const _VECTOR& vector)
	{
		_VECTOR result;
		result.X = this->X - vector.X;
		result.Y = this->Y - vector.Y;
		result.Z = this->Z - vector.Z;
		return result;
	}
	_VECTOR operator-=(const _VECTOR& vector)
	{
		this->X -= vector.X;
		this->Y -= vector.Y;
		this->Z -= vector.Z;
		return *this;
	}

	float GetMagnitude()
	{
		return (float)sqrt(
			pow(this->X, 2) + pow(this->Y, 2) + pow(this->Z, 2)
		);
	}
}VECTOR;

#pragma pack(1)
typedef struct _QUATERNION
{
	float X;
	float Y;
	float Z;
	float W;
	_QUATERNION()
	{
		this->X = 0;
		this->Y = 0;
		this->Z = 0;
		this->W = 1;
	}
	_QUATERNION(float x, float y, float z, float w)
	{
		this->X = x;
		this->Y = y;
		this->Z = z;
		this->W = w;
	}
}QUATERNION;

#endif