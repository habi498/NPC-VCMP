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
	bool m_bIsStreamedIn;
	uint8_t m_byteWeapon;
	Color m_Color;
	// Weapon data
	WORD					m_wSlotAmmo[9];
	BYTE					m_byteSlotWeapon[9];

public:
	uint16_t m_wVehicleId;
	uint8_t m_byteSeatId;
	uint8_t m_byteSkinId;
	uint8_t m_byteTeamId;
	void SetColor(Color color) { m_Color = color; }
	Color GetColor() { return m_Color; }
	bool IsStreamedIn() { return m_bIsStreamedIn; }
	void SetStreamedIn(bool streamedIn) { m_bIsStreamedIn = streamedIn; }
	void SetState(uint8_t byteState);
	uint8_t GetState() { return m_byteState; };
	uint32_t GetKeys() { return m_dwLastKeys; };
	void SetKeys(uint32_t dw_keys) {
		//used in player: npc
		m_dwLastKeys = dw_keys;
	}
	INCAR_SYNC_DATA* GetINCAR_SYNC_DATA() { return &m_icSync; };
	ONFOOT_SYNC_DATA* GetONFOOT_SYNC_DATA() { 
		m_ofSync.byteArmour = m_byteArmour;
		m_ofSync.byteHealth = m_byteHealth;
		m_ofSync.fAngle = m_fAngle;
		m_ofSync.vecPos = m_vecPos;
		return &m_ofSync; };
	CPlayer();
	~CPlayer() {};
	uint8_t m_byteHealth;
	uint8_t m_byteArmour;
	VECTOR m_vecPos;
	float m_fAngle;
	void UpdatePosition(float x, float y, float z);
	void SetID(uint8_t bytePlayerID) { m_bytePlayerID = bytePlayerID; };
	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA* pofSync);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA* picSync);

	void SetWeaponSlot(BYTE byteSlot, BYTE byteWeapon, WORD wAmmo);

	BYTE GetSlotWeapon(BYTE bSlot)
	{
		if (bSlot < 9)
			return m_byteSlotWeapon[bSlot];
		else return 0;
	};
	WORD GetSlotAmmo(BYTE bSlot) {
		if (bSlot < 9)
			return m_wSlotAmmo[bSlot];
		else return 0;
	};
	BYTE GetCurrentWeapon() { return m_byteWeapon; };
	WORD GetCurrentWeaponAmmo();
	void UpdateWeaponSlot(uint8_t byteWeapon, WORD wAmmo);
	//only for npcs SetCurrentWeaponAmmo
	void SetCurrentWeaponAmmo(WORD wAmmo);

	//only for npcs SetCurrentWeapon
	//----------------------------------------------------
	void SetCurrentWeapon(BYTE byteWeapon);

	void RemoveAllWeapons()
	{
		for (uint8_t i = 0; i < 9; i++)
		{
			m_byteSlotWeapon[i] = 0;
			m_wSlotAmmo[i] = 0;
		}
	}

	void RemoveWeaponAtSlot(uint8_t byteSlot)
	{
		SetWeaponSlot(byteSlot, 0, 0);
	}

	void RemoveWeapon(uint8_t byteWeapon)
	{
		BYTE slotId=GetSlotIdFromWeaponId(byteWeapon);
		RemoveWeaponAtSlot(slotId);
	}
};
#endif