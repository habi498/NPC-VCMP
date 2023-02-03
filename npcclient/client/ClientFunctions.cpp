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
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
uint8_t GetSlotId(uint8_t byteWeapon);
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

void CFunctions::SendShotInfo(bodyPart bodypart, int animation)
{
    SetLastError(funcError::NoError); 
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
    iNPC->ClassSelectionCounter = 0;
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
    if (npc->GetSlotAmmo(GetSlotId(byteWeaponId)) == 0)
        return SetLastError(funcError::WeaponNotPossessed);
    npc->SetCurrentWeapon(byteWeaponId);
    npc->SetCurrentWeaponAmmo(npc->GetSlotAmmo(GetSlotId(byteWeaponId)));
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
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_ENTER_VEHICLE_REQUEST));
    bsOut.Write(wVehicleId);
    switch (byteSeatId)
    {
    case 1:bsOut.Write((uint8_t)0x0b);
        bsOut.Write((uint8_t)0x04); break;
    case 2:bsOut.Write((uint8_t)0x10);
        bsOut.Write((uint8_t)0x02); break;
    case 3:bsOut.Write((uint8_t)0x0c);
        bsOut.Write((uint8_t)0x08); break;
    case 0:bsOut.Write((uint8_t)0x0b);
        bsOut.Write((uint8_t)0x04); break;
    default:return funcError::VehicleSeatIdInvalid;
    }
    if (byteSeatId == 0)
        bsOut.Write((uint8_t)0x12);
    else
        bsOut.Write((uint8_t)0x11);
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
