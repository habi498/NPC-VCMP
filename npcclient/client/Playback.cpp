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

uint8_t ReadAndSendDataBlock();


float ConvertUINT16_TtoFloat(uint16_t compressedFloat, float base)
{
    float value=((float)compressedFloat / 32767.5f - 1.0f)*base;
    return value;
}

DWORD ProcessPlaybacks()
{
    if (mPlayback.running == false || mPlayback.IsPaused()) {
        if (mPlayback.IsPaused())
        {
            if (mPlayback.type == PLAYER_RECORDING_TYPE_DRIVER)
            {
                DWORD dw_TimeDifference = GetTickCount() - mPlayback.dw_pausedTime;
                bool bCondition1 = (dw_TimeDifference % 100) >= (100 - CYCLE_SLEEP);
                if (bCondition1)
                {
                    //To stop the car or wait in car (otherwise car respawns!)
                    INCAR_SYNC_DATA* m_pIcSyncData = npc->GetINCAR_SYNC_DATA();
                    ZeroVEC(m_pIcSyncData->vecMoveSpeed);
                    SendNPCSyncData(m_pIcSyncData);
                }
            }
        }
        return CYCLE_SLEEP;
    }
    //Playback is running
    DWORD byteSleepTime;
    byteSleepTime = ReadAndSendDataBlock();
    if (byteSleepTime == 0)
    {
        return CYCLE_SLEEP;
    }
    else
    {
       return byteSleepTime;
    }
}


uint8_t ReadAndSendDataBlock()
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
            m_pIcDatablock.m_pIcSyncData.VehicleID = npc->m_wVehicleId;
        #endif
        SendNPCSyncData(&m_pIcDatablock.m_pIcSyncData);
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
