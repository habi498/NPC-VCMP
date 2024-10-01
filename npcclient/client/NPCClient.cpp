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
#ifdef WIN32
extern bool bShutdownSignal;
#endif
extern bool bStdoutRedirected;
extern uint32_t configvalue;
HSQUIRRELVM v; 
HSQAPI sq = NULL;
SquirrelExports* pExp;
NPC* iNPC = new NPC();
RakNet::RakPeerInterface* peer;
RakNet::SystemAddress systemAddress;
CPlayerPool* m_pPlayerPool = NULL;
CEvents* m_pEvents = NULL;
CVehiclePool* m_pVehiclePool = NULL;
CPickupPool* m_pPickupPool = NULL;
CCheckpointPool* m_pCheckpointPool = NULL;
CObjectPool* m_pObjectPool = NULL;
extern CFunctions* m_pFunctions;
CSpawnClassPool* m_pSpawnClassPool = NULL;
CPlayer* npc = NULL;
#define CAR_HEALTH_MAX 1000
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, INCAR_SYNC_DATA* m_pIcSyncDataOut, uint8_t* bytePlayerIdOut);
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, ONFOOT_SYNC_DATA* m_pOfSyncDataOut, uint8_t* bytePlayerIdOut);
VECTOR ConvertFromUINT16_T(uint16_t w_ComponentX, uint16_t w_ComponentY, uint16_t w_ComponentZ, float base);
float ConvertFromUINT16_T(uint16_t compressedFloat, float base);
uint8_t GetSlotIdFromWeaponId(uint8_t byteWeapon); 
extern bool bConsoleInputEnabled;
extern void CheckForConsoleInput();
extern DWORD ProcessPlaybacks();
extern Playback mPlayback;
void ReceivePassengerSyncData(RakNet::BitStream* bsIn, INCAR_SYNC_DATA* m_pIcSyncDataOut, uint8_t* byteSeatIdOut, uint8_t* bytePlayerIdOut);
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
bool ReadPickup(uint32_t* dwSerialNoOut, uint16_t* wPickupIDOut, RakNet::BitStream* bsIn)
{
	//t1,t2,t3 for reading serial no and pickup id
	uint16_t t1=0;
	bsIn->Read(t1);
	uint8_t t2=0;
	bsIn->Read(t2);
	*dwSerialNoOut = t1 + (t2 >> 3);
	uint8_t t3;
	if (bsIn->Read(t3))
	{
		*wPickupIDOut = (t2 & 7) * 256 + t3;
		return true;
	}
	return false;
}
int ConnectToServer(std::string hostname, int port, std::string npcname,std::string password)
{
	peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;
	
	RakNet::SocketDescriptor sd;
	peer->Startup(1, &sd, 1);
	
	printf("Connecting to %s: %d. ", hostname.c_str(), port);
	peer->Connect(hostname.c_str(), port, password.c_str(), password.length() + 1);
	
	if (!m_pPlayerPool)
		m_pPlayerPool = new CPlayerPool();
	if (!m_pEvents)
		m_pEvents = new CEvents();
	if (!m_pVehiclePool)
		m_pVehiclePool = new CVehiclePool();
	if (!m_pPickupPool)
		m_pPickupPool = new CPickupPool();
	if (!m_pCheckpointPool)
		m_pCheckpointPool = new CCheckpointPool();
	if (!m_pObjectPool)
		m_pObjectPool = new CObjectPool();
	if (!m_pSpawnClassPool)
		m_pSpawnClassPool = new CSpawnClassPool();
	
#ifndef WIN32
	std::setvbuf(stdout, NULL, _IONBF, 0);
#endif
	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Connected. ");
				ConnectAsVCMPClient(peer, npcname.c_str(), (uint8_t)npcname.length(), packet->systemAddress);
				systemAddress = packet->systemAddress;
			}
			break;
			case ID_GAME_MESSAGE_PLAYER_CONNECTED:
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
				INCAR_SYNC_DATA m_IcSyncData;
				uint8_t byteSeatId;
				ReceivePassengerSyncData(&bsIn,  &m_IcSyncData, &byteSeatId, &bytePlayerId);
				CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
				if (player)
				{
					player->m_wVehicleId = m_IcSyncData.VehicleID;
					player->m_byteSeatId = byteSeatId;
					player->m_byteHealth = m_IcSyncData.bytePlayerHealth;
					player->m_byteArmour = m_IcSyncData.bytePlayerArmour;
					player->SetState(PLAYER_STATE_PASSENGER);
					m_pEvents->OnPlayerUpdate(bytePlayerId, vcmpPlayerUpdatePassenger);
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
				{
					player->StoreInCarFullSyncData(&m_IcSyncData);
					CVehicle* vehicle = m_pVehiclePool->GetAt(m_IcSyncData.VehicleID);
					if(vehicle)
						vehicle->CopyFromIcSyncData(bytePlayerId, &m_IcSyncData);
					if (bytePlayerId != iNPC->GetID() && m_IcSyncData.VehicleID == npc->m_wVehicleId)
					{
						if (mPlayback.running && mPlayback.type == PLAYER_RECORDING_TYPE_ALL)
						{
							//do not do anything. rec file will handle passenger data, etc and position also
							break;
						}
						//This is the vehicle in which npc is sitting as a passenger.
						//Update position of npc locally (i.e. do not send to server, it comes from server )
						npc->m_vecPos = m_IcSyncData.vecPos;
						if (iNPC->PSLimit != DISABLE_AUTO_PASSENGER_SYNC)
						{
							if (iNPC->PSCounter >= iNPC->PSLimit)
							{
								SendPassengerSyncData();
								iNPC->PSCounter = 0;
							}
							else iNPC->PSCounter++;
							
							if (iNPC->PSOnServerCycle)
							{
								//Now no more server-cycle based passenger syncing
								//The driver will take care of syncing, as the same will be based on driver's update packets
								iNPC->PSOnServerCycle = false;
							}
						}
					}
				}
				m_pEvents->OnPlayerUpdate(bytePlayerId, vcmpPlayerUpdateDriver);
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
						m_pEvents->OnPlayerUpdate(bytePlayerId, vcmpPlayerUpdateNormal);
					else
						m_pEvents->OnPlayerUpdate(bytePlayerId, vcmpPlayerUpdateAiming);
				}
			}
			break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				m_pEvents->OnNPCDisconnect(0);
				m_pPlayerPool->Delete(iNPC->GetID());
				StopSquirrel();
				printf("Disconnect Notification\n");
				if(bStdoutRedirected)
					fclose(stdout);
				exit(0);
			case ID_CONNECTION_LOST:
				m_pEvents->OnNPCDisconnect(0);
				m_pPlayerPool->Delete(iNPC->GetID());
				StopSquirrel();
				printf("Connection Lost\n");
				if (bStdoutRedirected)
					fclose(stdout);
				exit(0);
			case ID_GAME_MESSAGE_JOIN:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t npcid;//same as playerid
				bsIn.ReadAlignedBytes(&npcid, 1);
				iNPC->SetID(npcid);
				printf("ID %d ", npcid);
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
					printf("%s\n", name);
					free(name);
				}
				else {
					exit(1);
				}
				m_pEvents->OnNPCConnect(iNPC->GetID());
				RakNet::BitStream bsOut, bsOut2;
				#ifdef _REL004
				bsOut.Write((RakNet::MessageID)0xb8);
				#elif defined(_REL047)
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_JOINED);
				#else
				bsOut.Write((RakNet::MessageID)0xb9);
				#endif			
				uint8_t byteZero = 0;
				bsOut.Write(byteZero);
				bsOut.Write(byteZero);
				bsOut.Write(byteZero);
				#ifdef _REL047
				bsOut.Write((uint16_t)0);
				#endif
				peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				
				m_pFunctions->RequestClass(0,false);
				
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
						m_pEvents->OnClientMessage(r, g, b, (char*)message, len);
						free(message);
					}
					free(x);
				}
			}
			break;
			case ID_GAME_MESSAGE_SPECIAL_SIGNAL:
			{
				//This is send along with class grant as well as spawn grant/not grant
				//::BitStream bsIn(packet->data, packet->length, false);
				//bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				if (iNPC && iNPC->Initialized() && iNPC->IsSpawned() == false
					&& iNPC->bIsClassRequested==true)
				{
					iNPC->bIsClassRequested = false;
					//Add class details send by server 
					if(m_pSpawnClassPool)
					{
						uint8_t byteTeam = npc->m_byteTeamId;
						uint8_t byteSkin = npc->m_byteSkinId;
						Color color = npc->GetColor();
						uint8_t byteMaxClass = m_pSpawnClassPool->GetCount();
						SpawnClass* m_pSpawnClass; bool bClassFound = false;

						//SEARCH if class with same skin,team and color(rgb) exists?
						//class id can be negative. so looping all possible values.
						for (int8_t i = -byteMaxClass; i < byteMaxClass; i++)
						{
							m_pSpawnClass = m_pSpawnClassPool->GetByID(i);
							if (m_pSpawnClass == NULL)
								continue;
							if (m_pSpawnClass)
							{
								if (m_pSpawnClass->byteSkin == byteSkin &&
									m_pSpawnClass->byteTeam == byteTeam &&
									m_pSpawnClass->m_Color.r == color.r &&
									m_pSpawnClass->m_Color.g == color.g &&
									m_pSpawnClass->m_Color.b == color.b)
								{
									//this class id already exists
									
									bClassFound = true; 
									if (i == 0)
									{
										//We have looped and came back.
										if (iNPC->byteClassIndexRequested == CLASS_PREVIOUS)
										{
											//We have came back left arrow
											if (iNPC->PotentialClassID < 0)
											{
												//time to change -ve values to +ve ones.
												//-5,-4,-3,-2,-1,0. from -5 to 0
												//total classes decrypted= -(-5) + 1 =6
												//Add 6
												//1,2,3,4,5,0
												// Taking into account, server can add classes, but not remove
												int8_t totalclasses = -iNPC->PotentialClassID + 1;
												m_pSpawnClassPool->ChangeSignToPositive(totalclasses);
											}
										}
									}
									iNPC->PotentialClassID = i;
									break;
								}
							}
						}
						if (!bClassFound)
						{
							//Create a new class
							int8_t newclassid;
							if (iNPC->byteClassIndexRequested == CLASS_PREVIOUS)
							{
								if (iNPC->PotentialClassID <= 0)
								{
									newclassid = iNPC->PotentialClassID - 1;
								}
								else
								{
									printf("Error solving class id. Requested CLASS_PREVIOUS, current: %d\n", iNPC->PotentialClassID);
									exit(0);
								}
							}
							else if (iNPC->byteClassIndexRequested == CLASS_NEXT)
							{
								if (iNPC->PotentialClassID < 0)
								{
									printf("Error solving class id. Requested CLASS_NEXT, current: %d\n", iNPC->PotentialClassID);
									exit(0);
								}
								else
								{
									newclassid = iNPC->PotentialClassID + 1;
								}
							}
							else if (iNPC->byteClassIndexRequested == CLASS_CURRENT)
							{
								if(byteMaxClass==0)
									newclassid = iNPC->PotentialClassID;//happens in beginning
								else
								{
									printf("Error solving class id. Requested CLASS_CURRENT, current: %d\n", iNPC->PotentialClassID);
									exit(0);
								}
							}
							//We have assigned negative class id. because client does not
							//know how many classes in server, unless looped through
							if (m_pSpawnClassPool->New(newclassid, byteTeam, byteSkin, color))
							{
								iNPC->PotentialClassID = newclassid;
							}
						}
					}
					if (m_pEvents->OnNPCClassSelect() == 0)
					{
						if (!(mPlayback.running && mPlayback.type == PLAYER_RECORDING_TYPE_ALL))
						{
							//Used to simulate arrow-right through the classes until the class got
							if (iNPC->GetSpawnClass() == iNPC->PotentialClassID || iNPC->AbsClassNotSpecified)
							{
								RakNet::BitStream bsOut;
								bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_REQUEST_SPAWN);
								peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 5, packet->systemAddress, false);
							}
							else
							{
								//Absolute class like class 7 is specified.
								//Scroll through classes until iNPC->GetSpawnClass()
								//Danger of specifying invalid class like 26 by user. (instead of weapon)
								//Check it
								if (iNPC->GetClassRequestCounter() >= MAX_CLASSES_FOR_SPAWNING)
								{
									printf("Error. Invalid class id: %d. Visited %d classes\n",iNPC->GetSpawnClass(), iNPC->GetClassRequestCounter());
									iNPC->UnsetSpawnClass();
									break;
								}
								else {
									m_pFunctions->RequestClass(CLASS_NEXT, false);//arrow-right
									iNPC->IncrementClassRequestCounter();
								}
							}
						}
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
					
					npc->m_byteArmour = 0;
					
					iNPC->SetSpawnStatus(true);
					if (iNPC->AltSpawnStatus==AltSpawn::ON)
					{
						npc->m_vecPos.X = iNPC->fSpawnLocX;
						npc->m_vecPos.Y = iNPC->fSpawnLocY;
						npc->m_vecPos.Z = iNPC->fSpawnLocZ;
						npc->m_fAngle = iNPC->fSpawnAngle;
						
						iNPC->StoreFirstSpawnedTime();//for removing lock
						iNPC->AltSpawnStatus = AltSpawn::SPAWNED; //turn off
					}
					if (iNPC->SpecialSkin != NOT_DEFINED )
					{
						char buffer[100];
						sprintf(buffer, "_npc_skin_request %d", iNPC->SpecialSkin);
						m_pFunctions->SendCommandToServer(buffer);
						iNPC->SpecialSkin = 255;//turn off the feature
					}
					if (iNPC->SpawnWeapon != NOT_DEFINED )
					{
						uint8_t slotId = GetSlotIdFromWeaponId(iNPC->SpawnWeapon);
						if (slotId != 0 || iNPC->SpawnWeapon == 1)
						{
							if (npc->GetSlotWeapon(slotId) == iNPC->SpawnWeapon)
							{
								//npc have weapon
								npc->SetCurrentWeapon(iNPC->SpawnWeapon);
							}
							else
							{
								char buffer[100];
								sprintf(buffer, "_npc_weapon_request %d", iNPC->SpawnWeapon);
								m_pFunctions->SendCommandToServer(buffer);
							}
						}
						else
							printf("Warning: Invalid Weapon %d to spawn specified when connecting\n", iNPC->SpawnWeapon);

						iNPC->SpawnWeapon = 255;//turn off the feature.
					}
					else
					{
						//Set weapon to that of slot 0
						npc->SetCurrentWeapon(npc->GetSlotWeapon(0));
					}
					m_pEvents->OnNPCSpawn();
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
				m_pEvents->OnPlayerDeath(playerid);
			}
			break;
			/*case ID_GAME_MESSAGE_PLAYER_SPAWN:
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
			break;*/
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
					m_pEvents->OnPlayerText(playerid, (char*)text, length);
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
				else if (driver == 0x00 && seat[0] == 0x00 && seat[1] == 0x00)
				{
					//this must be boat. 
					seatid = 0;
				}
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
					player->m_wVehicleId = vehicleid;
					player->m_byteSeatId = seatid;
				}
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_VEHICLE_PUT_ACCEPTED);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, false);
				CVehicle* vehicle= m_pVehiclePool->GetAt(vehicleid);
				if (vehicle)
				{
					if(seatid==0)
						vehicle->SetDriver(playerid);
				}
				if (iNPC && iNPC->Initialized() )
				{
					if (playerid == iNPC->GetID())
					{
						if (vehicle)
							npc->m_vecPos = vehicle->GetPosition(); 
						
						if (seatid > 0) //to check if passenger
						{
							iNPC->PSCounter = 0;
							iNPC->PSOnServerCycle = true;
							//Update NPC's position locally
							
						}
						else if (seatid == 0)
						{
							//npc entering vehicle as driver.
							//send one 0x95 packet.(updateDriver)
							if (vehicle)
							{
								npc->GetINCAR_SYNC_DATA()->vecMoveSpeed = vehicle->GetSpeed();
								npc->GetINCAR_SYNC_DATA()->dDamage = vehicle->GetDamage();
								npc->GetINCAR_SYNC_DATA()->fCarHealth = vehicle->GetHealth();
								npc->GetINCAR_SYNC_DATA()->quatRotation = vehicle->GetRotation();
								npc->GetINCAR_SYNC_DATA()->Turretx = vehicle->GetTurretx();
								npc->GetINCAR_SYNC_DATA()->Turrety = vehicle->GetTurrety();
							}
							SendNPCIcSyncDataLV();
						}
						m_pEvents->OnNPCEnterVehicle(vehicleid, seatid);
					}
				}
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_EXIT_EVENT:
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
					/*if (iNPC->GetVehicleDriver(player->m_wVehicleId) == bytePlayerId)
					{
						//player was driver of a vehicle
						//iNPC->RemoveVehicleDriver(player->m_wVehicleId);
					}*/
					player->m_wVehicleId = 0;
					player->m_byteSeatId = -1;
					CVehicle* vehicle = m_pVehiclePool->GetAt(player->m_wVehicleId);
					if (vehicle)
					{
						if (vehicle->GetDriver() == bytePlayerId)
							vehicle->RemoveDriver();
					}
				}
				
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_LOST_OWNERSHIP:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t vehicleid;
				bsIn.Read(vehicleid);
				if (vehicleid == npc->m_wVehicleId)
				{
					npc->m_wVehicleId = 0;
					CVehicle* vehicle = m_pVehiclePool->GetAt(vehicleid);
					if (vehicle)
					{
						if (vehicle->GetDriver() == iNPC->GetID())
							vehicle->RemoveDriver();
					}
					if (iNPC && iNPC->Initialized())
					{
						if(iNPC->PSOnServerCycle)
							iNPC->PSOnServerCycle = false;
					}
					m_pEvents->OnNPCExitVehicle();
				}
			}
			break;
			case ID_GAME_MESSAGE_PLAYER_STREAM_IN:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t bytePlayerId;
				bsIn.ReadAlignedBytes(&bytePlayerId, 1);
				uint8_t byteTeamId;
				bsIn.Read(byteTeamId);
				uint8_t byteSkinId;
				bsIn.Read(byteSkinId);
				bsIn.IgnoreBytes(1);
				float x, y, z;
				bsIn.Read(z); bsIn.Read(y); bsIn.Read(x);
				bsIn.IgnoreBytes(4);//4 for angle 
				uint16_t wVehicleID;
				bsIn.Read(wVehicleID);
				uint8_t byteSeatID = -1;
				if (wVehicleID)
				{
					bsIn.Read(byteSeatID);
				}
				CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
				if (player)
				{
					player->m_wVehicleId = wVehicleID;
					player->m_byteSeatId = byteSeatID;
					player->m_byteSkinId = byteSkinId;
					player->m_byteTeamId = byteTeamId;
					player->UpdatePosition(x, y, z);
					if (player->GetState() != PLAYER_STATE_WASTED)
					{
						if (wVehicleID)
						{
							if (byteSeatID == 0)
								player->SetState(PLAYER_STATE_DRIVER);
							else
								player->SetState(PLAYER_STATE_PASSENGER);
						}
						else player->SetState(PLAYER_STATE_ONFOOT);
					}
					player->SetStreamedIn(true);
				}
				if (wVehicleID && byteSeatID==0 )
				{
					CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleID);
					if (vehicle)
					{
						vehicle->SetDriver(bytePlayerId);
					}
				}
				m_pEvents->OnPlayerStreamIn(bytePlayerId);
				if (iNPC&&iNPC->IsSpawned()&&(configvalue&CONFIG_SYNC_ON_PLAYER_STREAMIN))
				{
					if (npc->m_wVehicleId == 0)
					{
						//on foot
						SendNPCOfSyncDataLV();
					}
				}
			}
			break;
			case ID_GAME_MESSAGE_PLAYER_STREAM_OUT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.IgnoreBytes(3);
				uint8_t playerid;
				bsIn.ReadAlignedBytes(&playerid, 1);
				CPlayer* player = m_pPlayerPool->GetAt(playerid);
				if (player)
					player->SetStreamedIn(false);
				m_pEvents->OnPlayerStreamOut(playerid);
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
				float x, y, z; int16_t model;
				bsIn.IgnoreBytes(2);//00 08? 00 01?
				bsIn.Read(model);//guessing model is 2 bytes.
				bsIn.Read(z); bsIn.Read(y); bsIn.Read(x);
				QUATERNION quatRotation;
				bsIn.Read(quatRotation.W);
				bsIn.Read(quatRotation.Z);
				bsIn.Read(quatRotation.Y);
				bsIn.Read(quatRotation.X);
				bsIn.IgnoreBytes(2);//01 ff
				float fCarHealth;
				uint32_t dwDamage;
				bsIn.Read(fCarHealth); bsIn.Read(dwDamage);
				if (!m_pVehiclePool->GetSlotState(vehicleid))
				{
					m_pVehiclePool->New(vehicleid, VECTOR(x, y, z), model, quatRotation, fCarHealth, dwDamage);
				}
				/*if (iNPC && iNPC->Initialized())
				{
					iNPC->AddStreamedVehicle(vehicleid, VECTOR(x, y, z), model, quatRotation, fCarHealth, dwDamage);
				}*/
				m_pEvents->OnVehicleStreamIn(vehicleid);
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
				if (m_pVehiclePool->GetSlotState(vehicleid))
				{
					m_pVehiclePool->Delete(vehicleid);
				}
				/*if (iNPC && iNPC->Initialized())
				{
					iNPC->DestreamVehicle(vehicleid);
				}*/
				m_pEvents->OnVehicleStreamOut(vehicleid);
			}
			break;
			case ID_GAME_MESSAGE_SET_TEAM:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint8_t bytePlayerId, byteTeamId;
				bsIn.Read(bytePlayerId);
				bsIn.Read(byteTeamId);
				CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
				if (player)
				{
					player->m_byteTeamId = byteTeamId;
				}
			}
			break;
			case ID_GAME_MESSAGE_SET_SKIN:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint8_t bytePlayerId, byteSkinId;
				bsIn.Read(bytePlayerId);
				bsIn.Read(byteSkinId);
				CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
				if (player)
				{
					player->m_byteSkinId = byteSkinId;
					if (iNPC && iNPC->Initialized() && iNPC->GetID() == bytePlayerId)
					{
						if (configvalue & CONFIG_RESTORE_WEAPON_ON_SKIN_CHANGE)
						{
							if (iNPC->IsSpawned() && !npc->m_wVehicleId)
								SendNPCOfSyncDataLV();
						}
						else
						{
							//Server sets the armed weapon of player to 0 (or 1). So for other players 
						//npc will have weapon 0. We also sets this internally.
							npc->SetCurrentWeapon(npc->GetSlotWeapon(0));//

						}
					}
				}
			}
			break;
			case ID_GAME_MESSAGE_SET_COLOUR:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint8_t bytePlayerId;
				uint8_t byteRed, byteGreen, byteBlue;
				bsIn.Read(bytePlayerId);
				bsIn.IgnoreBytes(1);//00?
				bsIn.Read(byteRed);
				bsIn.Read(byteGreen);
				bsIn.Read(byteBlue);
				CPlayer* player = m_pPlayerPool->GetAt(bytePlayerId);
				if (player)
				{
					player->SetColor(Color(byteRed, byteGreen, byteBlue));
				}
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
					m_pEvents->OnNPCDisconnect(reason);
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
				if (iNPC->IsSpawned()&&iNPC->AltSpawnStatus!=AltSpawn::OFF &&
					(iNPC->AltSpawnStatus & AltSpawn::SetPosFromServerRecvd)
					== 0)
				{
					//This is the command from server to set pos. avoid it.
					iNPC->AltSpawnStatus |= AltSpawn::SetPosFromServerRecvd;
					break;
				}
				if (!iNPC->IsSyncPaused())
				{
					bsIn.Read(npc->m_vecPos.Z);
					bsIn.Read(npc->m_vecPos.Y);
					bsIn.Read(npc->m_vecPos.X);
					if (iNPC->IsSpawned())
					{
						if (npc->m_wVehicleId)
						{
							//Server ejected npc from vehicle
							npc->m_wVehicleId = 0;
							npc->m_byteSeatId = -1;
						}
						SendNPCOfSyncDataLV();
					}
				}
			}
			break;
			case ID_GAME_MESSAGE_SET_ANGLE:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				//NPC has spawn position or spawn angle set. don't read this
				if (iNPC->IsSpawned()&&iNPC->AltSpawnStatus != AltSpawn::OFF && (iNPC->AltSpawnStatus & AltSpawn::SetAngleFromServerRecd)
					== 0)
				{
					//This is the command from server to set angle. avoid it.
					iNPC->AltSpawnStatus |= AltSpawn::SetAngleFromServerRecd;
				}else if (!iNPC->IsSyncPaused())
				{
					bsIn.Read(npc->m_fAngle);
				}
				/* The setting of angle by server when npc is
				spawning is used as a 'go' for sending
				spawn datas. Below it happens*/
				if (iNPC->IsSpawned() && !npc->m_wVehicleId)
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
				uint8_t oldhealth = npc->m_byteHealth;
				npc->m_byteHealth = newhealth;
				if (!iNPC->IsSyncPaused())
				{
					if (!npc->m_wVehicleId)
					{
						SendNPCOfSyncDataLV();
					}
					else if (npc->m_byteSeatId > 0)
						SendPassengerSyncData();
				}
				if (oldhealth > 0 && newhealth == 0)
				{
					//m_pFunctions->SendShotInfo(bodyPart::Body, 0xd);
					m_pFunctions->SendShotInfo(bodyPart::Body, 0xd,true,44,255);
					/*iNPC->TimeShotInfoSend = GetTickCount();
					iNPC->WaitingToRemoveBody = true;
					iNPC->ShotInfoWeapon = 44; //Fell to death
					iNPC->KillerId = 255;//Server killed and not any player
					iNPC->byteAutodeathBodyPart = static_cast<uint8_t>(bodyPart::Body);*/
				}
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
				{
					if (!npc->m_wVehicleId)
						SendNPCOfSyncDataLV();
					else if (npc->m_byteSeatId > 0)
						SendPassengerSyncData();
				}
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
				npc->RemoveAllWeapons();
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
				npc->RemoveWeapon(weapon);
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
				uint8_t byte;
				bsIn.Read(byte);
				npc->SetWeaponSlot(GetSlotIdFromWeaponId(weapon), weapon, ammo);
				if (iNPC->IsSpawned()&& byte==1) //when .SetWeapon is used byte=1. 
					//it is 0 when three weapons are given at spawn.
				{
					//Set current weapon
					//npc->SetCurrentWeaponAmmo( ammo );
					if (npc->GetCurrentWeapon() != weapon)
					{
						npc->SetCurrentWeapon(weapon);
						if (!npc->m_wVehicleId)
							SendNPCOfSyncDataLV();
					}
				}
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
					m_pEvents->OnSniperRifleFired(playerid, weapon, x, y, z, dx, dy, dz);
			}
			break;
			case ID_GAME_MESSAGE_SERVER_DATA_RCVD:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				RakNet::BitSize_t unreadbits = bsIn.GetNumberOfUnreadBits();
				size_t size = unreadbits / 8;
				uint32_t dwStreamLen;
				bsIn.Read(dwStreamLen);
				if (dwStreamLen + 4 > size)
					break;
				uint8_t* data = (uint8_t*)calloc(dwStreamLen, sizeof(unsigned char));
				if (data)
				{
					if (bsIn.Read((char*)data, dwStreamLen))
					{
						m_pEvents->OnServerData(data, dwStreamLen);
					}
					free(data);
				}
			}
			break;
			case ID_GAME_MESSAGE_EXPLOSION_CREATED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				if (bsIn.GetNumberOfUnreadBits() >= 18 * 8)
				{
					uint8_t byteType; float x, y, z;
					bsIn.Read(byteType);
					bsIn.Read(x); bsIn.Read(y); bsIn.Read(z);
					bsIn.IgnoreBytes(3);//00 00 01
					uint8_t playerCaused, isOnGround;
					bsIn.Read(playerCaused);
					bsIn.Read(isOnGround);
					m_pEvents->OnExplosion(byteType, VECTOR(x, y, z),
						playerCaused, (bool)isOnGround);
				}
			}
			break;
			case ID_GAME_MESSAGE_PROJECTILE_FIRED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				if (bsIn.GetNumberOfUnreadBits() >= 47 * 8)
				{
					uint8_t bytePlayerId; uint8_t byteWeapon;
					float x, y, z;
					// 00 00 01 02 1e 00 13 z y x , f1, f2,...,f7 (02=pid)
					// 00 13 is some kind of index. it increases
					//after each projectile launch.
					bsIn.IgnoreBytes(3);
					bsIn.Read(bytePlayerId);
					bsIn.Read(byteWeapon);
					bsIn.IgnoreBytes(2);//some kind of index, not needed
					bsIn.Read(z); bsIn.Read(y); bsIn.Read(x);
					float r1, r2, r3, r4, r5, r6, r7;
					bsIn.Read(r1);
					bsIn.Read(r2);
					bsIn.Read(r3);
					bsIn.Read(r4);
					bsIn.Read(r5);
					bsIn.Read(r6);
					bsIn.Read(r7);
					m_pEvents->OnProjectileFired(bytePlayerId, byteWeapon,
						VECTOR(x, y, z), r1, r2, r3, r4, r5, r6, r7);
				}
			}
			break;
			
			case ID_GAME_MESSAGE_PICKUP_CREATED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t dwSerialNo; uint16_t wPickupID;
				bool s = ReadPickup(&dwSerialNo, &wPickupID, &bsIn);
				if (!s)
				{
					printf("Error reading pickup data\n");
					break;
				}
				uint16_t wModel;
				bsIn.Read(wModel);
				uint32_t dwQuantity;
				bsIn.Read(dwQuantity);
				VECTOR vecPos;
				bsIn.Read(vecPos.Z);bsIn.Read(vecPos.Y);bsIn.Read(vecPos.X);
				uint8_t bAlpha;
				bsIn.Read(bAlpha);
				
				if (!m_pPickupPool->GetByID(wPickupID))
				{
					//Pickup with this id does not exists
					bool success=m_pPickupPool->New(wPickupID, dwSerialNo, wModel, dwQuantity, vecPos, bAlpha);
					m_pEvents->OnPickupStreamIn(wPickupID);
				}
				else
				{
					//Special case shown by vito
					//Pickup with ID already exists. What to do?
					//Overwrite
					PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
					pickup->dwSerialNo = dwSerialNo;
					pickup->bAlpha = bAlpha;
					pickup->dwQuantity = dwQuantity;
					pickup->vecPos = vecPos;
					pickup->wModel = wModel;
				}
			}
			break;

			case ID_GAME_MESSAGE_PICKUP_DESTROYED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t dwSerialNo; uint16_t wPickupID;
				bool s = ReadPickup(&dwSerialNo, &wPickupID, &bsIn);
				if (!s)
				{
					printf("Error reading pickup data\n");
					break;
				}
				//Check it on pool
				PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
				if (pickup)
				{
					//It exists, destroy it after calling event.
					m_pEvents->OnPickupDestroyed(wPickupID);
					m_pPickupPool->Destroy(wPickupID);
				}
			}break;
			case ID_GAME_MESSAGE_PICKUP_CHANGE_ALPHA:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t dwSerialNo; uint16_t wPickupID;
				bool s = ReadPickup(&dwSerialNo, &wPickupID, &bsIn);
				if (!s)
				{
					printf("Error reading pickup data\n");
					break;
				}
				uint8_t bAlpha;
				bsIn.Read(bAlpha);
				PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
				if(pickup)
				{
					m_pPickupPool->GetByID(wPickupID)->bAlpha = bAlpha;
					m_pEvents->OnPickupUpdate(wPickupID, pickupUpdate::AlphaUpdate);
				}
			}break;
			
			case ID_GAME_MESSAGE_PICKUP_CHANGE_POS:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t dwSerialNo; uint16_t wPickupID;
				bool s = ReadPickup(&dwSerialNo, &wPickupID, &bsIn);
				if (!s)
				{
					printf("Error reading pickup data\n");
					break;
				}
				VECTOR vecPos;
				bsIn.Read(vecPos.X); bsIn.Read(vecPos.Y);
				bsIn.Read(vecPos.Z);
				PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
				if (pickup)
				{
					m_pPickupPool->GetByID(wPickupID)->vecPos = vecPos;
					m_pEvents->OnPickupUpdate(wPickupID, pickupUpdate::PositionUpdate);
				}
			}break;
			case ID_GAME_MESSAGE_REFRESH_PICKUP:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint32_t dwSerialNo; uint16_t wPickupID;
				bool s = ReadPickup(&dwSerialNo, &wPickupID, &bsIn);
				if (!s)
				{
					printf("Error reading pickup data\n");
					break;
				}
				PICKUP* pickup = m_pPickupPool->GetByID(wPickupID);
				if (pickup)
				{
					//update to new serial number
					pickup->dwSerialNo = dwSerialNo;
					m_pEvents->OnPickupUpdate(wPickupID, pickupUpdate::pickupRefreshed);
				}
			}
			break;
			//Every 5 seconds, server sends below packet
			case ID_GAME_MESSAGE_TICK:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned int tickcount; 
				bsIn.Read(tickcount); 
				m_pEvents->OnServerShareTick(tickcount);
			}
			break;

			//Every 10 seconds, server sends below packet 
			case ID_GAME_MESSAGE_TIMEWEATHER_SYNC:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned short timerate=0;
				bsIn.Read(timerate);
				uint8_t minute=0, hour=0, weather=0;
				bsIn.Read(minute);
				bsIn.Read(hour);
				bsIn.Read(weather);
				m_pEvents->OnTimeWeatherSync(timerate,minute,hour,weather);
			}
			break;

			case ID_GAME_MESSAGE_CREATE_CHECKPOINT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				CHECKPOINT cp;
				bsIn.Read(cp.wID);
				bsIn.Read(cp.byteIsSphere);
				bsIn.Read(cp.vecPos.Z);
				bsIn.Read(cp.vecPos.Y);
				bsIn.Read(cp.vecPos.X);
				bsIn.Read(cp.byteRed);
				bsIn.Read(cp.byteGreen);
				bsIn.Read(cp.byteBlue);
				bsIn.Read(cp.byteAlpha);
				bsIn.Read(cp.fRadius);
				if (!m_pCheckpointPool->GetByID(cp.wID))
				{
					bool success = m_pCheckpointPool->New(cp);
					if(success)
						m_pEvents->OnPickupStreamIn(cp.wID);
				}
			}
			break;
			case ID_GAME_MESSAGE_DESTROY_CHECKPOINT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wCheckpointID;
				bsIn.Read(wCheckpointID);
				CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
				if (cp)
				{
					m_pEvents->OnCheckpointDestroyed(wCheckpointID);
					m_pCheckpointPool->Destroy(wCheckpointID);
				}
			}break;
			case ID_GAME_MESSAGE_CHECKPOINT_CHANGE_RADIUS:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wCheckpointID; float fRadius;
				bsIn.Read(wCheckpointID);
				bsIn.Read(fRadius);
				CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
				if (cp)
				{
					cp->fRadius = fRadius;
					m_pEvents->OnCheckpointUpdate(wCheckpointID, checkpointUpdate::RadiusUpdate);
				}
			}break;
			case ID_GAME_MESSAGE_CHECKPOINT_CHANGE_POS:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wCheckpointID; VECTOR vecPos;
				bsIn.Read(wCheckpointID);
				bsIn.Read(vecPos.X);
				bsIn.Read(vecPos.Y);
				bsIn.Read(vecPos.Z);
				CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
				if (cp)
				{
					cp->vecPos = vecPos;
					m_pEvents->OnCheckpointUpdate(wCheckpointID, checkpointUpdate::PositionUpdate);
				}
			}break;
			case ID_GAME_MESSAGE_CHECKPOINT_CHANGE_COLOUR:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wCheckpointID; 
				bsIn.Read(wCheckpointID);
				
				CHECKPOINT* cp = m_pCheckpointPool->GetByID(wCheckpointID);
				if (cp)
				{
					bsIn.Read(cp->byteRed);
					bsIn.Read(cp->byteGreen);
					bsIn.Read(cp->byteBlue);
					bsIn.Read(cp->byteAlpha);
					m_pEvents->OnCheckpointUpdate(wCheckpointID, checkpointUpdate::ColourUpdate);
				}
			}break;

			case ID_GAME_MESSAGE_OBJECT_CREATED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				OBJECT obj;
				bsIn.Read(obj.wID);
				bsIn.Read(obj.wModel);
				bsIn.Read(obj.byteAlpha);
				bsIn.Read(obj.vecPos.Z);
				bsIn.Read(obj.vecPos.Y);
				bsIn.Read(obj.vecPos.X);
				bsIn.Read(obj.quatRot.W);
				bsIn.Read(obj.quatRot.Z);
				bsIn.Read(obj.quatRot.Y);
				bsIn.Read(obj.quatRot.X);
				bsIn.Read(obj.byteMaskTrackingShotsBumps);
				//one more byte=00 is there. don't know what it is
				if (!m_pObjectPool->GetByID(obj.wID))
				{
					bool success = m_pObjectPool->New(obj);
					if (success)
						m_pEvents->OnObjectStreamIn(obj.wID);
				}
			}
			break;
			case ID_GAME_MESSAGE_OBJECT_DESTROYED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wObjectId;
				bsIn.Read(wObjectId);
				OBJECT* obj = m_pObjectPool->GetByID(wObjectId);
				if (obj)
				{
					m_pEvents->OnObjectDestroyed(wObjectId);
					m_pObjectPool->Destroy(wObjectId);
				}
			}
			break;
			case ID_GAME_MESSAGE_OBJECT_CHANGE_ALPHA:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wObjectId, wFadeInTime; uint8_t byteAlpha;
				bsIn.Read(wObjectId);
				bsIn.Read(byteAlpha);
				bsIn.Read(wFadeInTime);
				OBJECT* obj = m_pObjectPool->GetByID(wObjectId);
				if (obj)
				{
					obj->byteAlpha = byteAlpha;
					m_pEvents->OnObjectUpdate(wObjectId, objectUpdate::AlphaUpdate);
				}
			}
			break;
			case ID_GAME_MESSAGE_OBJECT_CHANGE_SHOTS_BUMPS:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wObjectId;
				bsIn.Read(wObjectId);
				OBJECT* obj = m_pObjectPool->GetByID(wObjectId);
				if (obj)
				{
					bsIn.Read(obj->byteMaskTrackingShotsBumps);
					m_pEvents->OnObjectUpdate(wObjectId, objectUpdate::TrackingBumpsShots);
				}
			}
			break;
			case ID_GAME_MESSAGE_OBJECT_SETPOSITION:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wObjectId;
				bsIn.Read(wObjectId);
				OBJECT* obj = m_pObjectPool->GetByID(wObjectId);
				if (obj)
				{
					bsIn.Read(obj->vecPos.Z);
					bsIn.Read(obj->vecPos.Y);
					bsIn.Read(obj->vecPos.X);
					m_pEvents->OnObjectUpdate(wObjectId, objectUpdate::PositionUpdate);
				}
			}
			break;
			case ID_GAME_MESSAGE_OBJECT_MOVEDTO:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wObjectId; uint16_t time;
				bsIn.Read(wObjectId); bsIn.Read(time);
				OBJECT* obj = m_pObjectPool->GetByID(wObjectId);
				if (obj)
				{
					VECTOR vecCurrentPos, newPos;
					bsIn.Read(vecCurrentPos.Z);
					bsIn.Read(vecCurrentPos.Y);
					bsIn.Read(vecCurrentPos.X);
					bsIn.Read(newPos.Z);
					bsIn.Read(newPos.Y);
					bsIn.Read(newPos.X);
					obj->vecPos = newPos;//set obj's position to new one!
					m_pEvents->OnObjectUpdate(wObjectId, objectUpdate::MoveToUpdate);
				}
			}
			break;
			case ID_GAME_MESSAGE_OBJECT_ROTATE_TO:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wObjectId; uint16_t time;
				bsIn.Read(wObjectId); bsIn.Read(time);
				OBJECT* obj = m_pObjectPool->GetByID(wObjectId);
				if (obj)
				{
					QUATERNION quatCurRot, quatNewRot;
					bsIn.Read(quatCurRot.W);
					bsIn.Read(quatCurRot.Z);
					bsIn.Read(quatCurRot.Y);
					bsIn.Read(quatCurRot.X);
					bsIn.Read(quatNewRot.W);
					bsIn.Read(quatNewRot.Z);
					bsIn.Read(quatNewRot.Y);
					bsIn.Read(quatNewRot.X);
					obj->quatRot = quatNewRot;//set obj's rotation to new one!
					m_pEvents->OnObjectUpdate(wObjectId, objectUpdate::RotationToUpdate);
				}
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_ADDSPEED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wVehicleId; uint16_t time;
				//f0 00 89 (id =137) 40 40 00 00 (z) 40 00 00 00 (y) 3f 80 00 00 (x)
				bsIn.Read(wVehicleId);
				VECTOR vecSpeed;
				bsIn.Read(vecSpeed.Z);
				bsIn.Read(vecSpeed.Y);
				bsIn.Read(vecSpeed.X);
				m_pEvents->OnVehicleSetSpeedRequest(wVehicleId, vecSpeed, true, false);
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_ADDRELSPEED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wVehicleId; uint16_t time;
				//f0 00 89 (id =137) 40 40 00 00 (z) 40 00 00 00 (y) 3f 80 00 00 (x)
				bsIn.Read(wVehicleId);
				VECTOR vecSpeed;
				bsIn.Read(vecSpeed.Z);
				bsIn.Read(vecSpeed.Y);
				bsIn.Read(vecSpeed.X);
				m_pEvents->OnVehicleSetSpeedRequest(wVehicleId, vecSpeed, true, true);
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_SETSPEED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wVehicleId; uint16_t time;
				//f0 00 89 (id =137) 40 40 00 00 (z) 40 00 00 00 (y) 3f 80 00 00 (x)
				bsIn.Read(wVehicleId);
				VECTOR vecSpeed;
				bsIn.Read(vecSpeed.Z);
				bsIn.Read(vecSpeed.Y);
				bsIn.Read(vecSpeed.X);
				m_pEvents->OnVehicleSetSpeedRequest(wVehicleId, vecSpeed, false, false);
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_ADDTURNSPEED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wVehicleId; uint16_t time;
				//f0 00 89 (id =137) 40 40 00 00 (z) 40 00 00 00 (y) 3f 80 00 00 (x)
				bsIn.Read(wVehicleId);
				VECTOR vecSpeed;
				bsIn.Read(vecSpeed.Z);
				bsIn.Read(vecSpeed.Y);
				bsIn.Read(vecSpeed.X);
				m_pEvents->OnVehicleSetTurnSpeedRequest(wVehicleId, vecSpeed, true, false);
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_ADDRELTURNSPEED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wVehicleId; uint16_t time;
				//f0 00 89 (id =137) 40 40 00 00 (z) 40 00 00 00 (y) 3f 80 00 00 (x)
				bsIn.Read(wVehicleId);
				VECTOR vecSpeed;
				bsIn.Read(vecSpeed.Z);
				bsIn.Read(vecSpeed.Y);
				bsIn.Read(vecSpeed.X);
				m_pEvents->OnVehicleSetTurnSpeedRequest(wVehicleId, vecSpeed, true, true);
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_SETTURNSPEED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wVehicleId; uint16_t time;
				//f0 00 89 (id =137) 40 40 00 00 (z) 40 00 00 00 (y) 3f 80 00 00 (x)
				bsIn.Read(wVehicleId);
				VECTOR vecSpeed;
				bsIn.Read(vecSpeed.Z);
				bsIn.Read(vecSpeed.Y);
				bsIn.Read(vecSpeed.X);
				m_pEvents->OnVehicleSetTurnSpeedRequest(wVehicleId, vecSpeed, false, false);
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_SETPOSITION:
			{
				//Send to all players to whom the vehicle is streamed in
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wVehicleId;
				bsIn.Read(wVehicleId);
				VECTOR vecPos;
				bsIn.Read(vecPos.Z);
				bsIn.Read(vecPos.Y);
				bsIn.Read(vecPos.X);
				if (m_pVehiclePool->GetSlotState(wVehicleId))
				{
					CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
					if (vehicle)
					{
						vehicle->UpdatePosition(vecPos);
						if (npc && npc->m_wVehicleId==wVehicleId)
						{
							//npc is sitting on the vehicle as driver or passenger. update npc's position vector also
							npc->UpdatePosition(vecPos);
						}
					}
				}
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_SETROTATION:
			{
				//Send to all players to whom the vehicle is streamed in
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wVehicleId;
				bsIn.Read(wVehicleId);
				QUATERNION quatRotation;
				bsIn.Read(quatRotation.W);
				bsIn.Read(quatRotation.Z);
				bsIn.Read(quatRotation.Y);
				bsIn.Read(quatRotation.X);
				//here the rotation given must be accurate. otherwise the vehicle rotation actual will be different.
				//printf("(x,y,z,w)=(%f,%f,%f,%f)\n", quatRotation.X, quatRotation.Y, quatRotation.Z, quatRotation.W);
				if (m_pVehiclePool->GetSlotState(wVehicleId))
				{
					CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
					if (vehicle)
					{
						vehicle->UpdateRotation(quatRotation);
						if (npc && npc->m_wVehicleId == wVehicleId)
						{
							//npc can be driver or passenger
							if (vehicle->GetDriver() == iNPC->GetID())
							{
								//npc is driver
								npc->GetINCAR_SYNC_DATA()->quatRotation = quatRotation;
								SendNPCIcSyncDataLV();

							}
						}
					}
				}
			}
			break;
			case ID_GAME_MESSAGE_VEHICLE_SETHEALTH:
			{
				//Send to all players to whom the vehicle is streamed in
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				uint16_t wVehicleId;
				bsIn.Read(wVehicleId);
				float fHealth;
				bsIn.Read(fHealth);
				if (m_pVehiclePool->GetSlotState(wVehicleId))
				{
					CVehicle* vehicle = m_pVehiclePool->GetAt(wVehicleId);
					if (vehicle)
					{
						vehicle->SetHealth(fHealth);
					}
				}
			}
			break;


			}
		}
		OnCycle();
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
#ifdef _REL004
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, INCAR_SYNC_DATA* m_pIcSyncDataOut, uint8_t* bytePlayerIdOut)
{
	bsIn->IgnoreBytes(1);
	bsIn->ReadAlignedBytes(bytePlayerIdOut, 1);
	bsIn->Read(m_pIcSyncDataOut->byteCurrentWeapon);
	bsIn->Read(m_pIcSyncDataOut->bytePlayerArmour);
	bsIn->Read(m_pIcSyncDataOut->bytePlayerHealth);
	bsIn->Read(m_pIcSyncDataOut->wAmmo);
	bsIn->Read(m_pIcSyncDataOut->VehicleID);
	uint16_t wKeys;
	bsIn->Read(wKeys);
	m_pIcSyncDataOut->dwKeys = wKeys;
	bsIn->IgnoreBytes(2);//00 c8
	bsIn->Read(m_pIcSyncDataOut->Turrety);
	bsIn->Read(m_pIcSyncDataOut->Turretx);
	bsIn->IgnoreBytes(1);//0 normally, but 1 on water
	bsIn->Read(m_pIcSyncDataOut->fCarHealth);
	bsIn->Read(m_pIcSyncDataOut->dDamage);
	bsIn->Read(m_pIcSyncDataOut->quatRotation.W);
	bsIn->Read(m_pIcSyncDataOut->quatRotation.Z);
	bsIn->Read(m_pIcSyncDataOut->quatRotation.Y);
	bsIn->Read(m_pIcSyncDataOut->quatRotation.X);
	bsIn->Read(m_pIcSyncDataOut->vecMoveSpeed.Z);
	bsIn->Read(m_pIcSyncDataOut->vecMoveSpeed.Y);
	bsIn->Read(m_pIcSyncDataOut->vecMoveSpeed.X);
}
#elif defined(_REL0471)
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, INCAR_SYNC_DATA* m_pIcSyncDataOut, uint8_t* bytePlayerIdOut)
{
	bsIn->IgnoreBytes(1);
	bsIn->ReadBit();//read one bit which is zero.
	uint32_t sourcelen;//The original no:of bytes after messageid=0x30.
	bsIn->Read(sourcelen);
	bsIn->ReadBit();//read one bit which is zero
	uint32_t lenCompressedData;//The length of the compressed data
	bsIn->Read(lenCompressedData);
	unsigned char* compressedData = (unsigned char*)malloc(lenCompressedData);
	if (!compressedData)
	{
		printf("Could not allocate memory\n"); exit(0);
	}
	for (int i = 0; i < lenCompressedData; i++)
	{
		if (!bsIn->Read(compressedData[i]))
		{
			printf("Could not read bitstream\n"); exit(0);
		}
	}
	unsigned char* data = (unsigned char*)malloc(sourcelen);//here sourcelen,but precaution.
	if (!data) {
		printf("Error allocating memory. Exiting\n"); exit(0);
	}
	uLongf datalen = (uLongf)sourcelen;
	int ret = uncompress(data, &datalen, compressedData, lenCompressedData);
	if (ret == Z_OK)
	{
		if (datalen == sourcelen)
		{
			//Decompression was 100% successful.
			// 
			//Create a bitstream so that rest of code works.
			RakNet::BitStream bsIn2(sourcelen);
			for (int i = 0; i < sourcelen; i++)
				bsIn2.Write(data[i]);
			//Now free previous allocated memories
			free(data); free(compressedData);
			bsIn2.ResetReadPointer();
			bsIn2.ReadAlignedBytes(bytePlayerIdOut, 1);
			uint8_t action;
			bsIn2.ReadAlignedBytes(&action, 1);
			uint16_t wKeys;
			bsIn2.Read(wKeys);
			wKeys = _byteswap_ushort(wKeys);
			uint8_t byteVehId;
			bsIn2.Read(byteVehId);
			uint8_t byteNibbleVehId;
			ReadNibble(&byteNibbleVehId, &bsIn2);
			DecodeIDandKeys(byteNibbleVehId, byteVehId, wKeys, m_pIcSyncDataOut);
			bsIn2.Read(m_pIcSyncDataOut->bytePlayerHealth);
			if (action & 0x80)
			{
				bsIn2.Read(m_pIcSyncDataOut->byteCurrentWeapon);
				if (m_pIcSyncDataOut->byteCurrentWeapon > 11)
					bsIn2.IgnoreBytes(2);//ammo;
			}
			else
				m_pIcSyncDataOut->byteCurrentWeapon = 0;
			if (action & 0x40)
			{
				bsIn2.Read(m_pIcSyncDataOut->bytePlayerArmour);
			}
			else
				m_pIcSyncDataOut->bytePlayerArmour = 0;
			uint8_t byteTurret;
			ReadNibble(&byteTurret, &bsIn2);
			uint8_t byteType;
			ReadNibble(&byteType, &bsIn2);
			bsIn2.Read(m_pIcSyncDataOut->vecPos.X);
			bsIn2.Read(m_pIcSyncDataOut->vecPos.Y);
			bsIn2.Read(m_pIcSyncDataOut->vecPos.Z);
			if (byteType & 0x1)
			{
				uint16_t speedx, speedy, speedz;
				bsIn2.Read(speedx); bsIn2.Read(speedy); bsIn2.Read(speedz);
				m_pIcSyncDataOut->vecMoveSpeed = ConvertFromUINT16_T(speedx, speedy, speedz, 20.0);
			}
			else
				ZeroVEC(m_pIcSyncDataOut->vecMoveSpeed);
			uint8_t byteRotFlag = 0;
			ReadNibble(&byteRotFlag,&bsIn2);
			uint16_t w_RotX, w_RotY, w_RotZ;
			bsIn2.Read(w_RotX); bsIn2.Read(w_RotY); bsIn2.Read(w_RotZ);
			m_pIcSyncDataOut->quatRotation.X = ConvertFromUINT16_T(w_RotX, -1);
			m_pIcSyncDataOut->quatRotation.Y = ConvertFromUINT16_T(w_RotY, -1);
			m_pIcSyncDataOut->quatRotation.Z = ConvertFromUINT16_T(w_RotZ, -1);
			m_pIcSyncDataOut->quatRotation.W = \
				(float)sqrt(1 - (pow(m_pIcSyncDataOut->quatRotation.X, 2) + \
					pow(m_pIcSyncDataOut->quatRotation.Y, 2) + \
					pow(m_pIcSyncDataOut->quatRotation.Z, 2)));
			ApplyRotationFlags(&m_pIcSyncDataOut->quatRotation, byteRotFlag);
			if (byteType & 0x2)
				bsIn2.Read(m_pIcSyncDataOut->dDamage);
			else
				m_pIcSyncDataOut->dDamage = 0;
			if (byteType & 0x4)
				bsIn2.Read(m_pIcSyncDataOut->fCarHealth);
			else
				m_pIcSyncDataOut->fCarHealth = CAR_HEALTH_MAX;
			if (byteTurret)
			{
				uint16_t wHorizontal; uint16_t wVertical;
				bsIn2.Read(wHorizontal); bsIn2.Read(wVertical);
				m_pIcSyncDataOut->Turretx = ConvertFromUINT16_T(wHorizontal, 2 * (float)PI);
				m_pIcSyncDataOut->Turrety = ConvertFromUINT16_T(wVertical, 2 * (float)PI);
			}
			else
			{
				m_pIcSyncDataOut->Turretx = 0;
				m_pIcSyncDataOut->Turrety = 0;
			}
		}
		else {
			printf("Error. Decompress was not completely successful.\n");
			exit(0);
		}
	}
	else {
		printf("An error occured during decompressing data.\n");
		exit(0);
	}
	
}

#else
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
#endif

#ifdef _REL004
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, ONFOOT_SYNC_DATA* m_pOfSyncDataOut, uint8_t* bytePlayerIdOut)
{

	uint8_t byteMessageID; bool bIsAiming;
	bsIn->Read(byteMessageID);
	if (byteMessageID == ID_GAME_MESSAGE_ONFOOT_UPDATE)
		bIsAiming = false;
	else if (byteMessageID == ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM)
		bIsAiming = true;
	else exit(1);
	m_pOfSyncDataOut->IsPlayerUpdateAiming = bIsAiming;
	bsIn->ReadAlignedBytes(bytePlayerIdOut, 1);
	uint8_t byteCrouching;
	bsIn->Read(byteCrouching);
	if (byteCrouching == 2)
		m_pOfSyncDataOut->IsCrouching = true;
	else
		m_pOfSyncDataOut->IsCrouching = false;
	uint8_t byteWeaponId;
	bsIn->Read(byteWeaponId);
	m_pOfSyncDataOut->byteCurrentWeapon = byteWeaponId;
	uint8_t byte1;
	bsIn->Read(byte1); bsIn->Read(m_pOfSyncDataOut->byteAction);
	if (bIsAiming == false)
	{
		bool bLookBehind;
		if ((byte1 >> 4) == 3 || (byte1 >> 4) == 6)
			bLookBehind = true;
		else bLookBehind = false;
	}
	else
	{
		bool bIsReloading;
		if ((byte1 >> 4) == 2)
			bIsReloading = true;
		else if ((byte1 >> 4) == 0xa)
			bIsReloading = false;
		else bIsReloading = false;//not necessary
		m_pOfSyncDataOut->bIsReloading = bIsReloading;
		//0x27 01 reloading
		//0xa7 10 shooting
	}
	uint8_t byteArmour, byteHealth; uint16_t wAmmo;
	bsIn->Read(byteArmour); bsIn->Read(byteHealth);
	bsIn->Read(wAmmo);
	m_pOfSyncDataOut->byteArmour = byteArmour;
	m_pOfSyncDataOut->byteHealth = byteHealth;
	m_pOfSyncDataOut->wAmmo = wAmmo;
	bsIn->IgnoreBytes(2);//zero
	uint16_t wKeys;
	bsIn->Read(wKeys);
	m_pOfSyncDataOut->dwKeys = wKeys;
	float speedx, speedy, speedz;
	bsIn->Read(speedz); bsIn->Read(speedy); bsIn->Read(speedx);
	m_pOfSyncDataOut->vecSpeed = VECTOR(speedx, speedy, speedz);
	float fAngle, x, y, z;
	bsIn->Read(fAngle);
	bsIn->Read(z); bsIn->Read(y); bsIn->Read(x);
	m_pOfSyncDataOut->fAngle = fAngle;
	m_pOfSyncDataOut->vecPos = VECTOR(x, y, z);
	if (bIsAiming)
	{
		float aimposx, aimposy, aimposz;
		bsIn->Read(aimposz);
		bsIn->Read(aimposy);
		bsIn->Read(aimposx);
		m_pOfSyncDataOut->vecAimPos = VECTOR(aimposx, aimposy, aimposz);

		float aimdirx, aimdiry, aimdirz;
		bsIn->Read(aimdirz);
		bsIn->Read(aimdiry);
		bsIn->Read(aimdirx);
		m_pOfSyncDataOut->vecAimDir = VECTOR(aimdirx, aimdiry, aimdirz);
	}
}
#elif defined(_REL0470)
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
	if (action & OF_FLAG_EXTRA_NIBBLE)
	{
		//This means a nibble is there to read
		ReadNibble(&nibble, bsIn);//This is used to find player is crouching and/or reloading 
	}
	//bsIn->IgnoreBytes(36);//don't know what is it
	float matrix[9];
	for (uint8_t i = 0; i < 9; i++)
	{
		bsIn->Read(matrix[i]);
	}
	VECTOR vecMatrixRight;
	VECTOR vecMatrixUp;
	VECTOR vecMatrixPosition;
	vecMatrixRight.Z = matrix[0];
	vecMatrixRight.Y = matrix[1];
	vecMatrixRight.X = matrix[2];
	vecMatrixUp.X = matrix[3];
	vecMatrixUp.Y = matrix[5];
	vecMatrixUp.Z = matrix[4];
	vecMatrixPosition.Y = matrix[6];//this is 1 always
	vecMatrixPosition.X = matrix[7];//does not matter
	vecMatrixPosition.Z = matrix[8];//does not matter
	float x= 0.0, y= 0.0, z= 0.0, fAngle=0.0;
	bsIn->Read(z); bsIn->Read(y); bsIn->Read(x); bsIn->Read(fAngle);
	bsIn->IgnoreBytes(4);
	float speedx= 0.0, speedy= 0.0, speedz= 0.0;
	bsIn->Read(speedz); bsIn->Read(speedy); bsIn->Read(speedx);
	bsIn->IgnoreBytes(12);//??
	uint8_t byteWeapon = 0; uint16_t ammo = 0;
	if (action & OF_FLAG_WEAPON)
	{
		bsIn->Read(byteWeapon);
		if (byteWeapon > 11)
			bsIn->Read(ammo);//ammo
	}
	uint32_t dw_Keys = 0; uint8_t byteAction = 1;
	if (action & OF_FLAG_KEYS_OR_ACTION)
	{
		uint8_t keybyte1, keybyte2;
		bsIn->Read(keybyte1); bsIn->Read(keybyte2);
		uint8_t byte, nib; uint16_t value;
		bsIn->Read(byte);
		ReadNibble(&nib, bsIn);
		value = (byte << 4) | nib;
		byteAction = (value & ~0x80);
		dw_Keys |= keybyte1;
		dw_Keys |= (keybyte2 << 8);
		if (value & 0x80)
			dw_Keys |= 65536;
	}
	uint8_t byteHealth;
	bsIn->Read(byteHealth);
	uint8_t byteArmour = 0;
	if (action & OF_FLAG_ARMOUR)
		bsIn->Read(byteArmour);
	bsIn->IgnoreBytes(1);
	if (bIsAiming)
	{
		//bsIn->IgnoreBytes(12);//???
		VECTOR vecUnknown;
		bsIn->Read(vecUnknown.Z);
		bsIn->Read(vecUnknown.Y);
		bsIn->Read(vecUnknown.X);

		VECTOR aimDir, aimPos;
		bsIn->Read(aimDir.Z);
		bsIn->Read(aimDir.Y);
		bsIn->Read(aimDir.X);
		bsIn->Read(aimPos.Z);
		bsIn->Read(aimPos.Y);
		bsIn->Read(aimPos.X);
		m_pOfSyncDataOut->vecAimPos = aimPos;
		m_pOfSyncDataOut->vecAimDir = aimDir;
	}
	m_pOfSyncDataOut->byteArmour = byteArmour;
	m_pOfSyncDataOut->byteCurrentWeapon = byteWeapon;
	m_pOfSyncDataOut->wAmmo = ammo;
	m_pOfSyncDataOut->byteHealth = byteHealth;
	m_pOfSyncDataOut->dwKeys = dw_Keys;
	m_pOfSyncDataOut->byteAction = byteAction;
	m_pOfSyncDataOut->fAngle = fAngle;
	m_pOfSyncDataOut->IsPlayerUpdateAiming = bIsAiming;
	m_pOfSyncDataOut->IsCrouching = (nibble & OF_FLAG_NIBBLE_CROUCHING) == OF_FLAG_NIBBLE_CROUCHING;
	m_pOfSyncDataOut->vecPos.X = x;
	m_pOfSyncDataOut->vecPos.Y = y;
	m_pOfSyncDataOut->vecPos.Z = z;
	m_pOfSyncDataOut->vecSpeed = VECTOR(speedx,speedy,speedz);
	m_pOfSyncDataOut->bIsReloading = (nibble & OF_FLAG_NIBBLE_RELOADING) == OF_FLAG_NIBBLE_RELOADING;
	m_pOfSyncDataOut->vecMatrixRight = vecMatrixRight;
	m_pOfSyncDataOut->vecMatrixUp = vecMatrixUp;
	m_pOfSyncDataOut->vecMatrixPosition = vecMatrixPosition;
}
#elif defined(_REL0471)
void ReceivePlayerSyncData(RakNet::BitStream* bsIn, ONFOOT_SYNC_DATA* m_pOfSyncDataOut, uint8_t* bytePlayerIdOut)
{
	uint8_t byteMessageID; bool bIsAiming;
	bsIn->Read(byteMessageID);
	if (byteMessageID == ID_GAME_MESSAGE_ONFOOT_UPDATE)
		bIsAiming = false;
	else if (byteMessageID == ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM)
		bIsAiming = true;
	else exit(1);
	bsIn->ReadBit();//read one bit which is zero.
	uint32_t sourcelen;//The original no:of bytes after messageid=0x30.
	bsIn->Read(sourcelen);
	bsIn->ReadBit();//read one bit which is zero
	uint32_t lenCompressedData;//The length of the compressed data
	bsIn->Read(lenCompressedData);
	unsigned char* compressedData=(unsigned char*)malloc(lenCompressedData);
	if (!compressedData)
	{
		printf("Could not allocate memory\n"); exit(0);
	}
	for (int i = 0; i < lenCompressedData; i++)
	{
		if (!bsIn->Read(compressedData[i]))
		{
			printf("Could not read bitstream\n"); exit(0);
		}
	}
	unsigned char* data = (unsigned char*)malloc(sourcelen);//here sourcelen,but precaution.
	if (!data) {
		printf("Error allocating memory. Exiting\n"); exit(0);
	}
	uLongf datalen = (uLongf)sourcelen;
	int ret=uncompress(data, &datalen, compressedData, lenCompressedData);
	if (ret == Z_OK)
	{
		if (datalen == sourcelen)
		{
			//Decompression was 100% successful.
			// 
			//Create a bitstream so that rest of code works.
			RakNet::BitStream bsIn2(sourcelen);
			for (int i = 0; i < sourcelen; i++)
				bsIn2.Write(data[i]);
			//Now free previous allocated memories
			free(data); free(compressedData);
			bsIn2.ResetReadPointer();
			bsIn2.ReadAlignedBytes(bytePlayerIdOut, 1);
			uint8_t action;
			bsIn2.ReadAlignedBytes(&action, 1);
			uint8_t nibble = 0;
			if (action & OF_FLAG_EXTRA_NIBBLE)
			{
				//This means a nibble is there to read
				ReadNibble(&nibble, &bsIn2);//This is used to find player is crouching and/or reloading 
			}
			float x, y, z;
			bsIn2.Read(x); bsIn2.Read(y); bsIn2.Read(z);
			uint16_t w_encodedAngle;
			bsIn2.Read(w_encodedAngle);
			float fAngle = ConvertFromUINT16_T(w_encodedAngle, 2 * (float)PI);
			//Read 3f aa aa ab = 1.333333
			float fUnknown;
			bsIn2.Read(fUnknown);
			//printf("pid %d, %f\n", *bytePlayerIdOut, fUnknown);
			uint16_t speedx = 0, speedy = 0, speedz = 0;
			VECTOR vecSpeed; vecSpeed.X = 0; vecSpeed.Y = 0; vecSpeed.Z = 0;
			if (action & OF_FLAG_SPEED)
			{
				bsIn2.Read(speedx); bsIn2.Read(speedy);
				bsIn2.Read(speedz);
				vecSpeed.X = ConvertFromUINT16_T(speedx, 20.0);
				vecSpeed.Y = ConvertFromUINT16_T(speedy, 20.0);
				vecSpeed.Z = ConvertFromUINT16_T(speedz, 20.0);
			}
			uint8_t byteWeapon = 0; uint16_t ammo = 0;
			if (action & OF_FLAG_WEAPON)
			{
				bsIn2.Read(byteWeapon);
				if (byteWeapon > 11)
					bsIn2.Read(ammo);//ammo
			}
			uint32_t dw_Keys = 0; uint8_t byteAction = 1;
			if (action & OF_FLAG_KEYS_OR_ACTION)
			{
				uint8_t keybyte1, keybyte2;
				bsIn2.Read(keybyte1); bsIn2.Read(keybyte2);
				uint8_t byte, nib; uint16_t value;
				bsIn2.Read(byte);
				ReadNibble(&nib, &bsIn2);
				value = (byte << 4) | nib;
				byteAction = (value & ~0x80);
				dw_Keys |= keybyte1;
				dw_Keys |= (keybyte2 << 8);
				if (value & 0x80)
					dw_Keys |= 65536;
			}
			uint8_t byteHealth;
			bsIn2.Read(byteHealth);
			uint8_t byteArmour = 0;
			if (action & OF_FLAG_ARMOUR)
				bsIn2.Read(byteArmour);
			bsIn2.IgnoreBytes(1);
			if (bIsAiming)
			{
				uint16_t wAimDirX, wAimDirY, wAimDirZ;
				bsIn2.Read(wAimDirX);	bsIn2.Read(wAimDirY);
				bsIn2.Read(wAimDirZ);
				float fAimDirX, fAimDirY, fAimDirZ;
				fAimDirX = ConvertFromUINT16_T(wAimDirX, 2 * (float)PI);
				fAimDirY = ConvertFromUINT16_T(wAimDirY, 2 * (float)PI);
				fAimDirZ = ConvertFromUINT16_T(wAimDirZ, 2 * (float)PI);
				m_pOfSyncDataOut->vecAimDir.X = fAimDirX;
				m_pOfSyncDataOut->vecAimDir.Y = fAimDirY;
				m_pOfSyncDataOut->vecAimDir.Z = fAimDirZ;
				float fAimPosX, fAimPosY, fAimPosZ;
				bsIn2.Read(fAimPosX); bsIn2.Read(fAimPosY);
				bsIn2.Read(fAimPosZ);
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
			m_pOfSyncDataOut->byteAction = byteAction;
			m_pOfSyncDataOut->fAngle = fAngle;
			m_pOfSyncDataOut->IsPlayerUpdateAiming = bIsAiming;
			m_pOfSyncDataOut->IsCrouching = (nibble & OF_FLAG_NIBBLE_CROUCHING) == OF_FLAG_NIBBLE_CROUCHING;
			m_pOfSyncDataOut->vecPos.X = x;
			m_pOfSyncDataOut->vecPos.Y = y;
			m_pOfSyncDataOut->vecPos.Z = z;
			m_pOfSyncDataOut->vecSpeed = vecSpeed;
			m_pOfSyncDataOut->bIsReloading = (nibble & OF_FLAG_NIBBLE_RELOADING) == OF_FLAG_NIBBLE_RELOADING;
			
		}
		else {
			printf("Error. Decompress was not completely successful.\n"); 
			exit(0);
		}
	}
	else {
		printf("An error occured during decompressing data.\n");
		exit(0);
	}
	
	
}
#else
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
	if (action & OF_FLAG_EXTRA_NIBBLE)
	{
		//This means a nibble is there to read
		ReadNibble(&nibble, bsIn);//This is used to find player is crouching and/or reloading 
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
	uint32_t dw_Keys = 0; uint8_t byteAction = 1;
	if (action & OF_FLAG_KEYS_OR_ACTION)
	{
		uint8_t keybyte1, keybyte2;
		bsIn->Read(keybyte1); bsIn->Read(keybyte2);
		uint8_t byte, nib; uint16_t value;
		bsIn->Read(byte);
		ReadNibble(&nib, bsIn);
		value = (byte << 4) | nib;
		byteAction = (value & ~0x80);
		dw_Keys |= keybyte1;
		dw_Keys |= (keybyte2 << 8);
		if (value & 0x80)
				dw_Keys |= 65536;
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
	m_pOfSyncDataOut->byteAction = byteAction;
	m_pOfSyncDataOut->fAngle = fAngle;
	m_pOfSyncDataOut->IsPlayerUpdateAiming = bIsAiming;
	m_pOfSyncDataOut->IsCrouching = (nibble & OF_FLAG_NIBBLE_CROUCHING)==OF_FLAG_NIBBLE_CROUCHING;
	m_pOfSyncDataOut->vecPos.X = x;
	m_pOfSyncDataOut->vecPos.Y = y;
	m_pOfSyncDataOut->vecPos.Z = z;
	m_pOfSyncDataOut->vecSpeed = vecSpeed;
	m_pOfSyncDataOut->bIsReloading = (nibble & OF_FLAG_NIBBLE_RELOADING) == OF_FLAG_NIBBLE_RELOADING;
	/*printf("Data Arrived========\n \
Player ID:%u, Position: (%f,%f,%f), Angle:%f, Speed:(%f,%f,%f) Weapon: %u, \
Game Keys: %u, Health: %u, Armour: %u, AimDir: (%f,%f,%f), AimPos: (%f, %f %f)\
\n", *bytePlayerIdOut, x, y, z, fAngle, vecSpeed.X, vecSpeed.Y,\
vecSpeed.Z, byteWeapon, dw_Keys, byteHealth, byteArmour,\
m_pOfSyncDataOut->vecAimDir.X,m_pOfSyncDataOut->vecAimDir.Y,

m_pOfSyncDataOut->vecAimDir.Z,m_pOfSyncDataOut->vecAimPos.X, \
m_pOfSyncDataOut->vecAimPos.Y, m_pOfSyncDataOut->vecAimPos.Z);*/
}
#endif
#ifdef _REL0471
void ReceivePassengerSyncData(RakNet::BitStream* bsIn, INCAR_SYNC_DATA* m_pIcSyncDataOut, uint8_t* byteSeatIdOut, uint8_t* bytePlayerIdOut)
{
	bsIn->ReadBit();//read one bit which is zero.
	uint32_t sourcelen;//The original no:of bytes after messageid=0x30.
	bsIn->Read(sourcelen);
	bsIn->ReadBit();//read one bit which is zero
	uint32_t lenCompressedData;//The length of the compressed data
	bsIn->Read(lenCompressedData);
	unsigned char* compressedData = (unsigned char*)malloc(lenCompressedData);
	if (!compressedData)
	{
		printf("Could not allocate memory\n"); exit(0);
	}
	for (int i = 0; i < lenCompressedData; i++)
	{
		if (!bsIn->Read(compressedData[i]))
		{
			printf("Could not read bitstream\n"); exit(0);
		}
	}
	unsigned char* data = (unsigned char*)malloc(sourcelen);//here sourcelen,but precaution.
	if (!data) {
		printf("Error allocating memory. Exiting\n"); exit(0);
	}
	uLongf datalen = (uLongf)sourcelen;
	int ret = uncompress(data, &datalen, compressedData, lenCompressedData);
	if (ret == Z_OK)
	{
		if (datalen == sourcelen)
		{
			//Decompression was 100% successful.
			// 
			//Create a bitstream so that rest of code works.
			RakNet::BitStream bsIn2(sourcelen);
			for (int i = 0; i < sourcelen; i++)
				bsIn2.Write(data[i]);
			//Now free previous allocated memories
			free(data); free(compressedData);
			bsIn2.ResetReadPointer();
			bsIn2.ReadAlignedBytes(bytePlayerIdOut, 1);
			uint16_t wVehicleId;
			uint8_t byte;
			bsIn2.Read(byte);
			wVehicleId = byte;
			uint8_t byte1;
			bsIn2.Read(byte1);
			wVehicleId += 256 * (byte1 >> 6);

			uint8_t bytePlayerHealth;
			bytePlayerHealth = (byte1 & 63) * 4;

			uint8_t byte2;
			bsIn2.Read(byte2);
			bytePlayerHealth += byte2 >> 6;

			uint8_t bytePlayerArmour;
			uint8_t byteSeatId;
			if (byte2 & 32)//then armour is present
			{
				bytePlayerArmour = (byte2 & 31) * 8;
				uint8_t byte3;
				bsIn2.Read(byte3);
				bytePlayerArmour += byte3 >> 5;
				byteSeatId = (byte3 >> 2) & 3;
			}
			else
			{
				bytePlayerArmour = 0;
				byteSeatId = (byte2 >> 2) & 3;
			}
			m_pIcSyncDataOut->VehicleID = wVehicleId;
			m_pIcSyncDataOut->bytePlayerHealth = bytePlayerHealth;
			m_pIcSyncDataOut->bytePlayerArmour = bytePlayerArmour;
			*byteSeatIdOut = byteSeatId;
		}
		else {
			printf("Error. Decompress was not completely successful.\n");
			exit(0);
		}
	}
	else {
		printf("An error occured during decompressing data.\n");
		exit(0);
	}
}
#elif defined(_REL004)
void ReceivePassengerSyncData(RakNet::BitStream* bsIn, INCAR_SYNC_DATA* m_pIcSyncDataOut, uint8_t* byteSeatIdOut, uint8_t* bytePlayerIdOut)
{
	bsIn->ReadAlignedBytes(bytePlayerIdOut, 1);
	bsIn->ReadAlignedBytes(byteSeatIdOut, 1);
	uint8_t byteArmour, byteHealth; uint16_t wVehicleId;
	bsIn->Read(byteArmour);
	bsIn->Read(byteHealth);
	bsIn->Read(wVehicleId);
	m_pIcSyncDataOut->VehicleID = wVehicleId;
	m_pIcSyncDataOut->bytePlayerHealth = byteHealth;
	m_pIcSyncDataOut->bytePlayerArmour = byteArmour;
}
#else 
void ReceivePassengerSyncData(RakNet::BitStream* bsIn, INCAR_SYNC_DATA* m_pIcSyncDataOut, uint8_t* byteSeatIdOut, uint8_t* bytePlayerIdOut)
{
	bsIn->ReadAlignedBytes(bytePlayerIdOut, 1);
	uint16_t wVehicleId;
	uint8_t byte;
	bsIn->Read(byte);
	wVehicleId = byte;
	uint8_t byte1;
	bsIn->Read(byte1);
	wVehicleId += 256 * (byte1 >> 6);

	uint8_t bytePlayerHealth;
	bytePlayerHealth = (byte1 & 63) * 4;

	uint8_t byte2;
	bsIn->Read(byte2);
	bytePlayerHealth += byte2 >> 6;

	uint8_t bytePlayerArmour;
	uint8_t byteSeatId;
	if (byte2 & 32)//then armour is present
	{
		bytePlayerArmour = (byte2 & 31) * 8;
		uint8_t byte3;
		bsIn->Read(byte3);
		bytePlayerArmour += byte3 >> 5;
		byteSeatId = (byte3 >> 2) & 3;
	}
	else
	{
		bytePlayerArmour = 0;
		byteSeatId = (byte2 >> 2) & 3;
	}
	m_pIcSyncDataOut->VehicleID = wVehicleId;
	m_pIcSyncDataOut->bytePlayerHealth = bytePlayerHealth;
	m_pIcSyncDataOut->bytePlayerArmour = bytePlayerArmour;
	*byteSeatIdOut = byteSeatId;
}
#endif
uint8_t GetSlotIdFromWeaponId(uint8_t byteWeapon)
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
		// If invalid weapon then return 0
		byteWeaponSlot = 0;
		break;
	}
	return byteWeaponSlot;
}
/*This function sends passenger sync packets to server if the driver player is not streamed in or there is no driver.
It also stores the time in ms when it last send the packets.*/
void HandlePassengerSync()
{
	if (iNPC && iNPC->Initialized()
		&& iNPC->PSLimit != DISABLE_AUTO_PASSENGER_SYNC )
	{
		if (mPlayback.running && mPlayback.type == PLAYER_RECORDING_TYPE_ALL)
		{
			//do not send sync data.
			return;
		}
		if (npc->m_wVehicleId && npc->m_byteSeatId > 0)
		{
			//npc is sitting as passenger 
			//Does the vehicle have driver?
			CVehicle* vehicle = m_pVehiclePool->GetAt(npc->m_wVehicleId);
			if(vehicle)
			{
				CPlayer* driver = m_pPlayerPool->GetAt(vehicle->GetDriver());
				if (
					(driver && !driver->IsStreamedIn()
						) ||
					!driver)
				{
					//The driver is not streamed in yet or there is no driver.
					DWORD now = GetTickCount();
					if (now - iNPC->PSLastSyncedTick > 350)
					{
						SendPassengerSyncData();
						iNPC->PSLastSyncedTick = now;
					}
				}
			}
		}
	}
}
void CheckPassengerSync()
{
	if (iNPC && iNPC->Initialized()
		&& iNPC->PSOnServerCycle)
		HandlePassengerSync();
}
//used when server set player.Health=0.
void CheckRemoveDeadBody()
{
	if (iNPC && iNPC->Initialized()
		&& iNPC->WaitingToRemoveBody)
	{
		if (npc && npc->m_byteHealth == 0)
		{
			DWORD t = iNPC->TimeShotInfoSend;
			DWORD n = GetTickCount();
			if (n - t > 2100 || n - t < 0)
			{
				m_pFunctions->SendDeathInfo(iNPC->ShotInfoWeapon, iNPC->KillerId, static_cast<bodyPart>(iNPC->byteAutodeathBodyPart));
				iNPC->WaitingToRemoveBody = false;
				return;
			}
		}
	}
}
void CheckSendOnFootSyncVE()
{
	if (iNPC && iNPC->Initialized()
		&& iNPC->WaitingForVEOnFootSync)
	{
		DWORD t = iNPC->VETickCount;
		DWORD n = GetTickCount();
		if (n - t > 650 || n - t < 0) //650 ms, after exit vehicle, send ofsyncdata
		{
			SendNPCOfSyncDataLV();
			iNPC->WaitingForVEOnFootSync = false;
			return;
		}
	}
}
void OnCycle()
{
#ifdef WIN32
	if (bShutdownSignal)
	{
		peer->CloseConnection(systemAddress, true);
	}
#endif
	DWORD dwSleepTime = ProcessPlaybacks();
	DWORD dw_TickOne = GetTickCount();
	ProcessTimers(dw_TickOne);//SetTimerEx, SetTimer
	CheckSendOnFootSyncVE();
	CheckPassengerSync();
	if (bConsoleInputEnabled)
		CheckForConsoleInput();
	CheckRemoveDeadBody();
	m_pEvents->OnCycle();
	DWORD dw_TickTwo = GetTickCount();
	DWORD dw_interval = dw_TickTwo - dw_TickOne;
	if (dw_interval >= dwSleepTime)
		 Sleep(0);
	else if (dw_interval >= 0)
	{
		DWORD i3 = dwSleepTime - dw_interval;
		//i3 will be non negative.
		//Sleep(dwSleepTime);
		Sleep(i3);
	}
	else Sleep(0);
}
uint32_t bytes_swap_u32(uint32_t i) { //aa bb cc dd
	//00 00 00 aa | 00 aa bb cc | bb cc dd 00 | dd 00 00 00
	uint32_t u = (i >> 24) |
		((i >> 8) & 0x0000FF00) |
		((i << 8) & 0x00FF0000) |
		(i << 24);
	return u;
}