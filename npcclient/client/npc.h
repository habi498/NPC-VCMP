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
#include <map>
#include <iterator>

#ifndef NPC_H
#define NPC_H
#define MAX_PLAYERS 100
#define MAX_VEHICLES 1000
#define NPC_SHOOTING_ENABLED
#define SPAWN_LOCK 4000  //amount in ms not to respond to server setting pos
#define NOT_DEFINED 255
#define DISABLE_AUTO_PASSENGER_SYNC -1
#define MAX_CLASSES_FOR_SPAWNING 50  //actually 50
class CVehicle
{
	VECTOR vecPos;
	uint16_t model;
	uint8_t driverId;
	float fCarHealth;
	uint32_t dwDamage;
	QUATERNION quatRotation;
	VECTOR vecSpeed;
	float fTurretx; float fTurrety;
public:
	CVehicle(VECTOR vecPos, uint16_t model, QUATERNION quatRot, float fCarHealth, uint32_t dwDamage)
	{
		this->vecPos = vecPos;
		this->model = model;
		//this->streamedIn = streamedIn;
		this->driverId = 255;
		this->fCarHealth = fCarHealth;
		this->dwDamage = dwDamage;
		quatRotation = quatRot;
		vecSpeed = VECTOR(); fTurretx = 0, fTurrety = 0;
	}
	CVehicle()
	{
		vecPos=VECTOR(), model = 0;
		//streamedIn = false;
		driverId = 255;
		fCarHealth = 1000.0;
		dwDamage = 0;
		quatRotation = QUATERNION(0, 0, 0, 1);
		vecSpeed = VECTOR(); fTurretx = 0, fTurrety = 0;
	}
	uint16_t GetModel()
	{
		return this->model;
	}
	void CopyFromIcSyncData(uint8_t bytePlayerId, INCAR_SYNC_DATA* pIcSyncData)
	{
		this->driverId = bytePlayerId;
		this->dwDamage = pIcSyncData->dDamage;
		this->fCarHealth = pIcSyncData->fCarHealth;
		this->fTurretx = pIcSyncData->Turretx;
		this->fTurrety = pIcSyncData->Turrety;
		this->vecPos = pIcSyncData->vecPos;
		this->vecSpeed = pIcSyncData->vecMoveSpeed;
		this->quatRotation = pIcSyncData->quatRotation;
		//this->streamedIn = true;
	}
	void UpdatePosition(VECTOR vecPos)
	{
		this->vecPos = vecPos;
	}
	void UpdateRotation(QUATERNION quatRotation)
	{
		this->quatRotation = quatRotation;
	}
	VECTOR GetPosition()
	{
		return vecPos;
	}
	QUATERNION GetRotation()
	{
		return quatRotation;
	}
	float GetTurretx() { return fTurretx; }
	float GetTurrety() { return fTurrety; }
	VECTOR GetSpeed() { return vecSpeed; }
	float GetHealth() { return fCarHealth; }
	void SetHealth(float fHealth) { fCarHealth = fHealth; }
	uint32_t GetDamage() { return dwDamage; }
	void SetDriver(uint8_t driverId)
	{
		this->driverId = driverId;
	}
	void RemoveDriver() { this->driverId = 255; }
	/*void Destream() { streamedIn = false; }
	void StreamedIn() { streamedIn = true; }
	bool IsStreamedIn() { return streamedIn;}*/
	uint8_t GetDriver() { return driverId; 
	}
	~CVehicle()
	{
	}
};
enum AltSpawn
{
	OFF,
	ON,
	SPAWNED,
	SetPosFromServerRecvd=4,
	SetAngleFromServerRecd=8,
};
//used to store internal data
class NPC
{
private:
	unsigned char ID;

	bool isSpawned = false;
	bool initialized = false;//true implies ID is set
	//bool strmdplrs[MAX_PLAYERS];
	bool bLockOnSync = false;//prevent server setting position
	DWORD dw_SpawnedTick = 0;//dword to store tickcount of first spawn
	uint8_t SpawnClass = 0;
	uint8_t byteClassRequestCounter = 0;
public:
	uint8_t AltSpawnStatus = AltSpawn::OFF;
	bool bIsClassRequested = false;
	uint8_t byteClassIndexRequested = CLASS_CURRENT;
	float fSpawnLocX = 0.0, fSpawnLocY = 0.0, fSpawnLocZ = 0.0;
	float fSpawnAngle = 0.0;
	uint8_t SpecialSkin = 255;
	uint8_t SpawnWeapon = 255;
	bool AbsClassNotSpecified = true;
	uint8_t GetSpawnClass() { return SpawnClass; }
	void SetSpawnClass(uint8_t classID) {
		SpawnClass = classID; AbsClassNotSpecified = false;
		byteClassRequestCounter = 0;
	}
	void UnsetSpawnClass(){  AbsClassNotSpecified = true; } //npc will not spawn at SpawnClass uint8_t
	uint8_t GetClassRequestCounter() { return byteClassRequestCounter; }
	void IncrementClassRequestCounter() { byteClassRequestCounter++; };
	//uint8_t ClassSelectionCounter = 0;
	uint16_t PSLimit = 2;//sync passenger packets per 2 In Car Sync Packets of driver
	uint16_t PSCounter = 0;//PS for Passenger Sync
	bool PSOnServerCycle = false;//Automatic Passenger Syncing when
	//vehicle has no driver or driver not streamed in
	DWORD PSLastSyncedTick = 0;
	DWORD TimeShotInfoSend = 0;
	bool WaitingToRemoveBody = false;
	uint8_t KillerId = 255;//assumming killer cannot quit and other player connect within 2100 ms
	uint8_t ShotInfoWeapon = 44;//Fell to death
	uint8_t byteAutodeathBodyPart = 0;
	//Something to help send foot sync after exiting vehicle
	bool WaitingForVEOnFootSync = false;
	DWORD VETickCount = 0;
	char PotentialClassID = 0;
public:
	NPC()
	{
		anticheatID = 0;
	}
	~NPC() {};
	uint32_t anticheatID;//Every time server sets health, armour position or anything of npc, this count is increased by one.
	
	
	
	unsigned char GetID() { return ID; }
	void SetID(unsigned char id)
	{ 
		ID = id; 
		if (initialized == false)
		initialized = true;
	}
	bool Initialized() { return initialized; }
	bool IsSpawned() { return isSpawned; }
	void SetSpawnStatus(bool status) {  isSpawned = status; }
	void SetSpawnLocation(float x, float y, float z, float angle=0.0)
	{
		//bSpawnAtXYZ = true;
		AltSpawnStatus = AltSpawn::ON;
		fSpawnLocX = x;
		fSpawnLocY = y;
		fSpawnLocZ = z;
		fSpawnAngle = angle;
		bLockOnSync = true;
	}
	void SetSpawnWeapon(BYTE weapon)
	{
		SpawnWeapon = weapon;
		bLockOnSync = true;
	}
	void StoreFirstSpawnedTime()
	{
		dw_SpawnedTick = GetTickCount();
	}
	bool IsSyncPaused()
	{
		if (bLockOnSync)
		{
			DWORD tick = GetTickCount();
			if (tick > dw_SpawnedTick)
			{
				if (tick > (dw_SpawnedTick + SPAWN_LOCK) )
					return false;
				else return true;
			}
			else
			{
				//This will not be reached ordinary 
				//one possibility with 49.7 days system wrap around zero
				if (tick > SPAWN_LOCK) //wait for SPAWN_LOCK ms anyway
					return false;
				else return true;
			}
		}
		return false;
	}
};
typedef enum tagNPCFIELDS
{
	I_KEYS,
	F_POSX,
	F_POSY,
	F_POSZ,
	F_ANGLE,
	I_HEALTH,
	I_ARMOUR,
	I_CURWEP,
	I_CURWEP_AMMO,
	V_ONFOOT_SPEED,
	V_AIMPOS,
	V_AIMDIR,
	V_POS,
	Q_CAR_ROTATION,
	F_CAR_HEALTH,
	I_CAR_DAMAGE,
	V_CAR_SPEED,
	F_CAR_TURRETX,
	F_CAR_TURRETY,
	F_CAR_ROTATIONX,
	F_CAR_ROTATIONY,
	F_CAR_ROTATIONZ,
	F_CAR_ROTATIONW,
	I_ACTION
}NPCFIELDS;
#endif