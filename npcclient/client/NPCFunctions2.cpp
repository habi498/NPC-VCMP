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
extern CPlayer* npc;
uint8_t GetSlotIdFromWeaponId(uint8_t byteWeapon);
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
        uint8_t weapon = player->GetCurrentWeapon();
        sq_pushinteger(v, weapon);
        return 1;
    }
    sq_pushinteger(v, -1);
    return 1;
}
SQInteger fn_GetPlayerSlotWeapon(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        SQInteger slot;
        sq_getinteger(v, 3, &slot);
        uint8_t weapon = player->GetSlotWeapon(slot);
        sq_pushinteger(v, weapon);
        return 1;
    }
    sq_pushinteger(v, -1);
    return 1;
}
SQInteger fn_GetWeaponSlotFromWeaponId(HSQUIRRELVM v)
{
    SQInteger weaponId;
    sq_getinteger(v, 2, &weaponId);
    uint8_t slotId = GetSlotIdFromWeaponId(weaponId);
    sq_pushinteger(v, slotId);
    return 1;
}
SQInteger fn_GetPlayerSlotAmmo(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        SQInteger slot;
        sq_getinteger(v, 3, &slot);
        uint16_t wAmmo = player->GetSlotAmmo(slot);
        sq_pushinteger(v, wAmmo);
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
        uint16_t vehicleId = player->m_wVehicleId;
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
SQInteger fn_SetTimerEx(HSQUIRRELVM v)
{
    // char * pFuncName, float interval, int maxPulses
    if (sq_gettop(v) < 4)
        return sq_throwerror(v, "Unexpected number of parameters for SetTimerEx [string, integer, integer]");
    else if (sq_gettype(v, 2) != OT_STRING)
        return sq_throwerror(v, "The function name must be given as a string.");
    else if (sq_gettype(v, 3) != OT_INTEGER)
        return sq_throwerror(v, "The interval must be integer.");
    else if (sq_gettype(v, 4) != OT_INTEGER)
        return sq_throwerror(v, "The maximum number of timer pulses must be integer.");

    const SQChar* pFuncName;
    SQInteger maxPulses;
    SQInteger Interval;

    sq_getstring(v, 2, &pFuncName);
    {
        sq_getinteger(v, 3, &Interval);

        sq_getinteger(v, 4, &maxPulses);
    }
    //Check if the function exists
    int top = sq_gettop(v); //saves the stack size before the call
    sq_pushroottable(v); //pushes the global table
    sq_pushstring(v, _SC(pFuncName), -1);
    bool bFuncExists = SQ_SUCCEEDED(sq_get(v, -2));
    sq_settop(v, top); //restores the original stack size
    if (!bFuncExists)  //gets the field 'foo' from the global table
        return sq_throwerror(v, "The given timer callback does not exist.");
    sq_settop(v, top); //restores the original stack size
    if (Interval <= 0)
        return sq_throwerror(v, "The timer's interval must be > 0");
    else if (maxPulses < 0)
        return sq_throwerror(v, "The timer's maximum number of pulses must be >= 0");

    CTimer* pTimer = new CTimer;
    if (sq_gettop(v) > 4)
    {
        pTimer->paramCount = sq_gettop(v) - 4;
        for (int i = 5; i <= sq_gettop(v); i++)
        {
            TimerParam pTempParam;
            pTempParam.datatype = sq_gettype(v, i);
            switch (pTempParam.datatype)
            {
            case OT_INTEGER:
                pTempParam.pData = new SQInteger();
                sq_getinteger(v, i, (SQInteger*)pTempParam.pData);
                break;

            case OT_FLOAT:
                pTempParam.pData = new SQFloat();
                sq_getfloat(v, i, (SQFloat*)pTempParam.pData);
                break;

            case OT_BOOL:
                pTempParam.pData = new SQBool();
                sq_getbool(v, i, (SQBool*)pTempParam.pData);
                break;

            case OT_STRING:
            {
                const SQChar* pString = NULL;
                sq_getstring(v, i, &pString);
                size_t len = strlen(pString);
                char* data = (char*)malloc((len + 1) * sizeof(SQChar));
                if (data)
                {
                    strcpy(data, pString);
                    pTempParam.pData = data;
                }

                break;
            }

            /*case OT_TABLE:
            case OT_ARRAY:
            case OT_CLASS:*/
            case OT_INSTANCE:
                SQBool IsVector;
                if (SQ_SUCCEEDED(sq_isvector(v, i, &IsVector)))
                {
                    if (IsVector)
                    {
                        VECTOR* vector = new VECTOR();
                        if (SQ_SUCCEEDED(sq_getvector(v, i, vector)))
                        {
                            pTempParam.pData = vector;
                            pTempParam.datatype = (SQObjectType)0x00040000;
                            break;
                        }
                        else delete vector;
                    }
                    else
                    {
                        SQBool IsQuaternion;
                        if (SQ_SUCCEEDED(sq_isquaternion(v, i, &IsQuaternion)))
                        {
                            if (IsQuaternion)
                            {
                                QUATERNION* quaternion = new QUATERNION();
                                if (SQ_SUCCEEDED(sq_getquaternion(v, i, quaternion)))
                                {
                                    pTempParam.pData = quaternion;
                                    pTempParam.datatype = (SQObjectType)0x00080000;
                                    break;
                                }
                                else delete quaternion;
                            }
                        }
                    }
                }
            case OT_USERDATA:
            case OT_USERPOINTER:
            case OT_CLOSURE:
            case OT_NATIVECLOSURE:
            {
                HSQOBJECT* o = new HSQOBJECT();
                sq_getstackobj(v, i, o);
                pTempParam.pData = o;
                break;
            }

            case OT_NULL:
            default:
                break;
            }

            pTimer->params.push_back(pTempParam);
        }

    }//else of get_top > 4
    pTimer->pFunc = const_cast<SQChar*>(pFuncName);
    pTimer->intervalInTicks = Interval;
    
    pTimer->maxNumberOfPulses = maxPulses;
    pTimer->SetAlarmParam();//Equivalent of calling _Update in ordinary 
    int TimerID = AddTimer(pTimer);
    sq_pushinteger(v, TimerID);
    return 1;//we return timer id*/
}
SQInteger fn_KillTimer(HSQUIRRELVM v)
{
    SQInteger TimerID;
    sq_getinteger(v, 2, &TimerID);
    KillTimer(TimerID);
    return 0;//return nothing
}
SQInteger fn_GetPlayerSkin(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t skin = player->m_byteSkinId;
        sq_pushinteger(v, skin);
        return 1;
    }
    sq_pushinteger(v, -1);
    return 1;
}
SQInteger fn_GetPlayerTeam(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t team = player->m_byteTeamId;
        sq_pushinteger(v, team);
        return 1;
    }
    sq_pushinteger(v, -1);
    return 1;
}
SQInteger fn_GetClosestPlayer(HSQUIRRELVM v)
{
    SQBool bCheckTeam;
    if (sq_gettop(v) == 2)
    {
        sq_getbool(v, 2, &bCheckTeam);
    }
    else bCheckTeam = false;
    float fMinDis = 1000, dis;
    uint8_t closesplrid = 255;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (i == iNPC->GetID())continue;
        CPlayer* plr = m_pPlayerPool->GetAt(i);
        
        if (plr && plr->IsStreamedIn())
        {
            if (bCheckTeam &&
                plr->m_byteTeamId == npc->m_byteTeamId)
                continue;//same team
            dis = (plr->m_vecPos - npc->m_vecPos).GetMagnitude();
            if (dis < fMinDis)
            {
                closesplrid = i;
                fMinDis = dis;
            }
        }
    }
   sq_pushinteger(v, closesplrid);
   return 1;
}

SQInteger fn_GetPlayerPos(HSQUIRRELVM v)
{
    SQInteger bytePlayerId;
    sq_getinteger(v, 2, &bytePlayerId);
    if (m_pPlayerPool->GetSlotState(bytePlayerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
        VECTOR pos = player->m_vecPos;
        if (SQ_SUCCEEDED(sq_pushvector(v, pos)))
            return 1;
        else
            return sq_throwerror(v, "Error occured when getting vector");
    }
    else return 0;
}
SQInteger fn_IsPlayerInRangeOfPoint(HSQUIRRELVM v)
{
    //tif|ix
    SQFloat range; SQInteger temp;
    if (sq_gettype(v, 3) == OT_INTEGER)
    {
        sq_getinteger(v, 3, &temp);
        range = (SQFloat)temp;
    }
    else if (sq_gettype(v, 3) == OT_FLOAT)
    {
        sq_getfloat(v, 3, &range);
    }
    else return 0;
    SQInteger bytePlayerId;
    sq_getinteger(v, 2, &bytePlayerId);

    VECTOR point;
    if (!SQ_SUCCEEDED(sq_getvector(v, 4, &point)))
    {
        return sq_throwerror(v, "Parameter 3 should be Vector");
    }
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        VECTOR pos = player->m_vecPos;
        VECTOR diff = pos - point;
        if (diff.GetMagnitude() <= range)
        {
            sq_pushbool(v, SQTrue);
            return 1;
        }
        else
        {
            sq_pushbool(v, SQFalse);
            return 1;
        }
    }
    else return 0;
}
SQInteger fn_GetPlayerColor(HSQUIRRELVM v)
{
    SQInteger bytePlayerId;
    sq_getinteger(v, 2, &bytePlayerId);
    if (m_pPlayerPool->GetSlotState(bytePlayerId))
    {
        CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
        Color col = player->GetColor();
        sq_newtable(v);
        sq_pushstring(v, "r", -1);
        sq_pushinteger(v, col.r);
        sq_newslot(v, -3, SQFalse);
        sq_pushstring(v, "g", -1);
        sq_pushinteger(v, col.g);
        sq_newslot(v, -3, SQFalse);
        sq_pushstring(v, "b", -1);
        sq_pushinteger(v, col.b);
        sq_newslot(v, -3, SQFalse);
        return 1;
    }
    else return 0;
}
SQInteger fn_GetMyColor(HSQUIRRELVM v)
{
    if(npc)
    {
        Color col = npc->GetColor();
        sq_newtable(v);
        sq_pushstring(v, "r", -1);
        sq_pushinteger(v, col.r);
        sq_newslot(v, -3, SQFalse);
        sq_pushstring(v, "g", -1);
        sq_pushinteger(v, col.g);
        sq_newslot(v, -3, SQFalse);
        sq_pushstring(v, "b", -1);
        sq_pushinteger(v, col.b);
        sq_newslot(v, -3, SQFalse);
        return 1;
    }
    else return 0;
}
void RegisterNPCFunctions2()
{
	register_global_func(v, ::fn_IsPlayerConnected, "IsPlayerConnected", 2, "ti");
	register_global_func(v, ::fn_GetPlayerName, "GetPlayerName", 2, "ti");
	register_global_func(v, ::fn_GetPlayerState, "GetPlayerState", 2, "ti");
	register_global_func(v, ::fn_GetPlayerVehicleID, "GetPlayerVehicleID", 2, "ti");
	register_global_func(v, ::fn_GetPlayerArmedWeapon, "GetPlayerArmedWeapon", 2, "ti");
	
    register_global_func(v, ::fn_GetPlayerSlotWeapon, "GetPlayerSlotWeapon", 3, "tii");
	register_global_func(v, ::fn_GetPlayerSlotAmmo, "GetPlayerSlotAmmo", 3, "tii");
    register_global_func(v, ::fn_GetWeaponSlotFromWeaponId, "GetSlotFromWeaponId", 2, "ti");

	register_global_func(v, ::fn_GetPlayerHealth, "GetPlayerHealth", 2, "ti");
	register_global_func(v, ::fn_GetPlayerArmour, "GetPlayerArmour", 2, "ti");
	register_global_func(v, ::fn_GetPlayerKeys, "GetPlayerKeys", 2, "ti");
	register_global_func(v, ::fn_GetPlayerFacingAngle, "GetPlayerFacingAngle", 2, "ti");
	register_global_func(v, ::fn_GetPlayerPosX, "GetPlayerPosX", 2, "ti");
	register_global_func(v, ::fn_GetPlayerPosY, "GetPlayerPosY", 2, "ti");
	register_global_func(v, ::fn_GetPlayerPosZ, "GetPlayerPosZ", 2, "ti");
	register_global_func(v, ::fn_SetTimer, "SetTimer", 4, "tsii");
	register_global_func(v, ::fn_KillTimer, "KillTimer", 2, "ti");
	
    register_global_func(v, ::fn_SetTimerEx, "SetTimerEx", 0, "");
    register_global_func(v, ::fn_GetPlayerSkin, "GetPlayerSkin", 2, "ti");
    register_global_func(v, ::fn_GetPlayerTeam, "GetPlayerTeam", 2, "ti");

    register_global_func(v, ::fn_GetClosestPlayer, "GetClosestPlayer", -1, "tb");
    register_global_func(v, ::fn_GetPlayerPos, "GetPlayerPos", 2, "ti");
    register_global_func(v, ::fn_IsPlayerInRangeOfPoint, "IsPlayerInRangeOfPoint", 4, "tif|ix");

    register_global_func(v, ::fn_GetPlayerColor, "GetPlayerColor", 2, "ti");
    register_global_func(v, ::fn_GetMyColor, "GetMyColor", 1, "t");


}