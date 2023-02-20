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
#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA
{
	uint32_t dwKeys;
	VECTOR vecPos;
	float fAngle;
	uint8_t byteHealth;
	uint8_t byteArmour;
	uint8_t byteCurrentWeapon;
	bool IsCrouching;
	VECTOR vecSpeed;
	bool IsPlayerUpdateAiming;
	VECTOR vecAimDir;
	VECTOR vecAimPos;
	bool bIsReloading; //1 byte
	uint16_t wAmmo; // 2 byte
	_ONFOOT_SYNC_DATA()
	{
		dwKeys = 0; vecPos = VECTOR(0, 0, 0);
		fAngle = 0.0; byteHealth = 100;
		byteArmour = 0; byteCurrentWeapon = 0;
		IsCrouching = false;
		vecSpeed = VECTOR(0, 0, 0);
		IsPlayerUpdateAiming = false;
		vecAimDir = VECTOR(0, 0, 0);
		vecAimPos = VECTOR(0, 0, 0);
		bIsReloading = false;
		wAmmo = 0;
	}
} ONFOOT_SYNC_DATA;

#pragma pack(1)
typedef struct _INCAR_SYNC_DATA
{
	uint16_t VehicleID;
	uint32_t dwKeys;
	QUATERNION quatRotation;
	VECTOR vecPos;
	VECTOR vecMoveSpeed;
	float fCarHealth;
	uint8_t bytePlayerHealth;
	uint8_t bytePlayerArmour;
	uint8_t byteCurrentWeapon;
	uint32_t dDamage;
	float Turretx;
	float Turrety;
	uint16_t wAmmo;
} INCAR_SYNC_DATA;

#endif