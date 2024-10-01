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

#include <stdint.h>
#ifdef _WIN32
	#include <windows.h>
#endif
#ifndef WIN32
	typedef unsigned int DWORD;
#endif
#pragma pack(1)
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


//The following is used in recupdate v2 and v3
#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA_V2ANDV3
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
} ONFOOT_SYNC_DATA_V2ANDV3;

//The following is used in recupdate v4
#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA_V4
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
	uint8_t byteAction;
	uint8_t byteReserved;
} ONFOOT_SYNC_DATA_V4;

//Used by recupdate version prior to 3.0
#pragma pack(1)
typedef struct _INCAR_SYNC_DATA_BEFOREV3
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
} INCAR_SYNC_DATA_BEFOREV3;

//Recupdate data out in v3.0
typedef struct _INCAR_SYNC_DATA_V3
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
} INCAR_SYNC_DATA_V3;

/*Used to save to .rec file*/
#pragma pack(1)
typedef struct _ONFOOT_DATABLOCK_V1
{
	DWORD time;
	ONFOOT_SYNC_DATA_V1  m_pOfSyncData;
} ONFOOT_DATABLOCK_V1;
#pragma pack(1)
typedef struct _ONFOOT_DATABLOCK_V2ANDV3
{
	DWORD time;
	ONFOOT_SYNC_DATA_V2ANDV3  m_pOfSyncData;
} ONFOOT_DATABLOCK_V2ANDV3;
#pragma pack(1)
typedef struct _ONFOOT_DATABLOCK_V4
{
	DWORD time;
	ONFOOT_SYNC_DATA_V4  m_pOfSyncData;
} ONFOOT_DATABLOCK_V4;

#pragma pack(1)
typedef struct _INCAR_DATABLOCK_BEFOREV3
{
	DWORD time;
	INCAR_SYNC_DATA_BEFOREV3  m_pIcSyncData;
} INCAR_DATABLOCK_BEFOREV3;

#pragma pack(1)
typedef struct _INCAR_DATABLOCK_V3
{
	DWORD time;
	INCAR_SYNC_DATA_V3  m_pIcSyncData;
} INCAR_DATABLOCK_V3;
