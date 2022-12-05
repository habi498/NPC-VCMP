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
HSQUIRRELVM v; NPC* iNPC = new NPC();
RakNet::RakPeerInterface* peer;
RakNet::SystemAddress systemAddress;
CPlayerPool* m_pPlayerPool;
CPlayer* npc;
#define CAR_HEALTH_MAX 1000
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, INCAR_SYNC_DATA* m_pIcSyncDataOut, uint8_t* bytePlayerIdOut);
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, ONFOOT_SYNC_DATA* m_pOfSyncDataOut, uint8_t* bytePlayerIdOut);
VECTOR ConvertFromUINT16_T(uint16_t w_ComponentX, uint16_t w_ComponentY, uint16_t w_ComponentZ, float base);
float ConvertFromUINT16_T(uint16_t compressedFloat, float base);
uint8_t GetSlotId(uint8_t byteWeapon); 
unsigned short calculate(unsigned char h, unsigned char t, unsigned char r)
{
	return h * 32 + t * 2 + (r >> 3);
}
bool ReadNibble(uint8_t* nibble, RakNet::BitStream* bsIn)
{
	uint8_t bytearray[] = { 0 };
	bool bSuccess=bsIn->ReadBits(bytearray, 4);
	if (bSuccess)
	{
		*nibble = bytearray[0];
		return true;
	}
	else return false;
}
void ApplyRotationFlags(QUATERNION* quatRotation, uint8_t flag)
{
	if (flag & 4)
		quatRotation->X > 0 ? quatRotation->X *= -1 : 0;
	if (flag & 2)
		quatRotation->Y > 0 ? quatRotation->Y *= -1 : 0;
	if (flag & 1)
		quatRotation->Z > 0 ? quatRotation->Z *= -1 : 0;
	if (flag & 8)
		quatRotation->W > 0 ? quatRotation->W *= -1 : 0;
}
int ConnectToServer(std::string hostname, int port, std::string npcname,std::string password)
{
	peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;
	
	RakNet::SocketDescriptor sd;
	peer->Startup(1, &sd, 1);
	
	printf("Starting the client.\n");
	peer->Connect(hostname.c_str(), port, password.c_str(), password.length() + 1);
	
	if (!m_pPlayerPool)
		m_pPlayerPool = new CPlayerPool();
#ifndef WIN32
	std::setvbuf(stdout, NULL, _IONBF, 0);
#endif
	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			//printf("packet with data[0] = %d received\n", packet->data[0]);
			switch (packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				ConnectAsVCMPClient(peer, npcname.c_str(), (uint8_t)npcname.length(), packet->systemAddress);
				systemAddress = packet->systemAddress;
			}
			break;
			case ID_SERVER_MESSAGE_CLIENT_CONNECT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t playerid;
				bsIn.ReadAlignedBytes(&playerid, 1);
				bsIn.IgnoreBytes(6);
				uint8_t namelen;
				bsIn.ReadAlignedBytes(&namelen, 1);
				char* playername;
				playername = (char*)malloc(sizeof(char) * (namelen + 1));
				bsIn.Read(playername, namelen);
				if (playername)
				{
					playername[namelen] = '\0';
					if (m_pPlayerPool->GetSlotState(playerid))
					{
						if (strcmp(playername, m_pPlayerPool->GetPlayerName(playerid)) == 0);
						else
						{
							printf("Same slot- different player names\n");
							m_pPlayerPool->Delete(playerid);
							m_pPlayerPool->New(playerid, playername);
						}
					}
					else
					{
						m_pPlayerPool->New(playerid, playername);
					}
				}
				else
				{
					printf("Failed to allocate name\n");
					exit(1);
				}
			}
			break;
			case ID_GAME_MESSAGE_PLAYERUPDATE_PASSENGER:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint8_t bytePlayerId;
				bsIn.Read(bytePlayerId);
				uint8_t byteIDrem; uint8_t byteIDquotient;
				bsIn.Read(byteIDrem);//R-- 
				ReadNibble(&byteIDquotient,&bsIn);//Q-- Quotient
				uint16_t wVehicleID;
				wVehicleID = byteIDrem + (byteIDquotient >> 2);
				CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
				if (player)
				{
					player->m_VehicleID = wVehicleID;
					player->SetState(PLAYER_STATE_PASSENGER);
				}
			}
			break;
			case ID_GAME_MESSAGE_PLAYERUPDATE_DRIVER:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				uint8_t bytePlayerId;
				INCAR_SYNC_DATA m_IcSyncData; 
				ReceivePlayerSyncData(&bsIn, &m_IcSyncData, &bytePlayerId);
				CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
				if (player)
					player->StoreInCarFullSyncData(&m_IcSyncData);
				call_OnPlayerUpdate(bytePlayerId, vcmpPlayerUpdateDriver);
			}
			break;
			case ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM:
			case ID_GAME_MESSAGE_ONFOOT_UPDATE:
			{
				//Receive on foot update of other players. 
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				uint8_t bytePlayerId;
				ONFOOT_SYNC_DATA m_OfSyncData;
				ReceivePlayerSyncData(&bsIn, &m_OfSyncData, &bytePlayerId);
				CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
				if (player)
				{
					player->StoreOnFootFullSyncData(&m_OfSyncData); 
					if (packet->data[0] == ID_GAME_MESSAGE_ONFOOT_UPDATE)
						call_OnPlayerUpdate(bytePlayerId, vcmpPlayerUpdateNormal);
					else
						call_OnPlayerUpdate(bytePlayerId, vcmpPlayerUpdateAiming);
					/*int top = sq_gettop(v); //saves the stack size before the call
					sq_pushroottable(v); //pushes the global table
					sq_pushstring(v, _SC("OnPlayerUpdate"), -1);
					if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
						sq_pushroottable(v); //push the '.dw'
						sq_pushinteger(v, bytePlayerId);
						uint8_t updateType = packet->data[0] == ID_GAME_MESSAGE_ONFOOT_UPDATE ? vcmpPlayerUpdateNormal :
							vcmpPlayerUpdateAiming;
						sq_pushinteger(v, updateType);
						sq_newclass(v, false);
						
						sq_pushstring(v, _SC("Keys"), -1);
						sq_pushinteger(v, m_OfSyncData.dwKeys);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("PosX"), -1);
						sq_pushfloat(v, m_OfSyncData.vecPos.X);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("PosY"), -1);
						sq_pushfloat(v, m_OfSyncData.vecPos.Y);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("PosZ"), -1);
						sq_pushfloat(v, m_OfSyncData.vecPos.Z);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("Angle"), -1);
						sq_pushfloat(v, m_OfSyncData.fAngle);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("Health"), -1);
						sq_pushinteger(v, m_OfSyncData.byteHealth);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("Armour"), -1);
						sq_pushinteger(v, m_OfSyncData.byteArmour);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("Weapon"), -1);
						sq_pushinteger(v, m_OfSyncData.byteCurrentWeapon);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("IsCrouching"), -1);
						sq_pushbool(v, m_OfSyncData.IsCrouching);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("SpeedX"), -1);
						sq_pushfloat(v, m_OfSyncData.vecSpeed.X);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("SpeedY"), -1);
						sq_pushfloat(v, m_OfSyncData.vecSpeed.Y);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("SpeedZ"), -1);
						sq_pushfloat(v, m_OfSyncData.vecSpeed.Z);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("AimDirX"), -1);
						sq_pushfloat(v, m_OfSyncData.vecAimDir.X);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("AimDirY"), -1);
						sq_pushfloat(v, m_OfSyncData.vecAimDir.Y);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("AimDirZ"), -1);
						sq_pushfloat(v, m_OfSyncData.vecAimDir.Z);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("AimPosX"), -1);
						sq_pushfloat(v, m_OfSyncData.vecAimPos.X);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("AimPosY"), -1);
						sq_pushfloat(v, m_OfSyncData.vecAimPos.Y);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("AimPosZ"), -1);
						sq_pushfloat(v, m_OfSyncData.vecAimPos.Z);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("IsReloading"), -1);
						sq_pushbool(v, m_OfSyncData.bIsReloading);
						sq_newslot(v, -3, SQTrue);

						sq_pushstring(v, _SC("Ammo"), -1);
						sq_pushinteger(v, m_OfSyncData.wAmmo);
						sq_newslot(v, -3, SQTrue);

						sq_call(v, 4, 0, 1); //calls the function 
					}
					sq_settop(v, top); //restores the original stack size */
					

				}
			}
			break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				call_OnNPCDisconnect(0);
				m_pPlayerPool->Delete(iNPC->GetID());
				StopSquirrel();
				printf("Disconnected\n");
				exit(0);
			case ID_CONNECTION_LOST:
				call_OnNPCDisconnect(0);
				m_pPlayerPool->Delete(iNPC->GetID());
				StopSquirrel();
				printf("Disconnected\n");
				exit(0);
			case ID_GAME_MESSAGE_JOIN:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t npcid;//same as playerid
				bsIn.ReadAlignedBytes(&npcid, 1);
				iNPC->SetID(npcid);
				char* name;
				name = (char*)malloc(sizeof(char) * (npcname.length() + 1));
				if (name)
				{
					strcpy(name, npcname.c_str());
					name[npcname.length()] = '\0';
					bool bSuccess=m_pPlayerPool->New(npcid, name);
					if (bSuccess)
						npc = m_pPlayerPool->GetAt(npcid);
					if (!bSuccess || !npc)
					{
						exit(1);
					}
					delete name;
				}
				else {
					exit(1);
				}
				call_OnNPCConnect(iNPC->GetID());
				RakNet::BitStream bsOut, bsOut2;
				bsOut.Write((RakNet::MessageID)0xb9);
				bsOut.Write0(); bsOut.Write0(); bsOut.Write0();
				peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				bsOut2.Write((RakNet::MessageID)0xbd);
				bsOut2.Write0(); bsOut2.Write0(); bsOut2.Write0();
				bsOut2.Write1(); bsOut2.Write1();
				peer->Send(&bsOut2, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				//Not tested thouroughly
				{
					RakNet::BitStream bsOut3;
					bsOut3.Write((RakNet::MessageID)ID_GAME_MESSAGE_REQUEST_CLASS);
					bsOut3.Write0();
					peer->Send(&bsOut3, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 5, packet->systemAddress, false);
				}
				
				//Sometimes client sends an additional message which is omitted here: 0xbd. 
			}	
			break;
			case ID_GAME_MESSAGE_CLIENT_MESSAGE:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint8_t r, g, b;
				bsIn.ReadAlignedBytes(&r, 1);
				bsIn.ReadAlignedBytes(&g, 1);
				bsIn.ReadAlignedBytes(&b, 1);
				bsIn.IgnoreBytes(3);//Ignore two bytes.
				uint8_t* x;
				x = (uint8_t*)malloc(2);
				if (x != NULL)
				{
					uint16_t len;
					bsIn.ReadAlignedBytes(x, 2);
					len = x[0] * 256 + x[1];
					char* message;
					message = (char*)malloc(len + 1);
					if (message != NULL)
					{
						bsIn.ReadAlignedBytes((unsigned char*)message, len);
						message[len] = '\0';
						call_OnClientMessage(r, g, b, (char*)message, len);
						free(message);
					}
					free(x);
				}
			}
			break;
			/*case 0xa0:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint8_t playerid;
				bsIn.ReadAlignedBytes(&playerid, 1);
				if (iNPC && iNPC->Initialized() && playerid == iNPC->GetID() && iNPC->IsSpawned() == false)
				{
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_REQUEST_SPAWN);
					peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 5, packet->systemAddress, false);
				}
			}
			break;
			*/
			case ID_GAME_MESSAGE_CLASS_GRANTED:
			{
				//::BitStream bsIn(packet->data, packet->length, false);
				//bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				if (iNPC && iNPC->Initialized() && iNPC->IsSpawned() == false)
				{
					if ((iNPC->SpawnClass - iNPC->ClassSelectionCounter++)==0)
					{
						RakNet::BitStream bsOut;
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_REQUEST_SPAWN);
						peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 5, packet->systemAddress, false);
					}else
					{
						RakNet::BitStream bsOut3;
						bsOut3.Write((RakNet::MessageID)ID_GAME_MESSAGE_REQUEST_CLASS);
						bsOut3.Write((uint8_t)1);
						peer->Send(&bsOut3, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 5, packet->systemAddress, false);
						
					}
					
					
				}
			}
			break;
			case ID_GAME_MESSAGE_SPAWN_GRANTED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				if (iNPC->IsSpawned() == false)
				{
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_SPAWN);
					peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE, 0, packet->systemAddress, false);
					npc->m_byteHealth = 100;
					npc->SetState(PLAYER_STATE_SPAWNED);
					//npc->m_byteCurWeap = 0;
					iNPC->SetSpawnStatus(true);
					if (iNPC->bSpawnAtXYZ)
					{
						npc->m_vecPos.X = iNPC->fSpawnLocX;
						npc->m_vecPos.Y = iNPC->fSpawnLocY;
						npc->m_vecPos.Z = iNPC->fSpawnLocZ;
						npc->m_fAngle = iNPC->fSpawnAngle;
						
						iNPC->StoreFirstSpawnedTime();//for removing lock
						iNPC->bSpawnAtXYZ = false; //turn off
					}
					if (iNPC->SpecialSkin != NOT_DEFINED )
					{
						char buffer[100];
						sprintf(buffer, "_npc_skin_request %d", iNPC->SpecialSkin);
						SendCommandToServer(buffer);
						iNPC->SpecialSkin = 255;//turn off the feature
					}
					if (iNPC->SpawnWeapon != NOT_DEFINED )
					{
						npc->SetCurrentWeapon(iNPC->SpawnWeapon);
						iNPC->SpawnWeapon = 255;//turn off the feature.
					}
					call_OnNPCSpawn();
				}
			}
			break;
			case ID_GAME_MESSAGE_PLAYER_DEATH:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t playerid;
				bsIn.ReadAlignedBytes(&playerid, 1);
				CPlayer* player = m_pPlayerPool->GetAt(playerid);
				if (player)
					player->SetState(PLAYER_STATE_WASTED);
				call_OnPlayerDeath(playerid);
			}
			break;
			case ID_GAME_MESSAGE_PLAYER_SPAWN:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(7); 
				uint8_t bytePlayerId;
				if (bsIn.Read(bytePlayerId))
				{
					CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
					if (player)
						player->SetState(PLAYER_STATE_SPAWNED);
				}
			}
			break;
			case ID_GAME_MESSAGE_PLAYER_TEXT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t playerid;
				bsIn.ReadAlignedBytes(&playerid, 1);
				bsIn.IgnoreBytes(1);
				unsigned char len[2];
				bsIn.ReadBits(len, 12);
				uint16_t length = calculate(len[0] >> 4, (len[0] % 16), len[1]);
				unsigned char* text;
				text = (unsigned char*)malloc((length) * sizeof(char));
				uint8_t rem = len[1];
				if (text != NULL)
				{
					unsigned char temp;
					for (int i = 0; i < length; i++)
					{
						bsIn.ReadBits(&temp, 8);
						text[i] = (unsigned char)calculate((rem % 8), (temp >> 4), temp % 16);
						rem = temp;
					}
					call_OnPlayerText(playerid, (char*)text, length);
				}
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_ENTER:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t playerid; uint8_t vd[2];
				bsIn.ReadAlignedBytes(&playerid, 1);
				bsIn.ReadAlignedBytes(vd, 2);
				uint16_t vehicleid;
				vehicleid = vd[0] * 256 + vd[1];
				uint8_t driver; uint8_t seat[2];
				bsIn.ReadAlignedBytes(seat, 2);
				bsIn.ReadAlignedBytes(&driver, 1);
				uint8_t seatid = -1;
				if (driver == 0x12)seatid = 0;
				else if (driver == 0x11)
				{
					if (seat[0] == 0x10 && seat[1] == 0x02)seatid = 2;
					else if (seat[0] == 0x0c && seat[1] == 0x08)seatid = 3;
					else if (seat[0] == 0x0b && seat[1] == 0x04)seatid = 1;
					else seatid = 1;//anyway passenger
				}
				CPlayer* player = m_pPlayerPool->GetAt(playerid);
				if (player)
				{
					if (seatid == 0)
						player->SetState(PLAYER_STATE_ENTER_VEHICLE_DRIVER);
					else
						player->SetState(PLAYER_STATE_ENTER_VEHICLE_PASSENGER);
					player->m_VehicleID = vehicleid;
				}
				if (iNPC && iNPC->Initialized()  && playerid == iNPC->GetID())
				{
					//npc->m_VehicleID = vehicleid;
					call_OnNPCEnterVehicle(vehicleid, seatid);
				}
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_EXIT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t bytePlayerId;
				bsIn.Read(bytePlayerId);
				CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
				if (player)
				{
					player->SetState(PLAYER_STATE_EXIT_VEHICLE);
					player->m_VehicleID = 0;
				}
			}
			break;
			case ID_GAME_MESSAGE_NPC_VEHICLE_EXIT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t vehicleid;
				bsIn.Read(vehicleid);
				if (vehicleid == npc->m_VehicleID)
				{
					npc->m_VehicleID = 0;//not clear. server sending many 0xf8/0xf9 messages
					call_OnNPCExitVehicle();
				}
			}
			break;
			case ID_GAME_MESSAGE_PLAYER_STREAM_IN:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t playerid;
				bsIn.ReadAlignedBytes(&playerid, 1);
				iNPC->AddStreamedPlayer(playerid);
				call_OnPlayerStreamIn(playerid);
				
			}
			break;
			case ID_GAME_MESSAGE_PLAYER_STREAM_OUT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t playerid;
				bsIn.ReadAlignedBytes(&playerid, 1);
				iNPC->RemoveStreamedPlayer(playerid);
				call_OnPlayerStreamOut(playerid);
			}
			break; 
			case ID_GAME_MESSAGE_VEHICLE_STREAM_IN:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint8_t vehdata[2];
				bsIn.ReadAlignedBytes(vehdata, 2);
				uint16_t vehicleid;
				vehicleid = vehdata[0] * 256 + vehdata[1];
				iNPC->AddStreamedVehicle(vehicleid);
				call_OnVehicleStreamIn(vehicleid);
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_STREAM_OUT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint8_t vehdata[2];
				bsIn.ReadAlignedBytes(vehdata, 2);
				uint16_t vehicleid;
				vehicleid = vehdata[0] * 256 + vehdata[1];
				iNPC->DestreamVehicle(vehicleid);
				call_OnVehicleStreamOut(vehicleid);
			}
			break; 
			case ID_GAME_MESSAGE_DISCONNECT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t playerid;
				bsIn.ReadAlignedBytes(&playerid,1);
				uint8_t reason;
				bsIn.ReadAlignedBytes(&reason, 1);
				if (iNPC && iNPC->Initialized() && playerid == iNPC->GetID())
				{
					call_OnNPCDisconnect(reason);
					m_pPlayerPool->Delete(playerid);
					StopSquirrel();
					exit(0);
				}
				else
					m_pPlayerPool->Delete(playerid);
				//printf("Player %d disconnected. Reason %d", playerid,reason);
			}
			break;
			case ID_GAME_MESSAGE_NAME_CHANGE:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t playerId;
				bsIn.Read(playerId);
				uint8_t len;
				bsIn.Read(len);
				char* name;
				name = (char*)malloc(sizeof(char) * len);
				if (name)
				{
					bsIn.Read(name, len);
					if(m_pPlayerPool->GetAt(playerId))
						m_pPlayerPool->SetPlayerName(playerId, name);
				}
			}
			break;
			case ID_GAME_MESSAGE_SET_POSITION:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				if (!iNPC->IsSyncPaused())
				{
					bsIn.Read(npc->m_vecPos.Z);
					bsIn.Read(npc->m_vecPos.Y);
					bsIn.Read(npc->m_vecPos.X);
					if (iNPC->IsSpawned())
						SendNPCOfSyncDataLV();
				}
			}
			break;
			case ID_GAME_MESSAGE_SET_ANGLE:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				//NPC has spawn position or spawn angle set. don't read this
				if (!iNPC->IsSyncPaused())
				{
					bsIn.Read(npc->m_fAngle);
				}
				/* The setting of angle by server when npc is
				spawning is used as a 'go' for sending
				spawn datas. Below it happens*/
				if (iNPC->IsSpawned())
					SendNPCOfSyncDataLV();
			}
			break;
			case ID_GAME_MESSAGE_SET_HEALTH:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t anticheatid; 
				bsIn.Read(anticheatid);
				iNPC->anticheatID = anticheatid;
				uint8_t newhealth;
				bsIn.Read(newhealth);
				npc->m_byteHealth = newhealth;
				if(!iNPC->IsSyncPaused())
					SendNPCOfSyncDataLV();
			}
			break;
			case ID_GAME_MESSAGE_SET_ARMOUR:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t anticheatid;
				bsIn.Read(anticheatid);
				iNPC->anticheatID = anticheatid;
				uint8_t newarmour;
				bsIn.Read(newarmour);
				npc->m_byteArmour = newarmour;
				if (!iNPC->IsSyncPaused())
					SendNPCOfSyncDataLV();
			}
			break;
			case ID_GAME_MESSAGE_SET_PLAYER_WEAPON_SLOT:
			{	
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				BYTE slotId;
				bsIn.Read(slotId);
				if (slotId < 9)
					npc->SetWeaponSlot(slotId, 0, 0);
			}
			break;
			case ID_GAME_MESSAGE_REMOVE_ALL_WEAPONS:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t anticheatid;
				bsIn.Read(anticheatid);
				iNPC->anticheatID = anticheatid;
				BYTE i;
				for (i = 0; i < 9; i++)
				{
					npc->m_byteSlotWeapon[i] = 0;
					npc->m_wSlotAmmo[i] = 0;
				}
				npc->SetCurrentWeapon(0);
			}
			break;
			case ID_GAME_MESSAGE_REMOVE_WEAPON:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t anticheatid;
				bsIn.Read(anticheatid);
				iNPC->anticheatID = anticheatid;
				uint8_t weapon; 
				bsIn.Read(weapon);
				npc->m_byteSlotWeapon[GetSlotId(weapon)] = 0;
				npc->m_wSlotAmmo[GetSlotId(weapon)] = 0;
				npc->SetCurrentWeapon(0);
			}
			break;
			case ID_GAME_MESSAGE_SET_WEAPON:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t anticheatid;
				bsIn.Read(anticheatid);
				iNPC->anticheatID = anticheatid;
				uint8_t weapon; uint16_t ammo;
				bsIn.Read(weapon);
				bsIn.Read(ammo);
				npc->SetWeaponSlot(GetSlotId(weapon), weapon, ammo);
				if (!iNPC->IsSyncPaused())
				{
					//Set current weapon
					npc->SetCurrentWeaponAmmo( ammo );
					if (npc->GetCurrentWeapon() != weapon)
					{
						npc->SetCurrentWeapon(weapon);
						if (iNPC->IsSpawned())
							SendNPCOfSyncDataLV();
					}
				}
				//one more byte is there. it was 01 when i tested.
			}
			break;
			case ID_GAME_MESSAGE_SNIPER_RIFLE_FIRED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);//need to find out 00 00 01 
				if (packet->length != 30)break;

				BYTE playerid;
				bsIn.Read(playerid);
				BYTE weapon;
				bsIn.Read(weapon);
				float x, y, z;
				bsIn.Read(z); bsIn.Read(y); bsIn.Read(x);
				float dx, dy, dz;
				bsIn.Read(dz); bsIn.Read(dy); bsIn.Read(dx);
				
				if( iNPC->GetID() != playerid )
					call_OnSniperRifleFired(playerid, weapon, x, y, z, dx, dy, dz);
			}
			break;
			}
		}
		OnServerCycle();
	}
	RakNet::RakPeerInterface::DestroyInstance(peer);
	return 0;
}
void DecodeIDandKeys(uint8_t nibble, uint8_t byte, uint16_t wKeys, INCAR_SYNC_DATA* m_pIcSyncData)
{
	m_pIcSyncData->dwKeys = wKeys;
	uint16_t wVehId = 0;
	wVehId |= nibble >> 2;
	wVehId |= ( (nibble & 3) * 256 );
	if (byte & 0x40)
	{
		m_pIcSyncData->dwKeys |= 65536;
		byte = byte ^ 0x40;
	}
	wVehId |= ( byte << 2 );
	m_pIcSyncData->VehicleID = wVehId;
}
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, INCAR_SYNC_DATA* m_pIcSyncDataOut, uint8_t* bytePlayerIdOut)
{
	bsIn->IgnoreBytes(1);
	bsIn->ReadAlignedBytes(bytePlayerIdOut, 1); 
	uint8_t action;
	bsIn->ReadAlignedBytes(&action, 1);
	uint16_t wKeys;
	bsIn->Read(wKeys);
	wKeys = _byteswap_ushort(wKeys);
	uint8_t byteVehId;
	bsIn->Read(byteVehId);
	uint8_t byteNibbleVehId;
	ReadNibble(&byteNibbleVehId, bsIn);
	DecodeIDandKeys(byteNibbleVehId, byteVehId, wKeys, m_pIcSyncDataOut);
	bsIn->Read(m_pIcSyncDataOut->bytePlayerHealth);
	if (action & 0x80)
	{
		bsIn->Read(m_pIcSyncDataOut->byteCurrentWeapon);
		if (m_pIcSyncDataOut->byteCurrentWeapon > 11)
			bsIn->IgnoreBytes(2);//ammo;
	}
	else
		m_pIcSyncDataOut->byteCurrentWeapon = 0;
	if (action & 0x40)
	{
		bsIn->Read(m_pIcSyncDataOut->bytePlayerArmour);
	}
	else
		m_pIcSyncDataOut->bytePlayerArmour = 0;
	uint8_t byteTurret;
	ReadNibble(&byteTurret,bsIn);
	uint8_t byteType;
	ReadNibble(&byteType, bsIn);
	bsIn->Read(m_pIcSyncDataOut->vecPos.X);
	bsIn->Read(m_pIcSyncDataOut->vecPos.Y);
	bsIn->Read(m_pIcSyncDataOut->vecPos.Z);
	if (byteType & 0x1)
	{
		uint16_t speedx, speedy, speedz;
		bsIn->Read(speedx); bsIn->Read(speedy); bsIn->Read(speedz);
		m_pIcSyncDataOut->vecMoveSpeed = ConvertFromUINT16_T(speedx, speedy, speedz, 20.0);
	}
	else
		ZeroVEC(m_pIcSyncDataOut->vecMoveSpeed);
	uint8_t byteRotFlag = 0;
	ReadNibble(&byteRotFlag, bsIn);
	uint16_t w_RotX, w_RotY, w_RotZ;
	bsIn->Read(w_RotX); bsIn->Read(w_RotY); bsIn->Read(w_RotZ);
	m_pIcSyncDataOut->quatRotation.X=ConvertFromUINT16_T(w_RotX,-1);
	m_pIcSyncDataOut->quatRotation.Y=ConvertFromUINT16_T(w_RotY,-1);
	m_pIcSyncDataOut->quatRotation.Z=ConvertFromUINT16_T(w_RotZ,-1);
	m_pIcSyncDataOut->quatRotation.W = \
		(float)sqrt(1 - (pow(m_pIcSyncDataOut->quatRotation.X, 2) + \
			pow(m_pIcSyncDataOut->quatRotation.Y, 2) + \
			pow(m_pIcSyncDataOut->quatRotation.Z, 2)));
	ApplyRotationFlags(&m_pIcSyncDataOut->quatRotation, byteRotFlag);
	if (byteType & 0x2)
		bsIn->Read(m_pIcSyncDataOut->dDamage);
	else
		m_pIcSyncDataOut->dDamage = 0;
	if (byteType & 0x4)
		bsIn->Read(m_pIcSyncDataOut->fCarHealth);
	else
		m_pIcSyncDataOut->fCarHealth = CAR_HEALTH_MAX;
	if (byteTurret)
	{
		uint16_t wHorizontal; uint16_t wVertical;
		bsIn->Read(wHorizontal); bsIn->Read(wVertical);
		m_pIcSyncDataOut->Turretx=ConvertFromUINT16_T(wHorizontal, 2 * (float)PI);
		m_pIcSyncDataOut->Turrety=ConvertFromUINT16_T(wVertical, 2 * (float)PI);
	}
	else
	{
		m_pIcSyncDataOut->Turretx = 0;
		m_pIcSyncDataOut->Turrety = 0;
	}
	//m_pIcSyncDataOut->VehicleID = wVehId;
	/*printf("Vehicle Data Arrived========\n \
Player ID:%u, Position: (%f,%f,%f), Vehicle Speed:(%f,%f,%f) Weapon: %u, \
Game Keys: %u, Health: %u, Armour: %u, Turretx: %f, Turrety: %f,\
VehicleID: %u, Damage: %u, Car Health: %f, Quaternion Rot:(%f,%f,%f,%f)\
", *bytePlayerIdOut, m_pIcSyncDataOut->vecPos.X, m_pIcSyncDataOut->vecPos.Y,
m_pIcSyncDataOut->vecPos.Z, m_pIcSyncDataOut->vecMoveSpeed.X, m_pIcSyncDataOut->vecMoveSpeed.Y,
m_pIcSyncDataOut->vecMoveSpeed.Z, m_pIcSyncDataOut->byteCurrentWeapon, m_pIcSyncDataOut->wKeys,
m_pIcSyncDataOut->bytePlayerHealth, m_pIcSyncDataOut->bytePlayerArmour,
m_pIcSyncDataOut->Turretx, m_pIcSyncDataOut->Turrety,
m_pIcSyncDataOut->VehicleID, m_pIcSyncDataOut->dDamage, m_pIcSyncDataOut->fCarHealth,
m_pIcSyncDataOut->quatRotation.X, m_pIcSyncDataOut->quatRotation.Y,
m_pIcSyncDataOut->quatRotation.Z, m_pIcSyncDataOut->quatRotation.W);*/

}
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, ONFOOT_SYNC_DATA* m_pOfSyncDataOut, uint8_t* bytePlayerIdOut)
{
	uint8_t byteMessageID; bool bIsAiming;
	bsIn->Read(byteMessageID);
	if (byteMessageID == ID_GAME_MESSAGE_ONFOOT_UPDATE)
		bIsAiming = false;
	else if (byteMessageID == ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM)
		bIsAiming = true;
	else exit(1);
	bsIn->ReadAlignedBytes(bytePlayerIdOut, 1);
	uint8_t action;
	bsIn->ReadAlignedBytes(&action, 1);
	uint8_t nibble = 0;
	if (action & OF_FLAG_CROUCHING)
	{
		//This means a nibble is there to read
		ReadNibble(&nibble, bsIn);//This may not be of anyuse in particular
	}
	float x, y, z;
	bsIn->Read(x); bsIn->Read(y); bsIn->Read(z);
	uint16_t w_encodedAngle;
	bsIn->Read(w_encodedAngle);
	float fAngle = ConvertFromUINT16_T(w_encodedAngle, 2 * (float)PI);
	uint16_t speedx = 0, speedy = 0, speedz = 0;
	VECTOR vecSpeed; vecSpeed.X = 0; vecSpeed.Y = 0; vecSpeed.Z = 0;
	if (action & OF_FLAG_SPEED)
	{
		bsIn->Read(speedx); bsIn->Read(speedy);
		bsIn->Read(speedz);
		vecSpeed.X = ConvertFromUINT16_T(speedx, 20.0);
		vecSpeed.Y = ConvertFromUINT16_T(speedy, 20.0);
		vecSpeed.Z = ConvertFromUINT16_T(speedz, 20.0);
	}
	uint8_t byteWeapon = 0; uint16_t ammo = 0;
	if (action & OF_FLAG_WEAPON)
	{
		bsIn->Read(byteWeapon);
		if (byteWeapon > 11)
			bsIn->Read(ammo);//ammo
	}
	uint32_t dw_Keys = 0;
	if (action & OF_FLAG_KEYS)
	{
		uint8_t keybyte1, keybyte2, msb; //bool bReloading = 0;
		bsIn->Read(keybyte1); bsIn->Read(keybyte2);
		bsIn->Read(msb);
		dw_Keys |= keybyte1;
		dw_Keys |= (keybyte2 << 8);
		if (msb & 0x8)dw_Keys |= 65536;//look behind keycode
		bsIn->IgnoreBits(4);//related to reloading weapon
	}
	uint8_t byteHealth;
	bsIn->Read(byteHealth);
	uint8_t byteArmour = 0;
	if (action & OF_FLAG_ARMOUR)
		bsIn->Read(byteArmour);
	bsIn->IgnoreBytes(1);
	if (bIsAiming)
	{
		uint16_t wAimDirX, wAimDirY, wAimDirZ;
		bsIn->Read(wAimDirX);	bsIn->Read(wAimDirY);
		bsIn->Read(wAimDirZ);
		float fAimDirX, fAimDirY, fAimDirZ;
		fAimDirX = ConvertFromUINT16_T(wAimDirX, 2 * (float)PI);
		fAimDirY = ConvertFromUINT16_T(wAimDirY, 2 * (float)PI);
		fAimDirZ = ConvertFromUINT16_T(wAimDirZ, 2 * (float)PI);
		m_pOfSyncDataOut->vecAimDir.X = fAimDirX;
		m_pOfSyncDataOut->vecAimDir.Y = fAimDirY;
		m_pOfSyncDataOut->vecAimDir.Z = fAimDirZ;
		float fAimPosX, fAimPosY, fAimPosZ;
		bsIn->Read(fAimPosX); bsIn->Read(fAimPosY);
		bsIn->Read(fAimPosZ);
		m_pOfSyncDataOut->vecAimPos.X = fAimPosX;
		m_pOfSyncDataOut->vecAimPos.Y = fAimPosY;
		m_pOfSyncDataOut->vecAimPos.Z = fAimPosZ;
	}
	//ONFOOT_SYNC_DATA m_pOfSyncDataPlayer;
	m_pOfSyncDataOut->byteArmour = byteArmour;
	m_pOfSyncDataOut->byteCurrentWeapon = byteWeapon;
	m_pOfSyncDataOut->wAmmo = ammo;
	m_pOfSyncDataOut->byteHealth = byteHealth;
	m_pOfSyncDataOut->dwKeys = dw_Keys;
	m_pOfSyncDataOut->fAngle = fAngle;
	m_pOfSyncDataOut->IsPlayerUpdateAiming = bIsAiming;
	m_pOfSyncDataOut->IsCrouching = (action & 0x80) != 0 ? 1 : 0;
	m_pOfSyncDataOut->vecPos.X = x;
	m_pOfSyncDataOut->vecPos.Y = y;
	m_pOfSyncDataOut->vecPos.Z = z;
	m_pOfSyncDataOut->vecSpeed = vecSpeed;
	/*printf("Data Arrived========\n \
Player ID:%u, Position: (%f,%f,%f), Angle:%f, Speed:(%f,%f,%f) Weapon: %u, \
Game Keys: %u, Health: %u, Armour: %u, AimDir: (%f,%f,%f), AimPos: (%f, %f %f)\
\n", *bytePlayerIdOut, x, y, z, fAngle, vecSpeed.X, vecSpeed.Y,\
vecSpeed.Z, byteWeapon, dw_Keys, byteHealth, byteArmour,\
m_pOfSyncDataOut->vecAimDir.X,m_pOfSyncDataOut->vecAimDir.Y,
m_pOfSyncDataOut->vecAimDir.Z,m_pOfSyncDataOut->vecAimPos.X, \
m_pOfSyncDataOut->vecAimPos.Y, m_pOfSyncDataOut->vecAimPos.Z);*/
}
uint8_t GetSlotId(uint8_t byteWeapon)
{
	uint8_t byteWeaponSlot;
	switch (byteWeapon)
	{
	case 0:
	case 1:
		byteWeaponSlot = 0;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		byteWeaponSlot = 1;
		break;
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		byteWeaponSlot = 2;
		break;
	case 17:
	case 18:
		byteWeaponSlot = 3;
		break;
	case 19:
	case 20:
	case 21:
		byteWeaponSlot = 4;
		break;
	case 22:
	case 23:
	case 24:
	case 25:
		byteWeaponSlot = 5;
		break;

	case 26:
	case 27:
		byteWeaponSlot = 6;
		break;
	case 28:
	case 29:
		byteWeaponSlot = 8;
		break;
	case 30:
	case 31:
	case 32:
	case 33:
		byteWeaponSlot = 7;
		break;

	default:
		// If invalid weapon then set fists as weapon
		byteWeaponSlot = 0;
		break;
	}
	return byteWeaponSlot;
}