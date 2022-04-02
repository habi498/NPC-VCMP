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
#define ZeroVEC(V){V.X=0;V.Y=0;V.Z=0;}
#define MulVEC(V,f){V.X*=f;V.Y*=f;V.Z*=f;}
#define PI 3.1415926

#define REC_DIR "npcscripts/recordings"
#define PLAYBACK_OVERRIDE_VEHICLEID
//Taken from VCMP plugin header
typedef enum {
	vcmpPlayerUpdateNormal = 0,
	vcmpPlayerUpdateAiming = 1,
	vcmpPlayerUpdateDriver = 2,
	vcmpPlayerUpdatePassenger = 3,
	forceSizeVcmpPlayerUpdate = INT32_MAX
} vcmpPlayerUpdate;
int ConnectToServer(std::string hostname, int port, std::string npcname, std::string password);
void OnServerCycle();//in Playback.cpp
void SendNPCUpdate();//in UpdateNPC.cpp
void WriteNibble(uint8_t nibble, RakNet::BitStream* bsOut);
#endif