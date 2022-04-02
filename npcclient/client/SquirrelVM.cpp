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
void call_OnNPCScriptLoad()
{
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC("OnNPCScriptLoad"), -1);
    if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
        sq_pushroottable(v); //push the 'this' (in this case is the global table)
        sq_call(v, 1, 0, 1); //calls the function 
    }
    sq_settop(v, top); //restores the original stack size
}
bool StartSquirrel(std::string file)
{
    v = sq_open(1024); // creates a VM with initial stack size 1024 
    RegisterNPCFunctions();
    RegisterNPCFunctions2();
    RegisterConsts();
    sqstd_seterrorhandlers(v);
    sq_setprintfunc(v, printfunc, printfunc); //sets the print function
    sq_pushroottable(v); //push the root table(were the globals of the script will be stored)
    sqstd_register_iolib(v);//Register IO library
    sqstd_register_systemlib(v);//Register System library
    sqstd_register_mathlib(v);//Register Math library
    sqstd_register_stringlib(v);//Register String library
    sqstd_register_bloblib(v);//Register the BLOB library
    if (!SQ_SUCCEEDED(sqstd_dofile(v, _SC(UNIT1), 0, 1)))
        return 0;
    if (!SQ_SUCCEEDED(sqstd_dofile(v, _SC(UNIT2), 0, 1)))
        return 0;
    if (!SQ_SUCCEEDED(sqstd_dofile(v, _SC(UNIT3), 0, 1)))
        return 0;
    if (SQ_SUCCEEDED(sqstd_dofile(v, _SC(file.c_str()), 0, 1))) // also prints syntax errors if any 
    {
        call_OnNPCScriptLoad();
        return 1;
    }
    else return 0;
}
int StopSquirrel()
{
    call_OnNPCScriptUnload();
    sq_pop(v, 1); //pops the root table
    sq_close(v);
    return 0;
}