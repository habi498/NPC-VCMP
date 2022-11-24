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
class NPC
{
private:
	unsigned char ID;
	bool isSpawned = false;
	bool initialized = false;//true implies ID is set
	std::map<uint16_t,bool>strmdvhcls;
	bool strmdplrs[MAX_PLAYERS];
public:
	NPC()
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
			strmdplrs[i] = false;
	}
	~NPC() {};
	uint32_t anticheatID;//Every time server sets health, armour position or anything of npc, this count is increased by one.
	void AddStreamedVehicle(uint16_t vehicle)
	{
		if (strmdvhcls.find(vehicle) == strmdvhcls.end())
			strmdvhcls.insert({ vehicle, true });//the true is dummy
	}
	void DestreamVehicle(uint16_t vehicle)
	{
		if (strmdvhcls.find(vehicle) != strmdvhcls.end())
			strmdvhcls.erase(vehicle);
	}
	bool IsVehicleStreamedIn(uint16_t vehicle)
	{
		if (strmdvhcls.find(vehicle) != strmdvhcls.end())
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