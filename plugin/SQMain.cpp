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
HSQUIRRELVM v;
HSQAPI sq; 
CPlayerPool* m_pPlayerPool;
void OnPlayerUpdate2(int32_t playerId, vcmpPlayerUpdate updateType)
{
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;
	m_pPlayer->ProcessUpdate2(VCMP,updateType);
}

uint8_t OnPlayerCommand2(int32_t playerId, const char* message)
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
	if (command == "_npc_skin_request")
	{
		int32_t skinId=(int32_t)strtol(text.c_str(), NULL, 10);
		vcmpError e= VCMP->SetPlayerSkin(playerId, skinId);
		if (e)
		{
			printf("Invalid Skin ID %d\n", skinId);
		}
	}
	return 1;
}
void OnPlayerConnect2(int32_t playerId)
{
	m_pPlayerPool->New(playerId);
}
void OnPlayerDisconnect2(int32_t playerId, vcmpDisconnectReason reason)
{
	/*CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (m_pPlayer->IsRecording())m_pPlayer->Abort();*/
	m_pPlayerPool->Delete(playerId);
}
uint8_t OnServerInitialise2 ()
{
	m_pPlayerPool = new CPlayerPool();
	CreateRecFolder();
	return 1;
}
void OnSquirrelScriptLoad() {
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
			RegisterFuncs(v);
			
			// Register constants
			RegisterConsts(v);
		}
	}
	else
		OutputError("Failed to attach to SQHost2.");
}

// Called when the server is loading the Squirrel plugin
uint8_t OnPluginCommand2(uint32_t type, const char* text) {
	switch (type) {
		case 0x7D6E22D8:
			OnSquirrelScriptLoad();
			break;
		default:
			break;
	}
	return 1;
}
#ifdef _WIN32
void CreateRecFolder()
{
	if (!CreateDirectoryA(RECDIR, NULL) &&
		ERROR_ALREADY_EXISTS != GetLastError())
		printf("NPC: Recording folder creation failed\n");
}
#else

void CreateRecFolder()
{
	mode_t mode = 0755;
	mkdir(RECDIR, mode);
}
#endif
extern "C" unsigned int VcmpPluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo) {
	VCMP = pluginFuncs;

	// Plugin information
	pluginInfo->pluginVersion = 0x110;
	pluginInfo->apiMajorVersion = PLUGIN_API_MAJOR;
	pluginInfo->apiMinorVersion = PLUGIN_API_MINOR;
	strcpy(pluginInfo->name, "NPC");
	pluginCalls->OnServerInitialise = OnServerInitialise2;
	pluginCalls->OnPluginCommand = OnPluginCommand2;
	pluginCalls->OnPlayerUpdate = OnPlayerUpdate2;

	pluginCalls->OnPlayerCommand = OnPlayerCommand2;

	pluginCalls->OnPlayerConnect = OnPlayerConnect2;
	pluginCalls->OnPlayerDisconnect = OnPlayerDisconnect2;
	return 1;
}

