#define API_MAJOR 1
#define API_MINOR 6
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
	NPCAlreadySpawned=13,
	NPCNotSpawned=14,
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
	Body=0,
	Torso=1,
	LeftArm=2,
	RightArm=3,
	LeftLeg=4,
	RightLeg=5,
	Head=6
};

struct PluginInfo {
	uint32_t structSize;
	uint32_t pluginId;
	char name[32];
	uint32_t pluginVersion;
	uint16_t apiMajorVersion;
	uint16_t apiMinorVersion;
} ;

struct PluginFuncs {
	uint32_t structSize;//48 functions
	funcError(*GetLastError)() {};
	void (*SendCommandToServer)(const char* message) {};
	void (*SendChatMessage)(const char* message) {};
	void (*FireSniperRifle)(uint8_t weapon, float x, float y, float z, float dx, float dy, float dz) {};
	void (*SendShotInfo)(bodyPart bodypart, int animation, bool bAutoDeath , uint8_t AutoDeathWep, uint8_t AutoDeathKiller ) {};
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
	WORD (*GetCurrentWeaponAmmo)() {};
	BYTE (*GetCurrentWeapon)(){};

	void (*SendOnFootSyncDataEx2)(ONFOOT_SYNC_DATA OfSyncData) {};
	void (*SendInCarSyncDataEx)(INCAR_SYNC_DATA IcSyncData) {};
	void (*GetOnFootSyncData)(ONFOOT_SYNC_DATA** pOfSyncData) {};
	void (*GetInCarSyncData)(INCAR_SYNC_DATA* pIcSyncData) {};
	void (*SetAmmoAtSlot)(uint8_t byteSlotId, WORD wAmmo) {};

	void (*FireProjectile)(uint8_t byteWeapon, VECTOR vecPos, float r1, float r2, float r3, float r4, float r5, float r6, float r7) {};
	funcError(*RequestClass)(uint8_t relativeindex, bool bIgnoreAbsoluteClass) {};
	funcError(*RequestSpawn)() {};

	//api 1.5
	void (*SetFPS)(double fps) {};
	//Success
	bool (*IsPickupStreamedIn)(uint16_t wPickupID) {};
	//GetLastError
	uint16_t(*GetPickupModel)(uint16_t wPickupID) {};
	funcError(*GetPickupPosition)(uint16_t wPickupID, VECTOR* vecPos) {};
	uint8_t(*GetPickupAlpha)(uint16_t wPickupID) {};
	uint32_t(*GetPickupQuantity)(uint16_t wPickupID) {};
	uint32_t(*GetStreamedPickupCount)() {};
	funcError(*ClaimPickup)(uint16_t wPickupID) {};
	funcError(*ClaimEnterCheckpoint)(uint16_t wCheckpointID) {};
	funcError(*ClaimExitCheckpoint)(uint16_t wCheckpointID) {};
	bool (*IsCheckpointStreamedIn)(uint16_t wCheckpointID) {};
	funcError(*GetCheckpointRadius)(uint16_t wCheckpointID, float* fRadius) {};
	funcError(*GetCheckpointColor)(uint16_t wCheckpointID, uint8_t* Red, uint8_t* Green, uint8_t* Blue, uint8_t* Alpha) {};
	funcError(*GetCheckpointPos)(uint16_t wCheckpointID, VECTOR* vecPos) {};
	funcError(*IsCheckpointSphere)(uint16_t wCheckpointID, uint8_t* isSphere) {};
	bool (*IsObjectStreamedIn)(uint16_t wObjectID) {};
	funcError(*GetObjectModel)(uint16_t wObjectID, uint16_t* wModel) {};
	funcError(*GetObjectPos)(uint16_t wObjectID, VECTOR* vecPos) {};
	funcError(*GetObjectRotation)(uint16_t wObjectID, QUATERNION* quatRot) {};
	funcError(*GetObjectAlpha)(uint16_t wObjectID, uint8_t* byteAlpha) {};
	bool (*IsObjectTouchReportEnabled)(uint16_t wObjectID) {};
	bool (*IsObjectShotReportEnabled)(uint16_t wObjectID) {};
	funcError(*ClaimObjectTouch)(uint16_t wObjectID) {};
	funcError(*ClaimObjectShot)(uint16_t wObjectID, uint8_t byteWeaponID) {};
	uint32_t (*GetStreamedCheckpointCount)() {};
	uint32_t (*GetStreamedObjectCount)() {};
	funcError(*ExitVehicle)() {};
	funcError(*ExitVehicleEx)(bool fosd , uint8_t style , uint8_t byte1, uint8_t byte2) {};
	uint8_t (*GetPlayerAction)(uint8_t bytePlayerId) {};
	funcError (*Suicide)(uint8_t reason) {};
	Color (*GetColor)() {};
	Color (*GetPlayerColor)(uint8_t bytePlayerId) {};
	void (*RequestAbsoluteClass)(uint8_t classID) {};
	bool (*IsNpcSpawned)() {};
	void (*SendPrivMessage)(uint8_t bytePlayerId, const char* message) {};
	void (*QuitServer)() {};
	bool (*IsWeaponAvailable)(uint8_t byteWeaponId) {};
	void (*SetConfig)(uint32_t dw_value) {};
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

	void (*OnExplosion)(uint8_t byteExplosionType, VECTOR vecPos, uint8_t bytePlayerCaused, bool bIsOnGround) {};
	void (*OnProjectileFired)(uint8_t bytePlayerId, uint8_t byteWeapon, VECTOR vecPos, float r1, float r2, float r3, float r4, float r5, float r6, float r7) {};
	
	uint8_t(*OnNPCClassSelect)() {};
	void (*OnServerShareTick)(unsigned int tickcount) {};

	void (*OnTimeWeatherSync)(uint16_t timerate, uint8_t minute, uint8_t hour, uint8_t weather) {};
	void (*OnPickupStreamIn)(uint16_t wPickupId) {};
	void (*OnPickupDestroyed)(uint16_t wPickupId) {};
	void (*OnPickupUpdate)(uint16_t wPickupId, pickupUpdate update) {};
	void (*OnCheckpointStreamIn)(uint16_t wCheckpointId) {};
	void (*OnCheckpointDestroyed)(uint16_t wCheckpointId) {};
	void (*OnCheckpointUpdate)(uint16_t wCheckpointId, checkpointUpdate update){};
	void (*OnObjectStreamIn)(uint16_t wObjectId) {};
	void (*OnObjectDestroyed)(uint16_t wObjectId) {};
	void (*OnObjectUpdate)(uint16_t wObjectId, objectUpdate update) {};

	void (*OnVehicleSetSpeedRequest)(uint16_t wVehicleId, VECTOR vecSpeed, bool bAdd, bool isRelative) {};
	void (*OnVehicleSetTurnSpeedRequest)(uint16_t wVehicleId, VECTOR vecSpeed, bool bAdd, bool isRelative) {};
};