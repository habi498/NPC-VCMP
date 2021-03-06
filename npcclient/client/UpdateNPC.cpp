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

#include "main.h"
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
extern NPC* iNPC;
extern CPlayer* npc;
void SetActionFlags(ONFOOT_SYNC_DATA* m_pOfSyncData, uint8_t* action);
void SetActionFlags(INCAR_SYNC_DATA* m_InSyncData, uint8_t* action);
void SetTypeFlags(INCAR_SYNC_DATA* m_InSyncData, uint8_t* type);
void SetRotationFlags(INCAR_SYNC_DATA* m_InSyncData, uint8_t* flag);
void SetVehicleIDFlag(INCAR_SYNC_DATA* m_pIcSyncData, uint8_t* nibble, uint8_t* byte);
float ConvertFromUINT16_T(uint16_t compressedFloat, float base)
{
	if (base != -1)
	{
		float value = ((float)compressedFloat / 32767.5f - 1.0f) * base;
		return value;
	}
	else
	{
		float value = ((float)compressedFloat / 65536);
		return value;
	}
}
VECTOR ConvertFromUINT16_T(uint16_t w_ComponentX, uint16_t w_ComponentY, uint16_t w_ComponentZ, float base)
{
	VECTOR vecResult;
	vecResult.X = ConvertFromUINT16_T(w_ComponentX, base);
	vecResult.Y = ConvertFromUINT16_T(w_ComponentY, base);
	vecResult.Z = ConvertFromUINT16_T(w_ComponentZ, base);
	return vecResult;
}
float VLen(VECTOR v)
{
	return (float)sqrt(pow(v.X, 2) + pow(v.Y, 2) + pow(v.Z, 2));
}
uint16_t ConvertToUINT16_T(float value, float base)
{
	if (base != -1)
	{
		uint16_t s = (uint16_t)((value / base + 1.0f) * 32767.5f);
		return s;
	}
	else
	{
		uint16_t s = (unsigned short)((abs(value)) * (65535));
		return s;
	}
}
void SendNPCSyncData(INCAR_SYNC_DATA* m_pInSyncData, PacketPriority mPriority=HIGH_PRIORITY )
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_PLAYERUPDATE_DRIVER);
	bsOut.Write(iNPC->anticheatID);
	uint8_t action = 0;
	SetActionFlags(m_pInSyncData,&action);
	bsOut.Write(action);
	bsOut.Write(_byteswap_ushort((uint16_t)(m_pInSyncData->dwKeys&0xFFFF)));//swapping is needed
	uint8_t nibble, byte;
	SetVehicleIDFlag(m_pInSyncData, &nibble, &byte);
	bsOut.Write(byte);
	WriteNibble(nibble, &bsOut);
	bsOut.Write(m_pInSyncData->bytePlayerHealth);
	if (action & 0x80)
	{
		bsOut.Write(m_pInSyncData->byteCurrentWeapon);
		if (m_pInSyncData->byteCurrentWeapon > 11)
			bsOut.Write((uint16_t)1);
	}
	if (action & 0x40)
		bsOut.Write(m_pInSyncData->bytePlayerArmour);
	bool bTurret = false;
	if (m_pInSyncData->Turretx != 0 || m_pInSyncData->Turrety != 0)
	{
		WriteNibble(1, &bsOut);
		bTurret = true;
	}
	else
		WriteNibble(0, &bsOut);
	uint8_t type = 0; SetTypeFlags(m_pInSyncData, &type);
	WriteNibble(type, &bsOut);
	bsOut.Write(m_pInSyncData->vecPos.X);
	bsOut.Write(m_pInSyncData->vecPos.Y);
	bsOut.Write(m_pInSyncData->vecPos.Z);
	if (type & 0x1)
	{
		uint16_t speedx, speedy, speedz;
		speedx = ConvertToUINT16_T(m_pInSyncData->vecMoveSpeed.X, 20.0);
		speedy = ConvertToUINT16_T(m_pInSyncData->vecMoveSpeed.Y, 20.0);
		speedz = ConvertToUINT16_T(m_pInSyncData->vecMoveSpeed.Z, 20.0);
		bsOut.Write(speedx); bsOut.Write(speedy); bsOut.Write(speedz);
	}
	uint8_t byteFlag = 0;
	SetRotationFlags(m_pInSyncData, &byteFlag);
	WriteNibble(byteFlag, &bsOut);
	uint16_t w_rotx, w_roty, w_rotz;
	w_rotx= ConvertToUINT16_T(m_pInSyncData->quatRotation.X, -1);
	w_roty= ConvertToUINT16_T(m_pInSyncData->quatRotation.Y, -1);
	w_rotz= ConvertToUINT16_T(m_pInSyncData->quatRotation.Z, -1);
	bsOut.Write(w_rotx);
	bsOut.Write(w_roty); bsOut.Write(w_rotz);
	if (type & 0x2)
		bsOut.Write(m_pInSyncData->dDamage);
	if (type & 0x4)
		bsOut.Write(m_pInSyncData->fCarHealth);
	if (bTurret)
	{
		uint16_t wHorizontal, wVertical;
		wHorizontal= ConvertToUINT16_T(m_pInSyncData->Turretx, 2 * (float)PI);
		wVertical= ConvertToUINT16_T(m_pInSyncData->Turrety, 2 * (float)PI);
		bsOut.Write(wHorizontal);
		bsOut.Write(wVertical);
	}
	//Mystery bytes. Might need investigation..
	bsOut.Write((uint8_t)0);
	bsOut.Write((uint8_t)0xc8);
	bsOut.Write((uint8_t)0);//Changes when vehicle is on water.?
	bsOut.Write((uint8_t)0);
	peer->Send(&bsOut, mPriority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
}
void SendNPCSyncData(ONFOOT_SYNC_DATA* m_pOfSyncData)
{
	RakNet::BitStream bsOut;
	if(!m_pOfSyncData->IsAiming)
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE);
	else
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM);
	bsOut.Write(iNPC->anticheatID);
	uint8_t action = 0;
	SetActionFlags(m_pOfSyncData, &action);
	uint8_t nibble = 0;
	if (action & 0x80)
		nibble |= 0x2;
	#ifdef NPC_SHOOTING_ENABLED
		bool reloading_weapon = false;
		if (m_pOfSyncData->IsAiming)
		{
			if ((m_pOfSyncData->dwKeys & 512) == 0)
			{
				//This means player is reloading weapon!
				reloading_weapon = true;
				nibble|= 0x8;
				action = 0xd0;
			}
		}
	#endif
	bsOut.Write(action);
	if(nibble)
		WriteNibble(nibble, &bsOut);

	bsOut.Write(m_pOfSyncData->vecPos.X);
	bsOut.Write(m_pOfSyncData->vecPos.Y);
	bsOut.Write(m_pOfSyncData->vecPos.Z);
	WORD angle=ConvertToUINT16_T(m_pOfSyncData->fAngle, 2 * (float)PI);
	bsOut.Write(angle);
	if (action & 1)
	{
		VECTOR vecSpeed = m_pOfSyncData->vecSpeed;
		uint16_t speedx= ConvertToUINT16_T(vecSpeed.X, 20.0);
		uint16_t speedy= ConvertToUINT16_T(vecSpeed.Y, 20.0);
		uint16_t speedz= ConvertToUINT16_T(vecSpeed.Z, 20.0);
		bsOut.Write(speedx); bsOut.Write(speedy); bsOut.Write(speedz);
	}
	if (action & 0x40)
	{
		bsOut.Write(m_pOfSyncData->byteCurrentWeapon);
		if (m_pOfSyncData->byteCurrentWeapon > 11)
			bsOut.Write((WORD)1);//ammo
	}
	if (action & 0x10)
	{
		uint8_t msb = 0x1;//most significant byte
		#ifdef NPC_SHOOTING_ENABLED
			msb = reloading_weapon ? 0 : msb;
		#endif
		if (m_pOfSyncData->dwKeys > 0xFFFF)
		{
			msb |= 0x8;
		}
		uint8_t keybyte1 = m_pOfSyncData->dwKeys & 0xFF;
		uint8_t keybyte2 = (m_pOfSyncData->dwKeys >> 8) & 0xFF;
		bsOut.Write(keybyte1);
		bsOut.Write(keybyte2);
		bsOut.Write(msb);
		#ifdef NPC_SHOOTING_ENABLED
			uint8_t byteVal;
			if ((m_pOfSyncData->dwKeys & 1) && !reloading_weapon)
				byteVal = 0xc;
			else if (reloading_weapon)
				byteVal = 0x1;
			WriteNibble(byteVal, &bsOut);
		#else	
			WriteNibble(1, &bsOut);
		#endif	
	}
	bsOut.Write(m_pOfSyncData->byteHealth);
	if (action & 0x04)
		bsOut.Write(m_pOfSyncData->byteArmour);
	#ifdef NPC_SHOOTING_ENABLED
		if (m_pOfSyncData->IsAiming)
			bsOut.Write((uint8_t)0xa7);
		else
			bsOut.Write((uint8_t)0x03);
		if (m_pOfSyncData->IsAiming)
		{
			uint16_t wAimDirX, wAimDirY, wAimDirZ;
			wAimDirX = ConvertToUINT16_T(m_pOfSyncData->vecAimDir.X, 2 * (float)PI);
			wAimDirY = ConvertToUINT16_T(m_pOfSyncData->vecAimDir.Y, 2 * (float)PI);
			wAimDirZ = ConvertToUINT16_T(m_pOfSyncData->vecAimDir.Z, 2 * (float)PI);
			bsOut.Write(wAimDirX); bsOut.Write(wAimDirY);
			bsOut.Write(wAimDirZ);
			bsOut.Write(m_pOfSyncData->vecAimPos.X);
			bsOut.Write(m_pOfSyncData->vecAimPos.Y);
			bsOut.Write(m_pOfSyncData->vecAimPos.Z);
		}
	#else
		bsOut.Write((uint8_t)0x03);
	#endif
	peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, systemAddress, false);	
}


void WriteNibble(uint8_t nibble, RakNet::BitStream *bsOut)
{
	uint8_t bytearray[] = { nibble };
	bsOut->WriteBits(bytearray, 4);
}
void SetActionFlags(ONFOOT_SYNC_DATA* m_pOfSyncData, uint8_t* action)
{
	if (m_pOfSyncData->byteCurrentWeapon)(*action) |= 0x40;
	if (m_pOfSyncData->byteArmour)(*action) |= 0x04;
	if (m_pOfSyncData->vecSpeed.X != 0 || m_pOfSyncData->vecSpeed.Y != 0 ||
		m_pOfSyncData->vecSpeed.Z != 0)(*action) |= 0x01;
	if (m_pOfSyncData->dwKeys)(*action) |= 0x10;
	if (m_pOfSyncData->IsCrouching)(*action) |= 0x80;
}
//Set Flags for Weapon and Armour of player
void SetActionFlags(INCAR_SYNC_DATA* m_pInSyncData, uint8_t* action)
{
	if (m_pInSyncData->byteCurrentWeapon)(*action) |= 0x80;
	if (m_pInSyncData->bytePlayerArmour)(*action) |= 0x40;
}
//Set Flags for Health, Damage and Movement of Car
void SetTypeFlags(INCAR_SYNC_DATA* m_pInSyncData, uint8_t* type)
{
	if (m_pInSyncData->vecMoveSpeed.X != 0 || m_pInSyncData->vecMoveSpeed.Y != 0 ||
		m_pInSyncData->vecMoveSpeed.Z != 0)(*type) |= 0x1;
	if (m_pInSyncData->fCarHealth != 1000)(*type) |= 0x4;
	if (m_pInSyncData->dDamage != 0)(*type) |= 0x2;
}
//Set Flag for Quaternion Rotation
void SetRotationFlags(INCAR_SYNC_DATA* m_pInSyncData, uint8_t* flag)
{
	if (m_pInSyncData->quatRotation.X < 0)(*flag) |= 4;
	if (m_pInSyncData->quatRotation.Y < 0)(*flag) |= 2;
	if (m_pInSyncData->quatRotation.Z < 0)(*flag) |= 1;
	if (m_pInSyncData->quatRotation.W < 0)(*flag) |= 8;
}
void SetVehicleIDFlag(INCAR_SYNC_DATA* m_pIcSyncData, uint8_t* nibble, uint8_t* byte)
{
	if (m_pIcSyncData->VehicleID <= 0xFF)(*nibble) = (m_pIcSyncData->VehicleID & 3) << 2;
	else if (m_pIcSyncData->VehicleID <= 0x1FF)(*nibble) = ((m_pIcSyncData->VehicleID & 3) << 2)+1;
	else if (m_pIcSyncData->VehicleID <= 0x2FF)(*nibble) = ((m_pIcSyncData->VehicleID & 3) << 2)+2;
	else (*nibble) = ((m_pIcSyncData->VehicleID & 3) << 2)+3;
	(*byte)=m_pIcSyncData->VehicleID >> 2;
	if (m_pIcSyncData->dwKeys > 0xFFFF)(*byte) = (*byte) | (0x40);
}
void SendNPCUpdate()
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE);
	bsOut.Write(iNPC->anticheatID);
	uint8_t action = 0;
	if (npc->byteCurWeap != 0)action += 0x40;
	if (npc->m_byteArmour != 0)action += 0x04;
	bsOut.Write(action);
	bsOut.Write(npc->m_vecPos.X);
	bsOut.Write(npc->m_vecPos.Y);
	bsOut.Write(npc->m_vecPos.Z);
	float _angle = npc->m_fAngle;
	uint16_t angle= ConvertToUINT16_T(_angle, 2 * (float)PI);
	bsOut.Write(angle);
	if ((action & 64) == 64)
	{
		//NPC posses weapon
		bsOut.Write(npc->byteCurWeap);
		if (npc->byteCurWeap > 11)
			bsOut.Write((uint16_t)0x01);//Let ammo be 1
	}
	bsOut.Write(npc->m_byteHealth);
	if ((action & 4) == 4)
	{
		//NPC posses armour
		bsOut.Write(npc->m_byteArmour);
	}
	//Write the mystery byte
	bsOut.Write((uint8_t)0x03);
	//Send the packet
	peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
}	