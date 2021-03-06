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
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
#define CHAT_MESSAGE_ORDERING_CHANNEL 3
Playback mPlayback;
SQInteger register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char* fname, unsigned char nparamscheck, const SQChar* typemask)
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
    bool result=iNPC->IsPlayerStreamedIn(playerid);
    sq_pushbool(v, result);
    return 1;//return 1 because value is returned
}
SQInteger fn_IsVehicleStreamedIn(HSQUIRRELVM v)
{
    SQInteger vhclid;
    sq_getinteger(v, 2, &vhclid);
    bool result = iNPC->IsVehicleStreamedIn(vhclid);
    sq_pushbool(v, result);
    return 1;//return 1 because value is returned
}
SQInteger fn_StartRecordingPlayback(HSQUIRRELVM v)
{
    SQInteger nargs = sq_gettop(v); //number of arguments
    SQInteger type;//play back type
    const SQChar* filename;
    sq_getinteger(v, 2, &type);
    sq_getstring(v, 3, &filename);
    if (mPlayback.running == true)return 0;
    mPlayback.pFile = fopen(std::string(REC_DIR+ std::string("/") + std::string(filename)+std::string(".rec")).c_str(), "rb");
    if (mPlayback.pFile == NULL)
    {
        printf("Failed to open %s", filename);
        return 0;
    }
    unsigned char* buffer;
    buffer = (unsigned char*)malloc(8 * sizeof(char));
    if (buffer == NULL)return 0;
    int identifier;
    size_t m=fread(&identifier, sizeof(int), 1, mPlayback.pFile);
    if (m != 1)return 0;
    if (identifier != 1000)
    {
        //NOT NPC Recording
        printf("File format different. Cannot start playback\n");
        return 0;
    }
    mPlayback.identifier = identifier;
    int rectype;
    m = fread(&rectype, sizeof(int), 1, mPlayback.pFile);
    if (m != 1)return 0;
    if (rectype != type)
    {
        printf("Recording type mismatch. Aborting playback\n");
        return 0;
    }
    mPlayback.type = type;
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
    }
    return 0; 
}
SQInteger fn_SetMyFacingAngle(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    SQFloat angle;
    sq_getfloat(v, 2, &angle);
    npc->m_fAngle = angle;
    SendNPCUpdate();
    return 0;//0 because we are returning nothing
}

SQInteger fn_PauseRecordingPlayback(HSQUIRRELVM v)
{
    if (mPlayback.running == true)
        mPlayback.Pause();
    return 0;//0 because we are returning nothing
}
SQInteger fn_ResumeRecordingPlayback(HSQUIRRELVM v)
{
    if (mPlayback.running == true)
        mPlayback.Resume();
    return 0;//0 because we are returning nothing
}
SQInteger fn_StopRecordingPlayback(HSQUIRRELVM v)
{
    if (mPlayback.running == true)
        mPlayback.Stop();
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
SQInteger fn_SetMyPos(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    SQFloat x, y, z;
    sq_getfloat(v, 2, &x);
    sq_getfloat(v, 3, &y);
    sq_getfloat(v, 4, &z);

    npc->m_vecPos.X = x; npc->m_vecPos.Y = y; npc->m_vecPos.Z = z;
    SendNPCUpdate();
    return 0;//0 because we are returning nothing!
}
SQInteger fn_GetMyPosX(HSQUIRRELVM v)
{
    if (!npc) {
        sq_pushfloat(v, 0.0); return 1;
    }SQFloat val = npc->m_vecPos.X;
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
    
    double d= pow(px - x,  2) + pow(py - y, 2) + pow(pz - z, 2);
    distance = (float)sqrt(d);
    sq_pushfloat(v, distance);
    return 1;
}
SQInteger fn_SendChat(HSQUIRRELVM v)
{
    const SQChar* message;
    sq_getstring(v, 2, &message);
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_CHAT));
    uint16_t len = (uint16_t)strlen(message);
    bsOut.Write(len);
    bsOut.Write(message, len);
    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_SEQUENCED, CHAT_MESSAGE_ORDERING_CHANNEL, systemAddress, false);
    return 0;
}

SQInteger fn_SendCommand(HSQUIRRELVM v)
{
    const SQChar* message;
    sq_getstring(v, 2, &message);
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_COMMAND);
    uint16_t len = (uint16_t)strlen(message);
    bsOut.Write(len);
    bsOut.Write(message, len);
    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, systemAddress, false);
    return 0;
}

void RegisterNPCFunctions()
{
    register_global_func(v, ::fn_StartRecordingPlayback,"StartRecordingPlayback",3,"tis");
    register_global_func(v, ::fn_IsPlayerStreamedIn,"IsPlayerStreamedIn",2,"ti");
    register_global_func(v, ::fn_IsVehicleStreamedIn,"IsVehicleStreamedIn",2,"ti");
    register_global_func(v, ::fn_SetMyFacingAngle,"SetMyFacingAngle",2,"tf");
    register_global_func(v, ::fn_GetMyFacingAngle,"GetMyFacingAngle",1,"t");
    register_global_func(v, ::fn_SetMyPos,"SetMyPos2",4,"tfff");
    register_global_func(v, ::fn_GetMyPosX,"GetMyPosX",1,"t");
    register_global_func(v, ::fn_GetMyPosY,"GetMyPosY",1,"t");
    register_global_func(v, ::fn_GetMyPosZ,"GetMyPosZ",1,"t");
    register_global_func(v, ::fn_GetDistanceFromMeToPoint,"GetDistanceFromMeToPoint2",4,"tfff");
    register_global_func(v, ::fn_PauseRecordingPlayback,"PauseRecordingPlayback",1,"t");
    register_global_func(v, ::fn_ResumeRecordingPlayback,"ResumeRecordingPlayback",1,"t");
    register_global_func(v, ::fn_StopRecordingPlayback,"StopRecordingPlayback",1,"t");
    register_global_func(v, ::fn_SendCommand, "SendCommand", 2, "ts");
    register_global_func(v, ::fn_SendChat,"SendChat",2,"ts");
}
SQInteger RegisterSquirrelConst(HSQUIRRELVM v, const SQChar* cname, SQInteger cvalue) {
    sq_pushconsttable(v);
    sq_pushstring(v, cname, -1);
    sq_pushinteger(v, cvalue);
    sq_newslot(v, -3, SQFalse);
    sq_pop(v, 1);
    return 0;
}
void RegisterConsts() {
    /*Credits: Gudio  https://forum.vc-mp.org/?topic=215*/
    RegisterSquirrelConst(v, "KEY_ONFOOT_FORWARD", 32768);
    RegisterSquirrelConst(v, "KEY_ONFOOT_BACKWARD", 16384);
    RegisterSquirrelConst(v, "KEY_ONFOOT_LEFT", 8192);
    RegisterSquirrelConst(v, "KEY_ONFOOT_RIGHT", 4096);
    RegisterSquirrelConst(v, "KEY_ONFOOT_JUMP", 2176);
    RegisterSquirrelConst(v, "KEY_ONFOOT_SPRINT", 1024);
    RegisterSquirrelConst(v, "KEY_ONFOOT_FIRE", 576);
    RegisterSquirrelConst(v, "KEY_ONFOOT_CROUCH", 288);
    RegisterSquirrelConst(v, "KEY_ONFOOT_PUNCH", 64);
    RegisterSquirrelConst(v, "KEY_ONFOOT_PREVWEP", 4);
    RegisterSquirrelConst(v, "KEY_ONFOOT_NEXTWEP", 2);
    RegisterSquirrelConst(v, "KEY_ONFOOT_AIM", 1);
    RegisterSquirrelConst(v, "KEY_ONFOOT_LOOKBHND", 65536);

    RegisterSquirrelConst(v, "KEY_INCAR_LEFT", 8192);
    RegisterSquirrelConst(v, "KEY_INCAR_RIGHT", 4096);
    RegisterSquirrelConst(v, "KEY_INCAR_BACKWARD", 2176);
    RegisterSquirrelConst(v, "KEY_INCAR_FORWARD", 1024);
    RegisterSquirrelConst(v, "KEY_INCAR_HORN", 288);
    RegisterSquirrelConst(v, "KEY_INCAR_LEANUP", 16);
    RegisterSquirrelConst(v, "KEY_INCAR_LEANDOWN", 8);
    RegisterSquirrelConst(v, "KEY_INCAR_LOOKLEFT", 4);
    RegisterSquirrelConst(v, "KEY_INCAR_LOOKRIGHT", 2);
    RegisterSquirrelConst(v, "KEY_INCAR_SUB_MISSION", 65536);
    RegisterSquirrelConst(v, "KEY_INCAR_HANDBRAKE", 1);//added  
}