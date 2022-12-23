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
#define NPC_SHOOTING_ENABLED
#define SPAWN_LOCK 4000  //amount in ms not to respond to server setting pos
#define NOT_DEFINED 255
#define DISABLE_AUTO_PASSENGER_SYNC -1
class Vehicle
{
	float x, y, z;
	uint16_t model;
	bool streamedIn;
	uint8_t driverId;
public:
	Vehicle(float x, float y, float z, uint16_t model, bool streamedIn = true)
	{
		this->x = x; this->y = y; this->z = z;
		this->model = model;
		this->streamedIn = streamedIn;
		this->driverId = 255;
	}
	Vehicle()
	{
		x = 0.0, y = 0.0, z = 0.0, model = 0;
		streamedIn = false;
		driverId = 255;
	}
	void UpdatePosition(float x, float y, float z)
	{
		this->x = x; this->y = y; this->z = z;
	}
	void GetPosition(float& x, float& y, float& z)
	{
		x = this->x; y = this->y; z = this->z;
	}
	void SetDriver(uint8_t driverId)
	{
		this->driverId = driverId;
	}
	void RemoveDriver() { this->driverId = 255; }
	void Destream() { streamedIn = false; }
	void StreamedIn() { streamedIn = true; }
	bool IsStreamedIn() { return streamedIn;}
	uint8_t GetDriver() { return driverId; }
	~Vehicle()
	{
	}
};
//used to store internal data
class NPC
{
private:
	unsigned char ID;
	bool isSpawned = false;
	bool initialized = false;//true implies ID is set
	std::map<uint16_t,Vehicle>strmdvhcls;
	bool strmdplrs[MAX_PLAYERS];
	bool bLockOnSync = false;//prevent server setting position
	DWORD dw_SpawnedTick = 0;//dword to store tickcount of first spawn
	
	
public:
	bool bSpawnAtXYZ = false;
	
	float fSpawnLocX = 0.0, fSpawnLocY = 0.0, fSpawnLocZ = 0.0;
	float fSpawnAngle = 0.0;
	uint8_t SpecialSkin = 255;
	uint8_t SpawnWeapon = 255;
	uint8_t SpawnClass = 0;
	uint8_t ClassSelectionCounter = 0;
	uint16_t PSLimit = 2;//sync passenger packets per 2 In Car Sync Packets of driver
	uint16_t PSCounter = 0;//PS for Passenger Sync
	bool PSOnServerCycle = false;//Automatic Passenger Syncing when
	//vehicle has no driver or driver not streamed in
	DWORD PSLastSyncedTick = 0;
public:
	NPC()
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
			strmdplrs[i] = false;
	}
	~NPC() {};
	uint32_t anticheatID;//Every time server sets health, armour position or anything of npc, this count is increased by one.
	void AddStreamedVehicle(uint16_t vehicleId, float x, float y, float z, int16_t model)
	{
		if (strmdvhcls.find(vehicleId) == strmdvhcls.end())
		{
			strmdvhcls.insert({ vehicleId, Vehicle(x,y,z,model) });
		}
		else
		{
			strmdvhcls[vehicleId].UpdatePosition(x, y, z);
			strmdvhcls[vehicleId].StreamedIn();
		}
	}
	void UpdateStreamedVehiclePosition(uint16_t vehicleId, float x, float y, float z)
	{
		if (strmdvhcls.find(vehicleId) != strmdvhcls.end())
		{
			strmdvhcls[vehicleId].UpdatePosition(x, y, z);
		}
	}
	void GetVehiclePosition(uint16_t vehicleId, float &x, float &y, float &z)
	{
		if (strmdvhcls.find(vehicleId) != strmdvhcls.end())
		{
			strmdvhcls[vehicleId].GetPosition(x, y, z);
		}
		else
		{
			x = 0; y = 0; z = 0;
		}
	}
	void SetVehicleDriver(uint16_t vehicleId, uint8_t driverId)
	{
		if (strmdvhcls.find(vehicleId) != strmdvhcls.end())
		{
			strmdvhcls[vehicleId].SetDriver(driverId);
		}
		else
		{
			//Create an instance with streamedIn=false;
			strmdvhcls.insert({ vehicleId, Vehicle(0.0,0.0,0.0,0,false) });
			strmdvhcls[vehicleId].SetDriver(driverId);
		}
	}
	void RemoveVehicleDriver(uint16_t vehicleId)
	{
		if (strmdvhcls.find(vehicleId) != strmdvhcls.end())
		{
			strmdvhcls[vehicleId].RemoveDriver();
		}
	}
	uint8_t GetVehicleDriver(uint16_t vehicleId)
	{
		if (strmdvhcls.find(vehicleId) != strmdvhcls.end())
		{
			return strmdvhcls[vehicleId].GetDriver();
		}
		return 255;
	}
	void DestreamVehicle(uint16_t vehicle)
	{
		if (strmdvhcls.find(vehicle) != strmdvhcls.end())
			strmdvhcls[vehicle].Destream();
	}
	bool IsVehicleStreamedIn(uint16_t vehicle)
	{
		if (strmdvhcls.find(vehicle) != strmdvhcls.end()
			&& strmdvhcls[vehicle].IsStreamedIn())
			return true;
		return false;
	}
	bool IsPlayerStreamedIn(uint8_t playerid) { return this->strmdplrs[playerid]; }
	void AddStreamedPlayer(uint8_t playerid)
	{
		strmdplrs[playerid] = true;
	}
	void RemoveStreamedPlayer(uint8_t playerid)
	{
		strmdplrs[playerid] = false;
	}
	//void SetHealth(uint8_t health) { this->health = health; }
	//void SetArmour(uint8_t armour) { this->armour = armour; }

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
		bSpawnAtXYZ = true;
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
	I_CURWEP_AMMO
}NPCFIELDS;
#endif