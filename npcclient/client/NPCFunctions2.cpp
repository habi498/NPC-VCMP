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
extern CPlayerPool* m_pPlayerPool;
extern NPC* iNPC;
SQInteger fn_GetPlayerPosZ(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        VECTOR pos = player->m_vecPos;
        sq_pushfloat(v, pos.Z);
        return 1;
    }
    return 0;
}
SQInteger fn_GetPlayerPosY(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        VECTOR pos = player->m_vecPos;
        sq_pushfloat(v, pos.Y);
        return 1;
    }
    return 0;
}
SQInteger fn_GetPlayerPosX(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        VECTOR pos = player->m_vecPos;
        sq_pushfloat(v, pos.X);
        return 1;
    }
    return 0;
}
SQInteger fn_GetPlayerFacingAngle(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        float angle = player->m_fAngle;
        sq_pushfloat(v, angle);
        return 1;
    }
    return 0;
}
SQInteger fn_GetPlayerKeys(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint32_t keys = player->GetKeys();
        sq_pushinteger(v, keys);
        return 1;
    }
    sq_pushinteger(v, -1);
    return 1;
}
SQInteger fn_GetPlayerArmour(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t armour = player->m_byteArmour;
        sq_pushinteger(v, armour);
        return 1;
    }
    sq_pushinteger(v, -1);
    return 1;
}
SQInteger fn_GetPlayerHealth(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t health = player->m_byteHealth;
        sq_pushinteger(v, health);
        return 1;
    }
    sq_pushinteger(v, -1);
    return 1;
}
SQInteger fn_GetPlayerArmedWeapon(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t weapon = player->byteCurWeap;
        sq_pushinteger(v, weapon);
        return 1;
    }
    sq_pushinteger(v, -1);
    return 1;
}
SQInteger fn_GetPlayerVehicleID(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint16_t vehicleId = player->m_VehicleID;
        sq_pushinteger(v, vehicleId);
        return 1;
    }
    sq_pushinteger(v, 0);
    return 1;
}
SQInteger fn_GetPlayerState(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t state=player->GetState();
        sq_pushinteger(v, state);
        return 1;
    }
    return 0;
}
SQInteger fn_IsPlayerConnected(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    bool isConn = m_pPlayerPool->GetSlotState(playerId);
    sq_pushbool(v, isConn);
    return 1;
}
SQInteger fn_GetPlayerName(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        char* name=m_pPlayerPool->GetPlayerName(playerId);
        sq_pushstring(v, name, strlen(name));
        return 1;
    }
    //sq_pushnull(v);
    return 0;
}
SQInteger fn_SetTimer(HSQUIRRELVM v)
{
    const SQChar* szFuncName;
    SQInteger interval;
    SQInteger Repeat;
    sq_getstring(v, 2, &szFuncName);
    sq_getinteger(v, 3, &interval);
    sq_getinteger(v, 4, & Repeat);
    CTimer* timer = new CTimer(interval, szFuncName, Repeat);
    int TimerID=AddTimer(timer);
    sq_pushinteger(v, TimerID);
    return 1;//we return timer id
}
SQInteger fn_KillTimer(HSQUIRRELVM v)
{
    SQInteger TimerID;
    sq_getinteger(v, 2, &TimerID);
    KillTimer(TimerID);
    return 0;//return nothing
}
void RegisterNPCFunctions2()
{
	register_global_func(v, ::fn_IsPlayerConnected, "IsPlayerConnected", 2, "ti");
	register_global_func(v, ::fn_GetPlayerName, "GetPlayerName", 2, "ti");
	register_global_func(v, ::fn_GetPlayerState, "GetPlayerState", 2, "ti");
	register_global_func(v, ::fn_GetPlayerVehicleID, "GetPlayerVehicleID", 2, "ti");
	register_global_func(v, ::fn_GetPlayerArmedWeapon, "GetPlayerArmedWeapon", 2, "ti");
	register_global_func(v, ::fn_GetPlayerHealth, "GetPlayerHealth", 2, "ti");
	register_global_func(v, ::fn_GetPlayerArmour, "GetPlayerArmour", 2, "ti");
	register_global_func(v, ::fn_GetPlayerKeys, "GetPlayerKeys", 2, "ti");
	register_global_func(v, ::fn_GetPlayerFacingAngle, "GetPlayerFacingAngle", 2, "ti");
	register_global_func(v, ::fn_GetPlayerPosX, "GetPlayerPosX", 2, "ti");
	register_global_func(v, ::fn_GetPlayerPosY, "GetPlayerPosY", 2, "ti");
	register_global_func(v, ::fn_GetPlayerPosZ, "GetPlayerPosZ", 2, "ti");
	register_global_func(v, ::fn_SetTimer, "SetTimer", 4, "tsii");
	register_global_func(v, ::fn_KillTimer, "KillTimer", 2, "ti");

}