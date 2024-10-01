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
extern CPlayerPool* m_pPlayerPool;
extern CVehiclePool* m_pVehiclePool;
extern NPC* iNPC;
extern CPlayer* npc;
extern CFunctions* m_pFunctions;
uint8_t ReadAndSendDataBlock();
bool SendPacketFromFile(uint8_t bytePacketType, FILE* file);

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
    else if (mPlayback.type == PLAYER_RECORDING_TYPE_ALL)
    {
        GENERALDATABLOCK m_pGenDatablock;
        size_t count = fread(&m_pGenDatablock, sizeof(m_pGenDatablock), 1, pFile);
        if (count != 1)
        {
            mPlayback.Abort(); return 0;
        }
        if (!SendPacketFromFile(m_pGenDatablock.bytePacketType, pFile))
        {
            mPlayback.Abort(); return 0;
        }
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
    fseek(pFile, -1*(long)(sizeof(DWORD)), SEEK_CUR);
    //careful here
    //nexttick is the tickcount the next packet is send in file
    //what is mplayback.prevtick -> stores previous tickcount of file, but mPlayback.nexttick
    //stores tickcount for npcclient application
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
bool ReadPacketFromFile(uint8_t bytePacketType, FILE* file, void* buffer,size_t sizeofbuffer)
{
    size_t count;
    switch (bytePacketType)
    {
    case PACKET_SPAWN:
        if (sizeof(SPAWNDATA) > sizeofbuffer)return 0;
        count = fread(buffer, sizeof(SPAWNDATA), 1, file);
        if (count != 1)return 0;
        break;
    case PACKET_DEATH:
        if (sizeof(DEATHDATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(DEATHDATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_VEHICLE_EXIT:break;
    case PACKET_VEHICLE_REQ_ENTER:
        //same as vehicle enter
    case PACKET_VEHICLE_ENTER:
        if (sizeof(VEHICLEENTER_REQUEST) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(VEHICLEENTER_REQUEST), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_ONFOOT:
    case PACKET_ONFOOT_AIM:
        if (sizeof(ONFOOT_SYNC_DATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(ONFOOT_SYNC_DATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_DRIVER:
        if (sizeof(INCAR_SYNC_DATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(INCAR_SYNC_DATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_PASSENGER:
        if (sizeof(PASSENGERDATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(PASSENGERDATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_PLAYER_COMMAND:
    case PACKET_PLAYER_CHAT:
    case PACKET_CLIENTSCRIPT_DATA:
        if (sizeof(COMMANDDATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(COMMANDDATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_PLAYER_PM:
        if (sizeof(PRIVMSGDATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(PRIVMSGDATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_PICKUP_PICKED:
        if (sizeof(PICKUPATTEMPTDATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(PICKUPATTEMPTDATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_CHECKPOINT_ENTERED:
    case PACKET_CHECKPOINT_EXITED:
        if (sizeof(CHECKPOINTENTEREDDATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(CHECKPOINTENTEREDDATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_OBJECT_TOUCH:
        if (sizeof(OBJECTTOUCH_DATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(OBJECTTOUCH_DATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_OBJECT_SHOT:
        if (sizeof(OBJECTSHOT_DATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(OBJECTSHOT_DATA), 1, file);
        if (count != 1)
            return 0;
        break;
    case PACKET_DISCONNECT:
        break;
    case PACKET_CHOOSECLASS:
        if (sizeof(CLASSCHOOSEDATA) > sizeofbuffer)
            return 0;
        count = fread(buffer, sizeof(CLASSCHOOSEDATA), 1, file);
        if (count != 1)
            return 0;
        break;
    }
    return 1;
}
bool SendPacketFromFile(uint8_t bytePacketType, FILE* file)
{
    int size = 64;//max PRIVMSGDATA, whose size is 26
    if (bytePacketType == PACKET_ONFOOT || bytePacketType == PACKET_ONFOOT_AIM || bytePacketType == PACKET_DRIVER)
        size = 256;
    void* buffer = malloc(size); 
    if (!buffer)return 0;
    if (!ReadPacketFromFile(bytePacketType, file, buffer, size))
    {
        free(buffer); 
        printf("Error. Cannot read packettype %d from file\n", bytePacketType);
        return 0;
    }
    //printf("bytePacketType is %d\n", bytePacketType);

    switch (bytePacketType)
    {
        case PACKET_DEATH:
        {    
            DEATHDATA* data = (DEATHDATA*)buffer;
            int anim;
            //taken from AttackScript
            switch (data->byteBodyPart)
            {
            case static_cast<int>(bodyPart::Body): anim = 0xd; break;
            case static_cast<int>(bodyPart::Head): anim = 17; break;
            case static_cast<int>(bodyPart::Torso): anim = 13; break;
            case static_cast<int>(bodyPart::LeftArm): anim = 19; break;
            case static_cast<int>(bodyPart::RightArm): anim = 20; break;
            case static_cast<int>(bodyPart::LeftLeg): anim = 21; break;
            case static_cast<int>(bodyPart::RightLeg): anim = 22; break;
            default: anim = 0xd; break;
            }
            BYTE bytePlayerId=m_pPlayerPool->GetPlayerId(data->killername);           
            m_pFunctions->SendShotInfo(static_cast<bodyPart>(data->byteBodyPart),anim,true,data->byteReason,bytePlayerId);
        }
        break;
        case PACKET_SPAWN:
        {
            if (iNPC->IsSpawned())
            {
                free(buffer);
                printf("Error. Cannot send spawn request when npc is already spawned\n");
                return 0;
            }
            SPAWNDATA* data = (SPAWNDATA*)buffer;
            if (data->byteClassId == iNPC->PotentialClassID || iNPC->PotentialClassID<0)
            {
                //almost good to go
                if (data->byteTeam == npc->m_byteTeamId && data->wSkin == npc->m_byteSkinId)
                {
                    m_pFunctions->RequestSpawn();
                }
                else
                {
                    printf("Error. Cannot spawn because team and(or) skin do not match\n");
                    free(buffer); return 0;
                }
            }
            else
            {
                printf("Error. Cannot spawn, class id not matching. current: %d, target: %d\n",iNPC->PotentialClassID,
                    data->byteClassId);
                free(buffer); return 0;
            }
        }
        break;
        case PACKET_CHOOSECLASS:
        {
            if (iNPC->IsSpawned())
            {
                printf("Error. Cannot choose class when npc is spawned\n");
                free(buffer); return 0;
            }
            CLASSCHOOSEDATA* data = (CLASSCHOOSEDATA*)buffer;
            uint8_t current = iNPC->PotentialClassID;
            if (current == data->byteclassId)m_pFunctions->RequestClass(CLASS_CURRENT);
            else if (current == (data->byteclassId - 1))m_pFunctions->RequestClass(CLASS_NEXT);
            else if (current == (data->byteclassId + 1))m_pFunctions->RequestClass(CLASS_PREVIOUS);//go back
            else if (current == 0 && data->byteIndex == 255)m_pFunctions->RequestClass(CLASS_PREVIOUS);
            //loop right to class id 0
            else if (data->byteclassId == 0 && data->byteIndex == 1)m_pFunctions->RequestClass(CLASS_NEXT);
            //negative classes
            else if(iNPC->PotentialClassID <0 && data->byteIndex==255)m_pFunctions->RequestClass(CLASS_PREVIOUS);
            else {
                printf("Error. Cannot determine at class selection( current class:%d, target class: %d, relative index: %d).\
                    Try matching class id with rec file before Playback\n", current, data->byteclassId, data->byteIndex);
                free(buffer);
                return 0;//cannot choose class
            }
    }break;
        case PACKET_PLAYER_CHAT:
        {
            COMMANDDATA* data = (COMMANDDATA*)buffer;
            char* msg = (char*)malloc(sizeof(char) * data->len + 1);
            
            if (msg)
            {
                size_t count = fread(msg, sizeof(char), data->len, file);
                if (count != data->len)
                {
                    printf("Error. Failed reading string from file\n");
                    free(buffer); return 0;
                }
                msg[data->len] = 0;
                m_pFunctions->SendChatMessage(msg);
            }
            else {
                printf("Error. Memory allocation failed\n");
                free(buffer); return 0;
            }
        }
        break;
        case PACKET_PLAYER_COMMAND:
        {
            COMMANDDATA* data = (COMMANDDATA*)buffer;
            char* msg = (char*)malloc(sizeof(char) * data->len + 1);

            if (msg)
            {
                size_t count = fread(msg, sizeof(char), data->len, file);
                if (count != data->len)
                {
                    printf("Error. Failed reading string from file\n");
                    free(buffer); return 0;
                }
                msg[data->len] = 0;
                m_pFunctions->SendCommandToServer(msg);
            }
            else {
                printf("Error. Memory allocation failed\n");
                free(buffer); return 0;
            }
        }break;
        case PACKET_PLAYER_PM:
        {
            PRIVMSGDATA* data = (PRIVMSGDATA*)buffer;
            char* msg = (char*)malloc(sizeof(char) * data->msglen + 1);

            if (msg)
            {
                size_t count = fread(msg, sizeof(char), data->msglen, file);
                if (count != data->msglen)
                {
                    printf("Error. Failed reading string from file\n");
                    free(buffer); return 0;
                }
                msg[data->msglen] = 0;
                BYTE bytePlayerId=m_pPlayerPool->GetPlayerId(data->target);
                if (bytePlayerId != INVALID_PLAYER_ID)
                {
                    m_pFunctions->SendPrivMessage(bytePlayerId,msg);
                }
                else printf("Warning: Could not send priv msg since player %s is not online\n", data->target);
            }
            else {
                printf("Error. Memory allocation failed\n");
                free(buffer); return 0;
            }
        }break;
        case PACKET_VEHICLE_REQ_ENTER:
        {
            if (npc->m_wVehicleId)
            {
                printf("Error. Cannot send vehicle enter request when inside vehicle\n");
                free(buffer); return 0;
            }
            if(!iNPC->IsSpawned())
            {
                printf("Error. Cannot send vehicle enter request when not spawned\n");
                free(buffer); return 0;
            }

            VEHICLEENTER_REQUEST* data = (VEHICLEENTER_REQUEST*)buffer;
            if (!m_pFunctions->IsVehicleStreamedIn(data->wVehicleID))
            {
                printf("Error. Cannot send enter request because vehicle not streamed in.\n");
                free(buffer); return 0;
            }
    
            if (m_pFunctions->GetVehicleModel(data->wVehicleID) != data->wModel)
            {
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_VEHMODEL))
                {
                    printf("Error. Vehicle Model is different from file.\n");
                    free(buffer); return 0;
                }
            }
            funcError e= m_pFunctions->RequestVehicleEnter(data->wVehicleID, data->SeatId);
            if (e != funcError::NoError)
            {
                printf("Error occured when entering vehicle %d\n",data->wVehicleID);
                free(buffer); return 0;
            }
            mPlayback.dw_VehicleRequestedTime = GetTickCount();
        }break;
        case PACKET_VEHICLE_EXIT:
        {
            if (!npc->m_wVehicleId)
            {
                printf("Error. Npc on foot cannot exit vehicle\n");
                free(buffer); return 0;
            }
            if (!iNPC->IsSpawned())
            {
                printf("Error. Npc not spawned\n");
                free(buffer); return 0;
            }
            //this packet has no buffer to read
            funcError e = m_pFunctions->ExitVehicle();
            if (e != funcError::NoError)
            {
                printf("Error occured when exiting vehicle \n");
                free(buffer); return 0;
            }
        }break;
        case PACKET_VEHICLE_ENTER:
        {
            //NPCClient.cpp expected to automatically puts npc in vehicle
        }break;
        case PACKET_PASSENGER:
        {
            if (!npc->m_wVehicleId)
            {
                DWORD dw_Now = GetTickCount();
                if (dw_Now - mPlayback.dw_VehicleRequestedTime > 5000) //5 seconds
                {
                    //abort playback
                    printf("Error. Vehicle not acquired by npc\n");
                    free(buffer); return 0;
                }
                else {
                    //do nothing. wait.
                }
            }
            else
            {
                PASSENGERDATA* data = (PASSENGERDATA*)buffer;
                if (npc->m_byteSeatId < 1)
                {
                    printf("Error. Cannot send passenger data - invalid seat id\n");
                    free(buffer); return 0;
                }
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_PASSENGER_HEALTH))
                {
                    npc->m_byteHealth = data->byteHealth;
                    npc->m_byteArmour = data->byteArmour;
                }
                else
                {
                    data->byteArmour = npc->m_byteArmour;
                    data->byteHealth = npc->m_byteHealth;
                }
                if (data->SeatId != npc->m_byteSeatId)
                {
                    if (!(mPlayback.dw_Flags & PLAY_IGNORE_SEATID))
                    {
                        printf("Error. Seat id in rec file different. See flags for overriding\n");
                        free(buffer); return 0;
                    }
                    else
                    {
                        data->SeatId = npc->m_byteSeatId;
                    }
                }
                if (data->wVehicleID != npc->m_wVehicleId)
                {
                    if (!(mPlayback.dw_Flags & PLAY_IGNORE_VEHICLEID))
                    {
                        printf("Error. Vehicle id from rec file different. See flags for overriding\n");
                        free(buffer); return 0;
                    }
                    else
                    {
                        data->wVehicleID = npc->m_wVehicleId;
                    }
                }
                if (data->wModel != m_pFunctions->GetVehicleModel(npc->m_wVehicleId))
                {
                    if (!(mPlayback.dw_Flags & PLAY_IGNORE_VEHMODEL))
                    {
                        printf("Error. Model of vehicle in rec file different. See flags for overriding\n");
                        free(buffer); return 0;
                    }
                    else
                    {
                        data->wModel = m_pFunctions->GetVehicleModel(npc->m_wVehicleId);
                    }
                }
                m_pFunctions->SendPassengerSync();
                if (m_pFunctions->GetLastError() != funcError::NoError)
                {
                    printf("Error occured when sending passenger data\n");//already handled
                    free(buffer); return 0;
                }
            }
        }break;
        case PACKET_DRIVER:
        {
            if (!npc->m_wVehicleId)
            {
                DWORD dw_Now = GetTickCount();
                if (dw_Now - mPlayback.dw_VehicleRequestedTime > 5000) //5 seconds
                {
                    //abort playback
                    printf("Error. Vehicle not acquired by npc\n");
                    free(buffer); return 0;
                }
                else {
                    //do nothing. wait.
                }
            }
            else
            {
                INCAR_SYNC_DATA* data = (INCAR_SYNC_DATA*)buffer;
                if (npc->m_byteSeatId !=0)
                {
                    printf("Error. Cannot send drive vehicle in passenger seat\n");
                    free(buffer); return 0;
                }
                
                if (data->VehicleID != npc->m_wVehicleId)
                {
                    if (!(mPlayback.dw_Flags & PLAY_IGNORE_VEHICLEID))
                    {
                        printf("Error. Vehicle id from rec file different. See flags for overriding\n");
                        free(buffer); return 0;
                    }
                    else 
                    {
                        data->VehicleID = npc->m_wVehicleId;//needed
                    }
                }
                m_pFunctions->SendInCarSyncDataEx(*data);
            }
        }break;
        case PACKET_ONFOOT:
        case PACKET_ONFOOT_AIM:
        {
            {
                ONFOOT_SYNC_DATA* data = (ONFOOT_SYNC_DATA*)buffer;
                m_pFunctions->SendOnFootSyncDataEx2(*data);
            }
        }break;
        case PACKET_DISCONNECT:
        {
            m_pFunctions->QuitServer();
        }break;
        case PACKET_CHECKPOINT_ENTERED:
        {
            CHECKPOINTENTEREDDATA* data = (CHECKPOINTENTEREDDATA*)buffer;
            if (!m_pFunctions->IsCheckpointStreamedIn(data->wCheckpointID))
            {
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_CHECKPOINTS))
                {
                    printf("Error. Checkpoint with id %u not found. See flags for overriding\n",data->wCheckpointID);
                    free(buffer); return 0;
                }
                free(buffer); return 1;//ignore
            }
            VECTOR pos;
            m_pFunctions->GetCheckpointPos(data->wCheckpointID, &pos);
            VECTOR dis = pos - data->vecPos;
            if (dis.GetMagnitude() > 5)
            {
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_CHECKPOINT_POSITIONS))
                {
                    printf("Error. Checkpoint position different from rec file. See flags for overriding\n");
                    free(buffer); return 0;
                }
            }
            funcError e=m_pFunctions->ClaimEnterCheckpoint(data->wCheckpointID);
            if (e != funcError::NoError)
            {
                printf("Error. Cannot enter checkpoint\n");
                free(buffer); return 0;
            }
        }break;
        
        case PACKET_CHECKPOINT_EXITED:
        {
            CHECKPOINTENTEREDDATA* data = (CHECKPOINTENTEREDDATA*)buffer;
            if (!m_pFunctions->IsCheckpointStreamedIn(data->wCheckpointID))
            {
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_CHECKPOINTS))
                {
                    printf("Error. Checkpoint with id %u not found. See flags for overriding\n", data->wCheckpointID);
                    free(buffer); return 0;
                }
                free(buffer); return 1;//ignore the packet
            }
            VECTOR pos;
            m_pFunctions->GetCheckpointPos(data->wCheckpointID, &pos);
            VECTOR dis = pos - data->vecPos;
            if (dis.GetMagnitude() > 5)
            {
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_CHECKPOINT_POSITIONS))
                {
                    printf("Error. Checkpoint position different from rec file. See flags for overriding\n");
                    free(buffer); return 0;
                }
            }
            funcError e = m_pFunctions->ClaimExitCheckpoint(data->wCheckpointID);
            if (e != funcError::NoError)
            {
                printf("Error. Cannot exit checkpoint\n");
                free(buffer); return 0;
            }
        }break; 
        case PACKET_CLIENTSCRIPT_DATA:
        {
            COMMANDDATA* data = (COMMANDDATA*)buffer;
            uint8_t* msg = (uint8_t*)malloc(sizeof(uint8_t) * data->len );

            if (msg)
            {
                size_t count = fread(msg, sizeof(uint8_t), data->len, file);
                if (count != data->len)
                {
                    printf("Error. Failed reading bytes from file\n");
                    free(buffer); return 0;
                }
                m_pFunctions->SendServerData((void*)msg, data->len);
            }
            else {
                printf("Error. Memory allocation failed\n");
                free(buffer); return 0;
            }
        }break;
        case PACKET_PICKUP_PICKED:
        {
            PICKUPATTEMPTDATA* data = (PICKUPATTEMPTDATA*)buffer;
            if (!m_pFunctions->IsPickupStreamedIn(data->wPickupID))
            {
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_UNSTREAMED_PICKUPS))
                {
                    printf("Error. Pickup with id %u not found. See flags for overriding\n", data->wPickupID);
                    free(buffer); return 0;
                }
                free(buffer); return 1;//ignore
            }
            if (m_pFunctions->GetPickupModel(data->wPickupID) != data->wModel)
            {
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_PICKUP_MODEL))
                {
                    printf("Error. Pickup model different. See flags for overriding\n");
                    free(buffer); return 0;
                }
            }
            m_pFunctions->ClaimPickup(data->wPickupID);
        }break;
        case PACKET_OBJECT_SHOT:
        {
            OBJECTSHOT_DATA* data = (OBJECTSHOT_DATA*)buffer;
            if (!m_pFunctions->IsObjectStreamedIn(data->wObjectID))
            {
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_UNSTREAMED_OBJECTS))
                {
                    printf("Error. Object with id %u not found. See flags for overriding\n", data->wObjectID);
                    free(buffer); return 0;
                }
                free(buffer); return 1;//ignore
            }
            uint16_t model;
            if (m_pFunctions->GetObjectModel(data->wObjectID, &model) != funcError::NoError)
            {
                if (model != data->wModel)
                {
                    if (!(mPlayback.dw_Flags & PLAY_IGNORE_OBJECT_MODEL))
                    {
                        printf("Error. Object model different from rec file. See flags for overriding\n");
                        free(buffer); return 0;
                    }
                }
            }
            if (!m_pFunctions->IsWeaponAvailable(data->byteWeapon))
            {
                //weapon not available.
                if (!(mPlayback.dw_Flags & PLAY_AVOID_OBJECTSHOT_WEAPONCHECK))
                {
                    printf("Error. Weapon %d not available for shooting object. See flags for overriding\n", data->byteWeapon);
                    free(buffer); return 0;
                }
            }
            funcError e=m_pFunctions->ClaimObjectShot(data->wObjectID, data->byteWeapon);
            if (e != funcError::NoError)
            {
                printf("Error occured when claiming object shot\n");
                free(buffer); return 0;
            }
        }break;
        case PACKET_OBJECT_TOUCH:
        {
            OBJECTTOUCH_DATA* data = (OBJECTTOUCH_DATA*)buffer;
            if (!m_pFunctions->IsObjectStreamedIn(data->wObjectID))
            {
                if (!(mPlayback.dw_Flags & PLAY_IGNORE_UNSTREAMED_OBJECTS))
                {
                    printf("Error. Object with id %u not found. See flags for overriding\n", data->wObjectID);
                    free(buffer); return 0;
                }
                free(buffer); return 1;//ignore
            }
            uint16_t model;
            if (m_pFunctions->GetObjectModel(data->wObjectID, &model) != funcError::NoError)
            {
                if (model != data->wModel)
                {
                    if (!(mPlayback.dw_Flags & PLAY_IGNORE_OBJECT_MODEL))
                    {
                        printf("Error. Object model different from rec file. See flags for overriding\n");
                        free(buffer); return 0;
                    }
                }
            }
            
            funcError e = m_pFunctions->ClaimObjectTouch(data->wObjectID);
            if (e != funcError::NoError)
            {
                printf("Error occured when claiming object touch\n");
                free(buffer); return 0;
            }
        }break;
        default:
        {
            printf("Error - Unknown packet type %d\n", bytePacketType);
            free(buffer); return 0;
        }break;
    }
    free(buffer);
    return 1;
}
bool GetNameFromRecFile(FILE* file, char* name, size_t size)
{
    if (file == NULL)
    {
        printf("File is invalid\n");
        return false;
    }
    uint32_t identifier;
    size_t m = fread(&identifier, sizeof(uint32_t), 1, file);
    if (m != 1)return false;
    if (identifier == NPC_RECFILE_IDENTIFIER_V5)
    {
        int s=fseek(file, 4, SEEK_CUR); //Skip recording type
        if (s == 0)
        {
            uint32_t flags;
            m = fread(&flags, sizeof(uint32_t), 1, file);
            if (m != 1)return false;
            if (flags & 1)
            {
                //name is present
                if (size < 24)
                {
                    printf("Error. Variable does not have enough size\n");
                    return false;
                }
                m = fread(name, sizeof(char), 24, file);
                if (m != 24)
                {
                    printf("Could not read 24 characters\n");
                    return false;
                }
                return true;
            }
            else {
                printf("Error. File does not contain name flag\n");
                return false;
            }
        }
        else {
            printf("Error. Could not seek file\n");
            return false;
        }
    }
    else
    {
        printf("Error. rec file not 1005\n");
        return false;
    }
}