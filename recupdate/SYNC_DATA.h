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
Note: The npcclient program also uses this file. So any editing
in the structure of the classes has to be made on the corresponding
file with same name in project npcclient.
*/
#include <stdint.h>
#ifdef _WIN32
	#include <windows.h>
#endif
#ifndef WIN32
	typedef unsigned int DWORD;
#endif
#pragma pack(1)
typedef struct _VECTOR
{
	float X;
	float Y;
	float Z;
}VECTOR;

#pragma pack(1)
typedef struct _QUATERNION
{
	float X;
	float Y;
	float Z;
	float W;
}QUATERNION;

#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA_V1
{
	uint32_t dwKeys;
	VECTOR vecPos;
	float fAngle;
	uint8_t byteHealth;
	uint8_t byteArmour;
	uint8_t byteCurrentWeapon;
	bool IsCrouching;
	VECTOR vecSpeed;
	bool IsAiming;
	VECTOR vecAimDir;
	VECTOR vecAimPos;
	//bool bIsReloading; //1 byte
	//uint16_t wAmmo; // 2 byte
} ONFOOT_SYNC_DATA_V1;
//61+ 4 =65 datablock
//65+8=73 size taken by header+first datablock
#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA_V2
{
	uint32_t dwKeys;
	VECTOR vecPos;
	float fAngle;
	uint8_t byteHealth;
	uint8_t byteArmour;
	uint8_t byteCurrentWeapon;
	bool IsCrouching;
	VECTOR vecSpeed;
	bool IsAiming;
	VECTOR vecAimDir;
	VECTOR vecAimPos;
	bool bIsReloading; //1 byte
	uint16_t wAmmo; // 2 byte
} ONFOOT_SYNC_DATA_V2;
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
} INCAR_SYNC_DATA;

/*Used to save to .rec file*/
#pragma pack(1)
typedef struct _ONFOOT_DATABLOCK_V1
{
	DWORD time;
	ONFOOT_SYNC_DATA_V1  m_pOfSyncData;
} ONFOOT_DATABLOCK_V1;
#pragma pack(1)
typedef struct _ONFOOT_DATABLOCK_V2
{
	DWORD time;
	ONFOOT_SYNC_DATA_V2  m_pOfSyncData;
} ONFOOT_DATABLOCK_V2;

#pragma pack(1)
typedef struct _INCAR_DATABLOCK
{
	DWORD time;
	INCAR_SYNC_DATA  m_pIcSyncData;
} INCAR_DATABLOCK;