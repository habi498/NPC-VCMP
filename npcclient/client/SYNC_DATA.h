/*
	Copyright (C) 2022  habi

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
/*
Note: The npcclient program and npc04relxx also uses this file. So any editing
in one of the files must be made on the other too
*/
#include <stdint.h>
#ifdef _WIN32
	#include <windows.h>
#endif
#pragma pack(1)
typedef struct _VECTOR
{
	float X;
	float Y;
	float Z;
	_VECTOR()
	{
		this->X = 0.0, this->Y = 0.0, this -> Z = 0.0;
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
		this->X +=  vector.X;
		this->Y +=  vector.Y;
		this->Z +=  vector.Z;
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
//64+ 4 =68 datablock
//68+8=76 size taken by header+first datablock

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

/*Used to save to .rec file*/
#pragma pack(1)
typedef struct _ONFOOT_DATABLOCK
{
	DWORD time;
	ONFOOT_SYNC_DATA  m_pOfSyncData;
} ONFOOT_DATABLOCK;

#pragma pack(1)
typedef struct _INCAR_DATABLOCK
{
	DWORD time;
	INCAR_SYNC_DATA  m_pIcSyncData;
} INCAR_DATABLOCK;