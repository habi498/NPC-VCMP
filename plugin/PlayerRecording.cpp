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
extern PluginFuncs* VCMP;
extern std::string szRecDirectory;
bool bNewPlayersRecord = false;
uint8_t byteNewPlayersRecordType = 3;
uint32_t dw_NewPlayersFlag = 60;
//Return 1: The function was executed successfully.
//Return 0: The function failed to execute. Either the 
//specified player does not exist or file creation failed.
//.rec extension is automatically added
bool StartRecordingPlayerData(int32_t playerId, uint8_t recordtype, std::string recordname,uint32_t flags)
{
	if (recordname == "")
	{
		time_t rawtime;
		struct tm* timeinfo;
		char buffer[100];
		time(&rawtime);
		timeinfo = localtime(&rawtime);//
		size_t chars_copied = strftime(buffer, 50, "%F %I-%M-%S %p ", timeinfo); //2001-08-23 02-55-02 PM
		if (chars_copied == 0)
		{
			printf("Error in creating logfile\n");
			exit(0);
		}
		char name[24];
		vcmpError e=VCMP->GetPlayerName(playerId, name, sizeof(name));
		if (e != vcmpErrorNone)return 0;

		strcat(buffer, name);
		recordname = std::string(buffer);
	}
	std::string filename = std::string(szRecDirectory + std::string("/") + recordname + std::string(".rec"));
	if (!m_pPlayerPool->GetSlotState(playerId))
		return 0;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	bool success=m_pPlayer->Start(recordtype, filename,flags);
	if (success)
		m_pPlayer->szFilename = recordname;
	return success;
}
//Return n = the no:of recordings newly started.
//n=0 means no players online or no recordings could be started anew.
uint8_t StartRecordingAllPlayerData(uint8_t recordtype, uint32_t flags, bool bRecordNewPlayers)
{
	uint32_t max = VCMP->GetMaxPlayers();  uint8_t n = 0; bool success;
	for (uint32_t i = 0; i < max; i++)
	{
		if (VCMP->IsPlayerConnected(i))
		{
			success = StartRecordingPlayerData(i, recordtype, "", flags);
			if (success)
				n++;
		}
	}
	if (bRecordNewPlayers)
	{
		bNewPlayersRecord = true;
		byteNewPlayersRecordType = recordtype;
		dw_NewPlayersFlag = flags;
	}
	return n;
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
//Return n = the no:of recordings stopped.
//n=0 means either no recordings were in progress or function was unable to stop any.
uint8_t StopRecordingAllPlayerData()
{
	uint32_t max = VCMP->GetMaxPlayers();  uint8_t n = 0; bool success;
	for (uint32_t i = 0; i < max; i++)
	{
		if (VCMP->IsPlayerConnected(i))
		{
			success = StopRecordingPlayerData(i);
			if (success)
				n++;
		}
	}
	bNewPlayersRecord = false;
	dw_NewPlayersFlag = 0;
	byteNewPlayersRecordType = 3;
	return n;
}
bool IsPlayerRecording(uint8_t bytePlayerId)
{
	if (VCMP->IsPlayerConnected(bytePlayerId))
	{
		if (!m_pPlayerPool->GetSlotState(bytePlayerId))
			return 0;
		CPlayer* m_pPlayer = m_pPlayerPool->GetAt(bytePlayerId);
		if (m_pPlayer)
		{
			return m_pPlayer->IsRecording();
		}
	}
	return 0;
}
uint8_t GetSlotIdFromWeaponId(uint8_t byteWeapon)
{
	uint8_t byteWeaponSlot;
	switch (byteWeapon)
	{
	case 0:
	case 1:
		byteWeaponSlot = 0;
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
		byteWeaponSlot = 1;
		break;
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		byteWeaponSlot = 2;
		break;
	case 17:
	case 18:
		byteWeaponSlot = 3;
		break;
	case 19:
	case 20:
	case 21:
		byteWeaponSlot = 4;
		break;
	case 22:
	case 23:
	case 24:
	case 25:
		byteWeaponSlot = 5;
		break;

	case 26:
	case 27:
		byteWeaponSlot = 6;
		break;
	case 28:
	case 29:
		byteWeaponSlot = 8;
		break;
	case 30:
	case 31:
	case 32:
	case 33:
		byteWeaponSlot = 7;
		break;

	default:
		// If invalid weapon then set fists as weapon
		byteWeaponSlot = 0;
		break;
	}
	return byteWeaponSlot;
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
		/* rec file v 4.0 start*/
		m_pOfSynDat.byteAction = (BYTE)VCMP->GetPlayerAction(playerId);
		uint8_t slotId = GetSlotIdFromWeaponId(m_pOfSynDat.byteCurrentWeapon);
		m_pOfSynDat.wAmmo = (BYTE)VCMP->GetPlayerAmmoAtSlot(playerId, slotId);
		uint32_t keys = m_pOfSynDat.dwKeys;
		//Check for reloading weapon
		if (keys & 64 && !(keys & 512) && m_pOfSynDat.byteCurrentWeapon > 11)
			m_pOfSynDat.bIsReloading = true;
		/* rec file v 4.0 end*/
		VCMP->GetPlayerPosition(playerId, &m_pOfSynDat.vecPos.X,
			&m_pOfSynDat.vecPos.Y, &m_pOfSynDat.vecPos.Z);
		VCMP->GetPlayerAimDirection(playerId, &m_pOfSynDat.vecAimDir.X,
			&m_pOfSynDat.vecAimDir.Y, &m_pOfSynDat.vecAimDir.Z);
		VCMP->GetPlayerAimPosition(playerId, &m_pOfSynDat.vecAimPos.X,
			&m_pOfSynDat.vecAimPos.Y, &m_pOfSynDat.vecAimPos.Z);
		if (this->m_RecordingType == PLAYER_RECORDING_TYPE_ONFOOT)
		{
			m_pOfDatablock.m_pOfSyncData = m_pOfSynDat;
			size_t t = fwrite((void*)&m_pOfDatablock, sizeof(m_pOfDatablock), 1, this->pFile);
		}
		else if (this->m_RecordingType == PLAYER_RECORDING_TYPE_ALL)
		{
			GENERALDATABLOCK datablock;
			datablock.bytePacketType = (updateType == vcmpPlayerUpdateNormal) ? PACKET_ONFOOT : PACKET_ONFOOT_AIM;
			datablock.time = time;
			size_t count=fwrite((void*)&datablock, sizeof(datablock), 1, this->pFile);
			if (count != 1) {
				this->Abort(); return;
			}
			count = fwrite((void*)&m_pOfSynDat, sizeof(m_pOfSynDat), 1, this->pFile);
			if (count != 1) {
				this->Abort(); return;
			}
		}
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
			&m_pIcSyncData.quatRotation.Y, &m_pIcSyncData.quatRotation.Z, &m_pIcSyncData.quatRotation.W);
		m_pIcSyncData.VehicleID = vehicleId;
		m_pIcSyncData.dwKeys = VCMP->GetPlayerGameKeys(playerId);
		if (this->m_RecordingType == PLAYER_RECORDING_TYPE_DRIVER)
		{
			m_pIcDatablock.m_pIcSyncData = m_pIcSyncData;
			fwrite((void*)&m_pIcDatablock, sizeof(m_pIcDatablock), 1, this->pFile);
		}
		else {//PLAYER_RECORDIN_TYPE_ALL
			GENERALDATABLOCK datablock;
			datablock.bytePacketType = PACKET_DRIVER;
			datablock.time = time;
			size_t count = fwrite((void*)&datablock, sizeof(datablock), 1, this->pFile);
			if (count != 1) 
			{
				this->Abort(); return;
			}
			count = fwrite((void*)&m_pIcSyncData, sizeof(m_pIcSyncData), 1, this->pFile);
			if (count != 1) {
				this->Abort(); return;
			}
		}
	}
	else if (updateType == vcmpPlayerUpdatePassenger)
	{
		if (this->m_RecordingType == PLAYER_RECORDING_TYPE_ALL)
		{
			GENERALDATABLOCK datablock;
			datablock.bytePacketType = PACKET_PASSENGER;
			datablock.time = GetTickCount();
			size_t count = fwrite((void*)&datablock, sizeof(datablock), 1, this->pFile);
			if (count != 1)
			{
				this->Abort(); return;
			}
			PASSENGERDATA data{};
			data.wVehicleID = (uint16_t)VCMP->GetPlayerVehicleId(playerId);
			data.wModel = (uint16_t)VCMP->GetVehicleModel(data.wVehicleID);
			data.SeatId = (uint8_t)VCMP->GetPlayerInVehicleSlot(playerId);
			data.byteHealth = (uint8_t)VCMP->GetPlayerHealth(playerId);
			data.byteArmour = (uint8_t)VCMP->GetPlayerArmour(playerId);
			count = fwrite((void*)&data, sizeof(data), 1, this->pFile);
			if (count != 1)
			{
				this->Abort(); return;
			}
		}
	}
}
/*
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
*/
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
bool CPlayer::Start(uint32_t recordtype, std::string filename, uint32_t flags)
{
	if (IsRecording())return false;
	bool success = this->CreateRecordingFile(filename);
	if (!success)return 0;
	this->m_RecordingType = recordtype;
	this->m_dwRecFlags = flags;
	this->init = true;
	bool s = WriteHeader(NPC_RECFILE_IDENTIFIER_V5);//this will write recordint type also
	if (!s) { this->Abort(); return 0; }
	//Just Write Flags to indicate name is present or not.
	uint32_t f = (flags & 1) == 1 ? 1 : 0;
	size_t count = fwrite(&f, sizeof(uint32_t), 1, pFile);
	if (count != 1) { this->Abort(); return 0; }
	if (flags & REC_NAME)
	{
		char name[24] = { 0 };
		vcmpError e=VCMP->GetPlayerName(this->m_PlayerID, name, sizeof(name));
		if (e == vcmpErrorNone)
		{
			size_t count = fwrite(name, sizeof(char), sizeof(name), pFile);
			if (count != sizeof(name))
			{
				this->Abort();
				return 0;
			}
		}
	}
	if (flags & REC_SPAWN)
	{
		sgbytePreviousClassID = VCMP->GetPlayerClass(this->m_PlayerID);
	}
	return 1;
}