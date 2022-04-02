/*
	Copyright (C) 2004-2005 SA:MP Team

	Version: $Id: player.cpp,v 1.39 2006/05/07 15:35:32 kyeman Exp $

	Modified by and Copyright (C) 2022  habi

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
CPlayer::CPlayer()
{
	m_bytePlayerID = INVALID_PLAYER_ID;
	m_VehicleID = 0;
	m_byteState = PLAYER_STATE_NONE;
	m_dwLastKeys = 0;

	m_vecPos.X = 0.0f;
	m_vecPos.Y = 0.0f;
	m_vecPos.Z = 0.0f;
	m_fAngle = 0.0f;
	m_byteHealth = 0;
	m_byteArmour = 0;
	byteCurWeap = 0;
}
void CPlayer::SetState(BYTE byteNewState)
{
	BYTE byteOldState;
	if (m_byteState != byteNewState) {
		byteOldState = m_byteState;
		m_byteState = byteNewState;
	}
}
void CPlayer::UpdatePosition(float x, float y, float z)
{
	m_vecPos.X = x;
	m_vecPos.Y = y;
	m_vecPos.Z = z;
}
void CPlayer::StoreOnFootFullSyncData(ONFOOT_SYNC_DATA* pofSync)
{
	m_VehicleID = 0;
	memcpy(&m_ofSync, pofSync, sizeof(ONFOOT_SYNC_DATA));
	UpdatePosition(pofSync->vecPos.X, pofSync->vecPos.Y, pofSync->vecPos.Z);
	m_fAngle = pofSync->fAngle;
	m_byteHealth = pofSync->byteHealth;
	m_byteArmour = pofSync->byteArmour;
	byteCurWeap=pofSync->byteCurrentWeapon;
	if(m_dwLastKeys!=pofSync->dwKeys)
		m_dwLastKeys = pofSync->dwKeys;
	if (pofSync->IsAiming)
		SetState(PLAYER_STATE_AIM);
	else SetState(PLAYER_STATE_ONFOOT);
}

void CPlayer::StoreInCarFullSyncData(INCAR_SYNC_DATA* picSync)
{
	m_VehicleID = picSync->VehicleID;
	memcpy(&m_icSync, picSync, sizeof(INCAR_SYNC_DATA));

	UpdatePosition(m_icSync.vecPos.X, m_icSync.vecPos.Y, m_icSync.vecPos.Z);
	//asin(vehicle.Rotation.z)*2;
	m_fAngle = (float)asin(m_icSync.quatRotation.Z) * 2;
	m_byteHealth = m_icSync.bytePlayerHealth;
	m_byteArmour = m_icSync.bytePlayerArmour;
	byteCurWeap = m_icSync.byteCurrentWeapon;
	if (m_dwLastKeys != m_icSync.dwKeys)
		m_dwLastKeys = m_icSync.dwKeys;
	SetState(PLAYER_STATE_DRIVER);
}
