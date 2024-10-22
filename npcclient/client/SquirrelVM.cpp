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
extern HSQUIRRELVM v;
extern NPC* iNPC;
bool bSquirrelVMRunning = false;
extern bool bConsoleInputEnabled;
void stop_consoleinput();
void printfunc(HSQUIRRELVM vm, const SQChar* s, ...)
{
    va_list arglist;
    va_start(arglist, s);
    scvprintf(s, arglist);
    va_end(arglist);
}
void call_OnNPCConnect(unsigned char npcplayerid)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnNPCConnect"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, npcplayerid);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnNPCDisconnect(unsigned char reason)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnNPCDisconnect"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, reason);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}

void call_OnServerScriptData(const uint8_t* data, size_t size)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnServerData"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        SQUserPointer blob=sqstd_createblob(v, size);
        memcpy(blob, (const void*)data, size);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    
    sq_settop(v, top); //restores the original stack size
}

void call_OnClientMessage(uint8_t r, uint8_t g, uint8_t b, char* message, uint16_t len)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnClientMessage"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, r);
        sq_pushinteger(v, g);
        sq_pushinteger(v, b);
        sq_pushstring(v, message, len);
        sq_call(v, 5, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnNPCSpawn()
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnNPCSpawn"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_call(v, 1, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnPlayerDeath(uint8_t playerid)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnPlayerDeath"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, playerid);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
uint8_t call_OnNPCClassSelect()
{
    SQInteger retval = 0;
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnNPCClassSelect"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_call(v, 1, 1, 1); //calls the function 
        if (sq_gettype(v, -1) == OT_INTEGER||sq_gettype(v,-1)==OT_FLOAT)
        {
            HSQOBJECT obj;
            if (SQ_SUCCEEDED(sq_getstackobj(v, -1, &obj)))
            {
                retval = sq_objtointeger(&obj);
            }
        }
        else if (sq_gettype(v, -1) == OT_BOOL)
        {
            SQBool b;
            if (SQ_SUCCEEDED(sq_getbool(v, -1, &b)))
            {
                retval = (uint8_t)b;
            }

        }
    }
    
    sq_settop(v, top); //restores the original stack size
    return (uint8_t)retval;
}
void call_OnServerShareTick(uint32_t tickcount)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnServerShareTick"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, tickcount);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnTimeWeatherSync(uint16_t timerate, uint8_t minute, uint8_t hour, uint8_t weather)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnTimeWeatherSync"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, timerate);
        sq_pushinteger(v, minute);
        sq_pushinteger(v, hour);
        sq_pushinteger(v, weather);
        sq_call(v, 5, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnNPCEnterVehicle(uint16_t vehicleid, uint8_t seatid)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnNPCEnterVehicle"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, vehicleid);
        sq_pushinteger(v, seatid);
        sq_call(v, 3, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnNPCExitVehicle()
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnNPCExitVehicle"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_call(v, 1, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnPlayerStreamIn(uint8_t playerid)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnPlayerStreamIn"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, playerid);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnPlayerStreamOut(uint8_t playerid)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnPlayerStreamOut"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, playerid);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnPlayerText(uint8_t playerid, char* message, uint16_t len)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnPlayerText"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, playerid);
        sq_pushstring(v, message, len);
        sq_call(v, 3, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnVehicleStreamIn(uint16_t vehicleid)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnVehicleStreamIn"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, vehicleid);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnVehicleStreamOut(uint16_t vehicleid)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnVehicleStreamOut"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, vehicleid);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnPickupStreamIn(uint16_t wPickupId)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnPickupStreamIn"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wPickupId);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnPickupDestroyed(uint16_t wPickupId)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnPickupDestroyed"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wPickupId);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnPickupUpdate(uint16_t wPickupId, pickupUpdate update)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnPickupUpdate"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wPickupId);
        sq_pushinteger(v, static_cast<uint8_t>(update));
        sq_call(v, 3, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnCheckpointStreamIn(uint16_t wCheckpointID)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnCheckpointStreamIn"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wCheckpointID);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnCheckpointDestroyed(uint16_t wCheckpointID)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnCheckpointDestroyed"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wCheckpointID);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnCheckpointUpdate(uint16_t wCheckpointID, checkpointUpdate update)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnCheckpointUpdate"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wCheckpointID);
        sq_pushinteger(v, static_cast<uint8_t>(update));
        sq_call(v, 3, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnObjectStreamIn(uint16_t wObjectID)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnObjectStreamIn"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wObjectID);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnObjectDestroyed(uint16_t wObjectID)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnObjectDestroyed"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wObjectID);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnObjectUpdate(uint16_t wObjectID, objectUpdate update)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnObjectUpdate"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wObjectID);
        sq_pushinteger(v, static_cast<uint16_t>(update));
        sq_call(v, 3, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnRecordingPlaybackEnd()
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnRecordingPlaybackEnd"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_call(v, 1, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnNPCScriptUnload()
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnNPCScriptUnload"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_call(v, 1, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnNPCScriptLoad(std::vector<std::string>argv)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnNPCScriptLoad"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_newarray(v, 0);
        for (size_t i = 0; i < argv.size(); i++)
        {
            sq_pushstring(v, _SC(argv[i].c_str()), -1);
            sq_arrayappend(v, -2);
        }
        sq_call(v, 2, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnPlayerUpdate(uint8_t bytePlayerID, vcmpPlayerUpdate UpdateType)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnPlayerUpdate"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this'
        sq_pushinteger(v, bytePlayerID);
        sq_pushinteger(v, UpdateType);
        sq_call(v, 3, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnSniperRifleFired(uint8_t bytePlayerID, uint8_t byteWeaponId, float x, float y, float z, float dx, float dy, float dz)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnSniperRifleFired"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this'
        sq_pushinteger(v, bytePlayerID);
        sq_pushinteger(v, byteWeaponId);
        sq_pushfloat(v, x);
        sq_pushfloat(v, y);
        sq_pushfloat(v, z);
        sq_pushfloat(v, dx);
        sq_pushfloat(v, dy);
        sq_pushfloat(v, dz);
        sq_call(v, 9, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnConsoleInput(char* input)
{
    if (!bSquirrelVMRunning)return;
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnConsoleInput"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this'
        sq_pushstring(v, input, -1);
        sq_call(v, 2, 0, 1); //calls the function 
    }
    else
    {
        SQRESULT a=sq_compilebuffer(v, input, strlen(input), "STDIN", true);
        sq_pushroottable(v);
        sq_call(v, 1, 0, 1);
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnExplosion(uint8_t byteExplosionType, VECTOR vecPos, uint8_t bytePlayerCaused, bool bIsOnGround)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnExplosion"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, byteExplosionType);
        sq_pushvector(v, vecPos);
        sq_pushinteger(v, bytePlayerCaused==(uint8_t)(-1)?-1:bytePlayerCaused);
        if (bIsOnGround)sq_pushbool(v, SQTrue);
        else sq_pushbool(v, SQFalse);
        sq_call(v, 5, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}

void call_OnProjectileFired(uint8_t bytePlayerId, uint8_t byteWeapon, VECTOR vecPos, float r1, float r2, float r3, float r4, float r5, float r6, float r7)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnProjectileFired"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, bytePlayerId);
        sq_pushinteger(v, byteWeapon);
        sq_pushvector(v, vecPos);
        sq_pushfloat(v, r1);
        sq_pushfloat(v, r2);
        sq_pushfloat(v, r3);
        sq_pushfloat(v, r4);
        sq_pushfloat(v, r5);
        sq_pushfloat(v, r6);
        sq_pushfloat(v, r7);
        sq_call(v, 11, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnVehicleSetSpeedRequest(uint16_t wVehicleId, VECTOR vecSpeed, bool bAdd, bool isRelative)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnVehicleSetSpeedRequest"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wVehicleId);
        sq_pushvector(v, vecSpeed);
        sq_pushbool(v,bAdd);
        sq_pushbool(v,isRelative);
        sq_call(v, 5, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnVehicleSetTurnSpeedRequest(uint16_t wVehicleId, VECTOR vecSpeed, bool bAdd, bool isRelative)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnVehicleSetTurnSpeedRequest"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, wVehicleId);
        sq_pushvector(v, vecSpeed);
        sq_pushbool(v, bAdd);
        sq_pushbool(v, isRelative);
        sq_call(v, 5, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnBulletFired(uint8_t byteWeaponId, VECTOR vecSourcePos)
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnBulletFired"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_pushinteger(v, byteWeaponId);
        sq_pushvector(v, vecSourcePos);
        sq_call(v, 3, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
void call_OnStoreDownloadComplete()
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnStoreDownloadComplete"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_call(v, 1, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
bool StartSquirrel(std::string file, std::string location, std::vector<std::string> params, std::string execstring)
{
    v = sq_open(1024); // creates a VM with initial stack size 1024 
    RegisterNPCFunctions();
    RegisterNPCFunctions2();
    RegisterNPCFunctions3();
    RegisterNPCFunctions4();
    RegisterNPCFunctions5();
    RegisterNPCFunctions6();

    RegisterConsts();
    sqstd_seterrorhandlers(v);
    sq_setprintfunc(v, printfunc, printfunc); //sets the print function
    sq_pushroottable(v); //push the root table(were the globals of the script will be stored)
    sqstd_register_iolib(v);//Register IO library
    sqstd_register_systemlib(v);//Register System library
    sqstd_register_mathlib(v);//Register Math library
    sqstd_register_stringlib(v);//Register String library
    sqstd_register_bloblib(v);//Register the BLOB library
    register_vectorlib(v);
    register_quaternionlib(v);
    
    bool bDefaultScriptLoaded = true;
    if (!SQ_SUCCEEDED(sqstd_dofile(v, _SC(UNIT), 0, 1)))
        bDefaultScriptLoaded = false;

    bSquirrelVMRunning = true;
    if (SQ_SUCCEEDED(sq_compilebuffer(v, execstring.c_str(), execstring.length(), "execArg", SQTrue)))
    {
        //function pushed in stack
        sq_pushroottable(v);
        if (SQ_SUCCEEDED(sq_call(v, 1, SQFalse, SQTrue)))
        {
        }
        sq_pop(v, 1);//the compiled buffer as function
    }
    if (location.length() > 0)
    {
        char* loc = (char*)malloc(sizeof(char) * location.length()+1);
        if (loc)
        {
            strcpy(loc, location.c_str());
            char* pch = strtok(loc, " ");
            uint8_t flag = 0; std::string val;
            double x, y, z, angle; uint8_t skin = 0;
            uint8_t weapon = 0;
            uint8_t spawnclass = 0;
            while (pch != NULL)
            {
                switch (pch[0])
                {
                case 'x': flag |= 1;//p-656.4 756 11.2
                    x = strtod(pch + 1, NULL);
                    break;
                case 'y': flag |= 2;//p-656.4 756 11.2
                    y = strtod(pch + 1, NULL);
                    break;
                case 'z': flag |= 4;//p-656.4 756 11.2
                    z = strtod(pch + 1, NULL);
                    break;
                case 'a': flag |= 8;
                    angle = strtod(pch + 1, NULL);
                    break;
                case 's': flag |= 16;
                    skin = (uint8_t)strtol(pch + 1, NULL, 10);
                    break;
                case 'w': flag |= 32;
                    weapon = (uint8_t)strtol(pch + 1, NULL, 10);
                    break;
                case 'c':flag |= 64;
                    spawnclass = (uint8_t)strtol(pch + 1, NULL, 10);
                    break;
                }
                pch = strtok(NULL, " ");
            }
            if ((flag & 7) == 7)
            {
                if (flag & 8)
                    iNPC->SetSpawnLocation((float)x, (float)y, (float)z, (float)angle);
                else
                    iNPC->SetSpawnLocation((float)x, (float)y, (float)z);
            }
            if (flag & 16)
                iNPC->SpecialSkin = skin;
            if (flag & 32)
                iNPC->SpawnWeapon = weapon;
            if (flag & 64)
            {
                iNPC->SetSpawnClass(spawnclass);
            }
        }
    }
    return 1;
}
int LoadScript(std::string file, std::vector<std::string> params)
{
    if (file.length() > 0)
    {
        if (SQ_SUCCEEDED(sqstd_dofile(v, _SC(file.c_str()), 0, 1))) // also prints syntax errors if any 
        {
            call_OnNPCScriptLoad(params);
            return 1;
        }
        else return 0;
    }
    else return 1; //npc without any script
}

int StopSquirrel()
{
    call_OnNPCScriptUnload();
    sq_pop(v, 1); //pops the root table
    sq_close(v);
    bSquirrelVMRunning = false;
#ifndef WIN32
    if (bConsoleInputEnabled)
        stop_consoleinput();
#endif
    return 0;
}