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
#ifndef MAIN_H
#define MAIN_H
#include <string>
#include <stdio.h>

#include "Raknet/RakPeerInterface.h"
#include "Raknet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"  // MessageID
#ifdef _WIN32

#else
#define Sleep(x) { usleep(x * 1000); }
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <byteswap.h>
#define _byteswap_ushort(x) __bswap_16 (x)
#endif

#include "system.h"
#include "npc.h"
#include "sqvm.h"
#include "Playback.h"
#include "messages.h"
#include "SYNC_DATA.h"
#include "Player.h"
#include "Playerpool.h"
#include "npc32.h"
#include "CTimer.h"
#include "ClientFunctions.h"
#define ZeroVEC(V){V.X=0;V.Y=0;V.Z=0;}
#define MulVEC(V,f){V.X*=f;V.Y*=f;V.Z*=f;}
#define PI 3.1415926

#define REC_DIR "npcscripts/recordings"
#define PLAYBACK_OVERRIDE_VEHICLEID


#define OF_FLAG_WEAPON 0x40
#define OF_FLAG_ARMOUR 0x04
#define OF_FLAG_SPEED 0x01
#define OF_FLAG_KEYS 0x10
#define OF_FLAG_CROUCHING 0x80
#define OF_FLAG_NOHEALTH 0x08

#define IC_FLAG_ARMOUR 0x40
#define IC_FLAG_WEAPON 0x80
#define IC_TFLAG_SPEED 0x01
#define IC_TFLAG_CARHEALTH 0x04
#define IC_TFLAG_DAMAGE 0x02
int ConnectToServer(std::string hostname, int port, std::string npcname, std::string password);
void OnServerCycle();//in Playback.cpp
void SendNPCUpdate();//in UpdateNPC.cpp
void SendNPCOfSyncDataLV();// in UpdateNPC.cpp
void WriteNibble(uint8_t nibble, RakNet::BitStream* bsOut);
#endif