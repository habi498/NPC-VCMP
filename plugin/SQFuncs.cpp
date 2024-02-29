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

#include <stdlib.h>
#include <stdio.h>
#include "SQFuncs.h"
#include <cstdio>
#include <vector>
extern PluginFuncs* VCMP;
extern HSQAPI sq;
extern NPCHideImports* npchideFuncs;
extern CServerRecording* pServerRecording;
extern PlaybackMultiple* pPlaybackMultiple;
extern bool npchideAvailable;
#ifdef WIN32
INT SW_STATUS = 0;
#endif
enum Version
{
	REL004 = 67000,
	REL006 = 67400
};
SQObject* ArrayIsNPC = NULL;
bool CallNPCClient(const char* szName, const char* szScript, bool bConsoleInputEnabled,
	const char* host, const char* plugins, const char* execArg, const char* loc, std::vector<const char*>params)
{
	CHAR szCmd[1536]; int iPort;
	ServerSettings s;
	VCMP->GetServerSettings(&s);
	iPort = s.port;//snprintf(szCmd,sizeof(szCmd),)
	if (strlen(szScript) > 0)
		snprintf(szCmd,sizeof(szCmd), "-h %s -p %d -n \"%s\" -m \"%s\"", host, iPort, szName, szScript);
	else
		snprintf(szCmd, sizeof(szCmd),"-h %s -p %d -n \"%s\"", host, iPort, szName);
	char szPassword[255];
	VCMP->GetServerPassword(szPassword, sizeof(szPassword));
	if (szPassword && szPassword[0] != '\0')
	{
		strcat(szCmd, " -z ");
		strcat(szCmd, szPassword);
	}
	if (strlen(loc) > 0)
	{
		//arguments specified
		strcat(szCmd, " -l \"");
		strcat(szCmd, loc);
		strcat(szCmd, "\"");
	}
	if (strlen(plugins) > 0)
	{
		strcat(szCmd, " -q \"");
		strcat(szCmd, plugins);
		strcat(szCmd, "\"");
	}
	//MultiArg tclap
	for (size_t i = 0; i < params.size(); i++)
	{
		strcat(szCmd, " -w \"");
		strcat(szCmd, params[i]);
		strcat(szCmd, "\"");
	}
	if (strlen(execArg) > 0)
	{
		strcat(szCmd, " -e \"");
		strcat(szCmd, execArg);
		strcat(szCmd, "\"");
	}
	if (bConsoleInputEnabled)
		strcat(szCmd, " -c");
	bool writelog = !bConsoleInputEnabled;
	uint32_t ver = VCMP->GetServerVersion();
	//printf("The command is %s\n", szCmd);
#ifdef WIN32
	INT STATUS_SW = SW_HIDE;
	if (bConsoleInputEnabled || SW_STATUS == SW_SHOW)
	{
		STATUS_SW = SW_SHOW;
		writelog = false; //Cannot write log. stdout will be stdout
	}
#endif
	if (writelog)
		strcat(szCmd, " -f");
#ifdef WIN32
	if(ver==REL004)
		ShellExecute(0, "open", "npcclient_r004.exe", szCmd, NULL, STATUS_SW);
	else if(ver==REL006)
		ShellExecute(0, "open", "npcclient.exe", szCmd, NULL, STATUS_SW);
	else {
		printf("Error server version: %d. ", ver);
		printf("Using REL006 = %d\n", REL006);
		ShellExecute(0, "open", "npcclient.exe", szCmd, NULL, STATUS_SW);
	}
#else
	char szDir[MAX_PATH];
	getcwd(szDir, MAX_PATH);
	char szCmd2[1024];
	char szBin[256];
	if (sizeof(long) == 4)
	{
		if (ver == REL004)
			sprintf(szBin, "npcclient%s", "32_r004");
		else
			sprintf(szBin, "npcclient%s", "32");
	}
	else if (sizeof(long) == 8)
	{
		if(ver == REL004)
			sprintf(szBin, "npcclient%s", "64_r004");
		else
			sprintf(szBin, "npcclient%s", "64");
	}
	sprintf(szCmd2, "%s/%s %s &",  szDir, szBin, szCmd);
	system(szCmd2);
#endif
	return 1;
}
_SQUIRRELDEF(SQ_ConnectNPC) {
    SQInteger iArgCount = sq->gettop(v);
	const SQChar* szName=NULL, *szScript=NULL, *host=NULL;
	SQBool bConsoleInput; const SQChar* szPlugins = NULL;
	const SQChar* szexecArg = NULL;
	sq->getstring(v, 2, &szName);
	if (iArgCount > 2)
	{
		sq->getstring(v, 3, &szScript);
		if (iArgCount > 3)
		{
			sq->getbool(v, 4, &bConsoleInput);
			if (iArgCount > 4)
			{
				sq->getstring(v, 5, &host);
				if (iArgCount > 5)
				{
					sq->getstring(v, 6, &szPlugins);
					if (iArgCount > 6)
					{
						sq->getstring(v, 7, &szexecArg);
					}
					else szexecArg = "";
				}
				else { szPlugins = ""; szexecArg = ""; }
			}
			else 
			{
				szPlugins = ""; 
				host = "127.0.0.1"; 
				szexecArg = "";
			}
		}
		else
		{
			szPlugins = ""; 
			host = "127.0.0.1";
			bConsoleInput = 0;
			szexecArg = "";
		}
	}
	else
	{	//Script not specified
		szScript = "";//start without script
		szPlugins = "";
		host = "127.0.0.1";
		bConsoleInput = 0;
		szexecArg = "";
	}
	if (strlen(host) == 0)
		host = "127.0.0.1";
	CallNPCClient(szName, szScript, bConsoleInput, host, szPlugins, szexecArg);
	sq->pushbool(v, SQTrue);
	return 1;
}
_SQUIRRELDEF(SQ_ConnectNPCEx) {
	SQInteger iArgCount = sq->gettop(v);
	const SQChar* szName = NULL, * szScript = NULL, * host = NULL,
		* szPlugins = NULL;
	sq->getstring(v, 2, &szName);
	VECTOR pos;
	float angle; SQInteger skinId, weaponId, classId;
	SQBool bConsoleInput;
	uint16_t flag = 0;
	const SQChar* szLoc = NULL;
	std::vector<const char*> params = {};
	if (iArgCount > 2) //means pos Vector(x,y,z) specified
	{
		sq->pushroottable(v);
		sq->pushstring(v, _SC("Vector"), -1);
		if (SQ_SUCCEEDED(sq->get(v, -2))) {
			sq->pushroottable(v); //push the 'this' (in this case is the global table)
			sq->pushfloat(v, 0.0);
			sq->pushfloat(v, 0.0);
			sq->pushfloat(v, 0.0);
			sq->call(v, 4, 1, 1); //calls the function 
			if (SQ_SUCCEEDED(sq->getclass(v, -1))) //-1 class
			{
				sq->push(v, 3);// -2 class,-1 instance 
				SQBool b, _b = SQFalse;
				//if instance at -1 is instance of class at -2. squirrel documentation is wrong here
				b = sq->instanceof(v);
				sq->pop(v, 2);//pop both instance and class.
				//now stack is roottable, closure, instance of Vector
				sq->pop(v, 2);//roottable left.
				sq->pushstring(v, _SC("EntityVector"), -1);
				if (SQ_SUCCEEDED(sq->get(v, -2))) {
					sq->pushroottable(v); //push the 'this' (in this case is the global table)
					sq->pushinteger(v, -1);
					sq->pushinteger(v, -1);
					sq->pushinteger(v, -1);
					sq->pushfloat(v, 0.0);
					sq->pushfloat(v, 0.0);
					sq->pushfloat(v, 0.0);
					sq->call(v, 7, 1, 1); //calls the function 
					if (SQ_SUCCEEDED(sq->getclass(v, -1))) //-1 class
					{
						sq->push(v, 3);// -2 class,-1 instance 
						_b = sq->instanceof(v);
						sq->pop(v, 2);//pop both instance and class.
						//now stack is roottable, closure, instance of EntityVector
						sq->pop(v, 2);//roottable left.
					}
				}
				//sq->instanceof might return integer other than SQTrue or SQFalse
				if (b == SQTrue || b == SQFalse || _b == SQTrue || _b == SQFalse)
				{
					if (b == SQTrue || _b == SQTrue)
					{
						sq->pushstring(v, "x", -1);
						sq->get(v, 3);
						sq->getfloat(v, -1, (SQFloat*)&pos.X);
						sq->poptop(v);
						sq->pushstring(v, "y", -1);
						sq->get(v, 3);
						sq->getfloat(v, -1, (SQFloat*)&pos.Y);
						sq->poptop(v);
						sq->pushstring(v, "z", -1);
						sq->get(v, 3);
						sq->getfloat(v, -1, (SQFloat*)&pos.Z);
						sq->poptop(v);
						sq->poptop(v);//pops roottable
						flag |= 1;
					}
				}
			}
		}
		if ((flag & 1) != 1)
			return sq->throwerror(v, "Error in obtaining vector position");
		
		if (iArgCount > 3)
		{
			sq->getfloat(v, 4, &angle);
			flag |= 2;
			if (iArgCount > 4)
			{
				sq->getinteger(v, 5, &skinId);
				flag |= 4;
				if (iArgCount > 5)
				{
					sq->getinteger(v, 6, &weaponId);
					flag |= 8;
					if (iArgCount > 6)
					{
						sq->getinteger(v, 7, &classId);
						flag |= 16;
						if (iArgCount > 7)
						{
							sq->getstring(v, 8, &szScript);
							flag |= 32;
							if (iArgCount > 8)
							{
								sq->getbool(v, 9, &bConsoleInput);
								flag |= 64;
								if(iArgCount > 9)
								{
									sq->getstring(v, 10, &host);
									flag |= 128;
									if (iArgCount > 10)
									{
										sq->getstring(v, 11, &szPlugins);
										flag |= 256;
										if (iArgCount > 11)
										{
											for (int i = 12; i <= iArgCount; i++)
											{
												if (sq->gettype(v, i) != OT_STRING)
												{
													printf("Error: Paramter %d is not string\n", i - 1);//1 for root table
													sq->throwerror(v, "");//throwerror is not displaying anything
													return 0;
												}
												const SQChar* szParam;
												sq->getstring(v, i, &szParam);
												params.push_back(szParam);
											}
											flag |= 512;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	char szArgs[512]; 
	if (!(flag & 256))
		szPlugins = "";
	if (!(flag & 32))
		szScript = "";
	if (!(flag & 64))
		bConsoleInput = 0;
	if (!(flag & 128)||strlen(host)==0)
		host = "127.0.0.1";
	if(flag & 16)
		sprintf(szArgs, "x%f y%f z%f a%f s%d w%d c%d", pos.X, pos.Y, pos.Z, angle, (int)skinId, (int)weaponId, (int)classId);
	else if (flag & 8)
		sprintf(szArgs, "x%f y%f z%f a%f s%d w%d", pos.X, pos.Y, pos.Z, angle, (int)skinId, (int)weaponId);
	else if (flag & 4)
		sprintf(szArgs, "x%f y%f z%f a%f s%d", pos.X, pos.Y, pos.Z, angle, (int)skinId);
	else if(flag&2)
		sprintf(szArgs, "x%f y%f z%f a%f", pos.X, pos.Y, pos.Z, angle);
	else 
		sprintf(szArgs, "x%f y%f z%f", pos.X, pos.Y, pos.Z);
	if(flag & 512)
		CallNPCClient(szName, szScript, bConsoleInput, host, szPlugins, "",szArgs, params);
	else 
		CallNPCClient(szName, szScript, bConsoleInput, host, szPlugins,"", szArgs);
	
	sq->pushbool(v, SQTrue);
	return 1;
}
bool IsPlayerNPC(uint8_t byteplayerId)
{
	char UID[255];
	VCMP->GetPlayerUID(byteplayerId, UID, sizeof(UID));
	
	if (strcmp(UID, "0000000000000000000000000000000000000001") == 0)
	{
		return true;
	}
	else return false;
}
_SQUIRRELDEF(SQ_IsPlayerNPC) {//using UIDs
	
	SQInteger playerid;
	sq->getinteger(v, 2, &playerid);
	if (VCMP->IsPlayerConnected((int32_t)playerid))
	{
		if (IsPlayerNPC((uint8_t)playerid))
		{
			sq->pushbool(v, SQTrue);
		}
		else sq->pushbool(v, SQFalse);
		return 1;
	}
	else return sq->throwerror(v, "Player not connected");
}
_SQUIRRELDEF(SQ_StartRecordingPlayerData) {
	//StartRecordingPlayerData(playerid, type, name);
	SQInteger playerid;
	sq->getinteger(v, 2, &playerid);
	SQInteger type;
	if (sq->gettop(v) >= 3)
	{
		if (sq->gettype(v, 3) == OT_INTEGER)
		{
			sq->getinteger(v, 3, &type);
		}
		else return sq->throwerror(v, "The recname parameter must be string");
	}
	else type = PLAYER_RECORDING_TYPE_ALL;
	
	const SQChar* recname;
	if (sq->gettop(v) >= 4 )
	{
		if (sq->gettype(v, 4) == OT_STRING)
		{
			sq->getstring(v, 4, &recname);
		}
		else return sq->throwerror(v, "The recname parameter must be string");
	}
	else recname = "";
	std::string recordname = std::string(recname);
	SQInteger flags = 60;
	if (sq->gettop(v) >= 5)
	{
		if (sq->gettype(v, 5) == OT_INTEGER)
		{
			sq->getinteger(v, 5, &flags);
		}
		else return sq->throwerror(v, "The flags parameter must be integer");
	}
	if (VCMP->IsPlayerConnected((int32_t)playerid))
	{
		bool success = StartRecordingPlayerData((int32_t)playerid, (uint8_t)type, recordname, flags);
		sq->pushbool(v, success);
	}
	else sq->pushbool(v, SQFalse);
	return 1;//1 because we are returning value
}
_SQUIRRELDEF(SQ_StartRecordingAllPlayerData) {
	//StartRecordingAllPlayerData(type, flags);
	SQInteger type;
	if (sq->gettop(v) >= 2)
	{
		if (sq->gettype(v, 2) == OT_INTEGER)
		{
			sq->getinteger(v, 2, &type);
		}
		else return sq->throwerror(v, "The rectype parameter must be integer");
	}
	else type = PLAYER_RECORDING_TYPE_ALL;

	
	SQInteger flags = 60;
	if (sq->gettop(v) >= 3)
	{
		if (sq->gettype(v, 3) == OT_INTEGER)
		{
			sq->getinteger(v, 3, &flags);
		}
		else return sq->throwerror(v, "The flags parameter must be integer");
	}
	SQBool bNewPlayers = SQFalse;
	if (sq->gettop(v) >= 4)
	{
		if ((sq->gettype(v, 4) == OT_BOOL)||(sq->gettype(v,4)==OT_INTEGER))
		{
			HSQOBJECT obj;
			sq->getstackobj(v, 4, &obj);
			bNewPlayers=sq->objtobool(&obj);
		}
		else return sq->throwerror(v, "The newplayers parameter must be boolean");
	}
	uint8_t n = StartRecordingAllPlayerData( (uint8_t)type, flags, (bool)bNewPlayers);
	sq->pushinteger(v, n);
	return 1;//1 because we are returning value
}
SQInteger SQ_PutServerInRecordingMode(HSQUIRRELVM v)
{
	const SQChar* szFilename;
	sq->getstring(v, 2, &szFilename);
	
	SQInteger type;
	if (sq->gettop(v) >= 3)
	{
		if (sq->gettype(v, 3) == OT_INTEGER)
		{
			sq->getinteger(v, 3, &type);
		}
		else return sq->throwerror(v, "The rectype parameter must be integer");
	}
	else type = PLAYER_RECORDING_TYPE_ALL;


	SQInteger flags = REC_ALLNAME;
	if (sq->gettop(v) >= 4)
	{
		if (sq->gettype(v, 4) == OT_INTEGER)
		{
			sq->getinteger(v, 4, &flags);
		}
		else return sq->throwerror(v, "The flags parameter must be integer");
	}
	
	if (pServerRecording->IsRecording())
	{
		sq->pushbool(v, SQFalse); return 1;
	}
	bool success = PutServerInRecordingMode(type, flags, szFilename);
	sq->pushbool(v, success ? SQTrue : SQFalse); return 1;
}
SQInteger SQ_StopServerInRecordingMode(HSQUIRRELVM v)
{
	bool s=StopServerInRecordingMode();
	sq->pushbool(v, s==true?SQTrue:SQFalse);
	return 1;
}
SQInteger SQ_IsServerInRecordingMode(HSQUIRRELVM v)
{
	sq->pushbool(v, pServerRecording->IsRecording() ? SQTrue : SQFalse);
	return 1;
}
SQInteger NPC04_RegisterSquirrelFunc(HSQUIRRELVM v, SQFUNCTION f, const SQChar* fname, char ucParams, const SQChar* szParams, bool bAvoidParamsCheck) {
	char szNewParams[32];

	sq->pushroottable(v);
	sq->pushstring(v, fname, -1);
	sq->newclosure(v, f, 0); /* create a new function */
	
	if (ucParams != 0) {
		if (ucParams < 0)ucParams--;
		else ucParams++; /* This is to compensate for the root table */
		sprintf(szNewParams, "t%s", szParams);
		sq->setparamscheck(v, ucParams, szNewParams); /* Add a param type check */
	}
	else
	{
		if(!bAvoidParamsCheck)
			sq->setparamscheck(v, 1, "t"); /* Root table only */
		else
			sq->setparamscheck(v, -1, "t");
	}
	sq->setnativeclosurename(v, -1, fname);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 1);
	return 0;
}
_SQUIRRELDEF(SQ_StopRecordingPlayerData) {
	SQInteger playerid;
	sq->getinteger(v, 2, &playerid);
	if (VCMP->IsPlayerConnected((int32_t)playerid))
	{
		bool success = StopRecordingPlayerData((int32_t)playerid);
		sq->pushbool(v, success);
	}else sq->pushbool(v, SQFalse);
	return 1;
}
_SQUIRRELDEF(SQ_StopRecordingAllPlayerData) {
	uint8_t n = StopRecordingAllPlayerData();
	sq->pushinteger(v, n);
	return 1;
}
_SQUIRRELDEF(SQ_SetMaxPlayersOut) {
	if (npchideAvailable)
	{
		SQInteger maxplayers; 
		sq->getinteger(v, 2, &maxplayers);
		if (npchideFuncs)
		{
			if (npchideFuncs->ShowMaxPlayersAs((uint16_t)maxplayers))
				sq->pushbool(v, SQTrue);
			else
				sq->pushbool(v, SQFalse);
			return 1;
		}
	}
	return sq->throwerror(v, "NPC_Hide Module not loaded.");
}
SQInteger SQ_SetNameOfAllNPC(HSQUIRRELVM v)
{
	if (npchideAvailable)
	{
		const SQChar* name;
		sq->getstring(v, 2, &name);
		if (npchideFuncs)
		{
			if (npchideFuncs->SetAllNPCNames(name, (uint8_t)strlen(name)))
				sq->pushbool(v, SQTrue);
			else
				sq->pushbool(v, SQFalse);
			return 1;
		}
	}
	return sq->throwerror(v, "NPC_Hide Module not loaded.");
}
#ifdef WIN32
SQInteger SQ_ShowWindow(HSQUIRRELVM v)
{
	SW_STATUS = SW_SHOW;
	return 0;
}
SQInteger SQ_HideWindow(HSQUIRRELVM v)
{
	SW_STATUS = SW_HIDE;
	return 0;
}
#endif
SQInteger IsNPC(HSQUIRRELVM v)
{
	//class instance at top
	sq->pushstring(v, "ID", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		//ID at top
		SQInteger dw_playerId;
		sq->getinteger(v, -1, &dw_playerId);
		if (IsPlayerNPC((uint8_t)dw_playerId))
		{
			sq->pushbool(v, SQTrue);
		}
		else
			sq->pushbool(v, SQFalse);
		return 1;
	}
	return sq->throwerror(v, "Error getting ID of player instance");
}
SQInteger SQ_KickAllNPC(HSQUIRRELVM v)
{
	uint8_t n = 0;
	for (uint8_t i = 0; i < VCMP->GetMaxPlayers(); i++)
	{
		if (VCMP->IsPlayerConnected(i) && IsPlayerNPC(i))
		{
			VCMP->KickPlayer(i); n++;
		}
	}
	sq->pushinteger(v, n);
	return 1;
}
SQInteger SQ_IsPlayerRecording(HSQUIRRELVM v)
{
	SQInteger bytePlayerId;
	if (SQ_SUCCEEDED(sq->getinteger(v, 2, &bytePlayerId)))
	{
		if (VCMP->IsPlayerConnected(bytePlayerId))
		{
			bool result=IsPlayerRecording(bytePlayerId);
			if (result)sq->pushbool(v, SQTrue);
			else sq->pushbool(v, SQFalse);
			return 1;
		}
		sq->pushbool(v, SQFalse);
		return 1;
	}
	else return sq->throwerror(v, "Error occurred in getting playerid");
}
void AddPropertyToCPlayer(HSQUIRRELVM v)
{
	sq->pushroottable(v);
	sq->pushstring(v, "CPlayer", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		//CPlayer class at top
		sq->pushstring(v, "__getTable", -1);
		if (SQ_SUCCEEDED(sq->get(v, -2)))
		{
			//__getTable on top
			sq->pushstring(v, "IsNPC", -1);
			sq->newclosure(v, IsNPC, 0);
			sq->newslot(v, -3, SQFalse);
			sq->poptop(v);//pops __getTable
		}
		sq->poptop(v);//pops class CPlayer
	}
	sq->poptop(v);//pops roottable
}
SQInteger SQ_ConnectMultipleNpcs(HSQUIRRELVM v)
{
	if (pPlaybackMultiple->running)return sq->throwerror(v, "Error. Connecting MultipleNpcs already running and not finished");
	const SQChar* filename, * host = "127.0.0.1", * execArg = ""; SQInteger flags = HREC_PLAY_FLAG_STANDARD;
	sq->getstring(v, 2, &filename);
	if (sq->gettop(v) >= 3)
	{
		if (!(sq->gettype(v, 3) == OT_STRING))
			return sq->throwerror(v, "Error. hostname if provided must be string");
		sq->getstring(v, 3, &host);

		if (sq->gettop(v) >= 4)
		{
			if (!(sq->gettype(v, 4) == OT_INTEGER))
				return sq->throwerror(v, "Error. flags if provided must be integer");
			sq->getinteger(v, 4, &flags);

			if (sq->gettop(v) >= 5)
			{
				if (!(sq->gettype(v, 5) == OT_STRING))
					return sq->throwerror(v, "Error. execArg if provided must be string");
				sq->getstring(v, 5, &execArg);
		
			}
		}
	}
	uint8_t s = ConnectMultipleNpcs(std::string(filename), std::string(host),(uint32_t)flags,std::string(execArg));
	if (s == HREC_INITIALIZE_SUCCESS)
	{
		sq->pushbool(v, SQTrue); return 1;
	}
	else
		return sq->throwerror(v, (std::string("Error processing hrec. Code: " )+ std::to_string(s)).c_str());
}
SQInteger SQ_StopConnectingMultipleNpc(HSQUIRRELVM v)
{
	if (pPlaybackMultiple->running)
	{
		pPlaybackMultiple->Abort();
		sq->pushbool(v, SQTrue);
	}
	else sq->pushbool(v, SQFalse);
	return 1;
}

void NPC04_RegisterFuncs(HSQUIRRELVM v) {
	NPC04_RegisterSquirrelFunc(v, SQ_ConnectNPC, "ConnectNPC", -1, "ssbss");
	NPC04_RegisterSquirrelFunc(v, SQ_ConnectNPCEx, "ConnectNPCEx", -2, "sxf|iiiisbss");
	NPC04_RegisterSquirrelFunc(v, SQ_IsPlayerNPC, "IsPlayerNPC", 1, "i");
	NPC04_RegisterSquirrelFunc(v, SQ_StartRecordingPlayerData, "StartRecordingPlayerData", -1, "i");
	NPC04_RegisterSquirrelFunc(v, SQ_StopRecordingPlayerData, "StopRecordingPlayerData", 1, "i");
//Need NPC_Hide Module. Otherwise throws error safely.
	NPC04_RegisterSquirrelFunc(v, SQ_SetMaxPlayersOut, "SetMaxPlayersOut", 1, "i");
	NPC04_RegisterSquirrelFunc(v, SQ_SetNameOfAllNPC, "SetNameOfAllNPC", 1, "s");

#ifdef WIN32
	NPC04_RegisterSquirrelFunc(v, SQ_ShowWindow, "ShowNPCConsole", 0, "");
	NPC04_RegisterSquirrelFunc(v, SQ_HideWindow, "HideNPCConsole", 0, "");
#endif


	NPC04_RegisterSquirrelFunc(v, SQ_CreateFunction, "F", 1, "s|u");
	NPC04_RegisterSquirrelFunc(v, SQ_CreateFunction2, "Fa", 1, "s|u");
	NPC04_RegisterSquirrelFunc(v, SQ_CreateFunction, "RFC", 2, "is|u");
	NPC04_RegisterSquirrelFunc(v, SQ_CreateFunction2, "RFCa", 2, "is|u");
	AddPropertyToCPlayer(v);

	NPC04_RegisterSquirrelFunc(v, SQ_KickAllNPC, "KickAllNPC", 0, "");
	NPC04_RegisterSquirrelFunc(v, SQ_StartRecordingAllPlayerData, "StartRecordingAllPlayerData", 0, "",true);
	NPC04_RegisterSquirrelFunc(v, SQ_StopRecordingAllPlayerData, "StopRecordingAllPlayerData", 0, "");
	NPC04_RegisterSquirrelFunc(v, SQ_IsPlayerRecording, "IsPlayerRecording", 1, "i");
	NPC04_RegisterSquirrelFunc(v, SQ_PutServerInRecordingMode, "PutServerInRecordingMode", -1, "s");
	NPC04_RegisterSquirrelFunc(v, SQ_StopServerInRecordingMode, "StopServerInRecordingMode", 0, "");
	NPC04_RegisterSquirrelFunc(v, SQ_ConnectMultipleNpcs, "ConnectMultipleNpcs", -1, "s");
	NPC04_RegisterSquirrelFunc(v, SQ_StopConnectingMultipleNpc, "StopConnectingMultipleNpcs", 0, "");
	NPC04_RegisterSquirrelFunc(v, SQ_IsServerInRecordingMode, "IsServerInRecordingMode", 0, "");
	



}