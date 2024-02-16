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
extern CPlayerPool* m_pPlayerPool;
extern CVehiclePool* m_pVehiclePool;
extern CPickupPool* m_pPickupPool;
extern CCheckpointPool* m_pCheckpointPool;
extern CObjectPool* m_pObjectPool;
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
extern uint32_t configvalue;
uint8_t GetSlotIdFromWeaponId(uint8_t byteWeapon);
funcError elastError = funcError::NoError;
//Returns that value which has to be returned
funcError SetLastError(funcError e) { elastError = e; return e; }
void ClearLastError() { elastError = funcError::NoError; }
funcError fn_GetLastError() { return elastError; }
void CFunctions::SendCommandToServer(const char* message)
{
    SetLastError(funcError::NoError);
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_COMMAND);
    uint16_t len = (uint16_t)strlen(message);
    bsOut.Write(len);
    bsOut.Write(message, len);
    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, systemAddress, false);
}
funcError CFunctions::GetLastError()
{
    return elastError;
}
funcError CFunctions::RequestClass(uint8_t relativeindex, bool bIgnoreAbsoluteClass)
{
    SetLastError(funcError::NoError);
    if (iNPC->IsSpawned() == false)
    {
        if (relativeindex == CLASS_CURRENT || relativeindex == CLASS_NEXT || relativeindex == CLASS_PREVIOUS)
        {
            npc->RemoveAllWeapons();
            RakNet::BitStream bsOut3;
            bsOut3.Write((RakNet::MessageID)ID_GAME_MESSAGE_REQUEST_CLASS);
            bsOut3.Write(relativeindex);
            peer->Send(&bsOut3, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 5, systemAddress, false);
            iNPC->bIsClassRequested = true;
            iNPC->byteClassIndexRequested = relativeindex;
            if(bIgnoreAbsoluteClass)iNPC->UnsetSpawnClass();//Unset absolute spawnclass
            //In NPCClient.cpp, there is a method of checking class==absolute class. when 'unset', checking will overriden
            
            return funcError::NoError;
        }
        else return SetLastError(funcError::ArgumentIsOutOfBounds);
    }
    else return SetLastError(funcError::NPCAlreadySpawned);
}
funcError CFunctions::RequestSpawn()
{
    SetLastError(funcError::NoError);
    if (iNPC->IsSpawned() == false)
    {
        RakNet::BitStream bsOut;
        bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_REQUEST_SPAWN);
        peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 5, systemAddress, false);
        return funcError::NoError;
    }
    else return SetLastError(funcError::NPCAlreadySpawned);
}
void CFunctions::SetFPS(double fps)
{
    SetLastError(funcError::NoError);
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_PLAYER_FPS));
    bsOut.Write(fps);
    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, systemAddress, false);
}
bool CFunctions::IsPickupStreamedIn(uint16_t wPickupID)
{
    ClearLastError();
    PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
    if (pickup)
    {
        return true;
    }
    else return false;
}
uint16_t CFunctions::GetPickupModel(uint16_t wPickupID)
{
    ClearLastError(); 
    PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
    if (pickup)
    {
        return pickup->wModel;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}
funcError CFunctions::GetPickupPosition(uint16_t wPickupID, VECTOR* vecPos)
{
    ClearLastError();
    PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
    if (pickup)
    {
        *vecPos = pickup->vecPos;
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);
}
uint8_t CFunctions::GetPickupAlpha(uint16_t wPickupID)
{
    ClearLastError();
    PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
    if (pickup)
    {
        return pickup->bAlpha;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}
uint32_t CFunctions::GetPickupQuantity(uint16_t wPickupID)
{
    ClearLastError();
    PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
    if (pickup)
    {
        return pickup->dwQuantity;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}
uint32_t CFunctions::GetStreamedPickupCount()
{
    ClearLastError();
    return m_pPickupPool->GetCount();
}
funcError CFunctions::ClaimPickup(uint16_t wPickupID)
{
    ClearLastError();
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_CLAIM_PICKUP));
    PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
    if (pickup)
    {
        unsigned int dwSerialNo = pickup->dwSerialNo;
        uint16_t wPickupID = pickup->wID;
        uint16_t word; uint8_t byte, byte2;
        word = (uint16_t)(dwSerialNo >> 5);
        byte = (dwSerialNo << 3) & 0xFF;
        byte |= (uint8_t)(wPickupID >> 8);

        byte2 = (wPickupID & 0xFF);
        bsOut.Write(word); bsOut.Write(byte); bsOut.Write(byte2);
        peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, systemAddress, false);
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);

}
funcError CFunctions::ClaimEnterCheckpoint(uint16_t wCheckpointID)
{
    ClearLastError();
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_CHECKPOINT_ENTER_EXIT));
    CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
    if (cp)
    {
        bsOut.Write(cp->wID);
        bsOut.Write((uint8_t)0x80);//0x80 for entering and 0x00 for exiting
        peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, CHECKPOINT_ENTER_ORDERING_CHANNEL, systemAddress, false);
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);

}
funcError CFunctions::ClaimExitCheckpoint(uint16_t wCheckpointID)
{
    ClearLastError();
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_CHECKPOINT_ENTER_EXIT));
    CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
    if (cp)
    {
        bsOut.Write(cp->wID);
        bsOut.Write((uint8_t)0x00);
        peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, CHECKPOINT_ENTER_ORDERING_CHANNEL, systemAddress, false);
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);
}
bool CFunctions::IsCheckpointStreamedIn(uint16_t wCheckpointID)
{
    ClearLastError();
    CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
    if (cp)
    {
        return true;
    }
    else return false;
}
funcError CFunctions::GetCheckpointColor(uint16_t wCheckpointID, uint8_t* Red, uint8_t* Green, uint8_t* Blue, uint8_t* Alpha)
{
    ClearLastError();
    CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
    if (cp)
    {
        *Red = cp->byteRed;
        *Green = cp->byteGreen;
        *Blue = cp->byteBlue;
        *Alpha = cp->byteAlpha;
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);
}
funcError CFunctions::GetCheckpointPos(uint16_t wCheckpointID, VECTOR* vecPos)
{
    ClearLastError();
    CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
    if (cp)
    {
        *vecPos = cp->vecPos;
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);
}
funcError CFunctions::IsCheckpointSphere(uint16_t wCheckpointID, uint8_t* isSphere)
{
    ClearLastError();
    CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
    if (cp)
    {
        *isSphere = cp->byteIsSphere;
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);
}
funcError CFunctions::GetCheckpointRadius(uint16_t wCheckpointID, float* fRadius)
{
    ClearLastError();
    CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
    if (cp)
    {
        *fRadius = cp->fRadius;
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);
}
bool CFunctions::IsObjectStreamedIn(uint16_t wObjectID)
{
    ClearLastError();
    OBJECT* obj = m_pObjectPool->GetByID(wObjectID);
    if (obj)
    {
        return true;
    }
    else return false;
}
funcError CFunctions::GetObjectModel(uint16_t wObjectID, uint16_t* wModel)
{
    ClearLastError();
    OBJECT* obj = m_pObjectPool->GetByID(wObjectID);
    if (obj)
    {
        *wModel = obj->wModel;
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}
funcError CFunctions::GetObjectPos(uint16_t wObjectID, VECTOR* vecPos)
{
    ClearLastError();
    OBJECT* obj = m_pObjectPool->GetByID(wObjectID);
    if (obj)
    {
        *vecPos = obj->vecPos;
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}
funcError CFunctions::GetObjectRotation(uint16_t wObjectID, QUATERNION* quatRot)
{
    ClearLastError();
    OBJECT* obj = m_pObjectPool->GetByID(wObjectID);
    if (obj)
    {
        *quatRot = obj->quatRot;
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}
funcError CFunctions::GetObjectAlpha(uint16_t wObjectID, uint8_t* byteAlpha)
{
    ClearLastError();
    OBJECT* obj = m_pObjectPool->GetByID(wObjectID);
    if (obj)
    {
        *byteAlpha = obj->byteAlpha;
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}
bool CFunctions::IsObjectTouchReportEnabled(uint16_t wObjectID)
{
    ClearLastError();
    OBJECT* obj = m_pObjectPool->GetByID(wObjectID);
    if (obj)
    {
        return obj->IsTouchReportEnabled();
    }
    else {
        SetLastError(funcError::EntityNotFound);
        return false;
    }
}
bool CFunctions::IsObjectShotReportEnabled(uint16_t wObjectID)
{
    ClearLastError();
    OBJECT* obj = m_pObjectPool->GetByID(wObjectID);
    if (obj)
    {
        return obj->IsShotReportEnabled();
    }
    else {
        SetLastError(funcError::EntityNotFound);
        return false;
    }
}
funcError CFunctions::ClaimObjectTouch(uint16_t wObjectID)
{
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_CLAIM_OBJECT_TOUCH));
    if (IsObjectStreamedIn(wObjectID))
    {
        bsOut.Write(wObjectID);
        peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, systemAddress, false);
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}
funcError CFunctions::ClaimObjectShot(uint16_t wObjectID, uint8_t byteWeaponID)
{
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_CLAIM_OBJECT_SHOT));
    if (IsObjectStreamedIn(wObjectID))
    {
        bsOut.Write(wObjectID);
        bsOut.Write(byteWeaponID);
        peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, systemAddress, false);
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}
uint32_t CFunctions::GetStreamedCheckpointCount()
{
    ClearLastError();
    return m_pCheckpointPool->GetCount();
}
VECTOR move(VECTOR vecCurPos, float fDistance, float fAngle)
{
    float newx = vecCurPos.X - (float)sin(fAngle) * fDistance;
    float newy = vecCurPos.Y + (float)cos(fAngle) * fDistance;
    return VECTOR(newx, newy, vecCurPos.Z);//assuming z more or less same.
}
VECTOR move2(VECTOR vecCurPos, float fCurAngle, float fDistance, float fAngle)
{
    return move(vecCurPos, fDistance, fCurAngle + fAngle);
}

funcError CFunctions::ExitVehicleEx(bool fosd,uint8_t style , uint8_t byte1, uint8_t byte2)
{
    ClearLastError();
    if (npc->m_wVehicleId)
    {
        RakNet::BitStream bsOut;
        bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_VEHICLE_EXIT));
        /*
        ad 01 00 00  normal
        ad 02 00 00 jump from vehicle
        ad 03 27 01 fall from bike hitting wall
        ad 03 27 03 fall from bike upside down
        ad 03 27 00 fall from bike while jumping*/
        bsOut.Write(style); bsOut.Write(byte1); bsOut.Write(byte2);
        peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, systemAddress, false);
        if (fosd) //follow onfoot sync data
        {
            VECTOR vecPos;
            GetVehiclePosition(npc->m_wVehicleId, &vecPos);
            uint8_t seatid = npc->m_byteSeatId;
            QUATERNION quatRotation;
            GetVehicleRotation(npc->m_wVehicleId, &quatRotation);
            float fCurAngle=(float)asin((double)quatRotation.Z) * 2;
            //print(move2(Vector(-362.514, -528.351, 12.5124), 0.0465442, 1.5, PI / 2))
            float fDistance = 1.5;
            float fTarAngle = (float)PI / 2;
            switch (seatid)
            {
            case 1: fTarAngle *= -1;//right side door
                break;
            case 2: fTarAngle = 3 * (float)PI / 4; fDistance = 2; break;
            case 3: fTarAngle= -3 * (float)PI / 4; fDistance = 2; break;
            default:break;
            }
            npc->m_vecPos=move2(vecPos, fCurAngle, fDistance, fTarAngle);
            iNPC->VETickCount = GetTickCount();
            iNPC->WaitingForVEOnFootSync = true;
        }
        return SetLastError(funcError::NoError);
    }
    else return SetLastError(funcError::VehicleNotEntered);
}
funcError CFunctions::ExitVehicle()
{
    ClearLastError();
    if (npc->m_wVehicleId)
    {
        RakNet::BitStream bsOut;
        bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_VEHICLE_EXIT));
        /*
        ad 01 00 00  normal
        ad 02 00 00 jump from vehicle
        ad 03 27 01 fall from bike hitting wall
        ad 03 27 03 fall from bike upside down
        ad 03 27 00 fall from bike while jumping*/
        bsOut.Write((uint8_t)0x01); bsOut.Write((uint16_t)0);//normal
        peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, systemAddress, false);
        return SetLastError(funcError::NoError);
    }
    else return SetLastError(funcError::VehicleNotEntered);
}
uint32_t CFunctions::GetStreamedObjectCount()
{
    ClearLastError();
    return m_pObjectPool->GetCount();
}
void CFunctions::SendChatMessage(const char* message)
{
    SetLastError(funcError::NoError); 
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_CHAT));
    uint16_t len = (uint16_t)strlen(message);
    bsOut.Write(len);
    bsOut.Write(message, len);
    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_SEQUENCED, CHAT_MESSAGE_ORDERING_CHANNEL, systemAddress, false);
}
void CFunctions::FireSniperRifle(uint8_t weapon, float x, float y, float z, float dx, float dy, float dz)
{
    SetLastError(funcError::NoError); 
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

}

void CFunctions::SendShotInfo(bodyPart bodypart, int animation, bool bAutoDeath, uint8_t AutoDeathWep, uint8_t AutoDeathKiller)
{
    SetLastError(funcError::NoError); 
    if (AutoDeathKiller != 255 && !IsPlayerConnected(AutoDeathKiller))
    {
        SetLastError(funcError::EntityNotFound);
        return;
    }
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_PLAYER_SHOT_BODYPART));
    char nbp; //new body part
    switch (bodypart)
    {
    case bodyPart::Head: nbp = 2; break;
    case bodyPart::Torso: nbp = 1; break;
    case bodyPart::LeftArm: nbp = 3; break;
    case bodyPart::RightArm: nbp = 4; break;
    case bodyPart::LeftLeg: nbp = 7; break;
    case bodyPart::RightLeg: nbp = 8; break;
    case bodyPart::Body: nbp = (uint8_t)255; break;
    default: nbp = (uint8_t)255; break;
    }
    bsOut.Write(nbp);
    bsOut.Write((char)animation);
    peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false);
    npc->m_byteArmour = 0;
    npc->m_byteHealth = 0;
    npc->SetKeys(0);
    //new addition
    if (bAutoDeath)
    {
        iNPC->TimeShotInfoSend = GetTickCount();
        iNPC->WaitingToRemoveBody = true;
        iNPC->ShotInfoWeapon = AutoDeathWep;
        iNPC->KillerId = AutoDeathKiller;
        iNPC->byteAutodeathBodyPart = static_cast<uint8_t>(bodypart);
    }
}
funcError CFunctions::SendInCarSyncData(uint32_t dwKeys, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteWeapon,uint16_t wAmmo, float fCarHealth, uint32_t dwDamage, VECTOR vecPos, QUATERNION quatRotation, VECTOR vecSpeed, float fTurretx, float fTurrety)
{
    SetLastError(funcError::NoError); 
    INCAR_SYNC_DATA pIcSyncData;
    if (npc->m_wVehicleId == 0)
        return funcError::VehicleNotEntered;
    else if(npc->m_byteSeatId!=0)
        return funcError::NotDriverOfVehicle;
    pIcSyncData.VehicleID = npc->m_wVehicleId;
    pIcSyncData.byteCurrentWeapon = byteWeapon;
    pIcSyncData.bytePlayerArmour = byteArmour;
    pIcSyncData.bytePlayerHealth = byteHealth;
    pIcSyncData.dDamage = dwDamage;
    pIcSyncData.dwKeys = dwKeys;
    pIcSyncData.fCarHealth = fCarHealth;
    pIcSyncData.quatRotation = quatRotation;
    pIcSyncData.Turretx = fTurretx;
    pIcSyncData.Turrety = fTurrety;
    pIcSyncData.vecMoveSpeed = vecSpeed;
    pIcSyncData.vecPos = vecPos;
    pIcSyncData.wAmmo = wAmmo;
    SendNPCSyncData(&pIcSyncData);
    return funcError::NoError;
}
void CFunctions::SendOnFootSyncDataEx(uint32_t dwKeys, VECTOR vecPos, float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon, uint16_t wAmmo, VECTOR vecSpeed, VECTOR vecAimPos, VECTOR vecAimDir, bool bIsCrouching, bool bIsReloading)
{
    return SendOnFootSyncData(dwKeys,vecPos.X,vecPos.Y,vecPos.Z, fAngle,
        byteHealth,byteArmour,byteCurrentWeapon,wAmmo,
        vecSpeed.X,vecSpeed.Y, vecSpeed.Z, vecAimPos.X,
        vecAimPos.Y, vecAimPos.Z, vecAimDir.X, vecAimDir.Y,
        vecAimDir.Z, bIsCrouching, bIsReloading);
}
void CFunctions::SendOnFootSyncData(uint32_t dwKeys, float x, float y, float z, float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon, uint16_t wAmmo, float speedx, float speedy, float speedz, float aimposx, float aimposy, float aimposz, float aimdirx, float aimdiry, float aimdirz, bool bIsCrouching, bool bIsReloading)
{
    SetLastError(funcError::NoError); 
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
    m_pOfSyncData.vecAimDir.X = aimdirx;
    m_pOfSyncData.vecAimDir.Y = aimdiry;
    m_pOfSyncData.vecAimDir.Z = aimdirz;

    m_pOfSyncData.vecAimPos.X = aimposx;
    m_pOfSyncData.vecAimPos.Y = aimposy;
    m_pOfSyncData.vecAimPos.Z = aimposz;

    m_pOfSyncData.vecPos.X = x;
    m_pOfSyncData.vecPos.Y = y;
    m_pOfSyncData.vecPos.Z = z;

    m_pOfSyncData.vecSpeed.X = speedx;
    m_pOfSyncData.vecSpeed.Y = speedy;
    m_pOfSyncData.vecSpeed.Z = speedz;

    m_pOfSyncData.wAmmo = static_cast<uint16_t>(wAmmo);
    m_pOfSyncData.bIsReloading = static_cast<bool>(bIsReloading);
    //if bIsReloading is provided then set off the fire key
    if (bIsReloading && ((dwKeys & 512) == 512))
        dwKeys = dwKeys & (~(1 << 9));//2^9 = 512. Set 9th bit from right off.
    m_pOfSyncData.dwKeys = (uint32_t)dwKeys;
    SendNPCSyncData(&m_pOfSyncData);
}

void CFunctions::SendDeathInfo(uint8_t weapon, uint8_t killerid, bodyPart bodypart)
{
    SetLastError(funcError::NoError); 
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_DEATH_INFO));
    bsOut.Write(weapon);
    bsOut.Write(killerid);
    bsOut.Write(static_cast<uint8_t>(bodypart));
    peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false);
    npc->SetState(PLAYER_STATE_WASTED);
    iNPC->SetSpawnStatus(false);
    //iNPC->ClassSelectionCounter = 0;
}

uint32_t CFunctions::GetPlayerKeys(uint8_t playerId)
{
    SetLastError(funcError::NoError); 
    CPlayer* player = m_pPlayerPool->GetAt(playerId);
    if (player)
    {
        return player->GetKeys();
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

funcError CFunctions::GetPlayerSpeed(uint8_t bytePlayerId, VECTOR* vecSpeedOut)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        *vecSpeedOut = player->GetONFOOT_SYNC_DATA()->vecSpeed;
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);
}
uint8_t CFunctions::GetPlayerAction(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->GetONFOOT_SYNC_DATA()->byteAction;
    }
    else {
        SetLastError(funcError::EntityNotFound);
        return 0;
    }
}
funcError CFunctions::Suicide(uint8_t reason)
{
    ClearLastError();
    if (iNPC->IsSpawned())
    {
        SendShotInfo(bodyPart::Body, 0xd);
        iNPC->TimeShotInfoSend = GetTickCount();
        iNPC->WaitingToRemoveBody = true;
        iNPC->ShotInfoWeapon = reason;//70=suicide, 44=fell to death
        iNPC->KillerId = 255;
        iNPC->byteAutodeathBodyPart = static_cast<uint8_t>(bodyPart::Body);
        return funcError::NoError;
    }
    else return SetLastError(funcError::NPCNotSpawned);
}
Color CFunctions::GetColor()
{
    SetLastError(funcError::NoError);
    if (iNPC->Initialized() && npc)return npc->GetColor();
    SetLastError(funcError::NPCNotConnected);
    return Color(0, 0, 0);
}
Color CFunctions::GetPlayerColor(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->GetColor();
    }
    else {
        SetLastError(funcError::EntityNotFound);
        return Color(0,0,0);
    }
}
void CFunctions::RequestAbsoluteClass(uint8_t classID)
{
    ClearLastError();
    if (classID >= MAX_CLASSES_FOR_SPAWNING)
    {
        SetLastError(funcError::ArgumentIsOutOfBounds);
        return;
    }
    if (iNPC)
    {
        iNPC->SetSpawnClass(classID);
        if (iNPC->IsSpawned() == false)
        {
            //Request class
            RequestClass(0);//to send the packet
            iNPC->AbsClassNotSpecified = false;//absolute class specified just now iNPC->SetSpawnClass
        }
        return;
    }
    SetLastError(funcError::NPCNotConnected);
    return;
}
bool CFunctions::IsNpcSpawned()
{
    ClearLastError(); 
    if (iNPC)
    {
        return iNPC->IsSpawned();
    }
    SetLastError(funcError::NPCNotConnected);
    return false;
}
void CFunctions::SendPrivMessage(uint8_t bytePlayerId, const char* message)
{
    if (!IsPlayerConnected(bytePlayerId))
    {
        SetLastError(funcError::EntityNotFound);
        return;
    }
    SetLastError(funcError::NoError);
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_PRIVMSG));
    bsOut.Write((uint8_t)bytePlayerId);
    uint16_t len = (uint16_t)strlen(message);
    bsOut.Write(len);
    bsOut.Write(message, len);
    //channel is same as CHAT message channel
    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_SEQUENCED, CHAT_MESSAGE_ORDERING_CHANNEL, systemAddress, false);
}
void CFunctions::QuitServer()
{
    peer->CloseConnection(systemAddress, true);
}
bool CFunctions::IsWeaponAvailable(uint8_t byteWeaponId)
{
    int npcid = iNPC->GetID(); uint8_t j;
    for (int i = 0; i <= 9; i++)
    {
        j = GetPlayerWeaponAtSlot(npcid, i);
        if (j == byteWeaponId)
        {
            return true;
        }
    }
    return false;
}
void CFunctions::SetConfig(uint32_t value)
{
    configvalue = value;
}
funcError CFunctions::GetPlayerAimDir(uint8_t bytePlayerId, VECTOR* vecAimDirOut)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        *vecAimDirOut = player->GetONFOOT_SYNC_DATA()->vecAimDir;
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);
}

funcError CFunctions::GetPlayerAimPos(uint8_t bytePlayerId, VECTOR* vecAimPosOut)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        *vecAimPosOut = player->GetONFOOT_SYNC_DATA()->vecAimPos;
        return funcError::NoError;
    }
    else return SetLastError(funcError::EntityNotFound);
}

uint16_t CFunctions::GetPlayerWeaponAmmo(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->GetONFOOT_SYNC_DATA()->wAmmo;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

playerState CFunctions::GetPlayerState(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return playerState(player->GetState());
    }
    else SetLastError(funcError::EntityNotFound);
    return playerState(0);
}

uint16_t CFunctions::GetPlayerVehicle(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->m_wVehicleId;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

uint8_t CFunctions::GetPlayerSeat(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->m_byteSeatId;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

uint8_t CFunctions::GetPlayerSkin(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->m_byteSkinId;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

uint8_t CFunctions::GetPlayerTeam(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->m_byteTeamId;
    }
    else SetLastError(funcError::EntityNotFound); 
    return 0;
}

uint8_t CFunctions::GetPlayerWeaponAtSlot(uint8_t bytePlayerId, uint8_t byteSlotId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        if (byteSlotId > 9)
        {
            SetLastError(funcError::ArgumentIsOutOfBounds);
            return 0;
        }
        return player->GetSlotWeapon(byteSlotId);
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

uint16_t CFunctions::GetPlayerAmmoAtSlot(uint8_t bytePlayerId, uint8_t byteSlotId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        if (byteSlotId > 9)
        {
            SetLastError(funcError::ArgumentIsOutOfBounds);
            return 0;
        }
        return player->GetSlotAmmo(byteSlotId);
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

funcError CFunctions::GetVehicleRotation(uint16_t wVehicleId, QUATERNION* quatRotOut)
{
    ClearLastError();
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (vehicle)
    {
        *quatRotOut = vehicle->GetRotation();
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}

uint16_t CFunctions::GetVehicleModel(uint16_t wVehicleId)
{
    ClearLastError();
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (vehicle)
    {
        return vehicle->GetModel();
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

funcError CFunctions::GetVehiclePosition(uint16_t wVehicleId, VECTOR* vecPosOut)
{
    ClearLastError();
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (vehicle)
    {
        *vecPosOut = vehicle->GetPosition();
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}

uint8_t CFunctions::GetVehicleDriver(uint16_t wVehicleId)
{
    ClearLastError();
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (vehicle)
    {
        return vehicle->GetDriver();
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

float CFunctions::GetVehicleHealth(uint16_t wVehicleId)
{
    ClearLastError();
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (vehicle)
    {
        return vehicle->GetHealth();
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

uint32_t CFunctions::GetVehicleDamage(uint16_t wVehicleId)
{
    ClearLastError();
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (vehicle)
    {
        return vehicle->GetDamage();
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

funcError CFunctions::GetPlayerName(uint8_t playerId, char* buffer, size_t size)
{
    SetLastError(funcError::NoError); 
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        char* name = m_pPlayerPool->GetPlayerName(playerId);
        if (size >= strlen(name) + 1)
        {
            #ifndef WIN32
                strcpy(buffer, name);
            #else   
                strcpy_s(buffer, strlen(name) + 1, name);
            #endif  
            return funcError::NoError;
        }
        else return funcError::BufferShortage;
    }
    else return funcError::EntityNotFound;
}
funcError CFunctions::GetPlayerPosition(uint8_t bytePlayerId, VECTOR* vecPosOut)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        *vecPosOut = player->m_vecPos;
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}
float CFunctions::GetPlayerAngle(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->m_fAngle;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0.0;
}
uint8_t CFunctions::GetPlayerHealth(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->m_byteHealth;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}
uint8_t CFunctions::GetPlayerArmour(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->m_byteArmour;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}
uint8_t CFunctions::GetPlayerWeapon(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->GetCurrentWeapon();
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}
bool CFunctions::IsPlayerCrouching(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        if (!player->m_wVehicleId)
            return player->GetONFOOT_SYNC_DATA()->IsCrouching;
        else
            return false;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}
bool CFunctions::IsPlayerReloading(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        if (!player->m_wVehicleId)
            return player->GetONFOOT_SYNC_DATA()->bIsReloading;
        else
            return false;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}
void CFunctions::SendPassengerSync()
{
    SetLastError(funcError::NoError); 
    if (!npc->m_wVehicleId)
    {
        SetLastError(funcError::VehicleNotEntered); return;
    }
    if (npc->m_byteSeatId < 1)
    {
        SetLastError(funcError::VehicleSeatIdInvalid); return;
    }
    SendPassengerSyncData();
}

void CFunctions::GetPosition(float * x, float * y, float * z)
{
    SetLastError(funcError::NoError); 
    *x = npc->m_vecPos.X;
    *y = npc->m_vecPos.Y;
    *z = npc->m_vecPos.Z;
}

void CFunctions::GetAngle(float* pfAngle)
{
    SetLastError(funcError::NoError); 
    *pfAngle = npc->m_fAngle;
}

void CFunctions::SetPosition(float x, float y, float z, bool sync)
{
    SetLastError(funcError::NoError); 
    npc->m_vecPos = VECTOR(x, y, z);
    if (sync)
        SendNPCOfSyncDataLV();
}
void CFunctions::SetAngle(float fAngle, bool sync)
{
    SetLastError(funcError::NoError); 
    npc->m_fAngle = fAngle;
    if (sync)
        SendNPCOfSyncDataLV();
}
void CFunctions::SetHealth(uint8_t byteHealth, bool sync)
{
    SetLastError(funcError::NoError);
    if (!iNPC->IsSpawned())
    {
        SetLastError(funcError::NPCNotSpawned);
        return; 
    }
    if (npc->m_byteHealth > 0 && byteHealth == 0)
    {
        Suicide(44);//44== fell to death, not suicide
        return;
    }
    npc->m_byteHealth = byteHealth;
    if (sync)
        SendNPCOfSyncDataLV();
}
void CFunctions::SetArmour(uint8_t byteArmour, bool sync)
{
    SetLastError(funcError::NoError);
    npc->m_byteArmour = byteArmour;
    if (sync)
        SendNPCOfSyncDataLV();
}
uint8_t CFunctions::GetNPCId()
{
    SetLastError(funcError::NoError);
    if (iNPC->Initialized())return iNPC->GetID();
    else SetLastError(funcError::NPCNotConnected);
    return 255;
}
void CFunctions::SendServerData(const void* data, size_t size)
{
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_SERVER_DATA);
    bsOut.Write((uint32_t)size);
    bsOut.Write((char*)data, size);
    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
}

funcError CFunctions::SetWeapon(uint8_t byteWeaponId, bool sync)
{
    SetLastError(funcError::NoError); 
    if (npc->GetSlotAmmo(GetSlotIdFromWeaponId(byteWeaponId)) == 0)
        return SetLastError(funcError::WeaponNotPossessed);
    npc->SetCurrentWeapon(byteWeaponId);
    npc->SetCurrentWeaponAmmo(npc->GetSlotAmmo(GetSlotIdFromWeaponId(byteWeaponId)));
    if (sync)
    {
        if (!npc->m_wVehicleId)
            SendNPCOfSyncDataLV();
    }
    return funcError::NoError;
}
funcError CFunctions::RequestVehicleEnter(uint16_t wVehicleId, uint8_t byteSeatId)
{
    SetLastError(funcError::NoError);
    
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (!vehicle)
    {
        return funcError::EntityNotFound;
    }
    uint16_t model = vehicle->GetModel();

    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_ENTER_VEHICLE_REQUEST));
    bsOut.Write(wVehicleId);
    switch (model)
    {
    case 136:
    case 160:case 176:
    case 183:case 184:
    case 202:case 203:case 214:case 223:
        bsOut.Write((uint8_t)0x00);
        bsOut.Write((uint8_t)0x00);
        bsOut.Write((uint8_t)0x00); 
        break;

    case 166:case 178:case 191:case 192:
    case 193:case 198:
        switch (byteSeatId)
        {
        case 0:bsOut.Write((uint8_t)0x0b);
            bsOut.Write((uint8_t)0x05); break;
        case 1:bsOut.Write((uint8_t)0x10);
            bsOut.Write((uint8_t)0x0a); break;
        default:return funcError::VehicleSeatIdInvalid;
        }
        if (byteSeatId == 0)
            bsOut.Write((uint8_t)0x12);
        else
            bsOut.Write((uint8_t)0x11);
        break;
    default:
        switch (byteSeatId)
        {
        case 0:bsOut.Write((uint8_t)0x0b);
            bsOut.Write((uint8_t)0x04); break;
        case 1:bsOut.Write((uint8_t)0x0b);
            bsOut.Write((uint8_t)0x04); break;
        case 2:bsOut.Write((uint8_t)0x10);
            bsOut.Write((uint8_t)0x02); break;
        case 3:bsOut.Write((uint8_t)0x0c);
            bsOut.Write((uint8_t)0x08); break;
        
        default:return funcError::VehicleSeatIdInvalid;
        }
        if (byteSeatId == 0)
            bsOut.Write((uint8_t)0x12);
        else
            bsOut.Write((uint8_t)0x11);
        break;
    }
    
    
    peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false);
    return funcError::NoError;
}

funcError CFunctions::GetVehicleSpeed(uint16_t wVehicleId, VECTOR* vecSpeedOut)
{
    ClearLastError();
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (vehicle)
    {
        *vecSpeedOut = vehicle->GetSpeed();
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}

funcError CFunctions::GetVehicleTurretRotation(uint16_t wVehicleId, float* Horizontal, float* Vertical)
{
    ClearLastError();
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (vehicle)
    {
        *Horizontal = vehicle->GetTurretx();
        *Vertical = vehicle->GetTurrety();
        return funcError::NoError;
    }
    else return funcError::EntityNotFound;
}

bool CFunctions::IsVehicleStreamedIn(uint16_t wVehicleId)
{
    ClearLastError();
    CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
    if (vehicle)
    {
        return true;
    }
    else return false;
}

bool CFunctions::IsPlayerStreamedIn(uint8_t bytePlayerId)
{
    ClearLastError();
    if (bytePlayerId == iNPC->GetID())
    {
        SetLastError(funcError::EntityNotFound);
        return false;
    }
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        return player->IsStreamedIn();
    }
    else SetLastError(funcError::EntityNotFound);
    return false;
}

bool CFunctions::IsPlayerSpawned(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (player)
    {
        uint8_t bytePlayerState = player->GetState();
        if (bytePlayerState != PLAYER_STATE_NONE
            && bytePlayerState != PLAYER_STATE_WASTED)
            return true;
        else return false;
    }
    else SetLastError(funcError::EntityNotFound);
    return 0;
}

bool CFunctions::IsPlayerConnected(uint8_t bytePlayerId)
{
    ClearLastError();
    CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
    if (m_pPlayerPool->GetSlotState(bytePlayerId))
    {
        return true;
    }
    else return false;
}
WORD CFunctions::GetCurrentWeaponAmmo()
{
    ClearLastError();
    WORD wAmmo=npc->GetCurrentWeaponAmmo();
    return wAmmo;
}

BYTE CFunctions::GetCurrentWeapon()
{
    ClearLastError();
    BYTE weapon=npc->GetCurrentWeapon();
    return weapon;
}

void CFunctions::SendOnFootSyncDataEx2(ONFOOT_SYNC_DATA OfSyncData)
{
    SendNPCSyncData(&OfSyncData);
}

void CFunctions::SendInCarSyncDataEx(INCAR_SYNC_DATA IcSyncData)
{
    SendNPCSyncData(&IcSyncData);
}

void CFunctions::GetOnFootSyncData(ONFOOT_SYNC_DATA** pOfSyncData)
{
    *pOfSyncData = npc->GetONFOOT_SYNC_DATA();
}

void CFunctions::GetInCarSyncData(INCAR_SYNC_DATA* pIcSyncData)
{
    pIcSyncData = npc->GetINCAR_SYNC_DATA();
}

void CFunctions::SetAmmoAtSlot(uint8_t byteSlotId, WORD wAmmo)
{
    npc->SetWeaponSlot(byteSlotId, npc->GetSlotWeapon(byteSlotId), wAmmo);
}

void CFunctions::FireProjectile(uint8_t byteWeapon, VECTOR vecPos, float r1, float r2, float r3, float r4, float r5, float r6, float r7)
{
    SetLastError(funcError::NoError);
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_FIRE_PROJECTILE));
    bsOut.Write((char)byteWeapon);
    bsOut.Write(vecPos.Z);// z first
    bsOut.Write(vecPos.Y);
    bsOut.Write(vecPos.X);
    bsOut.Write(r1);
    bsOut.Write(r2);
    bsOut.Write(r3);
    bsOut.Write(r4);
    bsOut.Write(r5);
    bsOut.Write(r6);
    bsOut.Write(r7);
    peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false);

}
