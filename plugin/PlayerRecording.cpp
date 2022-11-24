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
#include "SQMain.h"
extern CPlayerPool* m_pPlayerPool;
//Return 1: The function was executed successfully.
//Return 0: The function failed to execute. Either the 
//specified player does not exist or file creation failed.
//.rec extension is automatically added
bool StartRecordingPlayerData(int32_t playerId, uint8_t recordtype, std::string recordname)
{
	std::string filename = std::string(RECDIR + std::string("/") + recordname + std::string(".rec"));
	if (!m_pPlayerPool->GetSlotState(playerId))
		return 0;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	bool success=m_pPlayer->Start(recordtype, filename);
	return success;
}
//Return 1:The function executed successfully
//0: The function failed to execute. The player might not exist.
//Or the recording is not in progress
bool StopRecordingPlayerData(int32_t playerId)
{
	if (!m_pPlayerPool->GetSlotState(playerId))
		return 0;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (m_pPlayer->IsRecording())
	{
		m_pPlayer->Finish();
		return true;
	}
	return false;
}
void CPlayer::ProcessUpdate2(PluginFuncs* VCMP, vcmpPlayerUpdate updateType)
{
	uint8_t playerId = this->m_PlayerID;
	if ((this->m_RecordingType == PLAYER_RECORDING_TYPE_ONFOOT
		&& updateType > vcmpPlayerUpdateAiming) ||
		(this->m_RecordingType == PLAYER_RECORDING_TYPE_DRIVER
			&& updateType != vcmpPlayerUpdateDriver))
	{
		this->Abort();
		return;
	}
	if (updateType == vcmpPlayerUpdateAiming ||
		updateType == vcmpPlayerUpdateNormal)
	{
		ONFOOT_DATABLOCK m_pOfDatablock;
		//Write Time first
		DWORD time = GetTickCount();
		m_pOfDatablock.time = time;
		ONFOOT_SYNC_DATA m_pOfSynDat;
		m_pOfSynDat.IsPlayerUpdateAiming = (updateType == vcmpPlayerUpdateAiming) ? 1 : 0;
		VCMP->GetPlayerSpeed(playerId, &m_pOfSynDat.vecSpeed.X,
			&m_pOfSynDat.vecSpeed.Y, &m_pOfSynDat.vecSpeed.Z);
		m_pOfSynDat.dwKeys = VCMP->GetPlayerGameKeys(playerId);
		m_pOfSynDat.byteCurrentWeapon = VCMP->GetPlayerWeapon(playerId);
		m_pOfSynDat.byteArmour = (BYTE)VCMP->GetPlayerArmour(playerId);
		m_pOfSynDat.IsCrouching = (BOOL)VCMP->IsPlayerCrouching(playerId);
		m_pOfSynDat.byteHealth = (BYTE)VCMP->GetPlayerHealth(playerId);
		m_pOfSynDat.fAngle = (FLOAT)VCMP->GetPlayerHeading(playerId);
		VCMP->GetPlayerPosition(playerId, &m_pOfSynDat.vecPos.X,
			&m_pOfSynDat.vecPos.Y, &m_pOfSynDat.vecPos.Z);
		VCMP->GetPlayerAimDirection(playerId, &m_pOfSynDat.vecAimDir.X,
			&m_pOfSynDat.vecAimDir.Y, &m_pOfSynDat.vecAimDir.Z);
		VCMP->GetPlayerAimPosition(playerId, &m_pOfSynDat.vecAimPos.X,
			&m_pOfSynDat.vecAimPos.Y, &m_pOfSynDat.vecAimPos.Z);
		m_pOfDatablock.m_pOfSyncData = m_pOfSynDat;
		size_t t = fwrite((void*)&m_pOfDatablock, sizeof(m_pOfDatablock), 1, this->pFile);
	}
	else if (updateType == vcmpPlayerUpdateDriver)
	{
		WORD vehicleId = VCMP->GetPlayerVehicleId(playerId);
		DWORD time = GetTickCount();
		INCAR_DATABLOCK m_pIcDatablock;
		m_pIcDatablock.time = time;
		INCAR_SYNC_DATA m_pIcSyncData;
		m_pIcSyncData.byteCurrentWeapon = (BYTE)VCMP->GetPlayerWeapon(playerId);
		m_pIcSyncData.bytePlayerArmour = (BYTE)VCMP->GetPlayerArmour(playerId);
		m_pIcSyncData.bytePlayerHealth = (BYTE)VCMP->GetPlayerHealth(playerId);
		m_pIcSyncData.dDamage = VCMP->GetVehicleDamageData(vehicleId);
		m_pIcSyncData.fCarHealth = VCMP->GetVehicleHealth(vehicleId);
		VCMP->GetVehicleTurretRotation(vehicleId, &m_pIcSyncData.Turretx, &m_pIcSyncData.Turrety);
		VCMP->GetVehicleSpeed(vehicleId, &m_pIcSyncData.vecMoveSpeed.X,
			&m_pIcSyncData.vecMoveSpeed.Y, &m_pIcSyncData.vecMoveSpeed.Z, 0);
		VCMP->GetVehiclePosition(vehicleId, &m_pIcSyncData.vecPos.X,
			&m_pIcSyncData.vecPos.Y, &m_pIcSyncData.vecPos.Z);
		VCMP->GetVehicleRotation(vehicleId, &m_pIcSyncData.quatRotation.X,
			&m_pIcSyncData.quatRotation.Y, &m_pIcSyncData.quatRotation.Z,&m_pIcSyncData.quatRotation.W);
		m_pIcSyncData.VehicleID = vehicleId;
		m_pIcSyncData.dwKeys = VCMP->GetPlayerGameKeys(playerId);
		m_pIcDatablock.m_pIcSyncData = m_pIcSyncData;
		fwrite((void*)&m_pIcDatablock, sizeof(m_pIcDatablock), 1, this->pFile);
	}
}
void CPlayer::ProcessUpdate(PluginFuncs* VCMP, vcmpPlayerUpdate updateType)
{
	uint8_t playerId = this->m_PlayerID;
	if ((this->m_RecordingType == PLAYER_RECORDING_TYPE_ONFOOT
		&& updateType > vcmpPlayerUpdateAiming) ||
		(this->m_RecordingType == PLAYER_RECORDING_TYPE_DRIVER
			&& updateType != vcmpPlayerUpdateDriver))
	{
		this->Abort();
		return;
	}
	if (updateType == vcmpPlayerUpdateAiming ||
		updateType == vcmpPlayerUpdateNormal)
	{
			//Write Time first
			DWORD time = GetTickCount();
			size_t t = fwrite((void*)&time, sizeof(DWORD), 1, this->pFile);
			//Write Update type. Normal or Aiming
			fwrite(&updateType, sizeof(char), 1, this->pFile);
			//Write Action type. THis is a little complicated.
			//0x00 Normal. 0x01 directional velocity.
			//0x10 game keys pressed. 0x40 posses weapon
			//0x04 have armour. and 0x80 crouch.
			uint8_t action = 0x0;
			float u, v, w;
			VCMP->GetPlayerSpeed(playerId, &u, &v, &w);
			if (u != 0 || v != 0 || w != 0)
				action += 0x01;
			uint32_t keys = VCMP->GetPlayerGameKeys(playerId);
			if (keys != 0)
				action += 0x10;
			uint8_t weapon = (uint8_t)VCMP->GetPlayerWeapon(playerId);
			if (weapon != 0)
				action += 0x40;
			uint8_t armour = (uint8_t)VCMP->GetPlayerArmour(playerId);
			if (armour != 0)
				action += 0x04;
			bool crouching = VCMP->IsPlayerCrouching(playerId);
			if (crouching)
				action += 0x80;
			fwrite(&action, sizeof(action), 1, this->pFile);
			float x, y, z;
			VCMP->GetPlayerPosition(playerId, &x, &y, &z);
			fwrite(&x, sizeof(x), 1, this->pFile);
			fwrite(&y, sizeof(y), 1, this->pFile);
			fwrite(&z, sizeof(z), 1, this->pFile);
			float _angle;
			_angle = VCMP->GetPlayerHeading(playerId);
			uint16_t angle = ConvertToUINT16_T(_angle, 2 * (float)PI);
			fwrite(&angle, sizeof(angle), 1, this->pFile);
			fwrite(&weapon, sizeof(weapon), 1, this->pFile);
			uint8_t health = (uint8_t)VCMP->GetPlayerHealth(playerId);
			fwrite(&health, sizeof(health), 1, this->pFile);
			fwrite(&armour, sizeof(armour), 1, this->pFile);
			uint16_t p, q, r;
			p = ConvertToUINT16_T(u, 20.0);
			q = ConvertToUINT16_T(v, 20.0);
			r = ConvertToUINT16_T(w, 20.0);
			fwrite(&p, sizeof(p), 1, this->pFile);
			fwrite(&q, sizeof(q), 1, this->pFile);
			fwrite(&r, sizeof(r), 1, this->pFile);
			fwrite(&keys, sizeof(keys), 1, this->pFile);

			if (updateType == vcmpPlayerUpdateAiming)
			{
				float AimX, AimY, AimZ;
				VCMP->GetPlayerAimDirection(playerId, &AimX, &AimY, &AimZ);

				uint16_t aimx = ConvertToUINT16_T(AimX, 2 * (float)PI);
				uint16_t aimy = ConvertToUINT16_T(AimY, 2 * (float)PI);
				uint16_t aimz = ConvertToUINT16_T(AimZ, 2 * (float)PI);
				fwrite(&aimx, sizeof(aimx), 1, this->pFile);
				fwrite(&aimy, sizeof(aimy), 1, this->pFile);
				fwrite(&aimz, sizeof(aimz), 1, this->pFile);

				float DirX, DirY, DirZ;
				VCMP->GetPlayerAimPosition(playerId, &DirX, &DirY, &DirZ);
				fwrite(&DirX, sizeof(DirX), 1, this->pFile);
				fwrite(&DirY, sizeof(DirY), 1, this->pFile);
				fwrite(&DirZ, sizeof(DirZ), 1, this->pFile);
			}
			else
			{
				uint32_t i = 0; uint16_t j = 0;
				//Just write 18 bytes.
				fwrite(&i, sizeof(i), 1, this->pFile);
				fwrite(&i, sizeof(i), 1, this->pFile);
				fwrite(&i, sizeof(i), 1, this->pFile);
				fwrite(&i, sizeof(i), 1, this->pFile);
				fwrite(&j, sizeof(j), 1, this->pFile);
			}
		}
	
	if (updateType == vcmpPlayerUpdateDriver)
	{
		//Write Time first
		DWORD time = GetTickCount();
		size_t t = fwrite((void*)&time, sizeof(DWORD), 1, this->pFile);
		//Write Update type. 
		fwrite(&updateType, sizeof(char), 1, this->pFile);
		uint16_t vehicle = VCMP->GetPlayerVehicleId(playerId);
		uint8_t action = 0x0; uint8_t armour;
		armour = (uint8_t)VCMP->GetPlayerArmour(playerId);
		uint8_t weapon = (uint8_t)VCMP->GetPlayerWeapon(playerId);
		uint32_t keys = VCMP->GetPlayerGameKeys(playerId);
		uint8_t health = (uint8_t)VCMP->GetPlayerHealth(playerId);
		if (armour)action |= 0x40;
		if (weapon)action |= 0x80;
		fwrite(&action, sizeof(action), 1, this->pFile);
		fwrite(&keys, sizeof(keys), 1, this->pFile);
		fwrite(&vehicle, sizeof(vehicle), 1, this->pFile);

		//Write Health
		fwrite(&health, sizeof(health), 1, this->pFile);
		fwrite(&armour, sizeof(armour), 1, this->pFile);
		fwrite(&weapon, sizeof(weapon), 1, this->pFile);
		uint16_t model = VCMP->GetVehicleModel(vehicle);
		fwrite(&model, sizeof(model), 1, this->pFile);
		uint8_t type = 0;//a nibble representing if movement, health, damage
		float u, v, w;
		VCMP->GetVehicleSpeed(vehicle, &u, &v, &w, 0);
		uint32_t damage = VCMP->GetVehicleDamageData(vehicle);
		float vhealth = VCMP->GetVehicleHealth(vehicle);
		if (u != 0 || v != 0 || w != 0)
			if (u != 0 || v != 0 || w != 0)type += 0x1;
		if (damage != 0)type += 0x2;
		if (health != 1000)type += 0x4;//corrected.
		fwrite(&type, sizeof(type), 1, this->pFile);
		float x, y, z;
		VCMP->GetVehiclePosition(vehicle, &x, &y, &z);
		fwrite(&x, sizeof(x), 1, this->pFile);
		fwrite(&y, sizeof(y), 1, this->pFile);
		fwrite(&z, sizeof(z), 1, this->pFile);
		uint16_t p, q, r;
		p = ConvertToUINT16_T(u, 20.0);
		q = ConvertToUINT16_T(v, 20.0);
		r = ConvertToUINT16_T(w, 20.0);
		fwrite(&p, sizeof(p), 1, this->pFile);
		fwrite(&q, sizeof(q), 1, this->pFile);
		fwrite(&r, sizeof(r), 1, this->pFile);
		float Qx, Qy, Qz, Qw;//Quaternion components
		VCMP->GetVehicleRotation(vehicle, &Qx, &Qy, &Qz, &Qw);
		uint8_t nibble = 0;
		if (Qx < 0)nibble += 0x4;
		if (Qy < 0)nibble += 0x2;
		if (Qz < 0)nibble += 0x1;
		if (Qw < 0)nibble += 0x8;//What the hell man.?
		fwrite(&nibble, sizeof(nibble), 1, this->pFile);
		uint16_t a, b, c;
		a = ConvertToUINT16_T(Qx, -1);
		b = ConvertToUINT16_T(Qy, -1);
		c = ConvertToUINT16_T(Qz, -1);
		fwrite(&a, sizeof(a), 1, this->pFile);
		fwrite(&b, sizeof(b), 1, this->pFile);
		fwrite(&c, sizeof(c), 1, this->pFile);
		//Quaternion w component not needed.

		fwrite(&damage, sizeof(damage), 1, this->pFile);
		fwrite(&vhealth, sizeof(vhealth), 1, this->pFile);
		float hor, ver;//horizontal vertical
		VCMP->GetVehicleTurretRotation(vehicle, &hor, &ver);
		uint16_t _hor, _ver;
		_hor = ConvertToUINT16_T(hor, 2 * (float)PI);
		_ver = ConvertToUINT16_T(ver, 2 * (float)PI);
		fwrite(&_hor, sizeof(_hor), 1, this->pFile);
		fwrite(&_ver, sizeof(_ver), 1, this->pFile);
	}
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
