/*
	Copyright 2004-2005 SA:MP Team
	Version: $Id: playerpool.cpp,v 1.19 2006/05/07 15:35:32 kyeman Exp $

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
#include "main.h"
CVehiclePool::CVehiclePool()
{
	// loop through and initialize all net players to null and slot states to false
	for (WORD wVehicleID = 1; wVehicleID <= MAX_VEHICLES; wVehicleID++) {
		m_bVehicleSlotState[wVehicleID] = FALSE;
		m_pVehicles[wVehicleID] = NULL;
	}
	m_iVehicleCount = 0;
}
CVehiclePool::~CVehiclePool()
{
	for (WORD wVehicleID = 1; wVehicleID <= MAX_VEHICLES; wVehicleID++) {
		Delete(wVehicleID);
	}
	m_iVehicleCount = 0;
}
BOOL CVehiclePool::New(WORD wVehicleID, VECTOR vecPos, uint16_t model, QUATERNION quatRot, float fCarHealth, uint32_t dwDamage)
{
	if (wVehicleID > MAX_VEHICLES) return FALSE;
	
	m_pVehicles[wVehicleID] = new CVehicle(vecPos, model, quatRot, fCarHealth, dwDamage);

	if (m_pVehicles[wVehicleID])
	{
		m_bVehicleSlotState[wVehicleID] = TRUE;
		m_iVehicleCount++;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CVehiclePool::Delete(WORD wVehicleID)
{
	try
	{
		if (!GetSlotState(wVehicleID) || !m_pVehicles[wVehicleID])
		{
			return FALSE; // Vehicle already deleted or not used.
		}
		m_bVehicleSlotState[wVehicleID] = FALSE;
		delete m_pVehicles[wVehicleID];
		m_pVehicles[wVehicleID] = NULL;
		m_iVehicleCount--;

		return TRUE;
	}
	catch (...)
	{
		printf("Error when deleting m_pVehicles[%i]\n", wVehicleID);
		return FALSE;
	}
}