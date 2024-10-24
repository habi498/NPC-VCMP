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
extern CVehiclePool* m_pVehiclePool;
void SetActionFlags(ONFOOT_SYNC_DATA* m_pOfSyncData, uint8_t* action, uint8_t* nibble);
void SetActionFlags(INCAR_SYNC_DATA* m_InSyncData, uint8_t* action);
void SetTypeFlags(INCAR_SYNC_DATA* m_InSyncData, uint8_t* type);
void SetRotationFlags(INCAR_SYNC_DATA* m_InSyncData, uint8_t* flag);
void SetVehicleIDFlag(INCAR_SYNC_DATA* m_pIcSyncData, uint8_t* nibble, uint8_t* byte);
uint8_t GetSlotIdFromWeaponId(uint8_t byteWeapon);
void CheckAction(ONFOOT_SYNC_DATA* m_pOfSyncData);
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
void WriteBit0(RakNet::BitStream* b)
{
	uint8_t byteArray[] = { 0 };
	b->WriteBits(byteArray, 1, false);
}
void WriteBit1(RakNet::BitStream* b)
{
	uint8_t byteArray[] = { 128 };
	b->WriteBits(byteArray, 1, false);
}
#ifdef _REL004
void SendNPCSyncData(INCAR_SYNC_DATA* m_pInSyncData, PacketPriority mPriority)
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_PLAYERUPDATE_DRIVER);
	bsOut.Write(iNPC->anticheatID);
	bsOut.Write(m_pInSyncData->byteCurrentWeapon);
	bsOut.Write(m_pInSyncData->bytePlayerArmour);
	bsOut.Write(m_pInSyncData->bytePlayerHealth);
	bsOut.Write(m_pInSyncData->wAmmo);
	bsOut.Write(npc->m_wVehicleId);
	bsOut.Write((uint16_t)m_pInSyncData->dwKeys);
	//Mystery bytes (two). needs investigation
	bsOut.Write((uint8_t)0);
	bsOut.Write((uint8_t)0xc8); //some times changes to 0x42

	bsOut.Write(m_pInSyncData->Turrety);
	bsOut.Write(m_pInSyncData->Turretx);

	bsOut.Write((uint8_t)0);//changes to 1 on water..
	bsOut.Write(m_pInSyncData->fCarHealth);
	bsOut.Write(m_pInSyncData->dDamage);
	bsOut.Write(m_pInSyncData->quatRotation.W);
	bsOut.Write(m_pInSyncData->quatRotation.Z);
	bsOut.Write(m_pInSyncData->quatRotation.Y);
	bsOut.Write(m_pInSyncData->quatRotation.X);
	bsOut.Write(m_pInSyncData->vecMoveSpeed.Z);
	bsOut.Write(m_pInSyncData->vecMoveSpeed.Y);
	bsOut.Write(m_pInSyncData->vecMoveSpeed.X);
	bsOut.Write(m_pInSyncData->vecPos.Z);
	bsOut.Write(m_pInSyncData->vecPos.Y);
	bsOut.Write(m_pInSyncData->vecPos.X);
	peer->Send(&bsOut, mPriority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
	npc->StoreInCarFullSyncData(m_pInSyncData);
}
#elif defined(_REL0471)
void SendNPCSyncData(INCAR_SYNC_DATA* m_pInSyncData, PacketPriority mPriority)
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_PLAYERUPDATE_DRIVER);
	bsOut.Write(iNPC->anticheatID);
	uint8_t action = 0;
	SetActionFlags(m_pInSyncData, &action);
	bsOut.Write(action);
	bsOut.Write(_byteswap_ushort((uint16_t)(m_pInSyncData->dwKeys & 0xFFFF)));//swapping is needed
	uint8_t nibble, byte;
	SetVehicleIDFlag(m_pInSyncData, &nibble, &byte);
	bsOut.Write(byte);
	WriteNibble(nibble, &bsOut);
	bsOut.Write(m_pInSyncData->bytePlayerHealth);
	if (action & IC_FLAG_WEAPON)
	{
		bsOut.Write(m_pInSyncData->byteCurrentWeapon);
		if (m_pInSyncData->byteCurrentWeapon > 11)
			bsOut.Write((WORD)m_pInSyncData->wAmmo);
		//bsOut.Write((uint16_t)npc->GetSlotAmmo(GetSlotId(npc->GetCurrentWeapon())));
	}
	if (action & IC_FLAG_ARMOUR)
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
	if (type & IC_TFLAG_SPEED)
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
	w_rotx = ConvertToUINT16_T(m_pInSyncData->quatRotation.X, -1);
	w_roty = ConvertToUINT16_T(m_pInSyncData->quatRotation.Y, -1);
	w_rotz = ConvertToUINT16_T(m_pInSyncData->quatRotation.Z, -1);
	bsOut.Write(w_rotx);
	bsOut.Write(w_roty); bsOut.Write(w_rotz);
	if (type & IC_TFLAG_DAMAGE)
		bsOut.Write(m_pInSyncData->dDamage);
	if (type & IC_TFLAG_CARHEALTH)
		bsOut.Write(m_pInSyncData->fCarHealth);
	if (bTurret)
	{
		uint16_t wHorizontal, wVertical;
		wHorizontal = ConvertToUINT16_T(m_pInSyncData->Turretx, 2 * (float)PI);
		wVertical = ConvertToUINT16_T(m_pInSyncData->Turrety, 2 * (float)PI);
		bsOut.Write(wHorizontal);
		bsOut.Write(wVertical);
	}
	//Mystery bytes. Might need investigation..
	bsOut.Write((uint8_t)0);
	bsOut.Write((uint8_t)0xc8);
	bsOut.Write((uint8_t)0);//Changes when vehicle is on water.?
	bsOut.Write((uint8_t)0);
	unsigned char* data = bsOut.GetData();
	uint32_t sourcelen = bsOut.GetNumberOfBytesUsed();
	uLongf destLen = compressBound(sourcelen - 1);//An upper bound, we do not compress message id 0x32
	unsigned char* cmpdata = (unsigned char*)malloc(destLen);
	if (cmpdata)
	{
		int c = compress2(cmpdata, (uLongf*)&destLen, data + 1, sourcelen - 1,
			Z_DEFAULT_COMPRESSION);
		if (c == Z_OK)
		{
			RakNet::BitStream bsNew;
			bsNew.Write(data[0]);//Write Message ID 0x32
			WriteBit0(&bsNew);//Write a zero bit
			//NOw write thelength of original data
			bsNew.Write(sourcelen - 1);
			WriteBit0(&bsNew);//Write a zero bit
			bsNew.Write((uint32_t)destLen);
			//for (int i = 0; i < destLen; i++)printf("%x ", cmpdata[i]);
			//printf("\n");
			for (int i = 0; i < destLen; i++)
				bsNew.Write(cmpdata[i]);
			peer->Send(&bsNew, mPriority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
		}
		else {
			printf("Error. An error occured during compressing data\n");
			exit(0);
		}
		free(cmpdata);
	}
	else
	{
		printf("Error: Memory allocation failed\n"); exit(0);
	}
	//peer->Send(&bsOut, mPriority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
	npc->StoreInCarFullSyncData(m_pInSyncData);
}

#else
void SendNPCSyncData(INCAR_SYNC_DATA* m_pInSyncData, PacketPriority mPriority )
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
	if (action & IC_FLAG_WEAPON)
	{
		bsOut.Write(m_pInSyncData->byteCurrentWeapon);
		if (m_pInSyncData->byteCurrentWeapon > 11)
			bsOut.Write((WORD)m_pInSyncData->wAmmo);
			//bsOut.Write((uint16_t)npc->GetSlotAmmo(GetSlotId(npc->GetCurrentWeapon())));
	}
	if (action & IC_FLAG_ARMOUR)
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
	if (type & IC_TFLAG_SPEED)
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
	if (type & IC_TFLAG_DAMAGE)
		bsOut.Write(m_pInSyncData->dDamage);
	if (type & IC_TFLAG_CARHEALTH)
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
	npc->StoreInCarFullSyncData(m_pInSyncData);
}
#endif
#ifdef _REL004
void SendNPCSyncData(ONFOOT_SYNC_DATA* m_pOfSyncData, PacketPriority priority)
{
	RakNet::BitStream bsOut;
	if (!m_pOfSyncData->IsPlayerUpdateAiming)
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE);
	else
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM);
	bsOut.Write(iNPC->anticheatID);
	uint8_t byteCrouching = 0;
	if (m_pOfSyncData->IsCrouching)
		byteCrouching = 2;
	bsOut.Write(byteCrouching);
	bsOut.Write(m_pOfSyncData->byteCurrentWeapon);
	CheckAction(m_pOfSyncData);
	if (m_pOfSyncData->IsPlayerUpdateAiming)
	{
		if (m_pOfSyncData->bIsReloading)
		{
			bsOut.Write((uint8_t)0x27);
		}
		else
		{
			bsOut.Write((uint8_t)0xa7);
		}
	}
	else
	{
		bsOut.Write((uint8_t)0x03);//magic. 0x33 for look behind
	}
	bsOut.Write(m_pOfSyncData->byteAction);
	bsOut.Write(m_pOfSyncData->byteArmour);
	bsOut.Write(m_pOfSyncData->byteHealth);
	bsOut.Write(m_pOfSyncData->wAmmo);
	bsOut.Write((uint16_t)0);
	bsOut.Write((uint16_t)m_pOfSyncData->dwKeys);
	bsOut.Write(m_pOfSyncData->vecSpeed.Z);
	bsOut.Write(m_pOfSyncData->vecSpeed.Y);
	bsOut.Write(m_pOfSyncData->vecSpeed.X);
	bsOut.Write(m_pOfSyncData->fAngle);
	bsOut.Write(m_pOfSyncData->vecPos.Z);
	bsOut.Write(m_pOfSyncData->vecPos.Y);
	bsOut.Write(m_pOfSyncData->vecPos.X);
	if (m_pOfSyncData->IsPlayerUpdateAiming)
	{
		bsOut.Write(m_pOfSyncData->vecAimPos.Z);
		bsOut.Write(m_pOfSyncData->vecAimPos.Y);
		bsOut.Write(m_pOfSyncData->vecAimPos.X);
		bsOut.Write(m_pOfSyncData->vecAimDir.Z);
		bsOut.Write(m_pOfSyncData->vecAimDir.Y);
		bsOut.Write(m_pOfSyncData->vecAimDir.X);
	}
	peer->Send(&bsOut, priority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
	npc->StoreOnFootFullSyncData(m_pOfSyncData);
}
#elif defined(_REL0470)
void SendNPCSyncData(ONFOOT_SYNC_DATA* m_pOfSyncData, PacketPriority priority)
{
	RakNet::BitStream bsOut;
	if (!m_pOfSyncData->IsPlayerUpdateAiming)
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE);
	else
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM);
	bsOut.Write(iNPC->anticheatID);
	uint8_t action = 0; uint8_t nibble = 0;
	SetActionFlags(m_pOfSyncData, &action, &nibble);

#ifdef NPC_SHOOTING_ENABLED
	bool reloading_weapon = false;
	if (m_pOfSyncData->IsPlayerUpdateAiming)
	{
		if ((m_pOfSyncData->dwKeys & 512) == 0)
		{
			//This means player is reloading weapon!
			//key_onfoot_fire is 576
			reloading_weapon = true;

			//07 Aug 2023, commented 0xd0. If armour this lead error
			//action = 0xd0;
			action |= OF_FLAG_EXTRA_NIBBLE;//in somecases, this is already 'OR'ed.
			nibble |= OF_FLAG_NIBBLE_RELOADING;
		}
	}
#endif
	bsOut.Write(action);
	if (nibble)
		WriteNibble(nibble, &bsOut);
	bsOut.Write(m_pOfSyncData->vecMatrixRight.Z);
	bsOut.Write(m_pOfSyncData->vecMatrixRight.Y);
	bsOut.Write(m_pOfSyncData->vecMatrixRight.X);
	bsOut.Write(m_pOfSyncData->vecMatrixUp.X);
	bsOut.Write(m_pOfSyncData->vecMatrixUp.Z);
	bsOut.Write(m_pOfSyncData->vecMatrixUp.Y);
	bsOut.Write(m_pOfSyncData->vecMatrixPosition.Y);
	bsOut.Write(m_pOfSyncData->vecMatrixPosition.X);//does not matter
	bsOut.Write(m_pOfSyncData->vecMatrixPosition.Z);//does not matter
	bsOut.Write(m_pOfSyncData->vecPos.Z);
	bsOut.Write(m_pOfSyncData->vecPos.Y);
	bsOut.Write(m_pOfSyncData->vecPos.X);
	bsOut.Write(m_pOfSyncData->fAngle);
	bsOut.Write((uint32_t)0);//UNKNOWN
	bsOut.Write(m_pOfSyncData->vecSpeed.Z);
	bsOut.Write(m_pOfSyncData->vecSpeed.Y);
	bsOut.Write(m_pOfSyncData->vecSpeed.X);
	bsOut.Write((float)0);
	bsOut.Write((float)0);
	bsOut.Write((float)0);
	if (action & OF_FLAG_WEAPON)
	{
		bsOut.Write(m_pOfSyncData->byteCurrentWeapon);
		if (m_pOfSyncData->byteCurrentWeapon > 11)
			bsOut.Write((WORD)(m_pOfSyncData->wAmmo));//ammo
	}
	if (action & OF_FLAG_KEYS_OR_ACTION)
	{
		CheckAction(m_pOfSyncData);
		uint8_t keybyte1 = m_pOfSyncData->dwKeys & 0xFF;
		uint8_t keybyte2 = (m_pOfSyncData->dwKeys >> 8) & 0xFF;
		bsOut.Write(keybyte1);
		bsOut.Write(keybyte2);
		uint16_t value = 0;
		if (m_pOfSyncData->dwKeys > 0xFFFF)
			value |= 0x80;
		value |= m_pOfSyncData->byteAction;

		WriteNibble((value >> 8) & 15, &bsOut);
		bsOut.Write((uint8_t)(value & 0xFF));
	}
	if (!(action & OF_FLAG_NOHEALTH))
	{
		bsOut.Write(m_pOfSyncData->byteHealth);
		if (action & OF_FLAG_ARMOUR)
			bsOut.Write(m_pOfSyncData->byteArmour);
#ifdef NPC_SHOOTING_ENABLED
		if (m_pOfSyncData->IsPlayerUpdateAiming)
			bsOut.Write((uint8_t)0xa7);
		else
			bsOut.Write((uint8_t)0x03);
		if (m_pOfSyncData->IsPlayerUpdateAiming)
		{
			bsOut.Write((float)0);
			bsOut.Write((float)0);
			bsOut.Write((float)0);
			bsOut.Write(m_pOfSyncData->vecAimDir.Z);
			bsOut.Write(m_pOfSyncData->vecAimDir.Y);
			bsOut.Write(m_pOfSyncData->vecAimDir.X);
			bsOut.Write(m_pOfSyncData->vecAimPos.Z);
			bsOut.Write(m_pOfSyncData->vecAimPos.Y);
			bsOut.Write(m_pOfSyncData->vecAimPos.X);
		}
#else
		bsOut.Write((uint8_t)0x03);
#endif
	}
#ifdef _REL0471
	unsigned char* data=bsOut.GetData();
	uint32_t sourcelen = bsOut.GetNumberOfBytesUsed() ;
	printf("Original packet was: ");
	for (int i = 0; i < sourcelen; i++)
	{
		printf("%x ", data[i]);
	}
	printf("\n");
	uint32_t destLen = compressBound(sourcelen-1);//An upper bound, we do not compress message id 0x30
	unsigned char* cmpdata = (unsigned char*)malloc(destLen);
	if (cmpdata)
	{
		int c=compress2(cmpdata, (uLongf*)&destLen, data+1, sourcelen-1,
			Z_DEFAULT_COMPRESSION);
		if (c == Z_OK)
		{
			RakNet::BitStream bsNew;
			bsNew.Write(data[0]);//Write Message ID 0x30 or 0x31
			WriteBit0(&bsNew);//Write a zero bit
			//NOw write thelength of original data
			printf("sourcelen adjusted is %u. ", sourcelen - 1);
			bsNew.Write(sourcelen-1);
			WriteBit0(&bsNew);//Write a zero bit
			//Write length of upcoming compressed data
			printf("destLen is %u\n", destLen);
			bsNew.Write(destLen);
			for (int i = 0; i < destLen; i++)printf("%x ", cmpdata[i]);
			printf("\n");
			for (int i = 0; i < destLen; i++) 
				bsNew.Write(cmpdata[i]);
			peer->Send(&bsNew, priority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
		}
		else {
			printf("Error. An error occured during compressing data\n");
			exit(0);
		}
		free(cmpdata);
	}
	else
	{
		printf("Error: Memory allocation failed\n"); exit(0);
	}
#else
	peer->Send(&bsOut, priority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
#endif
	npc->StoreOnFootFullSyncData(m_pOfSyncData);//v1.4
}
#elif defined(_REL0471)
void SendNPCSyncData(ONFOOT_SYNC_DATA* m_pOfSyncData, PacketPriority priority)
{
	RakNet::BitStream bsOut;
	if (!m_pOfSyncData->IsPlayerUpdateAiming)
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE);
	else
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM);
	bsOut.Write(iNPC->anticheatID);
	uint8_t action = 0; uint8_t nibble = 0;
	SetActionFlags(m_pOfSyncData, &action, &nibble);

#ifdef NPC_SHOOTING_ENABLED
	bool reloading_weapon = false;
	if (m_pOfSyncData->IsPlayerUpdateAiming)
	{
		if ((m_pOfSyncData->dwKeys & 512) == 0)
		{
			//This means player is reloading weapon!
			//key_onfoot_fire is 576
			reloading_weapon = true;

			//07 Aug 2023, commented 0xd0. If armour this lead error
			//action = 0xd0;
			action |= OF_FLAG_EXTRA_NIBBLE;//in somecases, this is already 'OR'ed.
			nibble |= OF_FLAG_NIBBLE_RELOADING;
		}
	}
#endif
	bsOut.Write(action);
	if (nibble)
		WriteNibble(nibble, &bsOut);

	bsOut.Write(m_pOfSyncData->vecPos.X);
	bsOut.Write(m_pOfSyncData->vecPos.Y);
	bsOut.Write(m_pOfSyncData->vecPos.Z);
	WORD angle = ConvertToUINT16_T(m_pOfSyncData->fAngle, 2 * (float)PI);
	bsOut.Write(angle);
	float fUnknown = 4.0 / 3;
	bsOut.Write(fUnknown);
	if (action & OF_FLAG_SPEED)
	{
		VECTOR vecSpeed = m_pOfSyncData->vecSpeed;
		uint16_t speedx = ConvertToUINT16_T(vecSpeed.X, 20.0);
		uint16_t speedy = ConvertToUINT16_T(vecSpeed.Y, 20.0);
		uint16_t speedz = ConvertToUINT16_T(vecSpeed.Z, 20.0);
		bsOut.Write(speedx); bsOut.Write(speedy); bsOut.Write(speedz);
	}
	if (action & OF_FLAG_WEAPON)
	{
		bsOut.Write(m_pOfSyncData->byteCurrentWeapon);
		if (m_pOfSyncData->byteCurrentWeapon > 11)
			bsOut.Write((WORD)(m_pOfSyncData->wAmmo));//ammo
	}
	if (action & OF_FLAG_KEYS_OR_ACTION)
	{
		CheckAction(m_pOfSyncData);
		uint8_t keybyte1 = m_pOfSyncData->dwKeys & 0xFF;
		uint8_t keybyte2 = (m_pOfSyncData->dwKeys >> 8) & 0xFF;
		bsOut.Write(keybyte1);
		bsOut.Write(keybyte2);
		uint16_t value = 0;
		if (m_pOfSyncData->dwKeys > 0xFFFF)
			value |= 0x80;
		value |= m_pOfSyncData->byteAction;

		WriteNibble((value >> 8) & 15, &bsOut);
		bsOut.Write((uint8_t)(value & 0xFF));


	}
	if (!(action & OF_FLAG_NOHEALTH))
	{
		bsOut.Write(m_pOfSyncData->byteHealth);
		if (action & OF_FLAG_ARMOUR)
			bsOut.Write(m_pOfSyncData->byteArmour);
#ifdef NPC_SHOOTING_ENABLED
		if (m_pOfSyncData->IsPlayerUpdateAiming)
			bsOut.Write((uint8_t)0xa7);
		else 
		{
			/*if (m_pOfSyncData->byteAction == 0x11)
				bsOut.Write((uint8_t)0x07);
			else*/
				bsOut.Write((uint8_t)0x27);
		}
			
		if (m_pOfSyncData->IsPlayerUpdateAiming)
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
	}
	unsigned char* data = bsOut.GetData();
	/*for (int i = 0; i < bsOut.GetNumberOfBytesUsed(); i++)
	{
		printf("%0.2x ", data[i]);
	}
	printf("\n");*/
	uint32_t sourcelen = bsOut.GetNumberOfBytesUsed();
	uLongf destLen = compressBound(sourcelen - 1);//An upper bound, we do not compress message id 0x30
	unsigned char* cmpdata = (unsigned char*)malloc(destLen);
	if (cmpdata)
	{
		int c = compress2(cmpdata, (uLongf*)&destLen, data + 1, sourcelen - 1,
			Z_DEFAULT_COMPRESSION);
		if (c == Z_OK)
		{
			RakNet::BitStream bsNew;
			bsNew.Write(data[0]);//Write Message ID 0x30 or 0x31
			WriteBit0(&bsNew);//Write a zero bit
			//NOw write thelength of original data
			//printf("sourcelen adjusted is %u. ", sourcelen - 1);
			bsNew.Write(sourcelen - 1);
			WriteBit0(&bsNew);//Write a zero bit
			//Write length of upcoming compressed data
			//printf("destLen is %u\n", destLen);
			bsNew.Write((uint32_t)destLen);
			//for (int i = 0; i < destLen; i++)printf("%x ", cmpdata[i]);
			//printf("\n");
			for (int i = 0; i < destLen; i++)
				bsNew.Write(cmpdata[i]);
			peer->Send(&bsNew, priority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
		}
		else {
			printf("Error. An error occured during compressing data\n");
			exit(0);
		}
		free(cmpdata);
	}
	else
	{
		printf("Error: Memory allocation failed\n"); exit(0);
	}
	//peer->Send(&bsOut, priority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
	npc->StoreOnFootFullSyncData(m_pOfSyncData);//v1.4
}
#else
void SendNPCSyncData(ONFOOT_SYNC_DATA* m_pOfSyncData, PacketPriority priority)
{
	RakNet::BitStream bsOut;
	if(!m_pOfSyncData->IsPlayerUpdateAiming)
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE);
	else
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM);
	bsOut.Write(iNPC->anticheatID);
	uint8_t action = 0; uint8_t nibble = 0;
	SetActionFlags(m_pOfSyncData, &action,&nibble);
	
	#ifdef NPC_SHOOTING_ENABLED
		bool reloading_weapon = false;
		if (m_pOfSyncData->IsPlayerUpdateAiming)
		{
			if ((m_pOfSyncData->dwKeys & 512) == 0)
			{
				//This means player is reloading weapon!
				//key_onfoot_fire is 576
				reloading_weapon = true;
				
				//07 Aug 2023, commented 0xd0. If armour this lead error
				//action = 0xd0;
				action |= OF_FLAG_EXTRA_NIBBLE;//in somecases, this is already 'OR'ed.
				nibble |= OF_FLAG_NIBBLE_RELOADING;
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
	if (action & OF_FLAG_SPEED)
	{
		VECTOR vecSpeed = m_pOfSyncData->vecSpeed;
		uint16_t speedx= ConvertToUINT16_T(vecSpeed.X, 20.0);
		uint16_t speedy= ConvertToUINT16_T(vecSpeed.Y, 20.0);
		uint16_t speedz= ConvertToUINT16_T(vecSpeed.Z, 20.0);
		bsOut.Write(speedx); bsOut.Write(speedy); bsOut.Write(speedz);
	}
	if (action & OF_FLAG_WEAPON)
	{
		bsOut.Write(m_pOfSyncData->byteCurrentWeapon);
		if (m_pOfSyncData->byteCurrentWeapon > 11)	
			bsOut.Write((WORD)(m_pOfSyncData->wAmmo));//ammo
	}
	if (action & OF_FLAG_KEYS_OR_ACTION)
	{
		CheckAction(m_pOfSyncData);
		uint8_t keybyte1 = m_pOfSyncData->dwKeys & 0xFF;
		uint8_t keybyte2 = (m_pOfSyncData->dwKeys >> 8) & 0xFF;
		bsOut.Write(keybyte1);
		bsOut.Write(keybyte2);
		uint16_t value = 0;
		if (m_pOfSyncData->dwKeys > 0xFFFF)
			value |= 0x80;
		value |= m_pOfSyncData->byteAction;
		
		WriteNibble((value >> 8) & 15, &bsOut);
		bsOut.Write((uint8_t)(value & 0xFF));

		
	}
	if (!(action & OF_FLAG_NOHEALTH))
	{
		bsOut.Write(m_pOfSyncData->byteHealth);
		if (action & OF_FLAG_ARMOUR)
			bsOut.Write(m_pOfSyncData->byteArmour);
	#ifdef NPC_SHOOTING_ENABLED
		if (m_pOfSyncData->IsPlayerUpdateAiming)
			bsOut.Write((uint8_t)0xa7);
		else
			bsOut.Write((uint8_t)0x03);
		if (m_pOfSyncData->IsPlayerUpdateAiming)
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
	}
	peer->Send(&bsOut, priority, UNRELIABLE_SEQUENCED, 0, systemAddress, false);	
	npc->StoreOnFootFullSyncData(m_pOfSyncData);//v1.4
}
#endif

void WriteNibble(uint8_t nibble, RakNet::BitStream *bsOut)
{
	const uint8_t bytearray[] = { nibble };
	bsOut->WriteBits(bytearray, 4);
}
#ifndef _REL004
void SetActionFlags(ONFOOT_SYNC_DATA* m_pOfSyncData, uint8_t* action, uint8_t* nibble)
{
	if (m_pOfSyncData->byteCurrentWeapon)(*action) |= OF_FLAG_WEAPON;
	if (m_pOfSyncData->byteArmour)(*action) |= OF_FLAG_ARMOUR;
#ifndef _REL0470
	//REL0470 writes speed always
	if (m_pOfSyncData->vecSpeed.X != 0 || m_pOfSyncData->vecSpeed.Y != 0 ||
		m_pOfSyncData->vecSpeed.Z != 0)(*action) |= OF_FLAG_SPEED;
#endif
	if (m_pOfSyncData->dwKeys || m_pOfSyncData->byteAction>1)(*action) |= OF_FLAG_KEYS_OR_ACTION;
	if (m_pOfSyncData->IsCrouching)
	{

		(*action) |= OF_FLAG_EXTRA_NIBBLE;
		(*nibble) |= OF_FLAG_NIBBLE_CROUCHING;
	}
	if (m_pOfSyncData->byteHealth <= 0)(*action) |= OF_FLAG_NOHEALTH;
}
#endif
//Set Flags for Weapon and Armour of player
void SetActionFlags(INCAR_SYNC_DATA* m_pInSyncData, uint8_t* action)
{
	if (m_pInSyncData->byteCurrentWeapon)(*action) |= IC_FLAG_WEAPON;
	if (m_pInSyncData->bytePlayerArmour)(*action) |= IC_FLAG_ARMOUR;
}
//Set Flags for Health, Damage and Movement of Car
void SetTypeFlags(INCAR_SYNC_DATA* m_pInSyncData, uint8_t* type)
{
	if (m_pInSyncData->vecMoveSpeed.X != 0 || m_pInSyncData->vecMoveSpeed.Y != 0 ||
		m_pInSyncData->vecMoveSpeed.Z != 0)(*type) |= IC_TFLAG_SPEED;
	if (m_pInSyncData->fCarHealth != 1000)(*type) |= IC_TFLAG_CARHEALTH;
	if (m_pInSyncData->dDamage != 0)(*type) |= IC_TFLAG_DAMAGE;
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
//For npc.Action
void CheckAction(ONFOOT_SYNC_DATA* m_pOfSyncData)
{
	/*v1.7 beta 2. setting npc.Action .*/
		//new model(most probably work)
		/*First Calculate byteAction if not provided.*/
		//If reloading, aiming or attacking (0x10 and 0x11), set byteAction
	if ((m_pOfSyncData->dwKeys & 0x40) && !(m_pOfSyncData->dwKeys & 512))
		m_pOfSyncData->byteAction = 0x01; //when reloading keys=64, firing 572
	else
	{
		if ((m_pOfSyncData->byteCurrentWeapon == 26
			|| m_pOfSyncData->byteCurrentWeapon == 27
			|| m_pOfSyncData->byteCurrentWeapon == 28
			|| m_pOfSyncData->byteCurrentWeapon == 29
			|| m_pOfSyncData->byteCurrentWeapon == 30
			|| m_pOfSyncData->byteCurrentWeapon == 32) &&
			m_pOfSyncData->dwKeys & 0x01) //aiming
		{
			m_pOfSyncData->byteAction = 0x0c;
		}
		else if (m_pOfSyncData->dwKeys & 0x200) //attacking
		{
			if (m_pOfSyncData->byteCurrentWeapon <= 10)//melee weapons
				m_pOfSyncData->byteAction = 0x11;
			else
				m_pOfSyncData->byteAction = 0x10;
		}
		else
		{
			//If byteAction <=1, then OF_FLAG_KEYS_OR_ACTION will not be set 
			//unless dwKeys is not 0. And when OF_FLAG_KEYS_OR_ACTION is not set,
			//server will take action to be 0x1 (normal).
			if (m_pOfSyncData->dwKeys && m_pOfSyncData->byteAction == 0)
				m_pOfSyncData->byteAction = 0x01;//set it to normal.
		}
	}

}

void SendNPCOfSyncDataLV(PacketPriority prioty) //with existing values, send a packet. Normally to update health or angle
{
	ONFOOT_SYNC_DATA* m_pOfSyncData;
	m_pOfSyncData = npc->GetONFOOT_SYNC_DATA();
	m_pOfSyncData->byteHealth = npc->m_byteHealth;
	m_pOfSyncData->byteArmour = npc->m_byteArmour;
	m_pOfSyncData->byteCurrentWeapon = npc->GetCurrentWeapon();
	m_pOfSyncData->wAmmo = npc->GetCurrentWeaponAmmo();
	m_pOfSyncData->dwKeys = npc->GetKeys();
	m_pOfSyncData->fAngle = npc->m_fAngle;
	m_pOfSyncData->vecPos = npc->m_vecPos;
	//printf("npc->m_byteHealth is %d\n", npc->m_byteHealth);
	//printf("npcofsyncdatalv calling sendnpcsyncdata...\n");
	SendNPCSyncData(m_pOfSyncData, prioty);
}
void SendNPCIcSyncDataLV(PacketPriority prioty) //with existing values, send a packet. Normally to update health or angle
{
	INCAR_SYNC_DATA* m_pIcSyncData;
	if (npc->m_wVehicleId)
	{
		m_pIcSyncData = npc->GetINCAR_SYNC_DATA();
		m_pIcSyncData->bytePlayerHealth = npc->m_byteHealth;
		m_pIcSyncData->bytePlayerArmour = npc->m_byteArmour;
		m_pIcSyncData->byteCurrentWeapon = npc->GetCurrentWeapon();
		m_pIcSyncData->wAmmo = npc->GetCurrentWeaponAmmo();
		m_pIcSyncData->dwKeys = npc->GetKeys();
		m_pIcSyncData->vecPos = npc->m_vecPos;
		CVehicle* vehicle = m_pVehiclePool->GetAt(npc->m_wVehicleId);
		if (vehicle)
		{
			m_pIcSyncData->fCarHealth = vehicle->GetHealth();
			m_pIcSyncData->dDamage = vehicle->GetDamage();
			m_pIcSyncData->quatRotation = vehicle->GetRotation();
		}
		//In case player was put in new vehicle
		m_pIcSyncData->VehicleID = npc->m_wVehicleId;
		SendNPCSyncData(m_pIcSyncData, prioty);
	}
}
#ifdef _REL004
void SendPassengerSyncData()
{
	if (npc->m_wVehicleId != 0 && npc->m_byteSeatId != -1)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_PLAYERUPDATE_PASSENGER);
		bsOut.Write(iNPC->anticheatID);
		bsOut.Write(npc->m_byteSeatId);
		bsOut.Write(npc->m_byteArmour);
		bsOut.Write(npc->m_byteHealth);
		bsOut.Write(npc->m_wVehicleId);
		peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
	}
}
#elif defined(_REL0471)
void SendPassengerSyncData()
{
	if (npc->m_wVehicleId != 0 && npc->m_byteSeatId != -1)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_PLAYERUPDATE_PASSENGER);
		bsOut.Write(iNPC->anticheatID);
		bsOut.Write((uint8_t)(npc->m_wVehicleId % 256));
		uint8_t byte = (npc->m_wVehicleId / 256) << 6;
		uint8_t health = npc->m_byteHealth;
		byte += health / 4;
		bsOut.Write(byte);

		if (npc->m_byteArmour)
		{
			uint8_t byte2 = (health % 4) * 4 << 4;
			byte2 += 32;
			uint8_t armour = npc->m_byteArmour;
			byte2 += armour / 8;
			bsOut.Write(byte2);
			WriteNibble((armour % 8) * 2, &bsOut);
			WriteNibble(npc->m_byteSeatId * 4, &bsOut);
		}
		else
		{
			uint8_t byte2 = (health % 4) * 4 << 4;
			byte2 += npc->m_byteSeatId * 4;
			bsOut.Write(byte2);
		}
		unsigned char* data = bsOut.GetData();
		uint32_t sourcelen = bsOut.GetNumberOfBytesUsed();
		uLongf destLen = compressBound(sourcelen - 1);//An upper bound, we do not compress message id 0x30
		unsigned char* cmpdata = (unsigned char*)malloc(destLen);
		if (cmpdata)
		{
			int c = compress2(cmpdata, (uLongf*)&destLen, data + 1, sourcelen - 1,
				Z_DEFAULT_COMPRESSION);
			if (c == Z_OK)
			{
				RakNet::BitStream bsNew;
				bsNew.Write(data[0]);//Write Message ID 0x33
				WriteBit0(&bsNew);//Write a zero bit
				//Now write thelength of original data
				bsNew.Write(sourcelen - 1);
				WriteBit0(&bsNew);//Write a zero bit
				//Write length of upcoming compressed data
				bsNew.Write((uint32_t)destLen);
				for (int i = 0; i < destLen; i++)
					bsNew.Write(cmpdata[i]);
				peer->Send(&bsNew, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
			}
			else {
				printf("Error. An error occured during compressing data\n");
				exit(0);
			}
			free(cmpdata);
		}
		else
		{
			printf("Error: Memory allocation failed\n"); 
			exit(0);
		}
	}
}
#else
void SendPassengerSyncData()
{
	if (npc->m_wVehicleId != 0 && npc->m_byteSeatId != -1)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_PLAYERUPDATE_PASSENGER);
		bsOut.Write(iNPC->anticheatID);
		bsOut.Write((uint8_t)(npc->m_wVehicleId % 256));
		uint8_t byte = ( npc->m_wVehicleId / 256 ) << 6;
		uint8_t health = npc->m_byteHealth;
		byte += health / 4;
		bsOut.Write(byte);
		
		if (npc->m_byteArmour)
		{
			uint8_t byte2 = (health % 4)*4 << 4; 
			byte2 += 32;
			uint8_t armour = npc->m_byteArmour;
			byte2 += armour / 8;
			bsOut.Write(byte2);
			WriteNibble((armour%8)*2, &bsOut);
			WriteNibble(npc->m_byteSeatId * 4, &bsOut);
		}
		else
		{
			uint8_t byte2 = (health % 4)*4 << 4;
			byte2 += npc->m_byteSeatId * 4;
			bsOut.Write(byte2);
		}
		peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
	}
}
#endif