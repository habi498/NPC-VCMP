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
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
void CFunctions::SendCommandToServer(const char* message)
{
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_COMMAND);
    uint16_t len = (uint16_t)strlen(message);
    bsOut.Write(len);
    bsOut.Write(message, len);
    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, systemAddress, false);
}
void CFunctions::SendChatMessage(const char* message)
{
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_CHAT));
    uint16_t len = (uint16_t)strlen(message);
    bsOut.Write(len);
    bsOut.Write(message, len);
    peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_SEQUENCED, CHAT_MESSAGE_ORDERING_CHANNEL, systemAddress, false);
}
void CFunctions::FireSniperRifle(uint8_t weapon, float x, float y, float z, float dx, float dy, float dz)
{
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

void CFunctions::SendShotInfo(SQInteger bodypart, SQInteger animation)
{
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
}

void CFunctions::SendOnFootSyncData(uint32_t dwKeys, float x, float y, float z, float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon, uint16_t wAmmo, float speedx, float speedy, float speedz, float aimposx, float aimposy, float aimposz, float aimdirx, float aimdiry, float aimdirz, bool bIsCrouching, bool bIsReloading)
{
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

void CFunctions::SendDeathInfo(uint8_t weapon, uint8_t killerid, uint8_t bodypart)
{
    RakNet::BitStream bsOut;
    bsOut.Write((RakNet::MessageID)(ID_GAME_MESSAGE_DEATH_INFO));
    bsOut.Write(weapon);
    bsOut.Write(killerid);
    bsOut.Write(bodypart);
    peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE, 0, systemAddress, false);
    npc->SetState(PLAYER_STATE_WASTED);
    iNPC->SetSpawnStatus(false);
    iNPC->ClassSelectionCounter = 0;
}

uint32_t CFunctions::GetPlayerKeys(uint8_t playerId)
{
    CPlayer* player = m_pPlayerPool->GetAt(playerId);
    if (player)
    {
        return player->GetKeys();
    }
    else return 0;//throw error no such entity here
}

CFunctions::funcError CFunctions::GetPlayerName(uint8_t playerId, char* buffer, size_t size)
{
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        char* name = m_pPlayerPool->GetPlayerName(playerId);
        if (size >= strlen(name) + 1)
        {
            strcpy_s(buffer, strlen(name) + 1, name);
            return ErrorNoError;
        }
        else return ErrorBufferShortage;
    }
    else return ErrorEntityNotFound;
}
void CFunctions::SendPassengerSync()
{
    SendPassengerSyncData();
}

void CFunctions::GetPosition(float * x, float * y, float * z)
{
    *x = npc->m_vecPos.X;
    *y = npc->m_vecPos.Y;
    *z = npc->m_vecPos.Z;
}

void CFunctions::GetAngle(float* pfAngle)
{
    *pfAngle = npc->m_fAngle;
}

void CFunctions::SetPosition(float x, float y, float z, bool sync)
{
    npc->m_vecPos = VECTOR(x, y, z);
    if (sync)
        SendNPCOfSyncDataLV();
}
void CFunctions::SetAngle(float fAngle, bool sync)
{
    npc->m_fAngle = fAngle;
    if (sync)
        SendNPCOfSyncDataLV();
}
