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
#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H

class CFunctions
{
public:
	//47 functions
	static void SendCommandToServer(const char* message);
	
	static void SendChatMessage(const char* message);
	
	static void FireSniperRifle(uint8_t weapon, float x, float y, float z, float dx, float dy, float dz);
	
	static void SendShotInfo(bodyPart bodypart, int animation, bool bAutoDeath=false,uint8_t AutoDeathWep=0, uint8_t AutoDeathKiller=255);
	
	static funcError SendInCarSyncData(uint32_t dwKeys, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteWeapon, uint16_t wAmmo, float fCarHealth, uint32_t dwDamage, VECTOR vecPos, QUATERNION quatRotation, VECTOR vecSpeed, float fTurretx, float fTurrety);
	
	static void SendOnFootSyncDataEx(uint32_t dwKeys, VECTOR vecPos, float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon, uint16_t wAmmo, VECTOR vecSpeed, VECTOR vecAimPos, VECTOR vecAimDir, bool bIsCrouching, bool bIsReloading);
	
	static void SendOnFootSyncData(uint32_t dwKeys, float x, float y, float z,
		float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon,
		uint16_t wAmmo, float speedx, float speedy, float speedz, 
		float aimposx, float aimposy, float aimposz, 
		float aimdirx, float aimdiry, float aimdirz, bool bIsCrouching, bool bIsReloading);
	
	static void SendDeathInfo(uint8_t weapon, uint8_t killerid, bodyPart bodypart);

	static void SendPassengerSync();

	static void GetPosition(float* x, float* y, float* z);

	static void GetAngle(float* fAngle);

	static void SetPosition(float x, float y, float z, bool sync=true);

	static void SetAngle(float fAngle, bool sync=true);
	
	static funcError SetWeapon(uint8_t byteWeaponId, bool sync=true);

	static funcError RequestVehicleEnter(uint16_t wVehicleId, uint8_t byteSeatId);
	
	static funcError GetPlayerName(uint8_t playerId, char* buffer, size_t size);

	static funcError GetPlayerPosition(uint8_t bytePlayerId, VECTOR* vecPosOut);
	static float GetPlayerAngle(uint8_t bytePlayerId);
	static uint8_t GetPlayerHealth(uint8_t bytePlayerId);
	static uint8_t GetPlayerArmour(uint8_t bytePlayerId);
	static uint8_t GetPlayerWeapon(uint8_t bytePlayerId);
	static bool IsPlayerCrouching(uint8_t bytePlayerId);
	static bool IsPlayerReloading(uint8_t bytePlayerId);
	static uint32_t GetPlayerKeys(uint8_t bytePlayerId);
	static funcError GetPlayerSpeed(uint8_t bytePlayerId, VECTOR* vecSpeedOut);
	static funcError GetPlayerAimDir(uint8_t bytePlayerId, VECTOR* vecAimDirOut);
	static funcError GetPlayerAimPos(uint8_t bytePlayerId, VECTOR* vecAimPosOut);
	static uint16_t GetPlayerWeaponAmmo(uint8_t bytePlayerId);
	static playerState GetPlayerState(uint8_t bytePlayerId);
	static uint16_t GetPlayerVehicle(uint8_t bytePlayerId);
	static uint8_t GetPlayerSeat(uint8_t bytePlayerId);
	static uint8_t GetPlayerSkin(uint8_t bytePlayerId);
	static uint8_t GetPlayerTeam(uint8_t bytePlayerId);
	
	
	static uint8_t GetPlayerWeaponAtSlot(uint8_t bytePlayerId, uint8_t byteSlotId);
	static uint16_t GetPlayerAmmoAtSlot(uint8_t bytePlayerId, uint8_t byteSlotId);
	
	static funcError GetVehicleRotation(uint16_t wVehicleId, QUATERNION* quatRotOut);
	static uint16_t GetVehicleModel(uint16_t wVehicleId);
	static funcError GetVehiclePosition(uint16_t wVehicleId, VECTOR* vecPosOut);
	static uint8_t GetVehicleDriver(uint16_t wVehicleId);
	static float GetVehicleHealth(uint16_t wVehicleId);
	static uint32_t GetVehicleDamage(uint16_t wVehicleId);
	static funcError GetVehicleSpeed(uint16_t wVehicleId, VECTOR* vecSpeedOut);
	static funcError GetVehicleTurretRotation(uint16_t wVehicleId, float* Horizontal, float* Vertical);

	static bool IsVehicleStreamedIn(uint16_t wVehicleId);
	static bool IsPlayerStreamedIn(uint8_t bytePlayerId);
	static bool IsPlayerSpawned(uint8_t bytePlayerId);
	static bool IsPlayerConnected(uint8_t bytePlayerId);

	static void SetHealth(uint8_t health, bool sync);
	static void SetArmour(uint8_t armour, bool sync);
	static uint8_t GetNPCId();
	static void SendServerData(const void* data, size_t size);

	static WORD GetCurrentWeaponAmmo();
	static BYTE GetCurrentWeapon();
	
	static void SendOnFootSyncDataEx2(ONFOOT_SYNC_DATA OfSyncData);
	static void SendInCarSyncDataEx(INCAR_SYNC_DATA IcSyncData);
	static void GetOnFootSyncData(ONFOOT_SYNC_DATA** pOfSyncData);
	static void GetInCarSyncData(INCAR_SYNC_DATA* pIcSyncData);
	static void SetAmmoAtSlot(uint8_t byteSlotId, WORD wAmmo);

	static void FireProjectile(uint8_t byteWeapon, VECTOR vecPos, float r1, float r2, float r3, float r4, float r5, float r6, float r7);

	funcError GetLastError();
	static funcError RequestClass(uint8_t relativeindex, bool bIgnoreAbsoluteClass=true);
	static funcError RequestSpawn();

	static void SetFPS(double fps);
	//Success
	static bool IsPickupStreamedIn(uint16_t wPickupID);
	//GetLastError
	static uint16_t GetPickupModel(uint16_t wPickupID);
	static funcError GetPickupPosition(uint16_t wPickupID, VECTOR* vecPos);
	static uint8_t GetPickupAlpha(uint16_t wPickupID);
	static uint32_t GetPickupQuantity(uint16_t wPickupID);
	static uint32_t GetStreamedPickupCount();
	//static void SendModuleList(const char* message, uint16_t len);
	static funcError ClaimPickup(uint16_t wPickupID);
	static funcError ClaimEnterCheckpoint(uint16_t wCheckpointID);
	static funcError ClaimExitCheckpoint(uint16_t wCheckpointID);
	
	static bool IsCheckpointStreamedIn(uint16_t wCheckpointID);
	static funcError GetCheckpointRadius(uint16_t wCheckpointID, float* fRadius);
	static funcError GetCheckpointColor(uint16_t wCheckpointID, uint8_t* Red, uint8_t* Green, uint8_t* Blue, uint8_t* Alpha);
	static funcError GetCheckpointPos(uint16_t wCheckpointID, VECTOR* vecPos);
	static funcError IsCheckpointSphere(uint16_t wCheckpointID, uint8_t* isSphere);

	static bool IsObjectStreamedIn(uint16_t wObjectID);
	static funcError GetObjectModel(uint16_t wObjectID, uint16_t* wModel);
	static funcError GetObjectPos(uint16_t wObjectID, VECTOR* vecPos);
	static funcError GetObjectRotation(uint16_t wObjectID, QUATERNION* quatRot);
	static funcError GetObjectAlpha(uint16_t wObjectID, uint8_t* byteAlpha);
	static bool IsObjectTouchReportEnabled(uint16_t wObjectID);
	static bool IsObjectShotReportEnabled(uint16_t wObjectID);
	static funcError ClaimObjectTouch(uint16_t wObjectID);
	static funcError ClaimObjectShot(uint16_t wObjectID, uint8_t byteWeaponID);
	static uint32_t GetStreamedCheckpointCount();
	static uint32_t GetStreamedObjectCount();
	//1.6
	static funcError ExitVehicle();
	static funcError ExitVehicleEx(bool fosd=false, uint8_t style=1, uint8_t byte1=0,uint8_t byte2=0);
	static uint8_t GetPlayerAction(uint8_t bytePlayerId);
	
	static funcError Suicide(uint8_t reason=70);
	static Color GetColor();
	static Color GetPlayerColor(uint8_t bytePlayerId);
	static void RequestAbsoluteClass(uint8_t classID);
	static bool IsNpcSpawned();
	static void SendPrivMessage(uint8_t bytePlayerId,const char* message);
	static void QuitServer();
	static bool IsWeaponAvailable(uint8_t byteWeaponId);
	static void SetConfig(uint32_t value);
	static void FireBullet(uint8_t weaponid, float x, float y, float z);
};



#endif



