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
bool StartSquirrel(std::string file, std::string location, std::vector<std::string> params)
{
    v = sq_open(1024); // creates a VM with initial stack size 1024 
    RegisterNPCFunctions();
    RegisterNPCFunctions2();
    RegisterNPCFunctions3();
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
    bSquirrelVMRunning = true;
    if (location.length() > 0)
    {
        char* loc = (char*)malloc(sizeof(char) * location.length());
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
                iNPC->SpawnClass = spawnclass;
        }
    }

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