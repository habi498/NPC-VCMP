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
	m_wVehicleId = 0;
	m_byteSeatId = -1;
	m_byteSkinId = 0;
	m_byteTeamId = 0;
	m_byteState = PLAYER_STATE_NONE;
	m_dwLastKeys = 0;
	m_bIsStreamedIn = false;
	m_byteWeapon = 0;

	m_vecPos.X = 0.0f;
	m_vecPos.Y = 0.0f;
	m_vecPos.Z = 0.0f;
	m_fAngle = 0.0f;
	m_byteHealth = 0;
	m_byteArmour = 0;
	BYTE i;
	for (i = 0; i < 9; i++)
	{
		m_byteSlotWeapon[i] = 0;
		m_wSlotAmmo[i] = 0;
	}
	ONFOOT_SYNC_DATA* m_pOfSyncData = GetONFOOT_SYNC_DATA();
	m_pOfSyncData->byteArmour = 0;
	m_pOfSyncData->byteCurrentWeapon = 0;
	m_pOfSyncData->byteHealth = 0;
	m_pOfSyncData->dwKeys = 0;
	m_pOfSyncData->fAngle = 0;
	m_pOfSyncData->IsPlayerUpdateAiming = false;
	m_pOfSyncData->IsCrouching = false;
	m_pOfSyncData->bIsReloading = false;
	m_pOfSyncData->wAmmo = 0;
	ZeroVEC(m_pOfSyncData->vecAimDir);
	ZeroVEC(m_pOfSyncData->vecAimPos);
	ZeroVEC(m_pOfSyncData->vecPos);
	ZeroVEC(m_pOfSyncData->vecSpeed);
	INCAR_SYNC_DATA* m_pIcSyncData = GetINCAR_SYNC_DATA();
	m_pIcSyncData->byteCurrentWeapon = 0;
	m_pIcSyncData->bytePlayerArmour = 0;
	m_pIcSyncData->bytePlayerHealth = 0;
	m_pIcSyncData->dDamage = 0;
	m_pIcSyncData->dwKeys = 0;
	m_pIcSyncData->fCarHealth = 0;
	m_pIcSyncData->quatRotation = QUATERNION();
	m_pIcSyncData->Turretx = 0.0;
	m_pIcSyncData->Turrety = 0.0;
	m_pIcSyncData->vecMoveSpeed = VECTOR();
	m_pIcSyncData->vecPos = VECTOR();
	m_pIcSyncData->VehicleID = 0;
	m_pIcSyncData->wAmmo = 0;
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
	m_wVehicleId = 0;
	memcpy(&m_ofSync, pofSync, sizeof(ONFOOT_SYNC_DATA));
	UpdatePosition(pofSync->vecPos.X, pofSync->vecPos.Y, pofSync->vecPos.Z);
	m_fAngle = pofSync->fAngle;
	m_byteHealth = pofSync->byteHealth;
	m_byteArmour = pofSync->byteArmour;
	SetCurrentWeapon(pofSync->byteCurrentWeapon);
	UpdateWeaponSlot(pofSync->byteCurrentWeapon, pofSync->wAmmo);
	if(m_dwLastKeys!=pofSync->dwKeys)
		m_dwLastKeys = pofSync->dwKeys;
	if (pofSync->IsPlayerUpdateAiming)
		SetState(PLAYER_STATE_AIM);
	else SetState(PLAYER_STATE_ONFOOT);
}

void CPlayer::StoreInCarFullSyncData(INCAR_SYNC_DATA* picSync)
{
	m_wVehicleId = picSync->VehicleID;
	memcpy(&m_icSync, picSync, sizeof(INCAR_SYNC_DATA));

	UpdatePosition(m_icSync.vecPos.X, m_icSync.vecPos.Y, m_icSync.vecPos.Z);
	//asin(vehicle.Rotation.z)*2;
	m_fAngle = (float)asin(m_icSync.quatRotation.Z) * 2;
	m_byteHealth = m_icSync.bytePlayerHealth;
	m_byteArmour = m_icSync.bytePlayerArmour;
	SetCurrentWeapon(m_icSync.byteCurrentWeapon);
	UpdateWeaponSlot(m_icSync.byteCurrentWeapon, m_icSync.wAmmo);
	if (m_dwLastKeys != m_icSync.dwKeys)
		m_dwLastKeys = m_icSync.dwKeys;
	SetState(PLAYER_STATE_DRIVER);
}
//----------------------------------------------------

void CPlayer::SetWeaponSlot(BYTE byteSlot, BYTE byteWeapon, WORD wAmmo)
{
	if (byteSlot < 9)
	{
		m_byteSlotWeapon[byteSlot] = byteWeapon;
		m_wSlotAmmo[byteSlot] = wAmmo;
	}
}

//----------------------------------------------------
void CPlayer::SetCurrentWeapon(BYTE byteWeapon)
{
	//m_ofSync.byteCurrentWeapon = byteWeapon;
	m_byteWeapon = byteWeapon;
}
WORD CPlayer::GetCurrentWeaponAmmo()
{
	BYTE i;
	for (i = 0; i < 9; i++)
	{
		if (m_byteSlotWeapon[i] == m_byteWeapon)
		{
			return m_wSlotAmmo[i];
		}
	}
	return 0;
}
void CPlayer::UpdateWeaponSlot(uint8_t byteWeapon, WORD wAmmo)
{
	// Determine the slot of the current weapon.
	switch (byteWeapon)
	{
	case 0:
	case 1:
		m_byteSlotWeapon[0] = byteWeapon;
		m_wSlotAmmo[0] = wAmmo;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		m_byteSlotWeapon[1] = byteWeapon;
		m_wSlotAmmo[1] = wAmmo;
		break;
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		m_byteSlotWeapon[2] = byteWeapon;
		m_wSlotAmmo[2] = wAmmo;
		break;
	case 17:
	case 18:
		m_byteSlotWeapon[3] = byteWeapon;
		m_wSlotAmmo[3] = wAmmo;
		break;
	case 19:
	case 20:
	case 21:
		m_byteSlotWeapon[4] = byteWeapon;
		m_wSlotAmmo[4] = wAmmo;
		break;
	case 22:
	case 23:
	case 24:
	case 25:
		m_byteSlotWeapon[5] = byteWeapon;
		m_wSlotAmmo[5] = wAmmo;
		break;

	case 26:
	case 27:
		m_byteSlotWeapon[6] = byteWeapon;
		m_wSlotAmmo[6] = wAmmo;
		break;
	case 28:
	case 29:
		m_byteSlotWeapon[8] = byteWeapon;
		m_wSlotAmmo[8] = wAmmo;
		break;
	case 30:
	case 31:
	case 32:
	case 33:
		m_byteSlotWeapon[7] = byteWeapon;
		m_wSlotAmmo[7] = wAmmo;
		break;

	default:
		// If invalid weapon then set fists as weapon
		//m_byteSlotWeapon[0] = 0;
		//m_byteWeapon = 0;
		printf("Warning: Cannot update weapon slot for weapon %d\n", byteWeapon);
		break;
	}

}

void CPlayer::SetCurrentWeaponAmmo(WORD wAmmo)
{
	BYTE i;
	for (i = 0; i < 9; i++)
	{
		if (m_byteSlotWeapon[i] == m_byteWeapon)
		{
			m_wSlotAmmo[i] = wAmmo;
			break;
		}
	}
	//m_ofSync.wAmmo = wAmmo;
}


//----------------------------------------------------
