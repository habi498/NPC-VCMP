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
#ifdef _REL047
#include "dependencies/slikenet_0.1.3_source/slikenet/Source/RakPeerInterface.h"
#include "dependencies/slikenet_0.1.3_source/slikenet/Source/MessageIdentifiers.h"
#include "dependencies/slikenet_0.1.3_source/slikenet/Source/BitStream.h"
#include "dependencies/slikenet_0.1.3_source/slikenet/Source/RakNetTypes.h"
#else
#include "Raknet/RakPeerInterface.h"
#include "Raknet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"  // MessageID
#endif
#ifdef _WIN32

#else
#define Sleep(x) usleep(x * 1000)
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <byteswap.h>
#define _byteswap_ushort(x) __bswap_16 (x)
#include <sys/stat.h> //for mkdir
#endif
uint8_t GetSlotIdFromWeaponId(uint8_t byteWeapon);
#include "system.h"
#include <Utils.h>
#include "npc.h"
#include "sqvm.h"
#include "Playback.h"
#include "messages.h"

#include "Player.h"
#include "Playerpool.h"
#include "VehiclePool.h"
#include "pickuppool.h"
#include "checkpointpool.h"
#include "objectpool.h"
#include "npc32.h"
#include "CTimer.h"
#include "Plugins.h"
#include "ClientFunctions.h"
#include "EventHandler.h"
#include "SpawnClass.h"
#ifdef _REL047
#include "zlib.h"
#endif
uint32_t bytes_swap_u32(uint32_t i);
#define CONFIG_SYNC_ON_PLAYER_STREAMIN 1
#define CONFIG_RESTORE_WEAPON_ON_SKIN_CHANGE    2
#include "Exports.h"
#define ZeroVEC(V){V.X=0;V.Y=0;V.Z=0;}
#define MulVEC(V,f){V.X*=f;V.Y*=f;V.Z*=f;}
#define PI 3.1415926

#define REC_DIR "npcscripts/recordings"
#define PLAYBACK_OVERRIDE_VEHICLEID


#define OF_FLAG_WEAPON 0x40
#define OF_FLAG_ARMOUR 0x04
#define OF_FLAG_SPEED 0x01
#define OF_FLAG_KEYS_OR_ACTION 0x10
//#define OF_FLAG_CROUCHING 0x80
#define OF_FLAG_EXTRA_NIBBLE 0x80
#define OF_FLAG_NOHEALTH 0x08
#define OF_FLAG_NIBBLE_CROUCHING 0x2
#define OF_FLAG_NIBBLE_RELOADING 0x8

#define IC_FLAG_ARMOUR 0x40
#define IC_FLAG_WEAPON 0x80
#define IC_TFLAG_SPEED 0x01
#define IC_TFLAG_CARHEALTH 0x04
#define IC_TFLAG_DAMAGE 0x02
#define CHAT_MESSAGE_ORDERING_CHANNEL 3
#define CHECKPOINT_ENTER_ORDERING_CHANNEL 1
#define NPC_RECFILE_IDENTIFIER_V1 1000
#define NPC_RECFILE_IDENTIFIER_V2 1001
#define NPC_RECFILE_IDENTIFIER_V3 1002 //From Nov 2022 onwards
#define NPC_RECFILE_IDENTIFIER_V4 1004 //From August 2023 onwards
#define NPC_RECFILE_IDENTIFIER_V5 1005 //From December 2023 onwards
//#define NPC_RECFILE_IDENTIFIER_V6 1006 //From August 2024 onwards
//#define NPC_RECFILE_IDENTIFIER_LATEST NPC_RECFILE_IDENTIFIER_V6
#define NPC_RECFILE_IDENTIFIER_LATEST NPC_RECFILE_IDENTIFIER_V5
#define CYCLE_SLEEP_OLD 30  //30 ms sleep between cycles. 
#define CYCLE_SLEEP 5
int ConnectToServer(std::string hostname, int port, std::string npcname, std::string password);
void OnCycle();//in Playback.cpp
//void SendNPCUpdate();//in UpdateNPC.cpp
void SendNPCSyncData(ONFOOT_SYNC_DATA* m_pOfSyncData, PacketPriority priority = HIGH_PRIORITY);
void SendNPCOfSyncDataLV(PacketPriority prioty=HIGH_PRIORITY);// in UpdateNPC.cpp
void SendPassengerSyncData();
void SendNPCSyncData(INCAR_SYNC_DATA* m_pInSyncData, PacketPriority mPriority = HIGH_PRIORITY);
void SendNPCIcSyncDataLV(PacketPriority prioty = HIGH_PRIORITY);
void WriteNibble(uint8_t nibble, RakNet::BitStream* bsOut);
#endif