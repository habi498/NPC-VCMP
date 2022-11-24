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
#include "Squirrel/squirrel.h"
#include "Squirrel/sqstdio.h"
#include "Squirrel/sqstdaux.h"
#include "Squirrel/sqstdsystem.h"
#include "Squirrel/sqstdmath.h"
#include "Squirrel/sqstdstring.h"
#include "Squirrel/sqstdblob.h"
#define PLAYER_RECORDING_TYPE_DRIVER	2
#define PLAYER_RECORDING_TYPE_ONFOOT	1
#define UNIT1 "npcscripts/recordings/units/vector.nut"
#define UNIT2 "npcscripts/recordings/units/Position.nut"
#define UNIT3 "npcscripts/recordings/units/constants.nut"
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
void printfunc(HSQUIRRELVM v, const SQChar* s, ...);
bool StartSquirrel(std::string file);
int StopSquirrel();
SQInteger register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char* fname, unsigned char nparamscheck, const SQChar* typemask);
void call_OnNPCConnect(unsigned char npcplayerid);
void call_OnNPCDisconnect(unsigned char reason);
void call_OnClientMessage(uint8_t r, uint8_t g, uint8_t b, char* message, uint16_t len);
void call_OnNPCSpawn();
void call_OnPlayerDeath(uint8_t playerid);
void call_OnPlayerText(uint8_t playerid, char* message, uint16_t len);
void call_OnNPCEnterVehicle(uint16_t vehicleid, uint8_t seatid);
void call_OnNPCExitVehicle();
void call_OnPlayerStreamIn(uint8_t playerid);
void call_OnPlayerStreamOut(uint8_t playerid);
void call_OnVehicleStreamIn(uint16_t vehicleid);
void call_OnVehicleStreamOut(uint16_t vehicleid);
void call_OnRecordingPlaybackEnd();
void call_OnPlayerUpdate(uint8_t bytePlayerId, vcmpPlayerUpdate updateType );
void call_OnSniperRifleFired(uint8_t bytePlayerID, uint8_t byteWeaponId, float x, float y, float z, float dx, float dy, float dz);
SQInteger fn_SetLocalValue(HSQUIRRELVM v);
void RegisterNPCFunctions();
void RegisterNPCFunctions2();
void RegisterNPCFunctions3();
void RegisterConsts();
#endif