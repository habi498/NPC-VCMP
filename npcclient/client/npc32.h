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
#ifndef NPC32LIB_H
#define NPC32LIB_H
#ifdef _WIN32
#endif
enum Version
{
    REL004 = 67000,
    REL006 = 67400
};
#ifdef _REL004
uint32_t ConnectAsVCMPClient(RakNet::RakPeerInterface* peer, const char* npcname, uint8_t len, RakNet::SystemAddress systemAddress, uint32_t ver=REL004);
#else
uint32_t ConnectAsVCMPClient(RakNet::RakPeerInterface* peer, const char* npcname, uint8_t len, RakNet::SystemAddress systemAddress, uint32_t ver = REL006);
#endif
#endif