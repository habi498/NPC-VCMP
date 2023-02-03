#pragma once
#include <stdint.h>
#include "utils.h"

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
	NPCNotConnected=12,
	ErrorUnknown = INT32_MAX
};

enum class playerState {
	None = 0,
	OnFoot = 1,
	Aim = 2,
	Driver = 3,
	Passenger = 4,
	EnterVehicleDriver = 5,
	EnterVehiclePassenger = 6,
	ExitVehicle = 7,
	Wasted = 8,
	Spawned = 9
};
enum class bodyPart {
	Body = 0,
	Torso = 1,
	LeftArm = 2,
	RightArm = 3,
	LeftLeg = 4,
	RightLeg = 5,
	Head = 6
};
typedef enum {
	vcmpPlayerUpdateNormal = 0,
	vcmpPlayerUpdateAiming = 1,
	vcmpPlayerUpdateDriver = 2,
	vcmpPlayerUpdatePassenger = 3,
	forceSizeVcmpPlayerUpdate = INT32_MAX
} vcmpPlayerUpdate;

struct PluginInfo {
	uint32_t structSize;
	uint32_t pluginId;
	char name[32];
	uint32_t pluginVersion;
	uint16_t apiMajorVersion;
	uint16_t apiMinorVersion;
};

struct PluginFuncs {
	uint32_t structSize;//48 functions
	funcError(*GetLastError)() {};
	void (*SendCommandToServer)(const char* message) {};
	void (*SendChatMessage)(const char* message) {};
	void (*FireSniperRifle)(uint8_t weapon, float x, float y, float z, float dx, float dy, float dz) {};
	void (*SendShotInfo)(bodyPart bodypart, int animation) {};
	funcError(*SendInCarSyncData)(uint32_t dwKeys, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteWeapon, uint16_t wAmmo, float fCarHealth, uint32_t dwDamage, VECTOR vecPos, QUATERNION quatRotation, VECTOR vecSpeed, float fTurretx, float fTurrety) {};
	void (*SendOnFootSyncDataEx)(uint32_t dwKeys, VECTOR vecPos, float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon, uint16_t wAmmo, VECTOR vecSpeed, VECTOR vecAimPos, VECTOR vecAimDir, bool bIsCrouching, bool bIsReloading) {};
	void (*SendOnFootSyncData)(uint32_t dwKeys, float x, float y, float z,
		float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon,
		uint16_t wAmmo, float speedx, float speedy, float speedz,
		float aimposx, float aimposy, float aimposz,
		float aimdirx, float aimdiry, float aimdirz, bool bIsCrouching, bool bIsReloading) {};
	void (*SendDeathInfo)(uint8_t weapon, uint8_t killerid, bodyPart bodypart) {};
	void (*SendPassengerSync)() {};
	void (*GetPosition)(float* x, float* y, float* z) {};
	void (*GetAngle)(float* fAngle) {};
	void (*SetPosition)(float x, float y, float z, bool sync) {};
	void (*SetAngle)(float fAngle, bool sync) {};
	funcError(*SetWeapon)(uint8_t byteWeaponId, bool sync) {};
	funcError(*RequestVehicleEnter)(uint16_t wVehicleId, uint8_t byteSeatId) {};
	funcError(*GetPlayerName)(uint8_t playerId, char* buffer, size_t size) {};
	funcError(*GetPlayerPosition)(uint8_t bytePlayerId, VECTOR* vecPosOut) {};
	float (*GetPlayerAngle)(uint8_t bytePlayerId) {};
	uint8_t(*GetPlayerHealth)(uint8_t bytePlayerId) {};
	uint8_t(*GetPlayerArmour)(uint8_t bytePlayerId) {};
	uint8_t(*GetPlayerWeapon)(uint8_t bytePlayerId) {};
	bool (*IsPlayerCrouching)(uint8_t bytePlayerId) {};
	bool (*IsPlayerReloading)(uint8_t bytePlayerId) {};
	uint32_t(*GetPlayerKeys)(uint8_t bytePlayerId) {};
	funcError(*GetPlayerSpeed)(uint8_t bytePlayerId, VECTOR* vecSpeedOut) {};
	funcError(*GetPlayerAimDir)(uint8_t bytePlayerId, VECTOR* vecAimDirOut) {};
	funcError(*GetPlayerAimPos)(uint8_t bytePlayerId, VECTOR* vecAimPosOut) {};
	uint16_t(*GetPlayerWeaponAmmo)(uint8_t bytePlayerId) {};
	playerState(*GetPlayerState)(uint8_t bytePlayerId) {};
	uint16_t(*GetPlayerVehicle)(uint8_t bytePlayerId) {};
	uint8_t(*GetPlayerSeat)(uint8_t bytePlayerId) {};
	uint8_t(*GetPlayerSkin)(uint8_t bytePlayerId) {};
	uint8_t(*GetPlayerTeam)(uint8_t bytePlayerId) {};
	uint8_t(*GetPlayerWeaponAtSlot)(uint8_t bytePlayerId, uint8_t byteSlotId) {};
	uint16_t(*GetPlayerAmmoAtSlot)(uint8_t bytePlayerId, uint8_t byteSlotId) {};
	funcError(*GetVehicleRotation)(uint16_t wVehicleId, QUATERNION* quatRotOut) {};
	uint16_t(*GetVehicleModel)(uint16_t wVehicleId) {};
	funcError(*GetVehiclePosition)(uint16_t wVehicleId, VECTOR* vecPosOut) {};
	uint8_t(*GetVehicleDriver)(uint16_t wVehicleId) {};
	float (*GetVehicleHealth)(uint16_t wVehicleId) {};
	uint32_t(*GetVehicleDamage)(uint16_t wVehicleId) {};
	funcError(*GetVehicleSpeed)(uint16_t wVehicleId, VECTOR* vecSpeedOut) {};
	funcError(*GetVehicleTurretRotation)(uint16_t wVehicleId, float* Horizontal, float* Vertical) {};
	bool (*IsVehicleStreamedIn)(uint16_t wVehicleId) {};
	bool (*IsPlayerStreamedIn)(uint8_t bytePlayerId) {};
	bool (*IsPlayerSpawned)(uint8_t bytePlayerId) {};
	bool (*IsPlayerConnected)(uint8_t bytePlayerId) {};
	const void** (*GetSquirrelExports)(size_t* sizeofExport) {};

	void (*SetHealth)(uint8_t health, bool sync) {};
	void (*SetArmour)(uint8_t armour, bool sync) {};
	uint8_t(*GetNPCId)() {};
	void (*SendServerData)(const void* data, size_t size) {};
};

struct PluginCallbacks {
	uint32_t structSize;
	void (*OnPlayerDeath)(uint8_t bytePlayerId) {};
	void (*OnPlayerText)(uint8_t bytePlayerId, char* text, uint16_t length) {};
	void (*OnNPCEnterVehicle)(uint16_t vehicleid, uint8_t seatid) {};
	void (*OnNPCExitVehicle)() {};
	void (*OnPlayerStreamIn)(uint8_t bytePlayerId) {};
	void (*OnPlayerStreamOut)(uint8_t playerid) {};
	void (*OnVehicleStreamIn)(uint16_t vehicleid) {};
	void (*OnVehicleStreamOut)(uint16_t vehicleid) {};
	void (*OnNPCDisconnect)(uint8_t reason) {};
	void (*OnSniperRifleFired)(uint8_t playerid, uint8_t weapon, float x, float y, float z, float dx, float dy, float dz) {};
	void (*OnPlayerUpdate)(uint8_t bytePlayerId, vcmpPlayerUpdate updateType) {};
	void (*OnNPCConnect)(uint8_t byteId) {};
	void (*OnClientMessage)(uint8_t r, uint8_t g, uint8_t b, char* message, uint16_t len) {};
	void (*OnNPCSpawn)() {};
	void (*OnCycle) () {};
	void (*OnServerData)(const uint8_t* data, size_t size) {};
};


