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
enum class funcError {
	NoError = 0,
	EntityNotFound = 1,
	BufferShortage = 2,
	InputTooLarge = 3,
	ArgumentIsOutOfBounds = 4,
	ArgumentIsNull = 5,
	NameIsInvalid = 6,
	RequestIsDenied = 7,
	VehicleNotEntered = 8,
	NotDriverOfVehicle = 9,
	VehicleSeatIdInvalid = 10,
	WeaponNotPossessed = 11,
	ErrorUnknown = INT32_MAX
} ;



class CFunctions
{
public:
	//47 functions
	static void SendCommandToServer(const char* message);
	
	static void SendChatMessage(const char* message);
	
	static void FireSniperRifle(uint8_t weapon, float x, float y, float z, float dx, float dy, float dz);
	
	static void SendShotInfo(int bodypart, int animation);
	
	static funcError SendInCarSyncData(uint32_t dwKeys, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteWeapon, uint16_t wAmmo, float fCarHealth, uint32_t dwDamage, VECTOR vecPos, QUATERNION quatRotation, VECTOR vecSpeed, float fTurretx, float fTurrety);
	
	static void SendOnFootSyncDataEx(uint32_t dwKeys, VECTOR vecPos, float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon, uint16_t wAmmo, VECTOR vecSpeed, VECTOR vecAimPos, VECTOR vecAimDir, bool bIsCrouching, bool bIsReloading);
	
	static void SendOnFootSyncData(uint32_t dwKeys, float x, float y, float z,
		float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon,
		uint16_t wAmmo, float speedx, float speedy, float speedz, 
		float aimposx, float aimposy, float aimposz, 
		float aimdirx, float aimdiry, float aimdirz, bool bIsCrouching, bool bIsReloading);
	
	static void SendDeathInfo(uint8_t weapon, uint8_t killerid, uint8_t bodypart);

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
	static uint8_t GetPlayerState(uint8_t bytePlayerId);
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
	
};
#endif
