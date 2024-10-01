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
#include <stdio.h>
#include <string.h>
extern HSQUIRRELVM v;
extern NPC* iNPC;
extern CPlayer* npc;
extern CFunctions* m_pFunctions;
extern CVehiclePool* m_pVehiclePool;
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
extern CPlayerPool* m_pPlayerPool;
uint8_t GetSlotIdFromWeaponId(uint8_t byteWeapon);
Playback mPlayback;
SQInteger register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char* fname, SQInteger nparamscheck, const SQChar* typemask)
{
    sq_pushroottable(v);
    sq_pushstring(v, fname, -1);
    sq_newclosure(v, f, 0); //create a new function
    if (nparamscheck != 0) {
        
        sq_setparamscheck(v, nparamscheck, typemask); /* Add a param type check */
    }
    sq_setnativeclosurename(v, -1, fname);
    sq_createslot(v, -3);
    sq_pop(v, 1); //pops the root table
    return 0;
}
SQInteger fn_IsPlayerStreamedIn(HSQUIRRELVM v)
{
    SQInteger playerid;
    sq_getinteger(v, 2, &playerid);
    CPlayer* player = m_pPlayerPool->GetAt(playerid);
    if (player && player->IsStreamedIn())
        sq_pushbool(v, SQTrue);
    else 
        sq_pushbool(v, SQFalse);
    return 1;//return 1 because value is returned
}
SQInteger fn_IsVehicleStreamedIn(HSQUIRRELVM v)
{
    SQInteger vhclid;
    sq_getinteger(v, 2, &vhclid);
    CVehicle* vehicle = m_pVehiclePool->GetAt(vhclid);
    if (vehicle)
        sq_pushbool(v, SQTrue);
    else 
        sq_pushbool(v, SQFalse);
    return 1;//return 1 because value is returned
}
SQInteger fn_StartRecordingPlayback(HSQUIRRELVM v)
{
    SQInteger nargs = sq_gettop(v); //number of arguments
    SQInteger type;//play back type
    const SQChar* filename;
    sq_getinteger(v, 2, &type);
    sq_getstring(v, 3, &filename);
    SQInteger flags = 0;
    if (sq_gettop(v) > 3 )
    {
        if (sq_gettype(v, 4) == OT_INTEGER)
        {
            sq_getinteger(v, 4, &flags);
        }
        else return sq_throwerror(v, "Flags parameter must be integer");
    }
    if (mPlayback.running == true)return sq_throwerror(v, "Playback already running");
    mPlayback.pFile = fopen(std::string(REC_DIR+ std::string("/") + std::string(filename)+std::string(".rec")).c_str(), "rb");
    if (mPlayback.pFile == NULL)
    {
        printf("Failed to open %s", filename);
        return 0;
    }
    
    int identifier;
    size_t m=fread(&identifier, sizeof(int), 1, mPlayback.pFile);
    if (m != 1)return 0;
    if (
        identifier != NPC_RECFILE_IDENTIFIER_V4 && identifier!=NPC_RECFILE_IDENTIFIER_V5
        )
    {
        if (identifier == NPC_RECFILE_IDENTIFIER_V1||
        identifier == NPC_RECFILE_IDENTIFIER_V2||
            identifier== NPC_RECFILE_IDENTIFIER_V3
        //    ||identifier==NPC_RECFILE_IDENTIFIER_V4||
          //  identifier==NPC_RECFILE_IDENTIFIER_V5
            )
        {
            printf("This rec file cannot be played by this version of program. Use program recupdate.exe to convert playback recordings automatically.\n");
            return 0;
        }
        //NOT NPC Recording
        printf("File format different. Cannot start playback\n");
        return 0;
    }
    mPlayback.identifier = identifier;
    mPlayback.dw_Flags = (uint32_t)flags;
    int rectype;
    m = fread(&rectype, sizeof(int), 1, mPlayback.pFile);
    if (m != 1)return 0;
    if (rectype != type)
    {
        printf("Recording type mismatch. Aborting playback\n");
        return 0;
    }
    mPlayback.type = type;
    if(identifier==NPC_RECFILE_IDENTIFIER_V5)
    {
        uint32_t flags;
        //read flags
        m = fread(&flags, sizeof(int), 1, mPlayback.pFile);
        if (m != 1)return 0;
        if (flags & 1)
        {
            //Name is present
            char name[24];
            m = fread(name, sizeof(char), sizeof(name), mPlayback.pFile);
            if(m!=(sizeof(name)))return 0;
            //what to do with name?
        }
    }
    mPlayback.nexttick = 0;
    DWORD tick;
    size_t result=fread(&tick, sizeof(tick), 1, mPlayback.pFile);
    if (result != 1)
    {
        mPlayback.Abort();
        return 0;
    }
    else
    {
        mPlayback.prevtick = tick;
        mPlayback.running = true;
        fseek(mPlayback.pFile, -1 * (long)sizeof(DWORD), SEEK_CUR);
        sq_pushbool(v, SQTrue);
        return 1;
    }
}
SQInteger fn_IsPlaybackRunning(HSQUIRRELVM v)
{
    if (mPlayback.running == false)
        sq_pushbool(v, SQFalse);
    else if (mPlayback.IsPaused())
        sq_pushbool(v, SQFalse);
    else
        sq_pushbool(v, SQTrue);
    return 1;
}
SQInteger fn_IsPlaybackPaused(HSQUIRRELVM v)
{
    if (mPlayback.running == false)
        sq_pushbool(v, SQFalse);
    else if (mPlayback.IsPaused())
        sq_pushbool(v, SQTrue);
    else
        sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_SetMyFacingAngle(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    SQFloat angle;
    sq_getfloat(v, 2, &angle);
    npc->m_fAngle = angle;
    SendNPCOfSyncDataLV();
    return 0;//0 because we are returning nothing
}
SQInteger fn_SelectWeapon(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    SQInteger  weapon;
    sq_getinteger(v, 2, &weapon);
    funcError e= m_pFunctions->SetWeapon(weapon,true);
    if (e == funcError::NoError)sq_pushbool(v, true);
    else sq_pushbool(v, false);
    return 1;
}
SQInteger fn_GetAvailableWeapons(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    sq_newarray(v, 0);
    int npcid = iNPC->GetID(); uint8_t j;
    for (int i = 0; i <= 9; i++)
    {
        j = m_pFunctions->GetPlayerWeaponAtSlot(npcid, i);
        if (j != 0)
        {
            sq_pushinteger(v, j);
            sq_arrayappend(v, -2);
        }
    }
    return 1;
}
SQInteger fn_GetAmmoOfMyWeapon(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    SQInteger weapon;
    sq_getinteger(v, 2, &weapon);
    int slotid = GetSlotIdFromWeaponId(weapon);
    if (slotid == 0)
    {
        sq_pushinteger(v, 0);
        return 1;
    }
    else
    {
        uint8_t w=m_pFunctions->GetPlayerWeaponAtSlot(iNPC->GetID(), slotid);
        if (w != weapon)
        {
            sq_pushinteger(v, 0);
            return 1;
        }
        uint16_t ammo = m_pFunctions->GetPlayerAmmoAtSlot(iNPC->GetID(), slotid);
        funcError e = m_pFunctions->GetLastError();
        if (e == funcError::NoError)
            sq_pushinteger(v, ammo);
        else
            sq_pushinteger(v, 0);//some error occured
    }
    return 1;
}
SQInteger fn_PauseRecordingPlayback(HSQUIRRELVM v)
{
    if (mPlayback.running == true)
    {
        mPlayback.Pause();
        sq_pushbool(v, SQTrue);
        return 1;
    }
    return 0;//0 because we are returning nothing
}
SQInteger fn_ResumeRecordingPlayback(HSQUIRRELVM v)
{
    if (mPlayback.running == true)
    {
        mPlayback.Resume();
        sq_pushbool(v, SQTrue);
        return 1;
    }
    return 0;//0 because we are returning nothing
}
SQInteger fn_StopRecordingPlayback(HSQUIRRELVM v)
{
    if (mPlayback.running == true)
    {   
        mPlayback.Stop();
        sq_pushbool(v, SQTrue);
        return 1;
    }
    return 0;//0 because we are returning nothing
}
SQInteger fn_GetMyFacingAngle(HSQUIRRELVM v)
{
    if (!npc) {
        sq_pushfloat(v, 0.0); return 1;
    }
    float angle = npc->m_fAngle;
    sq_pushfloat(v, angle);
    return 1;//1 because we are returning angle
}
SQInteger fn_SetMyPos2(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    SQFloat x, y, z;
    sq_getfloat(v, 2, &x);
    sq_getfloat(v, 3, &y);
    sq_getfloat(v, 4, &z);

    npc->m_vecPos.X = x; npc->m_vecPos.Y = y; npc->m_vecPos.Z = z;
    SendNPCOfSyncDataLV();
    return 0;//0 because we are returning nothing!
}
SQInteger fn_SetMyPos(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    //vx
    VECTOR pos;
    if (SQ_SUCCEEDED(sq_getvector(v, 2, &pos)))
    {
        npc->m_vecPos = pos;
        SendNPCOfSyncDataLV();
        sq_pushbool(v, SQTrue);
        return 1;
    }
    else 
        return 0;//0 because we are returning nothing!
}
SQInteger fn_GetMyPosX(HSQUIRRELVM v)
{
    if (!npc) {
        sq_pushfloat(v, 0.0); return 1;
    }
    SQFloat val = npc->m_vecPos.X;
    sq_pushfloat(v, val);
    return 1;
}
SQInteger fn_GetMyPosY(HSQUIRRELVM v)
{
    if (!npc) {
        sq_pushfloat(v, 0.0); return 1;
    }
    SQFloat val = npc->m_vecPos.Y;
    sq_pushfloat(v, val);
    return 1;
}
SQInteger fn_GetMyPosZ(HSQUIRRELVM v)
{
    if (!npc) {
        sq_pushfloat(v, 0.0); return 1;
    }
    SQFloat val = npc->m_vecPos.Z;
    sq_pushfloat(v, val);
    return 1;
}
SQInteger fn_GetMyPos(HSQUIRRELVM v)
{
    if (npc)
    {
        VECTOR pos = npc->m_vecPos;
        sq_pushvector(v, pos);
        return 1;
    }
    else return 0;
}
SQInteger fn_GetDistanceFromMeToPoint(HSQUIRRELVM v)
{
    SQFloat x, y, z;
    sq_getfloat(v, 2, &x);
    sq_getfloat(v, 3, &y);
    sq_getfloat(v, 4, &z);
    float px, py, pz;//NPC's position co-ordinates
    if (!npc) {
        sq_pushfloat(v, 0.0); return 1;
    }
    px = npc->m_vecPos.X;
    py = npc->m_vecPos.Y;
    pz = npc->m_vecPos.Z;
    float distance;

    double d = pow(px - x, 2) + pow(py - y, 2) + pow(pz - z, 2);
    distance = (float)sqrt(d);
    sq_pushfloat(v, distance);
    return 1;
}
SQInteger fn_GetDistanceFromMeToPoint2(HSQUIRRELVM v)
{
    //tx
    VECTOR point;
    if (!SQ_SUCCEEDED(sq_getvector(v, 2, &point)))
    {
        sq_pushfloat(v, 0.0); return 1;
    }
    float px, py, pz;//NPC's position co-ordinates
    if (!npc) {
        sq_pushfloat(v, 0.0); return 1;
    }
    px = npc->m_vecPos.X;
    py = npc->m_vecPos.Y;
    pz = npc->m_vecPos.Z;
    float distance;
    
    double d= pow(px - point.X,  2) + pow(py - point.Y, 2) + pow(pz - point.Z, 2);
    distance = (float)sqrt(d);
    sq_pushfloat(v, distance);
    return 1;
}
SQInteger fn_SendChat(HSQUIRRELVM v)
{
    const SQChar* message;
    sq_getstring(v, 2, &message);
    m_pFunctions->SendChatMessage(message);
    return 0;
}

SQInteger fn_SendCommand(HSQUIRRELVM v)
{
    const SQChar* message;
    sq_getstring(v, 2, &message);
    m_pFunctions->SendCommandToServer(message);
    return 0;
}
SQInteger fn_GetTickCount(HSQUIRRELVM v)
{
    long t = GetTickCount();
    int t_ = static_cast<int>(t); //don't know what else to do with long 
    sq_pushinteger(v, t_);
    return 1;
}
SQInteger fn_GetMyName(HSQUIRRELVM v)
{
    if (!npc)return 0;
    char* name=m_pPlayerPool->GetPlayerName(iNPC->GetID());
    sq_pushstring(v, name, -1);
    return 1;
}
SQInteger fn_GetMyID(HSQUIRRELVM v)
{
    if (!npc)return 0;
    sq_pushinteger(v, iNPC->GetID());
    return 1;
}
SQInteger fn_Quit(HSQUIRRELVM v)
{
    peer->CloseConnection(systemAddress, true);
    sq_pushbool(v, SQTrue);
    return 1;
}
SQInteger fn_RequestSpawn(HSQUIRRELVM v)
{
    if (m_pFunctions->RequestSpawn() == funcError::NoError)
        sq_pushbool(v, SQTrue);
    else
        sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_RequestClass(HSQUIRRELVM v)
{
    SQInteger c = 0;
    if (sq_gettop(v) > 1)
    {
            sq_getinteger(v, 2, &c);
    }
    if (c != 0 && c != 1 && c != -1)
        return sq_throwerror(v, "Invalid relative class index specified. Use 1,0 or -1");
    funcError e=m_pFunctions->RequestClass(c==-1?255:c);
    if (e == funcError::NoError)
        sq_pushbool(v, SQTrue);
    else
        sq_pushbool(v, SQFalse);
    return 1;
}

void RegisterNPCFunctions()
{
    register_global_func(v, ::fn_StartRecordingPlayback,"StartRecordingPlayback",-3,"tis");
    register_global_func(v, ::fn_IsPlayerStreamedIn,"IsPlayerStreamedIn",2,"ti");
    register_global_func(v, ::fn_IsVehicleStreamedIn,"IsVehicleStreamedIn",2,"ti");
    register_global_func(v, ::fn_SetMyFacingAngle,"SetMyFacingAngle",2,"tf|i");
    register_global_func(v, ::fn_SelectWeapon,"SelectWeapon",2,"ti");
    register_global_func(v, ::fn_GetAvailableWeapons,"GetAvailableWeapons",1,"t");
    register_global_func(v, ::fn_GetAmmoOfMyWeapon,"GetAmmoOfMyWeapon",2,"ti");
    register_global_func(v, ::fn_GetMyFacingAngle,"GetMyFacingAngle",1,"t");
    register_global_func(v, ::fn_SetMyPos2,"SetMyPos2",4,"tf|if|if|i");
    register_global_func(v, ::fn_GetMyPosX,"GetMyPosX",1,"t");
    register_global_func(v, ::fn_GetMyPosY,"GetMyPosY",1,"t");
    register_global_func(v, ::fn_GetMyPosZ,"GetMyPosZ",1,"t");
    register_global_func(v, ::fn_GetDistanceFromMeToPoint,"GetDistanceFromMeToPoint2",4,"tf|if|if|i");
    register_global_func(v, ::fn_PauseRecordingPlayback,"PauseRecordingPlayback",1,"t");
    register_global_func(v, ::fn_ResumeRecordingPlayback,"ResumeRecordingPlayback",1,"t");
    register_global_func(v, ::fn_StopRecordingPlayback,"StopRecordingPlayback",1,"t");
    register_global_func(v, ::fn_SendCommand, "SendCommand", 2, "ts");
    register_global_func(v, ::fn_SendChat,"SendChat",2,"ts");
    register_global_func(v, ::fn_GetTickCount,"GetTickCount",1,"t");
    register_global_func(v, ::fn_GetMyPos,"GetMyPos",1,"t");
    register_global_func(v, ::fn_SetMyPos,"SetMyPos",2,"tx");
    register_global_func(v, ::fn_GetDistanceFromMeToPoint2,"GetDistanceFromMeToPoint",2,"tx");
    register_global_func(v, ::fn_GetMyName, "GetMyName", 1, "t");
    register_global_func(v, ::fn_GetMyID, "GetMyID", 1, "t");
    register_global_func(v, ::fn_Quit, "QuitServer", 1, "t");
    register_global_func(v, ::fn_RequestSpawn, "RequestSpawn", 1, "t");
    register_global_func(v, ::fn_RequestClass, "RequestClass", -1, "t");
    register_global_func(v, ::fn_IsPlaybackRunning, "IsPlaybackRunning", 1, "t");
    register_global_func(v, ::fn_IsPlaybackPaused, "IsPlaybackPaused", 1, "t");

}
SQInteger NPC04_RegisterSquirrelConst(HSQUIRRELVM v, const SQChar* cname, SQInteger cvalue) {
    sq_pushconsttable(v);
    sq_pushstring(v, cname, -1);
    sq_pushinteger(v, cvalue);
    sq_newslot(v, -3, SQFalse);
    sq_pop(v, 1);
    return 0;
}
void RegisterConsts() {
    NPC04_RegisterSquirrelConst(v, "PLAYER_RECORDING_TYPE_ONFOOT", PLAYER_RECORDING_TYPE_ONFOOT);
    NPC04_RegisterSquirrelConst(v, "PLAYER_RECORDING_TYPE_DRIVER", PLAYER_RECORDING_TYPE_DRIVER);
    NPC04_RegisterSquirrelConst(v, "PLAYER_RECORDING_TYPE_ALL", PLAYER_RECORDING_TYPE_ALL);

    NPC04_RegisterSquirrelConst(v, "MAX_PLAYERS", 100);
    NPC04_RegisterSquirrelConst(v, "MAX_PLAYER_NAME", 24);
    NPC04_RegisterSquirrelConst(v, "MAX_VEHICLES", 1000);
    NPC04_RegisterSquirrelConst(v, "INVALID_PLAYER_ID", 0xFF);
    NPC04_RegisterSquirrelConst(v, "INVALID_VEHICLE_ID", 0);

    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_NONE", PLAYER_STATE_NONE);
    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_ONFOOT", PLAYER_STATE_ONFOOT);
    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_AIM", PLAYER_STATE_AIM);
    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_DRIVER", PLAYER_STATE_DRIVER);
    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_PASSENGER", PLAYER_STATE_PASSENGER);
    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_ENTER_VEHICLE_DRIVER", PLAYER_STATE_ENTER_VEHICLE_DRIVER);
    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_ENTER_VEHICLE_PASSENGER", PLAYER_STATE_ENTER_VEHICLE_PASSENGER);
    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_EXIT_VEHICLE", PLAYER_STATE_EXIT_VEHICLE);
    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_WASTED", PLAYER_STATE_WASTED);
    NPC04_RegisterSquirrelConst(v, "PLAYER_STATE_SPAWNED", PLAYER_STATE_SPAWNED);
    
    /*Credits: Gudio  https://forum.vc-mp.org/?topic=215*/
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_FORWARD", 32768);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_BACKWARD", 16384);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_LEFT", 8192);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_RIGHT", 4096);
    //NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_JUMP", 2176);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_JUMP", 2048);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_SPRINT", 1024);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_FIRE", 576);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_CROUCH", 288);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_PUNCH", 64);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_PREVWEP", 4);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_NEXTWEP", 2);
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_AIM", 1);
    NPC04_RegisterSquirrelConst(v, "KEY_NONE", 0);
#ifndef _REL004
    NPC04_RegisterSquirrelConst(v, "KEY_ONFOOT_LOOKBHND", 65536);
#endif
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_LEFT", 8192);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_RIGHT", 4096);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_BACKWARD", 2176);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_FORWARD", 1024);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_HORN", 288);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_LEANUP", 16);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_LEANDOWN", 8);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_LOOKLEFT", 4);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_LOOKRIGHT", 2);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_SUB_MISSION", 65536);
    NPC04_RegisterSquirrelConst(v, "KEY_INCAR_HANDBRAKE", 1);//added  
    
   NPC04_RegisterSquirrelConst(v, "I_KEYS", I_KEYS);
   NPC04_RegisterSquirrelConst(v, "F_POSX", F_POSX);
    NPC04_RegisterSquirrelConst(v, "F_POSY", F_POSY);
    NPC04_RegisterSquirrelConst(v, "F_POSZ", F_POSZ);
    NPC04_RegisterSquirrelConst(v, "F_ANGLE", F_ANGLE);
    NPC04_RegisterSquirrelConst(v, "I_HEALTH", I_HEALTH);
    NPC04_RegisterSquirrelConst(v, "I_ARMOUR", I_ARMOUR );
    NPC04_RegisterSquirrelConst(v, "I_CURWEP", I_CURWEP);
    NPC04_RegisterSquirrelConst(v, "I_CURWEP_AMMO", I_CURWEP_AMMO);
    NPC04_RegisterSquirrelConst(v, "V_ONFOOT_SPEED", V_ONFOOT_SPEED);
    NPC04_RegisterSquirrelConst(v, "V_AIMPOS", V_AIMPOS);
    NPC04_RegisterSquirrelConst(v, "V_AIMDIR", V_AIMDIR);
    NPC04_RegisterSquirrelConst(v, "V_POS", V_POS);
    NPC04_RegisterSquirrelConst(v, "Q_CAR_ROTATION", Q_CAR_ROTATION);
    NPC04_RegisterSquirrelConst(v, "F_CAR_HEALTH", F_CAR_HEALTH);
    NPC04_RegisterSquirrelConst(v, "I_CAR_DAMAGE", I_CAR_DAMAGE);
    NPC04_RegisterSquirrelConst(v, "V_CAR_SPEED", V_CAR_SPEED);
    NPC04_RegisterSquirrelConst(v, "F_CAR_TURRETX", F_CAR_TURRETX);
    NPC04_RegisterSquirrelConst(v, "F_CAR_TURRETY", F_CAR_TURRETY);
    NPC04_RegisterSquirrelConst(v, "I_ACTION", I_ACTION);


    NPC04_RegisterSquirrelConst(v, "PLAYERUPDATE_NORMAL", vcmpPlayerUpdateNormal);
    NPC04_RegisterSquirrelConst(v, "PLAYERUPDATE_AIMING", vcmpPlayerUpdateAiming);
    NPC04_RegisterSquirrelConst(v, "PLAYERUPDATE_DRIVER", vcmpPlayerUpdateDriver);
    NPC04_RegisterSquirrelConst(v, "PLAYERUPDATE_PASSENGER", vcmpPlayerUpdatePassenger);

    NPC04_RegisterSquirrelConst(v, "BODYPART_BODY", static_cast<int>(bodyPart::Body));
    NPC04_RegisterSquirrelConst(v, "BODYPART_TORSO", static_cast<int>(bodyPart::Torso));
    NPC04_RegisterSquirrelConst(v, "BODYPART_LEFTARM", static_cast<int>(bodyPart::LeftArm));
    NPC04_RegisterSquirrelConst(v, "BODYPART_RIGHTARM", static_cast<int>(bodyPart::RightArm));
    NPC04_RegisterSquirrelConst(v, "BODYPART_LEFTLEG", static_cast<int>(bodyPart::LeftLeg));
    NPC04_RegisterSquirrelConst(v, "BODYPART_RIGHTLEG", static_cast<int>(bodyPart::RightLeg));
    NPC04_RegisterSquirrelConst(v, "BODYPART_HEAD", static_cast<int>(bodyPart::Head ));    
   
    NPC04_RegisterSquirrelConst(v, "CLASS_PREVIOUS", CLASS_PREVIOUS );    
    NPC04_RegisterSquirrelConst(v, "CLASS_NEXT", CLASS_NEXT );    
    NPC04_RegisterSquirrelConst(v, "CLASS_CURRENT", CLASS_CURRENT );  

    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_PASSENGER_HEALTH", PLAY_IGNORE_PASSENGER_HEALTH);
    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_SEATID", PLAY_IGNORE_SEATID);
    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_VEHMODEL", PLAY_IGNORE_VEHMODEL);
    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_VEHICLEID", PLAY_IGNORE_VEHICLEID);
    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_CHECKPOINTS", PLAY_IGNORE_CHECKPOINTS);
    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_CHECKPOINT_POSITIONS", PLAY_IGNORE_CHECKPOINT_POSITIONS);
    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_UNSTREAMED_PICKUPS", PLAY_IGNORE_UNSTREAMED_PICKUPS);
    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_PICKUP_MODEL", PLAY_IGNORE_PICKUP_MODEL);
    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_UNSTREAMED_OBJECTS", PLAY_IGNORE_UNSTREAMED_OBJECTS);
    NPC04_RegisterSquirrelConst(v, "PLAY_IGNORE_OBJECT_MODEL", PLAY_IGNORE_OBJECT_MODEL);
    NPC04_RegisterSquirrelConst(v, "PLAY_AVOID_OBJECTSHOT_WEAPONCHECK", PLAY_AVOID_OBJECTSHOT_WEAPONCHECK);

}