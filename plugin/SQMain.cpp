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
#include "SQMain.h"
#include "SQConsts.h"
#include "SQFuncs.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include "Exports.h"
void CreateRecFolder();
#define SUCCESS 0xFFFFFFFF
#define SVRMSG 0xFFFFFFFF
#ifndef _WIN32
long GetTickCount()
{
	tms tm;
	return (times(&tm) * 10);
}
#endif
#define PLAYBACK_CMDS_DISABLE
PluginFuncs* VCMP;
HSQUIRRELVM v = NULL;
HSQAPI sq = NULL;
NPCHideImports* npchideFuncs = NULL;
bool npchideAvailable = false;
CPlayerPool* m_pPlayerPool;
std::string szRecDirectory = RECDIR;
NPCExports* pExports;
int wMaxNpc = -1;
CServerRecording* pServerRecording;
PlaybackMultiple* pPlaybackMultiple;
extern bool bNewPlayersRecord;
extern uint8_t byteNewPlayersRecordType;
extern uint32_t dw_NewPlayersFlag;
bool WriteSpawnData(int32_t playerId, CPlayer* m_pPlayer);
bool CreateFolder(const char* name);
void NPC04_OnPlayerUpdate(int32_t playerId, vcmpPlayerUpdate updateType)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;
	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL)
	{
		switch (updateType)
		{
		case vcmpPlayerUpdateNormal:
			if (!(m_pPlayer->m_dwRecFlags & REC_ONFOOT_NORMAL))return;
			break;
		case vcmpPlayerUpdateAiming:
			if (!(m_pPlayer->m_dwRecFlags & REC_ONFOOT_AIM))return;
			break;
		case vcmpPlayerUpdateDriver:
			if (!(m_pPlayer->m_dwRecFlags & REC_VEHICLE_DRIVER))return;
			break;
		case vcmpPlayerUpdatePassenger:
			if (!(m_pPlayer->m_dwRecFlags & REC_VEHICLE_PASSENGER))return;
			break;
		default:break;
		}
		if (!VCMP->IsPlayerSpawned(playerId))
			return;
		//not enough
		if ((m_pPlayer->m_dwRecFlags&REC_SPAWN)&&!m_pPlayer->SpawnDataWritten)
		{
			if (!WriteSpawnData(playerId, m_pPlayer))
				return;
		}
	}
	m_pPlayer->ProcessUpdate2(VCMP,updateType);
}

uint8_t NPC04_OnPlayerCommand(int32_t playerId, const char* message)
{
	std::string mes = std::string(message);
	uint32_t r=(uint32_t)mes.find(' ');
	std::string command = mes.substr(0, r);
	std::string text = "";
	if (r!= std::string::npos&&mes.length() > r + 1)
	{
		text= mes.substr(r + 1);
	}
#ifndef PLAYBACK_CMDS_DISABLE
	if(command==std::string("ofrecord") )
	{
		if (VCMP->IsPlayerAdmin(playerId) == false)
		{
			VCMP->SendClientMessage(playerId, SVRMSG, "ofrecord: Admin privilage required!");
			return 1;
		}
		int32_t isOnVehicle=VCMP->GetPlayerVehicleId(playerId);
		if (isOnVehicle)
		{
			VCMP->SendClientMessage(playerId, SVRMSG, "You cannot be in vehicle to use /ofrecord");
			return 1;
		}
		if (text == "")
		{
			VCMP->SendClientMessage(playerId, SVRMSG, "Usage: /ofrecord filename");
			return 1;
		}
		bool success=StartRecordingPlayerData(playerId, PLAYER_RECORDING_TYPE_ONFOOT, text);
		if (success)
			VCMP->SendClientMessage(playerId, SUCCESS, "[Recording]Started");
		else
			VCMP->SendClientMessage(playerId, SVRMSG, "ofrecord-failed");
	}else if (command == std::string("vrecord"))
	{
		if (VCMP->IsPlayerAdmin(playerId) == false)
		{
			VCMP->SendClientMessage(playerId, SVRMSG, "vrecord: Admin privilage required!");
			return 1;
		}
		int32_t isOnVehicle = VCMP->GetPlayerVehicleId(playerId);
		if (!isOnVehicle)
		{
			VCMP->SendClientMessage(playerId, SVRMSG, "You must be in a vehicle to use /ofrecord");
			return 1;
		}
		if (text == "")
		{
			VCMP->SendClientMessage(playerId, SVRMSG, "Usage: /vrecord filename");
			return 1;
		}
		bool success = StartRecordingPlayerData(playerId, PLAYER_RECORDING_TYPE_DRIVER, text);
		if (success)
			VCMP->SendClientMessage(playerId, SUCCESS, "[Recording]Started");
		else
			VCMP->SendClientMessage(playerId, SVRMSG, "vrecord-failed");
	}
	else if (command == "stoprecord")
	{
		bool success=StopRecordingPlayerData(playerId);
		if (success)
			VCMP->SendClientMessage(playerId, SUCCESS, "[Recording]Stopped");
		else
			VCMP->SendClientMessage(playerId, SVRMSG, "Error");
	}
#endif
	if (IsPlayerNPC(playerId))
	{
		if (command == "_npc_skin_request")
		{
			int32_t skinId = (int32_t)strtol(text.c_str(), NULL, 10);
			vcmpError e = VCMP->SetPlayerSkin(playerId, skinId);
			if (e)
			{
				printf("Invalid Skin ID %d requested by npc\n", skinId);
			}
		}
		else if (command == "_npc_weapon_request")
		{
			int32_t weaponId = (int32_t)strtol(text.c_str(), NULL, 10);
			vcmpError e = VCMP->SetPlayerWeapon(playerId, weaponId,999);
			if (e)
			{
				printf("Invalid Weapon ID %d requested by npc\n", weaponId);
			}
		}
	}
	if (!m_pPlayerPool->GetSlotState(playerId))return 1;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return 1;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_PLAYER_COMMAND
			))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_PLAYER_COMMAND;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return 1;
		}
		COMMANDDATA data;
		data.len = strlen(message);
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return 1;
		}
		count = fwrite(message, sizeof(char), data.len, m_pPlayer->pFile);
		if (count != data.len)
		{
			m_pPlayer->Abort(); return 1;
		}
	}
	return 1;
}
void NPC04_OnPlayerConnect(int32_t playerId)
{
	bool success=m_pPlayerPool->New(playerId);
	if (success)
	{
		if (bNewPlayersRecord)
		{
			success=StartRecordingPlayerData(playerId, byteNewPlayersRecordType, "", dw_NewPlayersFlag);
			//write data of rec file in hrec
			if (success)
			{
				if (pServerRecording->IsRecording())
				{
					CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
					if (m_pPlayer && m_pPlayer->IsRecording())
					{
						std::string filename = m_pPlayer->GetFileName();
						char name[30]{};
						vcmpError e=VCMP->GetPlayerName(playerId, name, sizeof(name));
						if (e != vcmpErrorNone)return;
						if (filename != "")
						{
							DWORD dw_time = GetTickCount();
							pServerRecording->WriteNewData(dw_time, filename,name);
						}
					}
				}
			}
		}
	}
}
void NPC04_OnPlayerDisconnect(int32_t playerId, vcmpDisconnectReason reason)
{
	if (m_pPlayerPool->GetSlotState(playerId))
	{
		CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);

		if (m_pPlayer && m_pPlayer->IsRecording())
		{
			if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
				(
					m_pPlayer->m_dwRecFlags & REC_DISCONNETION
					))
			{
				GENERALDATABLOCK datablock;
				datablock.time = GetTickCount();
				datablock.bytePacketType = PACKET_DISCONNECT;
				size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
				if (count != 1)
				{
					m_pPlayer->Abort();
				}
			}
		}
	}
	m_pPlayerPool->Delete(playerId);
}
uint8_t NPC04_OnServerInitialise ()
{
	m_pPlayerPool = new CPlayerPool();
	CreateRecFolder();
	return 1;
}

void NPC04_OnSquirrelScriptLoad() {
	// See if we have any imports from Squirrel
	size_t size;
	int32_t sqId      = VCMP->FindPlugin(const_cast<char*>("SQHost2"));
	
	const void ** sqExports = VCMP->GetPluginExports(sqId, &size);
	
	// We do!
	if (sqExports != NULL && size > 0) {
		// Cast to a SquirrelImports structure
		SquirrelImports ** sqDerefFuncs = (SquirrelImports **)sqExports;
		
		// Now let's change that to a SquirrelImports pointer
		SquirrelImports * sqFuncs       = (SquirrelImports *)(*sqDerefFuncs);
		
		// Now we get the virtual machine
		if (sqFuncs) {
			// Get a pointer to the VM and API
			v = *(sqFuncs->GetSquirrelVM());
			sq = *(sqFuncs->GetSquirrelAPI());

			// Register functions
			NPC04_RegisterFuncs(v);
			
			// Register constants
			NPC04_RegisterConsts(v);
		}
	}
	else
		OutputError("Failed to attach to SQHost2.");
}

// Called when the server is loading the Squirrel plugin
uint8_t NPC04_OnPluginCommand2(uint32_t type, const char* text) {
	switch (type) {
		case 0x7D6E22D8:
			NPC04_OnSquirrelScriptLoad();
			break;
		case 0x10001000:
			if (strcmp(text, "NPCHideExportsReady") == 0)
			{
				int32_t pluginId = VCMP->FindPlugin("hide_npc"); 
				if (pluginId != -1)
				{
					size_t size;
					const void** NPCHideExports = VCMP->GetPluginExports(pluginId, &size);
					vcmpError e = VCMP->GetLastError();
					if (e == vcmpErrorNone)
					{
						if (NPCHideExports != NULL && size > 0)
						{
							NPCHideImports** npchideImports = (NPCHideImports**)NPCHideExports;
							npchideFuncs = (NPCHideImports*)(*npchideImports);
							if (npchideFuncs)
							{
								npchideAvailable = true;
							}
						}
					}
				}
			}
			break;
		case CMD_ECHO:
		{
			printf("%s\n", text);
		}break;
		case START_RECORDING_PLAYERDATA:
		{
			char* message = (char*)malloc((strlen(text)+1) * sizeof(char));
			if (message)
			{
				strcpy(message, text);
				// Returns first token 
				char* plrid = strtok(message, " ");
				if (plrid != NULL)
				{
					char* recordtype = strtok(NULL, " ");
					if (recordtype != NULL)
					{
						char* recordname = strtok(NULL, " ");
						if (recordname != NULL)
						{
							char* flags = strtok(NULL, " ");
							if (flags != NULL)
							{
								uint8_t byteplayerId, byteRecordType;
								uint32_t dw_flags;
								std::string szRecordName = std::string(recordname);
								dw_flags = std::stoi(flags, nullptr, 10);
								byteplayerId = std::stoi(plrid, nullptr, 10);
								byteRecordType = std::stoi(recordtype, nullptr, 10);
								if (!VCMP->IsPlayerConnected(byteplayerId))
								{
									free(message); 
									VCMP->SendPluginCommand(0x3b457A11, "Error: %u not connected", byteplayerId);
									return 0;
								}
								if (byteRecordType != PLAYER_RECORDING_TYPE_ONFOOT && byteRecordType != PLAYER_RECORDING_TYPE_DRIVER &&
									byteRecordType != PLAYER_RECORDING_TYPE_ALL)
								{
									free(message); 
									VCMP->SendPluginCommand(0x3b457A11, "Error: recordtype must be %u, %u or %u",
										PLAYER_RECORDING_TYPE_ONFOOT, PLAYER_RECORDING_TYPE_DRIVER,
										PLAYER_RECORDING_TYPE_ALL);
									return 0;
								}
								bool success = StartRecordingPlayerData(byteplayerId, byteRecordType, szRecordName, dw_flags);
								if (success)
									VCMP->SendPluginCommand(0x3b457A11, "Success. Recording Started for player %u",std::to_string(byteplayerId).c_str());
								else
									VCMP->SendPluginCommand(0x3b457A11, "Error: Could not start recording for % u", std::to_string(byteplayerId).c_str());
								free(message);
								return 0;
							}
							else {
								free(message);
								VCMP->SendPluginCommand(0x3b457A11, "Error: Flags not provided");
								return 0;
							}
						}
						else {
							free(message); 
							VCMP->SendPluginCommand(0x3b457A11, "Error: recordname not provided");
							return 0;
						}
					}
					else {
						free(message); 
						VCMP->SendPluginCommand(0x3b457A11, "Error: recordtype not specified");
						return 0;
					}
				}
				else {
					free(message); 
					VCMP->SendPluginCommand(0x3b457A11, "Error: plrid not provided");
					return 0;
				}

			}else VCMP->SendPluginCommand(0x3b457A11, "Error: Memory allocation failed");
			return 0;
		}break;
		case STOP_RECORDING_PLAYER_DATA:
		{
			//StopRecordingPlayerData
			char* message = (char*)malloc((strlen(text) + 1) * sizeof(char));
			if (message)
			{
				strcpy(message, text);
				// Returns first token 
				char* plrid = strtok(message, " ");
				if (plrid != NULL)
				{
					uint8_t byteplayerId = std::stoi(plrid, nullptr, 10);
					if (!VCMP->IsPlayerConnected(byteplayerId))
					{
						free(message);
						VCMP->SendPluginCommand(0x3b457A12, "Error: %u not connected", byteplayerId);
						return 0;
					}
					bool success = StopRecordingPlayerData(byteplayerId);
					if (success)
						VCMP->SendPluginCommand(0x3b457A12, "Success. Recording Stopped for %u", std::to_string(byteplayerId).c_str());
					else
						VCMP->SendPluginCommand(0x3b457A12, "Error occured.");
					free(message);
					return 0;
				}
				else {
					free(message);
					VCMP->SendPluginCommand(0x3b457A12, "Error: plrid not provided");
					return 0;
				}
			}
		}break;
		case START_RECORDING_ALL_PLAYERDATA:
		{
			//StartRecordingAllPlayerData
			char* message = (char*)malloc((strlen(text) + 1) * sizeof(char));
			if (message)
			{
				strcpy(message, text);
				// Returns first token 
				char* recordtype = strtok(message, " ");
				if (recordtype!=NULL)
				{
					char* flags = strtok(NULL, " ");
					if (flags != NULL)
					{
						char* newplayers = strtok(message, " ");
						if (newplayers != NULL)
						{
							uint8_t byteRecordType = std::stoi(recordtype, nullptr, 10);
							uint32_t dw_Flags= std::stoi(flags, nullptr, 10);
							uint8_t recordnewplayers = std::stoi(newplayers, nullptr, 10);
							bool bRecordNewPlayers = (recordnewplayers == 0 )? 0 : 1;
							uint8_t n=StartRecordingAllPlayerData(byteRecordType, dw_Flags, bRecordNewPlayers);
							VCMP->SendPluginCommand(0x3b457A13, "Success. No:of recordings started is %u", std::to_string(n).c_str());
							free(message);
							break;
						}else {
							free(message);
							VCMP->SendPluginCommand(0x3b457A13, "Error: recordnewplayers(0/1) not provided");
							break;
						}
					}else {
						free(message);
						VCMP->SendPluginCommand(0x3b457A13, "Error: flags not provided");
						break;
					}
				}
				else {
					free(message);
					VCMP->SendPluginCommand(0x3b457A13, "Error: recordtype not provided");
					break;
				}
			}
		}break;
		case STOP_RECORDING_ALL_PLAYER_DATA:
		{
			//StopRecordingAllPlayerData
			char* message = (char*)malloc((strlen(text) + 1) * sizeof(char));
			if (message)
			{
				strcpy(message, text);
				uint8_t n = StopRecordingAllPlayerData();
				VCMP->SendPluginCommand(0x3b457A14, "Success. No:of recordings stopped is %u", std::to_string(n).c_str());
				free(message);
				break;
			}
		}break;
		case PUT_SERVER_IN_RECORDING_MODE:
		{
			//PutServerInRecordingMode
			//plgncmd 0x3b457a05 3 60 filename
			char* message = (char*)malloc((strlen(text) + 1) * sizeof(char));
			if (message)
			{
				strcpy(message, text);
				// Returns first token 
				char* recordtype = strtok(message, " ");
				if (recordtype != NULL)
				{
					char* flags = strtok(NULL, " ");
					if (flags != NULL)
					{
						char* szfilename = strtok(message, " ");
						if (szfilename != NULL)
						{
							uint8_t byteRecordType = std::stoi(recordtype, nullptr, 10);
							uint32_t dw_Flags = std::stoi(flags, nullptr, 10);
							bool s=PutServerInRecordingMode(byteRecordType, dw_Flags, szfilename);
							VCMP->SendPluginCommand(0x3b457A15, "Success. Server is now in recording mode.");
							free(message);
							break;
						}
						else {
							free(message);
							VCMP->SendPluginCommand(0x3b457A15, "Error: filename not provided");
							break;
						}
					}
					else {
						free(message);
						VCMP->SendPluginCommand(0x3b457A15, "Error: flags not provided");
						break;
					}
				}
				else {
					free(message);
					VCMP->SendPluginCommand(0x3b457A15, "Error: recordtype not provided");
					break;
				}
			}
		}break;
		case STOP_SERVER_IN_RECORDING_MODE:
		{
			//StopServerInRecordingMode
			char* message = (char*)malloc((strlen(text) + 1) * sizeof(char));
			if (message)
			{
				strcpy(message, text);
				bool s = StopServerInRecordingMode();
				if(s)
					VCMP->SendPluginCommand(0x3b457A16, "Success. Server recording finished");
				else
					VCMP->SendPluginCommand(0x3b457A16, "Error: An Error occured.");
				free(message);
				break;
			}
		}break;
		case CONNECT_MULTIPLE_NPCS:
		{
			//ConnectMultipleNpcs
			//plgncmd 0x3b457a07 filename host=127.0.0.1
			char* message = (char*)malloc((strlen(text) + 1) * sizeof(char));
			if (message)
			{
				strcpy(message, text);
				// Returns first token 
				const char* filename = strtok(message, " ");
				if (filename != NULL)
				{
					const char* host = strtok(NULL, " ");
					if (host == NULL)host = "127.0.0.1";
					uint8_t s = pPlaybackMultiple->Start(filename, host);
					if (s == HREC_INITIALIZE_SUCCESS)
					{
						VCMP->SendPluginCommand(0x3b457A17, "Success.");
					}
					else
					{
						VCMP->SendPluginCommand(0x3b457A17, "Error processing hrec. Code: %d", s);
					}
					free(message);
					break;
				}
				else {
					free(message);
					VCMP->SendPluginCommand(0x3b457A17, "Error: filename not provided");
					break;
				}
			}else
				VCMP->SendPluginCommand(0x3b457A17, "Error: memory allocation failed");
		}break;
		case STOP_CONNECTING_MULTIPLE_NPCS:
		{
			//StopConnectingMultipleNpcs
			char* message = (char*)malloc((strlen(text) + 1) * sizeof(char));
			if (message)
			{
				strcpy(message, text);
				if (pPlaybackMultiple->running)
				{
					pPlaybackMultiple->Abort();
					VCMP->SendPluginCommand(0x3b457A18, "Success. ");
				}
				else VCMP->SendPluginCommand(0x3b457A18, "Error. MultipleNpcs file process was not running");
				free(message);
				break;
			}
		}break;
		case CALL_NPCCLIENT:
		{
			char* message = (char*)malloc((strlen(text) + 1) * sizeof(char));
			if (message)
			{
				strcpy(message, text);
				const char* script = "", * host = "127.0.0.1", * plugins = "", * execArg = "",
					* locx = "", * locy = "", * locz = "", * skin = "", * wep = "", * classid = "",
				*angle = "";
				const char* inputenabled = "0";
				//0x3b457A09 john npctest.nut 127.0.0.1 . . 1 x y z s w classid 
				// Returns first token 
				char* npcname = strtok(message, " ");
				if (npcname != NULL)
				{
					script= strtok(NULL, " ");
					if (script != NULL)
					{
						if (std::string(script) == ".")
							script = "";
						host = strtok(NULL, " ");
						if (host != NULL)
						{
							if (std::string(host) == ".")host = "127.0.0.1";
							plugins = strtok(NULL, " ");
							if (plugins != NULL)
							{
								if (std::string(plugins) == ".")plugins = "";
								execArg = strtok(NULL, " ");
								if (execArg != NULL)
								{
									if (std::string(execArg) == ".")execArg = "";
									inputenabled = strtok(NULL, " ");
									if (inputenabled != NULL)
									{
										if (std::string(inputenabled) == ".")inputenabled = "0";
										locx = strtok(NULL, " ");
										if (locx != NULL)
										{
											if (std::string(locx) == ".")locx = "";
											locy = strtok(NULL, " ");
											if (locy != NULL)
											{
												if (std::string(locy) == ".")locy = "";
												locz = strtok(NULL, " ");
												if (locz != NULL)
												{
													if (std::string(locz) == ".")locz = "";
													angle = strtok(NULL, " ");
													if (angle != NULL)
													{
														if (std::string(angle) == ".")angle = "";
														skin = strtok(NULL, " ");
														if (skin != NULL)
														{
															if (std::string(skin) == ".")skin = "";
															wep = strtok(NULL, " ");
															if (wep != NULL)
															{
																if (std::string(wep) == ".")wep = "";
																classid = strtok(NULL, " ");
																if (classid != NULL)
																{
																	if (std::string(classid) == ".")classid = "";
																}
																else classid = "";
															}
															else wep = "";
														}
														else skin = "";
													}
													else angle = "";
												}
											}
										}
									}
									else inputenabled = "0";
									
								}
								else execArg = "";
							}
							else plugins = "";
						}
						else host = "127.0.0.1";
					}
					else script = "";
					bool bConsoleInputEnabled = false;
					if (std::string(inputenabled) == "1")
						bConsoleInputEnabled = true;
					else if (std::string(inputenabled) != "0")
					{
						free(message);
						VCMP->SendPluginCommand(0x3b457A19, "Error: inputenabled is neither 0 nor 1");
						break;
					}
					//Set up loc parameter needed. x__ y__ z__ a__ w__ s__
					std::string szlocParam = "";
					if (locx != NULL && std::string(locx) != "" &&
						locy != NULL && std::string(locy) != "" &&
						locz != NULL && std::string(locz) != "")
					{
						//Try conversion
						float x, y, z;
						try
						{
							x = std::stof(locx);
							y = std::stof(locy);
							z = std::stof(locz);
						}
						catch (...)
						{
							free(message);
							VCMP->SendPluginCommand(0x3b457A19, "Error: x,y,z conversion to float failed");
							break;
						}
						szlocParam += "x" + std::to_string(x) + " y" + std::to_string(y) + " z" + std::to_string(z) + " ";
					}
					if (std::string(angle) != "")
					{
						float fangle;
						try
						{
							fangle = std::stof(angle);
						}
						catch (...)
						{
							free(message);
							VCMP->SendPluginCommand(0x3b457A19, "Error: angle conversion to float failed");
							break;
						}
						szlocParam += "a" + std::to_string(fangle) + " ";
					}
					if (std::string(skin) != "")
					{
						szlocParam += "s" + std::string(skin) + " ";
					}
					if (std::string(wep) != "")
					{
						szlocParam += "w" + std::string(wep) + " ";
					}
					if (std::string(classid) != "")
					{
						szlocParam += "c" + std::string(classid);
					}
					bool s=CallNPCClient(npcname, script, bConsoleInputEnabled, host, plugins, execArg,
						szlocParam.c_str());
					if (s)
					{
						VCMP->SendPluginCommand(0x3b457A19, "Success.");
					}
					else
					{
						VCMP->SendPluginCommand(0x3b457A19, "Error CallNPCClient false returned");
					}
					free(message);
					break;
				}
				else {
					free(message);
					VCMP->SendPluginCommand(0x3b457A19, "Error: npcname not provided");
					break;
				}
			}
			else
				VCMP->SendPluginCommand(0x3b457A19, "Error: memory allocation failed");
		}break;

		default:
			break;
	}
	return 1;
}

void CreateRecFolder()
{
	if(!CreateFolder(szRecDirectory.c_str()))
		printf("NPC: Recording folder creation failed\n");
}
void CreateHRecFolder()
{
	if (!CreateFolder(SERVER_REC_DIR))
		printf("NPC: Server Recording folder creation failed\n");
}
#ifdef _WIN32
bool CreateFolder(const char* name)
{
	if (!CreateDirectoryA(name, NULL) &&
		ERROR_ALREADY_EXISTS != GetLastError())return false;
	return true;
}
#else

bool CreateFolder(const char* name)
{
	mode_t mode = 0755;
	int s=mkdir(name, mode);
	if (s == 0|| errno == EEXIST)return true;
	return false;
}
#endif
void NPC04_OnPlayerSpawn (int32_t playerId)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;
	
	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL && (m_pPlayer->m_dwRecFlags & REC_SPAWN))
	{
		if (m_pPlayer->SpawnDataWritten)return;//no need to write it again
		if(!WriteSpawnData(playerId, m_pPlayer))
		return;
	}

}
bool WriteSpawnData(int32_t playerId,CPlayer* m_pPlayer)
{
	GENERALDATABLOCK datablock;
	datablock.time = GetTickCount();
	datablock.bytePacketType = PACKET_SPAWN;
	size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
	if (count != 1) {
		 m_pPlayer->Abort(); return false;
	}
	uint8_t team = (uint8_t)VCMP->GetPlayerTeam(playerId);
	uint16_t skin = (uint16_t)VCMP->GetPlayerSkin(playerId);
	SPAWNDATA data{};
	data.byteTeam = team; data.wSkin = skin; data.byteClassId = VCMP->GetPlayerClass(playerId);
	count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
	if (count != 1) {
		m_pPlayer->Abort(); return false;
	}
	m_pPlayer->SpawnDataWritten = true;
	return true;
}
void NPC04_OnPlayerDeath (int32_t playerId, int32_t killerId, int32_t reason, vcmpBodyPart bodyPart)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL && (m_pPlayer->m_dwRecFlags & REC_DEATH))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_DEATH;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)return m_pPlayer->Abort();
		DEATHDATA data{};
		if (VCMP->IsPlayerConnected(killerId))
		{
			VCMP->GetPlayerName(killerId, data.killername, sizeof(data.killername));
		}
		data.byteBodyPart = bodyPart;
		data.byteReason = (uint8_t)reason;
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)return m_pPlayer->Abort();
	}
	m_pPlayer->SpawnDataWritten = false;
}
uint8_t NPC04_OnPlayerRequestEnterVehicle (int32_t playerId, int32_t vehicleId, int32_t slotIndex)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return 1;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return 1;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL && 
		(
			(m_pPlayer->m_dwRecFlags & REC_VEHICLE_DRIVER)||
			(m_pPlayer->m_dwRecFlags & REC_VEHICLE_PASSENGER)
		)
		)
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_VEHICLE_REQ_ENTER;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return 1;
		}
		VEHICLEENTER_REQUEST data{};
		data.SeatId = (uint8_t)slotIndex;
		data.wVehicleID = vehicleId;
		data.wModel = (uint16_t)VCMP->GetVehicleModel(vehicleId);
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1) {
			m_pPlayer->Abort(); return 1;
		}
	}
	return 1;
}
void NPC04_OnPlayerEnterVehicle (int32_t playerId, int32_t vehicleId, int32_t slotIndex)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			(m_pPlayer->m_dwRecFlags & REC_VEHICLE_DRIVER) ||
			(m_pPlayer->m_dwRecFlags & REC_VEHICLE_PASSENGER)
			)
		)
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_VEHICLE_ENTER;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
		VEHICLEENTER_REQUEST data{};
		data.SeatId = (uint8_t)slotIndex;
		data.wVehicleID = vehicleId;
		data.wModel = (uint16_t)VCMP->GetVehicleModel(vehicleId);
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1) {
			m_pPlayer->Abort(); return;
		}
	}
}
void NPC04_OnPlayerExitVehicle(int32_t playerId, int32_t vehicleId)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			(m_pPlayer->m_dwRecFlags & REC_VEHICLE_DRIVER) ||
			(m_pPlayer->m_dwRecFlags & REC_VEHICLE_PASSENGER)
			)
		)
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_VEHICLE_EXIT;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
		//No additional data		
	}
}
void NPC04_OnObjectShot(int32_t objectId, int32_t playerId, int32_t weaponId)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_OBJECT_SHOT
		))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_OBJECT_SHOT;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
		OBJECTSHOT_DATA data;
		data.byteWeapon = (uint8_t)weaponId;
		data.wModel = (uint16_t)VCMP->GetObjectModel(objectId);
		data.wObjectID = (uint16_t)objectId;
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
	}
}

void NPC04_OnObjectTouched(int32_t objectId, int32_t playerId)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_OBJECT_TOUCH
			))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_OBJECT_TOUCH;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
		OBJECTTOUCH_DATA data;
		data.wModel = (uint16_t)VCMP->GetObjectModel(objectId);
		data.wObjectID = (uint16_t)objectId;
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
	}
}

void NPC04_OnPickupPicked(int32_t pickupId, int32_t playerId)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_PICKUP_PICKED
			))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_PICKUP_PICKED;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
		PICKUPATTEMPTDATA data;
		data.wPickupID = (uint16_t)pickupId;
		data.wModel = (uint16_t)VCMP->GetPickupModel(pickupId);
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
	}
}


void NPC04_OnCheckpointEntered(int32_t checkPointId, int32_t playerId)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_CHECKPOINT_ENTERED
			))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_CHECKPOINT_ENTERED;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
		CHECKPOINTENTEREDDATA data; float x=0, y=0, z=0;
		VCMP->GetCheckPointPosition(checkPointId, &x, &y, &z);
		data.vecPos = VECTOR(x, y, z);
		data.wCheckpointID = (uint16_t)checkPointId;
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
	}
}

void NPC04_OnCheckpointExited(int32_t checkPointId, int32_t playerId)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_CHECKPOINT_EXITED
			))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_CHECKPOINT_EXITED;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
		CHECKPOINTENTEREDDATA data; float x=0, y=0, z=0;
		VCMP->GetCheckPointPosition(checkPointId, &x, &y, &z);
		data.vecPos = VECTOR(x, y, z);
		data.wCheckpointID = (uint16_t)checkPointId;
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
	}
}
uint8_t NPC04_OnPlayerMessage(int32_t playerId, const char* message) 
{
	if (!m_pPlayerPool->GetSlotState(playerId))return 1;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return 1;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_PLAYER_CHAT
			))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_PLAYER_CHAT;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return 1;
		}
		COMMANDDATA data;
		data.len = strlen(message);
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return 1;
		}
		count = fwrite(message, sizeof(char), data.len, m_pPlayer->pFile);
		if (count != data.len)
		{
			m_pPlayer->Abort(); return 1;
		}
	}
	return 1;
}

uint8_t NPC04_OnPrivateMessage(int32_t playerId, int32_t targetPlayerId, const char* message) 
{
	if (!m_pPlayerPool->GetSlotState(playerId))return 1;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return 1;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_PLAYER_PM
			))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_PLAYER_PM;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return 1;
		}
		PRIVMSGDATA data{};
		data.msglen = strlen(message);
		if (VCMP->IsPlayerConnected(targetPlayerId))
		{
			VCMP->GetPlayerName(targetPlayerId, data.target, sizeof(data.target));
		}
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return 1;
		}
		count = fwrite(message, sizeof(char), data.msglen, m_pPlayer->pFile);
		if (count != data.msglen)
		{
			m_pPlayer->Abort(); return 1;
		}
	}
	return 1;
}
uint8_t NPC04_OnPlayerRequestClass (int32_t playerId, int32_t offset)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return 1;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return 1;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_SPAWN
			))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_CHOOSECLASS;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return 1;
		}
		_CLASSCHOOSEDATA data{};
		uint8_t previous = m_pPlayer->sgbytePreviousClassID;
		
		if (offset == previous)data.byteIndex = CLASS_CURRENT;
		else if (offset == (previous + 1))data.byteIndex = CLASS_NEXT;
		else if (offset == (previous - 1))data.byteIndex = CLASS_PREVIOUS;
		else if (offset == 0)data.byteIndex = CLASS_NEXT;
		else if (previous == 0)data.byteIndex = CLASS_PREVIOUS;
		if (offset == previous)data.byteIndex = CLASS_CURRENT;//overwrite
		/*if (data.byteIndex == CLASS_PREVIOUS && m_pPlayer->sgbytePreviousClassID == 0)
		{
			
		}*/
		data.byteclassId = (uint8_t)offset;
		m_pPlayer->sgbytePreviousClassID = offset;
		count = fwrite(&data, sizeof(data), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return 1;
		}
	}
	return 1;
}
void NPC04_OnServerShutdown()
{
	if (pServerRecording->IsRecording())
		pServerRecording->Finish();
	if (pPlaybackMultiple->running)
		pPlaybackMultiple->Stop();
}
void NPC04_OnServerFrame (float elapsedTime)
{
	if (pPlaybackMultiple->running)
	{
		pPlaybackMultiple->Process(elapsedTime);
	}
}
extern "C" unsigned int VcmpPluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo) {
	VCMP = pluginFuncs;

	// Plugin information
	pluginInfo->pluginVersion = 18003;//v1.8 beta3
	pluginInfo->apiMajorVersion = PLUGIN_API_MAJOR;
	pluginInfo->apiMinorVersion = PLUGIN_API_MINOR;
	strcpy(pluginInfo->name, "NPC");
	pluginCalls->OnServerInitialise = NPC04_OnServerInitialise;
	pluginCalls->OnPluginCommand = NPC04_OnPluginCommand2;
	pluginCalls->OnPlayerUpdate = NPC04_OnPlayerUpdate;
	pluginCalls->OnServerShutdown = NPC04_OnServerShutdown;
	pluginCalls->OnPlayerCommand = NPC04_OnPlayerCommand;
	pluginCalls->OnPlayerConnect = NPC04_OnPlayerConnect;
	pluginCalls->OnPlayerDisconnect = NPC04_OnPlayerDisconnect;
	
	pluginCalls->OnClientScriptData = NPC04_OnClientScriptData;
	pluginCalls->OnPlayerSpawn = NPC04_OnPlayerSpawn;
	pluginCalls->OnPlayerDeath= NPC04_OnPlayerDeath;
	pluginCalls->OnPlayerRequestEnterVehicle= NPC04_OnPlayerRequestEnterVehicle;
	pluginCalls->OnPlayerEnterVehicle= NPC04_OnPlayerEnterVehicle;
	pluginCalls->OnPlayerExitVehicle= NPC04_OnPlayerExitVehicle;
	pluginCalls->OnObjectShot = NPC04_OnObjectShot;
	pluginCalls->OnObjectTouched = NPC04_OnObjectTouched;
	pluginCalls->OnPickupPicked = NPC04_OnPickupPicked;
	pluginCalls->OnCheckpointEntered = NPC04_OnCheckpointEntered;
	pluginCalls->OnCheckpointExited = NPC04_OnCheckpointExited;
	pluginCalls->OnPlayerMessage = NPC04_OnPlayerMessage;
	pluginCalls->OnPlayerPrivateMessage = NPC04_OnPrivateMessage;
	pluginCalls->OnPlayerRequestClass = NPC04_OnPlayerRequestClass;
	pluginCalls->OnServerFrame = NPC04_OnServerFrame;

	pExports = new NPCExports();
	pExports->uStructSize = sizeof(NPCExports);
	pExports->StartRecordingPlayerData = StartRecordingPlayerData;
	pExports->StopRecordingPlayerData = StopRecordingPlayerData;
	pExports->IsPlayerNPC = IsPlayerNPC;
	pExports->CallNPCClient = CallNPCClient;
	pExports->StartRecordingAllPlayerData = StartRecordingAllPlayerData;
	pExports->StopRecordingAllPlayerData = StopRecordingAllPlayerData;
	pExports->IsPlayerRecording = IsPlayerRecording;
	pExports->PutServerInRecordingMode = PutServerInRecordingMode;
	pExports->StopServerInRecordingMode=StopServerInRecordingMode;
	pExports->IsServerInRecordingMode=IsServerInRecordingMode;
	pExports->ConnectMultipleNpcs=ConnectMultipleNpcs;
	pExports->StopConnectingMultipleNpcs=StopConnectingMultipleNpcs;
	VCMP->ExportFunctions(pluginInfo->pluginId,
		(const void**)&pExports, sizeof(NPCExports));
	
	//server.cfg

	char* szValue;
	cfg recdir;
	std::string szrecdir;
	recdir.read("server.cfg", "recdir");
	if (recdir.argc > 0)
	{
		szValue = recdir.ptr[0];
		szrecdir = std::string(szValue);
		if (szrecdir == "2")
		{
			szRecDirectory = RECDIR_ALTERNATE;
		}
		else if (szrecdir == "1")
		{
			szRecDirectory = RECDIR;
		}
		else printf("Warning: configuration 'recdir' in 'server.cfg' must have value 1 or 2. Specify 1 for \
%s and 2 for %s", RECDIR, RECDIR_ALTERNATE);
	}
	recdir.freememory();

	cfg configMaxNpc;
	std::string szMaxNpc;
	configMaxNpc.read("server.cfg", "max_npc");

	if (configMaxNpc.argc > 0)
	{
		szValue = configMaxNpc.ptr[0];
		szMaxNpc = std::string(szValue);
		
		try
		{
			int val = std::stoi(szMaxNpc, NULL);
			if (val >= -1 && val <= MAX_PLAYERS)
				wMaxNpc = val;
		}
		catch (...)
		{
			printf("Warning: max_npc value in server.cfg is incorrect\n");
		}
	}
	//else printf("Token max_npc not found in server.cfg");
	configMaxNpc.freememory();


	 pServerRecording= new CServerRecording();
	 pPlaybackMultiple = new PlaybackMultiple();
	return 1;
}
