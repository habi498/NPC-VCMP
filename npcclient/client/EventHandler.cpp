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
extern  CPlugins* m_pPlugins;
void CEvents::OnServerData(const uint8_t* data, size_t size)
{
    call_OnServerScriptData(data, size);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnServerData)
            pPlugin->pPluginCalls->OnServerData(data, size);
    }
}
void CEvents::OnExplosion(uint8_t byteExplosionType, VECTOR vecPos, uint8_t bytePlayerCaused, bool bIsOnGround)
{
    call_OnExplosion(byteExplosionType, vecPos, bytePlayerCaused, bIsOnGround);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnExplosion)
            pPlugin->pPluginCalls->OnExplosion(byteExplosionType, vecPos, bytePlayerCaused, bIsOnGround);
    }

}
void CEvents::OnProjectileFired(uint8_t bytePlayerId, uint8_t byteWeapon, VECTOR vecPos, float r1, float r2, float r3, float r4, float r5, float r6, float r7)
{
    call_OnProjectileFired(bytePlayerId, byteWeapon, vecPos, r1, r2, r3, r4, r5, r6, r7);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnProjectileFired)
            pPlugin->pPluginCalls->OnProjectileFired(bytePlayerId, byteWeapon, vecPos, r1, r2, r3, r4, r5, r6, r7);
    }

}
void CEvents::OnPlayerDeath(uint8_t bytePlayerId)
{
    call_OnPlayerDeath(bytePlayerId);
    PluginPool_s* pPlugin;
    for (DWORD i=0; i< m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnPlayerDeath)
            pPlugin->pPluginCalls->OnPlayerDeath(bytePlayerId);
    }
}

void CEvents::OnPlayerText(uint8_t bytePlayerId, char* text, uint16_t length)
{
    call_OnPlayerText(bytePlayerId, text, length);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnPlayerText)
            pPlugin->pPluginCalls->OnPlayerText(bytePlayerId, text, length);
    }
}

void CEvents::OnNPCEnterVehicle(uint16_t vehicleid, uint8_t seatid)
{
    call_OnNPCEnterVehicle(vehicleid, seatid);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnNPCEnterVehicle)
            pPlugin->pPluginCalls->OnNPCEnterVehicle(vehicleid, seatid);
    }
}

void CEvents::OnNPCExitVehicle()
{
    call_OnNPCExitVehicle();
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnNPCExitVehicle)
            pPlugin->pPluginCalls->OnNPCExitVehicle();
    }
}

void CEvents::OnPlayerStreamIn(uint8_t bytePlayerId)
{
    call_OnPlayerStreamIn(bytePlayerId);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnPlayerStreamIn)
            pPlugin->pPluginCalls->OnPlayerStreamIn(bytePlayerId);
    }
}

void CEvents::OnPlayerStreamOut(uint8_t playerid)
{
    call_OnPlayerStreamOut(playerid);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnPlayerStreamOut)
            pPlugin->pPluginCalls->OnPlayerStreamOut(playerid);
    }
}

void CEvents::OnVehicleStreamIn(uint16_t vehicleid)
{
    call_OnVehicleStreamIn(vehicleid);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnVehicleStreamIn)
            pPlugin->pPluginCalls->OnVehicleStreamIn(vehicleid);
    }
}

void CEvents::OnVehicleStreamOut(uint16_t vehicleid)
{
    call_OnVehicleStreamOut(vehicleid);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnVehicleStreamOut)
            pPlugin->pPluginCalls->OnVehicleStreamOut(vehicleid);
    }
}

void CEvents::OnNPCDisconnect(uint8_t reason)
{
    call_OnNPCDisconnect(reason);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnNPCDisconnect)
            pPlugin->pPluginCalls->OnNPCDisconnect(reason);
    }
}

void CEvents::OnSniperRifleFired(uint8_t playerid, uint8_t weapon, float x, float y, float z, float dx, float dy, float dz)
{
    call_OnSniperRifleFired(playerid, weapon, x, y, z, dx, dy, dz);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnSniperRifleFired)
            pPlugin->pPluginCalls->OnSniperRifleFired(playerid, weapon, x, y, z, dx, dy, dz);
    }
}

void CEvents::OnPlayerUpdate(uint8_t bytePlayerId, vcmpPlayerUpdate updateType)
{
    call_OnPlayerUpdate(bytePlayerId, updateType);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnPlayerUpdate)
            pPlugin->pPluginCalls->OnPlayerUpdate(bytePlayerId, updateType);
    }
}

void CEvents::OnNPCConnect(uint8_t byteId)
{
    call_OnNPCConnect(byteId);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnNPCConnect)
        {
            pPlugin->pPluginCalls->OnNPCConnect(byteId);
        }
    }
}

void CEvents::OnClientMessage(uint8_t r, uint8_t g, uint8_t b, char* message, uint16_t len)
{
    call_OnClientMessage(r, g, b, (char*)message, len);
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnClientMessage)
            pPlugin->pPluginCalls->OnClientMessage(r, g, b, message, len);
    }
}

void CEvents::OnNPCSpawn()
{
    call_OnNPCSpawn();
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnNPCSpawn)
            pPlugin->pPluginCalls->OnNPCSpawn();
    }
}

void CEvents::OnCycle()
{
    PluginPool_s* pPlugin;
    for (DWORD i = 0; i < m_pPlugins->GetPluginCount(); i++)
    {
        pPlugin = m_pPlugins->GetPlugin(i);
        if (pPlugin->pPluginCalls->OnCycle)
            pPlugin->pPluginCalls->OnCycle();
    }
}

