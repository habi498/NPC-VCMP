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
#include "main.h"
extern HSQUIRRELVM v;
extern NPC* iNPC;
extern CPlayerPool* m_pPlayerPool;
extern CPlayer* npc;
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
uint8_t GetSlotId(uint8_t byteWeapon);
//05-Nov 22
void SendNPCSyncData(ONFOOT_SYNC_DATA* m_pOfSyncData);
void SendNPCOfSyncData();
SQInteger fn_SendOnFootSyncData(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    //i fff f i i i fff fff fff
    SQInteger dwKeys;
    sq_getinteger(v, 2, &dwKeys);
    //Position
    SQFloat x, y, z;
    sq_getfloat(v, 3, &x);
    sq_getfloat(v, 4, &y);
    sq_getfloat(v, 5, &z);

    SQFloat fAngle;
    sq_getfloat(v, 6, &fAngle);

    SQInteger byteHealth;
    sq_getinteger(v, 7, &byteHealth);

    SQInteger byteArmour;
    sq_getinteger(v, 8, &byteArmour);

    SQInteger byteCurrentWeapon;
    sq_getinteger(v, 9, &byteCurrentWeapon);

    SQInteger wAmmo;
    sq_getinteger(v, 10, &wAmmo);

    //Speed
    SQFloat vx, vy, vz;
    sq_getfloat(v, 11, &vx);
    sq_getfloat(v, 12, &vy);
    sq_getfloat(v, 13, &vz);


    //Aim Position
    SQFloat s, t, u;
    sq_getfloat(v, 14, &s);
    sq_getfloat(v, 15, &t);
    sq_getfloat(v, 16, &u);

    //Aim Direction
    SQFloat p, q, r;
    sq_getfloat(v, 17, &p);
    sq_getfloat(v, 18, &q);
    sq_getfloat(v, 19, &r);

    //IsCrouching
    SQBool bIsCrouching;
    sq_getbool(v, 20, &bIsCrouching);

    //IsReloading
    SQBool bIsReloading;
    sq_getbool(v, 21, &bIsReloading);

    ONFOOT_SYNC_DATA m_pOfSyncData;
    m_pOfSyncData.byteArmour = (uint8_t)byteArmour;
    m_pOfSyncData.byteCurrentWeapon = (uint8_t)byteCurrentWeapon;
    m_pOfSyncData.byteHealth = (uint8_t)byteHealth;
    m_pOfSyncData.dwKeys = (uint32_t)dwKeys;
    m_pOfSyncData.fAngle = fAngle;
    if ((dwKeys & 512) == 512)
        m_pOfSyncData.IsPlayerUpdateAiming = true;
    else if ((dwKeys & 1) == 1 && byteCurrentWeapon > 11)
        m_pOfSyncData.IsPlayerUpdateAiming = true;
    else 
        m_pOfSyncData.IsPlayerUpdateAiming = false;
    m_pOfSyncData.IsCrouching = bIsCrouching != 0 ? true : false;
    m_pOfSyncData.vecAimDir.X = p;
    m_pOfSyncData.vecAimDir.Y = q;
    m_pOfSyncData.vecAimDir.Z = r;

    m_pOfSyncData.vecAimPos.X = s;
    m_pOfSyncData.vecAimPos.Y = t;
    m_pOfSyncData.vecAimPos.Z = u;

    m_pOfSyncData.vecPos.X = x;
    m_pOfSyncData.vecPos.Y = y;
    m_pOfSyncData.vecPos.Z = z;

    m_pOfSyncData.vecSpeed.X = vx;
    m_pOfSyncData.vecSpeed.Y = vy;
    m_pOfSyncData.vecSpeed.Z = vz;

    m_pOfSyncData.wAmmo = static_cast<uint16_t>(wAmmo);
    m_pOfSyncData.bIsReloading = static_cast<bool>(bIsReloading);
	//if bIsReloading is provided then set off the fire key
    if (bIsReloading && ( (dwKeys & 512) == 512))
        dwKeys = dwKeys & (~(1 << 9));//2^9 = 512. Set 9th bit from right off.
    m_pOfSyncData.dwKeys = (uint32_t)dwKeys;
    SendNPCSyncData(&m_pOfSyncData);
    return 0;//0 because we are returning nothing!
}
SQInteger fn_FireSniperRifle(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger weapon;
    float x, y, z, dx, dy, dz;
    // x y x aiming position
    sq_getinteger(v, 2, &weapon);
    sq_getfloat(v, 3, &x);
    sq_getfloat(v, 4, &y);
    sq_getfloat(v, 5, &z);
    sq_getfloat(v, 6, &dx);
    sq_getfloat(v, 7, &dy);
    sq_getfloat(v, 8, &dz);
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_SNIPERFIRE));
    bsOut.Write((char)weapon);
    bsOut.Write(z);// z first
    bsOut.Write(y);
    bsOut.Write(x);
    bsOut.Write(dz);
    bsOut.Write(dy);
    bsOut.Write(dx);
    peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false);
    return 0;
}
SQInteger fn_SendShotInfo(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger bodypart, animation;
    sq_getinteger(v, 2, &bodypart);
    sq_getinteger(v, 3, &animation);
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_PLAYER_SHOT_BODYPART));
    char nbp; //new body part
    switch (bodypart)
    {
    case 6: nbp = 2; break;
    case 1: nbp = 1; break;
    case 2: nbp = 3; break;
    case 3: nbp = 4; break;
    case 4: nbp = 7; break;
    case 5: nbp = 8; break;
    case 0: nbp = (uint8_t)255; break;
    default: nbp = bodypart; break;
    }
    bsOut.Write(nbp);
    bsOut.Write((char)animation);
    peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false);
    npc->m_byteArmour = 0;
    npc->m_byteHealth = 0;
    npc->SetKeys(0);
    return 0;
}
SQInteger fn_GetLocalValue(HSQUIRRELVM v)
{
    if (!npc)return 0;
    SQInteger field;
    sq_getinteger(v, 2, &field);
    switch (field)
    {
    case I_KEYS: sq_pushinteger(v, npc->GetKeys());
        break;
    case I_HEALTH: sq_pushinteger(v, npc->m_byteHealth);
        break;
    case I_ARMOUR: sq_pushinteger(v, npc->m_byteArmour);
        break;
    case I_CURWEP: sq_pushinteger(v, npc->GetCurrentWeapon());
        break;
    case I_CURWEP_AMMO: sq_pushinteger(v, npc->GetSlotAmmo(GetSlotId(npc->GetCurrentWeapon())));
        break;
    case F_POSX: sq_pushfloat(v, npc->m_vecPos.X);
        break;
    case F_POSY: sq_pushfloat(v, npc->m_vecPos.Y);
        break;
    case F_POSZ: sq_pushfloat(v, npc->m_vecPos.Z);
        break;
    case F_ANGLE: sq_pushfloat(v, npc->m_fAngle);
        break;
    default: return 0;
    }
    return 1;// we are returning a value;
}
SQInteger fn_SetLocalValue(HSQUIRRELVM v)
{
    if (!npc)return 0;
    SQInteger field;
    sq_getinteger(v, 2, &field);
    SQFloat value;
    if (sq_gettype(v, 3) == OT_INTEGER)
    {
        SQInteger temp;
        sq_getinteger(v, 3, &temp);
        value = static_cast<float>(temp);
    }
    else if (sq_gettype(v, 3) == OT_FLOAT)
    {
        sq_getfloat(v, 3, &value);
    }
    else return 0;
    uint8_t byteNewWeapon, byteSlot;
    switch (field)
    {
    case I_KEYS: npc->SetKeys(static_cast<uint32_t>(value));
        break;
    case I_HEALTH: npc->m_byteHealth = static_cast<uint8_t> (value);
        break;
    case I_ARMOUR: npc->m_byteArmour = static_cast<uint8_t> (value);
        break;
    case I_CURWEP: 
        byteNewWeapon = static_cast<uint8_t>(value);
        byteSlot = GetSlotId(byteNewWeapon);
        if (npc->GetSlotWeapon(byteSlot) == byteNewWeapon)
           npc->GetONFOOT_SYNC_DATA()->byteCurrentWeapon = byteNewWeapon;
        else return 0;
        break;
    case I_CURWEP_AMMO: 
        npc->SetCurrentWeaponAmmo(static_cast<uint16_t>(value));
        break; 
    case F_POSX: npc->m_vecPos.X = static_cast<float>(value);
        break;
    case F_POSY: npc->m_vecPos.Y = static_cast<float>(value);
        break;
    case F_POSZ: npc->m_vecPos.Z = static_cast<float>(value);
        break;
    case F_ANGLE: npc->m_fAngle = static_cast<float>(value);
        break;
    default: return 0;
    }
    sq_pushbool(v, SQTrue);
    return 1;
}
SQInteger fn_SendOnFootSyncDataLV(HSQUIRRELVM v)
{
    if (!npc)return 0;
    SendNPCOfSyncDataLV();
    return 0;
}
SQInteger fn_SendDeathInfo(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger weapon, killerid, bodypart;
    sq_getinteger(v, 2, &weapon);
    sq_getinteger(v, 3, &killerid);
    sq_getinteger(v, 4, &bodypart);
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_DEATH_INFO));
    bsOut.Write((char)weapon);
    bsOut.Write((char)killerid);
    bsOut.Write((char)bodypart);
    peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false);
    npc->SetState(PLAYER_STATE_WASTED);
    iNPC->SetSpawnStatus( false );
    iNPC->ClassSelectionCounter = 0;
    return 0;
}
SQInteger fn_FaceNPCToPlayer(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        //player is connected
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t state = player->GetState();
        if (state == PLAYER_STATE_ONFOOT ||
            state == PLAYER_STATE_AIM)
        {
            VECTOR vecPos = player->m_vecPos;
            VECTOR vecnpcPos = npc->m_vecPos;
            float angle = (float)atan2((float)(-(vecPos.X - vecnpcPos.X) ),
                (float)(vecPos.Y - vecnpcPos.Y));
            npc->m_fAngle = angle;
            SendNPCOfSyncDataLV();
            sq_pushbool(v, SQTrue);
            return 1;
        }
    }
    sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_WhereIsPlayer(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        //player is connected
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t state = player->GetState();
        if (state == PLAYER_STATE_ONFOOT || 
            state == PLAYER_STATE_AIM )
        {
            VECTOR vecPos = player->m_vecPos;
            VECTOR vecnpcPos = npc->m_vecPos;
            float theta = (float)atan2((float)(-(vecPos.X - vecnpcPos.X)),
                (float)(vecPos.Y - vecnpcPos.Y)); //npcs angle to player
            float alpha = npc->m_fAngle;//current angle of npc;
            float change = theta - alpha; //this much is the difference
            float beta;
            if (change > 0 && change < PI)
                beta = change; //positive
            else if (change > 0 && change > PI)
                beta = change - 2 * (float) PI; //negative
            else if (change <0 && change > -PI)
                beta = change; //negative
            else if (change < 0 && change < -PI)
                beta = change + 2 * (float) PI;//positive
            else if (change == PI || change == -PI)
                beta = change; //does not matter
            else if (change == 0)
                beta = 0;
            /* beta is like npc's angle, but considering
             the player as north pole */
            /* Or simply, beta is zero  means player is in front.
            is PI or -PI means is in the back
            and if it is positive it means player is on the left
            and negative means player is on the right*/
            sq_pushfloat(v, beta);
            return 1;
        }
    }
    return 0;
}



void RegisterNPCFunctions3()
{
    register_global_func(v, ::fn_SendOnFootSyncData, "SendOnFootSyncData", 21, "tiffffiiiifffffff|if|if|ibb");
    register_global_func(v, ::fn_FireSniperRifle, "FireSniperRifle", 8, "tiffff|if|if|i");
    //register_global_func(v, ::fn_FireProjectile, "FireProjectile", 7, "tifffff");
    register_global_func(v, ::fn_SendShotInfo, "SendShotInfo", 3, "tii");
    register_global_func(v, ::fn_SetLocalValue, "SetLocalValue", 3, "tii|f");
    register_global_func(v, ::fn_GetLocalValue, "GetLocalValue", 2, "ti");
    register_global_func(v, ::fn_SendOnFootSyncDataLV, "SendOnFootSyncDataLV", 1, "t");
    register_global_func(v, ::fn_SendDeathInfo, "SendDeathInfo", 4, "tiii");
    register_global_func(v, ::fn_FaceNPCToPlayer, "FaceNPCToPlayer", 2, "ti");
    register_global_func(v, ::fn_WhereIsPlayer, "WhereIsPlayer", 2, "ti");
}