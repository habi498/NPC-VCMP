/*
	Copyright 2004-2005 SA:MP Team
	Version: $Id: playerpool.h,v 1.12 2006/04/09 09:54:46 kyeman Exp $

	Modified and Copyright (C) 2023  habi

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
#ifndef VEHICLE_POOL_H
#define VEHICLE_POOL_H
class CVehiclePool
{
private:
	BOOL	m_bVehicleSlotState[MAX_VEHICLES+1];
	CVehicle* m_pVehicles[MAX_VEHICLES+1];
	int		m_iVehicleCount;
public:
	CVehiclePool();
	~CVehiclePool();
	//BOOL New(WORD wVehicleID);
	BOOL New(WORD wVehicleID, VECTOR vecPos, uint16_t model, QUATERNION quatRot, float fCarHealth, uint32_t dwDamage);
	BOOL Delete(WORD wVehicleID);

	// Retrieve a player
	CVehicle* GetAt(WORD wVehicleID) {
		if (wVehicleID > MAX_VEHICLES) { return NULL; }
		return m_pVehicles[wVehicleID];
	};
	// Find out if the slot is inuse.
	BOOL GetSlotState(WORD wVehicleID) {
		if (wVehicleID > MAX_VEHICLES) { return FALSE; }
		else return m_bVehicleSlotState[wVehicleID];
	};

	/*PCHAR GetPlayerName(BYTE bytePlayerID) {
		if (bytePlayerID >= MAX_PLAYERS) { return FALSE; }
		return m_szPlayerName[bytePlayerID];
	};
	void SetPlayerName(BYTE bytePlayerID, PCHAR szName) {
		strcpy(m_szPlayerName[bytePlayerID], szName);
	}*/
	int GetVehicleCount() { return m_iVehicleCount; };
};
#endif