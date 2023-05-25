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
#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H
class CEvents
{
public:
    void OnPlayerDeath(uint8_t bytePlayerId);
    void OnPlayerText(uint8_t bytePlayerId, char* text, uint16_t length);
    void OnNPCEnterVehicle(uint16_t vehicleid, uint8_t seatid);
    void OnNPCExitVehicle();
    void OnPlayerStreamIn(uint8_t bytePlayerId);
    void OnPlayerStreamOut(uint8_t playerid);
    void OnVehicleStreamIn(uint16_t vehicleid);
    void OnVehicleStreamOut(uint16_t vehicleid);
    void OnNPCDisconnect(uint8_t reason);
    void OnSniperRifleFired(uint8_t playerid, uint8_t weapon, float x, float y, float z, float dx, float dy, float dz);
    void OnPlayerUpdate(uint8_t bytePlayerId, vcmpPlayerUpdate updateType);
    void OnNPCConnect(uint8_t byteId);
    void OnClientMessage(uint8_t r, uint8_t g, uint8_t b, char* message, uint16_t len);
    void OnNPCSpawn();
    void OnCycle();
    void OnServerData(const uint8_t* data, size_t size);

    void OnExplosion(uint8_t byteExplosionType, VECTOR vecPos,
        uint8_t bytePlayerCaused, bool bIsOnGround);
    void OnProjectileFired(uint8_t bytePlayerId, uint8_t byteWeapon,
        VECTOR vecPos, float r1, float r2, float r3, float r4, float r5, float r6, float r7);
    uint8_t OnNPCClassSelect();

};
#endif