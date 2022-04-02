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
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
extern Playback mPlayback;
extern NPC* iNPC;
extern CPlayer* npc;
uint8_t ReadDatablock2();
#define CYCLE_SLEEP 30//30 ms sleep between cycles. 
void SendNPCSyncData(ONFOOT_SYNC_DATA* m_pOfSyncData);
void SendNPCSyncData(INCAR_SYNC_DATA* m_pInSyncData, PacketPriority mPriority=HIGH_PRIORITY);
float ConvertUINT16_TtoFloat(uint16_t compressedFloat, float base)
{
    float value=((float)compressedFloat / 32767.5f - 1.0f)*base;
    return value;
}
void ProcessTimersAndSleep(uint8_t time)
{
    DWORD dw_Now = GetTickCount();
    ProcessTimers(dw_Now);
    DWORD dw_After = GetTickCount();
    if (dw_After - dw_Now >= time)
        return;
    Sleep(time);
}
void OnServerCycle()
{
    if (mPlayback.running == false||mPlayback.IsPaused()) {
        if (mPlayback.IsPaused())
        {
            if (mPlayback.type == PLAYER_RECORDING_TYPE_DRIVER)
            {
                DWORD dw_TimeDifference = GetTickCount() - mPlayback.dw_pausedTime;
                bool bCondition1 = dw_TimeDifference % 100 >= (100 - CYCLE_SLEEP);
                if (  bCondition1 )
                {
                    //To stop the car or wait in car (otherwise car respawns!)
                    INCAR_SYNC_DATA* m_pIcSyncData = npc->GetINCAR_SYNC_DATA();
                    ZeroVEC(m_pIcSyncData->vecMoveSpeed);
                    SendNPCSyncData(m_pIcSyncData);
                }
            }
        }
        ProcessTimersAndSleep(CYCLE_SLEEP);
        return;
    }
    uint8_t r;
    r = ReadDatablock2();
    if (r == 0)
    {
        ProcessTimersAndSleep(CYCLE_SLEEP);
        return;
    }
    else
    {
        ProcessTimersAndSleep(r);
        return;
    }
}

uint8_t ReadDatablock2()
{
    DWORD tick = GetTickCount();
    if (tick < mPlayback.nexttick)
    {
        DWORD difference = mPlayback.nexttick - tick;
        if (difference < CYCLE_SLEEP)return (uint8_t)difference;
        else return 0;
    }
    FILE* pFile = mPlayback.pFile;
    if (mPlayback.type == PLAYER_RECORDING_TYPE_ONFOOT)
    {
        ONFOOT_DATABLOCK m_pOfDatablock;
        size_t count=fread(&m_pOfDatablock, sizeof(m_pOfDatablock), 1, pFile);
        if (count != 1)
        {
            mPlayback.Abort();
            return 0;
        }
        SendNPCSyncData(&m_pOfDatablock.m_pOfSyncData);
        npc->StoreOnFootFullSyncData(&m_pOfDatablock.m_pOfSyncData);
       
    }
    else if (mPlayback.type == PLAYER_RECORDING_TYPE_DRIVER)
    {
        INCAR_DATABLOCK m_pIcDatablock;
        size_t count=fread(&m_pIcDatablock, sizeof(m_pIcDatablock), 1, pFile);
        if (count != 1)
        {
            mPlayback.Abort(); return 0;
        }
        #ifdef PLAYBACK_OVERRIDE_VEHICLEID
            m_pIcDatablock.m_pIcSyncData.VehicleID = npc->m_VehicleID;
        #endif
        SendNPCSyncData(&m_pIcDatablock.m_pIcSyncData);
        npc->StoreInCarFullSyncData(&m_pIcDatablock.m_pIcSyncData);
    }
    DWORD nexttick;
    size_t count = fread(&nexttick, sizeof(nexttick), 1, pFile);
    if (count != 1)
    {
        //EOF reached
        mPlayback.Abort();
        call_OnRecordingPlaybackEnd();
        return 0;
    }
    fseek(pFile, -1*(long)(sizeof(DWORD)), SEEK_CUR);//NEED to test this.
    DWORD interval = (nexttick - mPlayback.prevtick);
    mPlayback.nexttick = tick + interval;
    mPlayback.prevtick = nexttick;
    DWORD TE = (GetTickCount() - tick);//time elapsed
    if (interval < CYCLE_SLEEP)
        return (uint8_t)(interval - TE);
    else return 0;
}
void Playback::Pause()
{
    this->paused = true;
    if (this->type == PLAYER_RECORDING_TYPE_DRIVER)
    {
        this->dw_pausedTime = GetTickCount();
        INCAR_SYNC_DATA* m_pIcSyncData=npc->GetINCAR_SYNC_DATA();
        m_pIcSyncData->dwKeys = 1;//brake 
        
        //ZeroVEC(m_pIcSyncData->vecMoveSpeed);
        SendNPCSyncData(m_pIcSyncData,IMMEDIATE_PRIORITY);
    }else if (this->type == PLAYER_RECORDING_TYPE_ONFOOT)
    {
        ONFOOT_SYNC_DATA* m_pOfSyncData = npc->GetONFOOT_SYNC_DATA();
        ZeroVEC(m_pOfSyncData->vecSpeed);
        m_pOfSyncData->dwKeys = 0;
        SendNPCSyncData(m_pOfSyncData);//this needs testing
    }
}
/*
uint8_t ReadDatablock()
{
    DWORD tick = GetTickCount();

    if (tick < mPlayback.nexttick)
    {
        DWORD difference = mPlayback.nexttick - tick;
        if (difference < CYCLE_SLEEP)return (uint8_t)difference;
        else return 0;
    }

    RakNet::BitStream bsOut;
    FILE* pFile = mPlayback.pFile;
    if (mPlayback.type == PLAYER_RECORDING_TYPE_ONFOOT)
    {
        //we are going to read datablock
        //first 4 bytes is time, which is already read.
        uint8_t type;

        fread(&type, sizeof(type), 1, pFile);

        if (type == vcmpPlayerUpdateNormal)
            bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE);
        else if (type == vcmpPlayerUpdateAiming)
        {

#ifndef SHOOTING_ENABLED
            bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE);
#else
            bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM);
#endif

        }
        else {
            mPlayback.Abort(); return 0;//This datablock cannot be send. 
        }
        bsOut.Write(iNPC->anticheatID);

        /////////////////////////////////////////////////

        uint8_t action;
        fread(&action, sizeof(action), 1, pFile);
        float x, y, z;
        fread(&x, sizeof(x), 1, pFile);
        fread(&y, sizeof(y), 1, pFile);
        fread(&z, sizeof(z), 1, pFile);
        uint16_t angle;
        fread(&angle, sizeof(angle), 1, pFile);
        uint8_t weapon, health, armour;
        fread(&weapon, sizeof(weapon), 1, pFile);
        fread(&health, sizeof(health), 1, pFile);
        fread(&armour, sizeof(armour), 1, pFile);
        uint16_t speedx, speedy, speedz;
        fread(&speedx, sizeof(speedx), 1, pFile);
        fread(&speedy, sizeof(speedy), 1, pFile);
        fread(&speedz, sizeof(speedz), 1, pFile);
        uint32_t key;
        fread(&key, sizeof(key), 1, pFile);
        uint8_t halfbyte = 0;
        if ((action & 128) == 128)
            halfbyte = 0x2;
#ifdef SHOOTING_ENABLED
        bool reloading = false;
        if (type == vcmpPlayerUpdateAiming)
        {
            if ((key & 512) == 0)
            {
                //This means player is reloading weapon!
                reloading = true;
                halfbyte += 0x8;
                action = 0xd0;
            }
        }
#endif
        bsOut.Write(action);
        if (halfbyte != 0)
        {
            uint8_t bytearray[] = { halfbyte };
            bsOut.WriteBits(bytearray, 4);
        }

        /////////////////////////////////////////////////

        bsOut.Write(x); bsOut.Write(y); bsOut.Write(z);

        ////////////////////////////////////////////////

        bsOut.Write(angle);

        //////////////////////////////////////////////
        //Writing player speed x y z
        //////////////////////////////////////////////
        if ((action & 0b1) == 1)
        {
            bsOut.Write(speedx);
            bsOut.Write(speedy);
            bsOut.Write(speedz);
        }

        ///////////////////////////////////////////////
        //Next write weapon and ammo. If there.////////
        ///////////////////////////////////////////////

        if ((action & 0b01000000) == 0b01000000)
        {
            //this means weapon is present. 0x40 or higher in action
            bsOut.Write(weapon);
            if (weapon > 11)//WEAPONS ID GREATER THAN CHAINSAW
                bsOut.Write((uint16_t)1);//Let ammo be 1
        }


        ///////////////////////////////////////////////
        //Now write the game keys part
        // This is 3 bytes long
        //////////////////////////////////////////////

        if ((action & 0b00010000) == 0b00010000)
        {
            //game keys present. So write it   
            uint8_t key_head = 0x1;//the most significant byte
#ifdef SHOOTING_ENABLED
            if (reloading == true)key_head = 0x0;
#endif
            if (key > 0xFFFF)
            {
                //look behind key is pressed down
                key_head += 0x08;
                //above value is from wireshark analysis
                key -= 0x010000;
            }

            //after subtraction, key must be 2 bytes long
            bsOut.Write((uint8_t)(key % 256));
            bsOut.Write((uint8_t)(key / 256));
            bsOut.Write(key_head);

            //At last one half octect. It seems always to be 1
#ifdef SHOOTING_ENABLED
            uint8_t val = 0;//Default 0

            if (key % 2 == 1 && reloading == false)
                val = 0xc;//Aiming

            //When reloading it becomes 1
            if (key % 2 == 1 && reloading == true)
                val = 1;
            //Aiming does not matter when reloading
            if (reloading == true)
                val = 1;

            uint8_t bytearray2[] = { val };
#else
            uint8_t bitarray2[] = { 1 };
#endif
            bsOut.WriteBits(bytearray2, 4);
        }

        //Write Health//
        bsOut.Write(health);

        //Check for armour//
        if ((action & 0b00000100) == 0b00000100)
        {
            //Armour is present
            bsOut.Write(armour);
        }

        //Finally one more byte which is usually 0x03 or 0x07. 
        // This might need investigation
#ifdef SHOOTING_ENABLED
        if (type == vcmpPlayerUpdateAiming)
            bsOut.Write((uint8_t)0xa7);
        else bsOut.Write((uint8_t)0x03);
#else
        bsOut.Write((uint8_t)0x03);

#endif

#ifdef SHOOTING_ENABLED
        //Read values for aim direction and aim position
        if (type == vcmpPlayerUpdateAiming)
        {
            uint16_t aimx, aimy, aimz;
            fread(&aimx, sizeof(aimx), 1, pFile);
            fread(&aimy, sizeof(aimy), 1, pFile);
            fread(&aimz, sizeof(aimz), 1, pFile);
            bsOut.Write(aimx);
            bsOut.Write(aimy);
            bsOut.Write(aimz);
            float apx, apy, apz;//aimposx, aimposy,aimposz
            fread(&apx, sizeof(apx), 1, pFile);
            fread(&apy, sizeof(apy), 1, pFile);
            fread(&apz, sizeof(apz), 1, pFile);
            bsOut.Write(apx); bsOut.Write(apy); bsOut.Write(apz);
        }
        else
        {
            //Seek the file 18 bytes more
            fseek(pFile, 18, SEEK_CUR);
        }
#endif

        /////////////////////////////////////////////////
        //////done. Send the packet//////////////////////
        /////////////////////////////////////////////////

        peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
        //Save the data to iNPC
        //iNPC.SetPos(x, y, z);
        float _angle;
        _angle = ConvertUINT16_TtoFloat(angle, 2 * (float)PI);
        //iNPC.SetFacingAngle(_angle);
        //iNPC.SetArmour(armour);
        //iNPC.SetHealth(health);
        //iNPC.SetWeapon(weapon);
        //calculations for next packet
        DWORD tick2;
        size_t result = fread(&tick2, sizeof(tick2), 1, pFile);
        if (result != 1)
        {
            //EOF reached
            mPlayback.Abort();
            call_OnRecordingPlaybackEnd();
            return 0;
        }
        DWORD difference = (tick2 - mPlayback.prevtick);
        mPlayback.nexttick = tick + difference;
        mPlayback.prevtick = tick2;
        DWORD TE = (GetTickCount() - tick);//time elapsed
        if (difference < CYCLE_SLEEP)return (uint8_t)(difference - TE);
        else return 0;
    }
    else if (mPlayback.type == PLAYER_RECORDING_TYPE_DRIVER)
    {
        uint8_t updatetype;
        fread(&updatetype, sizeof(updatetype), 1, pFile);
        if (updatetype == vcmpPlayerUpdateDriver)
            bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_PLAYERUPDATE_DRIVER);
        else
        {
            mPlayback.Abort(); return 0;//This datablock cannot be send!
        }
        bsOut.Write(iNPC->anticheatID);
        uint8_t action;
        fread(&action, sizeof(action), 1, pFile);
        bsOut.Write(action);
        uint32_t keys; uint16_t vehicleId; uint8_t health, armour;
        uint8_t weapon;
        fread(&keys, sizeof(keys), 1, pFile);
        uint8_t keybyte1 = keys % 256;
        uint8_t keybyte2 = (keys - keybyte1) / 256;
        bsOut.Write(keybyte1);
        bsOut.Write(keybyte2);
        fread(&vehicleId, sizeof(vehicleId), 1, pFile);
        //Write the vehicle id
        {
            uint8_t nibble = vehicleId % 4;
            uint8_t byte = (vehicleId - nibble) / 4;
            nibble = nibble * 4;
            bsOut.Write(byte);
            uint8_t bytearray[] = { nibble };
            bsOut.WriteBits(bytearray, 4);
        }

        fread(&health, sizeof(health), 1, pFile);
        bsOut.Write(health);
        fread(&armour, sizeof(armour), 1, pFile);
        fread(&weapon, sizeof(weapon), 1, pFile);
        if ((action & 0x80) == 0x80)
        {
            //Weapon is present.
           bsOut.Write(weapon);
            if (weapon > 11)//WEAPONS ID GREATER THAN CHAINSAW
                bsOut.Write((uint16_t)1);//Let ammo be 1
        }
        if ((action & 0x40) == 0x40)
        {
            //Armour is present
            bsOut.Write(armour);
        }
        uint16_t model;
        fread(&model, sizeof(model), 1, pFile);
        uint8_t bytearray2[] = { HasTurret(model) };
        bsOut.WriteBits(bytearray2, 4);
        uint8_t type;
        fread(&type, sizeof(type), 1, pFile);
        uint8_t bytearray3[] = { type };
        bsOut.WriteBits(bytearray3, 4);
        //Position co-ordinates x y z
        float x, y, z;
        fread(&x, sizeof(x), 1, pFile);
        fread(&y, sizeof(y), 1, pFile);
        fread(&z, sizeof(z), 1, pFile);
        bsOut.Write(x); bsOut.Write(y); bsOut.Write(z);
        uint16_t speedx, speedy, speedz;
        fread(&speedx, sizeof(speedx), 1, pFile);
        fread(&speedy, sizeof(speedy), 1, pFile);
        fread(&speedz, sizeof(speedz), 1, pFile);
        if ((type & 0x1) == 0x1)
        {
            //Movement is there. i.e. Vehicle Speed
            bsOut.Write(speedx);
            bsOut.Write(speedy);
            bsOut.Write(speedz);
        }
        uint8_t nibble;
        fread(&nibble, sizeof(nibble), 1, pFile);
        uint16_t a, b, c;// x y z Quaternions
        fread(&a, sizeof(a), 1, pFile);
        fread(&b, sizeof(b), 1, pFile);
        fread(&c, sizeof(c), 1, pFile);
        uint8_t bytearray4[] = { nibble };
        bsOut.WriteBits(bytearray4, 4);
        bsOut.Write(a); bsOut.Write(b); bsOut.Write(c);
        uint32_t damage; float vhealth;
        fread(&damage, sizeof(damage), 1, pFile);
        fread(&vhealth, sizeof(vhealth), 1, pFile);
        //uint32_t _damage = _byteswap_ulong(damage);
        if ((type & 0x2) == 0x2)
        {
            bsOut.Write(damage);
        }
        if ((type & 0x4) == 0x4)
            bsOut.Write(vhealth);
        uint16_t turretx, turrety;
        fread(&turretx, sizeof(turretx), 1, pFile);
        fread(&turrety, sizeof(turrety), 1, pFile);
        bsOut.Write(turretx); bsOut.Write(turrety);
        //Write 'mystery' bytes
        bsOut.Write((uint8_t)0);
        bsOut.Write((uint8_t)0xc8);
        bsOut.Write((uint8_t)0);//Changes when vehicle is on water.?
        bsOut.Write((uint8_t)0);
        //Send the packet
        peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, systemAddress, false);
        //calculations for next packet
        DWORD tick2;
        size_t result = fread(&tick2, sizeof(tick2), 1, pFile);
        if (result != 1)
        {
            //EOF reached
            mPlayback.Abort();
            call_OnRecordingPlaybackEnd();
            return 0;
        }
        DWORD difference = (tick2 - mPlayback.prevtick);
        mPlayback.nexttick = tick + difference;
        mPlayback.prevtick = tick2;
        DWORD TE = (GetTickCount() - tick);//time elapsed
        if (difference < CYCLE_SLEEP)return (uint8_t)(difference - TE);
        else return 0;
    }
    else exit(1);
}
bool HasTurret(uint16_t modelid) 
{
    switch (modelid)
    {
    case 160: // predator
    case 166: //angel
    case 178: //pizzaboy
    case 182: //speeder
    case 183: //reefer
    case 184: //tropic
    case 191: //pcj 600
    case 192: //faggio
    case 193: //freeway
    case 198: //sanchez
    case 202: //coast guard
    case 203: //dinghy
    case 214: //marquis
    case 223: // cuban jetmax
        return 0;
    default:return 1;
    };
}
*/