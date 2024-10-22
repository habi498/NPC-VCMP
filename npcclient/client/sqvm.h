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
#ifndef SQVM_H
#define SQVM_H
#include <stdarg.h> 
#include <string>
#include <vector>
#include "squirrel.h"
#include "sqstdio.h"
#include "sqstdaux.h"
#include "sqstdsystem.h"
#include "sqstdmath.h"
#include "sqstdstring.h"
#include "sqstdblob.h"
#include "SqVector.h"
#define PLAYER_RECORDING_TYPE_DRIVER	2
#define PLAYER_RECORDING_TYPE_ONFOOT	1
#define PLAYER_RECORDING_TYPE_ALL       3
/*#define UNIT1 "npcscripts/recordings/units/vector.nut"
#define UNIT2 "npcscripts/recordings/units/Position.nut"
#define UNIT3 "npcscripts/recordings/units/constants.nut"*/
#define UNIT "npcscripts/recordings/units/default.nut"
//using namespace std;
#ifdef _MSC_VER
//#pragma comment (lib ,"squirrel_static.lib")
//#pragma comment (lib ,"sqstdlib_static.lib")
#endif
#ifdef SQUNICODE 
#define scvprintf vwprintf 
#else 
#define scvprintf vprintf 
#endif 
typedef enum {
    vcmpPlayerUpdateNormal = 0,
    vcmpPlayerUpdateAiming = 1,
    vcmpPlayerUpdateDriver = 2,
    vcmpPlayerUpdatePassenger = 3,
    forceSizeVcmpPlayerUpdate = INT32_MAX
} vcmpPlayerUpdate;
enum class pickupUpdate {
    AlphaUpdate = 0,
    PositionUpdate = 1,
    pickupRefreshed = 2
};
enum class checkpointUpdate {
    RadiusUpdate = 0,
    PositionUpdate = 1,
    ColourUpdate = 2
};
enum class objectUpdate {
    AlphaUpdate=0,
    PositionUpdate=1,
    RotationToUpdate=2,
    MoveToUpdate=3,
    TrackingBumpsShots=4
};
void printfunc(HSQUIRRELVM v, const SQChar* s, ...);
bool StartSquirrel(std::string file, std::string location, std::vector<std::string>params, std::string execstring);
int StopSquirrel();
SQInteger register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char* fname, SQInteger nparamscheck, const SQChar* typemask);
void call_OnNPCConnect(unsigned char npcplayerid);
void call_OnNPCDisconnect(unsigned char reason);
void call_OnClientMessage(uint8_t r, uint8_t g, uint8_t b, char* message, uint16_t len);
void call_OnServerScriptData(const uint8_t* data, size_t size);
void call_OnNPCSpawn();
void call_OnPlayerDeath(uint8_t playerid);
uint8_t call_OnNPCClassSelect();
void call_OnServerShareTick(uint32_t tickcount);
void call_OnTimeWeatherSync(uint16_t timerate, uint8_t minute, uint8_t hour, uint8_t weather);
void call_OnPlayerText(uint8_t playerid, char* message, uint16_t len);
void call_OnNPCEnterVehicle(uint16_t vehicleid, uint8_t seatid);
void call_OnNPCExitVehicle();
void call_OnPlayerStreamIn(uint8_t playerid);
void call_OnPlayerStreamOut(uint8_t playerid);
void call_OnVehicleStreamIn(uint16_t vehicleid);
void call_OnVehicleStreamOut(uint16_t vehicleid);
void call_OnPickupStreamIn(uint16_t wPickupId);
void call_OnPickupDestroyed(uint16_t wPickupId);
void call_OnPickupUpdate(uint16_t wPickupId, pickupUpdate update);
void call_OnCheckpointStreamIn(uint16_t wCheckpointID);
void call_OnCheckpointDestroyed(uint16_t wCheckpointID);
void call_OnCheckpointUpdate(uint16_t wCheckpointID, checkpointUpdate update);
void call_OnObjectStreamIn(uint16_t wObjectID);
void call_OnObjectDestroyed(uint16_t wObjectID);
void call_OnObjectUpdate(uint16_t wObjectID, objectUpdate update);
void call_OnRecordingPlaybackEnd();
void call_OnPlayerUpdate(uint8_t bytePlayerId, vcmpPlayerUpdate updateType );
void call_OnSniperRifleFired(uint8_t bytePlayerID, uint8_t byteWeaponId, float x, float y, float z, float dx, float dy, float dz);
void call_OnExplosion(uint8_t byteExplosionType, VECTOR vecPos, uint8_t bytePlayerCaused, bool bIsOnGround);
void call_OnProjectileFired(uint8_t bytePlayerId, uint8_t byteWeapon, VECTOR vecPos, float r1, float r2, float r3, float r4, float r5, float r6, float r7);
SQInteger fn_SetLocalValue(HSQUIRRELVM v);
void RegisterNPCFunctions();
void RegisterNPCFunctions2();
void RegisterNPCFunctions3();
void RegisterNPCFunctions4();
void RegisterNPCFunctions5();
void RegisterNPCFunctions6();
void RegisterConsts();
int LoadScript(std::string file, std::vector<std::string> params);
void call_OnVehicleSetSpeedRequest(uint16_t wVehicleId, VECTOR vecSpeed, bool bAdd, bool isRelative);
void call_OnVehicleSetTurnSpeedRequest(uint16_t wVehicleId, VECTOR vecSpeed, bool bAdd, bool isRelative);
void call_OnBulletFired(uint8_t byteWeaponId, VECTOR vecSourcePos);
void call_OnStoreDownloadComplete();
#endif