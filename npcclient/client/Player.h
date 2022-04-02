/*
	Copyright (C) 2004-2005 SA:MP Team
	Version: $Id: player.h,v 1.30 2006/05/07 15:35:32 kyeman Exp $

	Modified and Copyright (C) 2022  habi

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
#ifndef PLAYER_H
#define PLAYER_H
#define PLAYER_STATE_NONE						0
#define PLAYER_STATE_ONFOOT						1
#define PLAYER_STATE_AIM						2
#define PLAYER_STATE_DRIVER						3
#define PLAYER_STATE_PASSENGER					4
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER		5
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER	6
#define PLAYER_STATE_EXIT_VEHICLE				7
#define PLAYER_STATE_WASTED						8
#define PLAYER_STATE_SPAWNED					9

class CPlayer
{
private:
	uint8_t m_bytePlayerID;
	uint32_t m_dwLastKeys;
	uint8_t m_byteState;
	ONFOOT_SYNC_DATA		m_ofSync;
	INCAR_SYNC_DATA			m_icSync;
	//PASSENGER_SYNC_DATA		m_psSync;
	//AIM_SYNC_DATA			m_aimSync;
public:
	uint16_t m_VehicleID;
	uint8_t byteCurWeap;
	void SetState(uint8_t byteState);
	uint8_t GetState() { return m_byteState; };
	uint32_t GetKeys() { return m_dwLastKeys; };
	INCAR_SYNC_DATA* GetINCAR_SYNC_DATA() { return &m_icSync; };
	ONFOOT_SYNC_DATA* GetONFOOT_SYNC_DATA() { return &m_ofSync; };
	CPlayer();
	~CPlayer() {};
	uint8_t m_byteHealth;
	uint8_t m_byteArmour;
	VECTOR m_vecPos;
	float m_fAngle;
	void UpdatePosition(float x, float y, float z);
	void SetID(uint8_t bytePlayerID) { m_bytePlayerID = bytePlayerID; };
	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA* pofSync);
	//void StoreAimSyncData(AIM_SYNC_DATA* paimSync);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA* picSync);
};
#endif