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
enum Version
{
	REL004 = 67000,
	REL006 = 67400
};
#include <stdlib.h>
#include <stdio.h>
#include "SQFuncs.h"
#include <cstdio>
#include <vector>
extern PluginFuncs* VCMP;
extern HSQAPI sq;
#ifdef WIN32
INT SW_STATUS = 0;
#endif
bool CallNPCClient(const char* szName, const char* szScript, bool bConsoleInputEnabled=false,
	const char* host="127.0.0.1", const char* loc = "", std::vector<const char*>params = {})
{
	CHAR szCmd[1024]; int iPort;
	ServerSettings s;
	VCMP->GetServerSettings(&s);
	iPort = s.port;
	if (strlen(szScript) > 0)
		sprintf(szCmd, "-h %s -p %d -n \"%s\" -m \"%s\"", host, iPort, szName, szScript);
	else
		sprintf(szCmd, "-h %s -p %d -n \"%s\"", host, iPort, szName);
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
	//MultiArg tclap
	for (size_t i = 0; i < params.size(); i++)
	{
		strcat(szCmd, " -w \"");
		strcat(szCmd, params[i]);
		strcat(szCmd, "\"");
	}
	if (bConsoleInputEnabled)
		strcat(szCmd, " -c");
	//printf("%s\n", szCmd);
#ifdef WIN32
	if(bConsoleInputEnabled)
		SW_STATUS = SW_SHOW;
	uint32_t ver=VCMP->GetServerVersion();
	if(ver==REL004)
		ShellExecute(0, "open", "npcclient_r004.exe", szCmd, NULL, SW_STATUS);
	else if(ver==REL006)
		ShellExecute(0, "open", "npcclient.exe", szCmd, NULL, SW_STATUS);
	else {
		printf("Error server version: %d. ", ver);
		printf("Using REL006 = %d\n", REL006);
		ShellExecute(0, "open", "npcclient.exe", szCmd, NULL, SW_STATUS);
	}
#else
	char szDir[MAX_PATH];
	getcwd(szDir, MAX_PATH);
	char szCmd2[256];
	char szBin[256];
	if (sizeof(long) == 4)
		sprintf(szBin, "npcclient%s", "32");
	else if (sizeof(long) == 8)
		sprintf(szBin, "npcclient%s", "64");

	sprintf(szCmd2, "%s/%s %s &",  szDir, szBin, szCmd);
	system(szCmd2);
#endif
	return 1;
}
_SQUIRRELDEF(SQ_ConnectNPC) {
    SQInteger iArgCount = sq->gettop(v);
	const SQChar* szName=NULL, *szScript=NULL, *host=NULL;
	SQBool bConsoleInput;
	sq->getstring(v, 2, &szName);
	if (iArgCount > 2)
	{
		sq->getstring(v, 3, &szScript);
		if (iArgCount > 3)
		{
			sq->getbool(v, 4, &bConsoleInput);
			if (iArgCount > 4)
				sq->getstring(v, 5, &host);
			else host = "127.0.0.1";
		}
		else
		{
			host = "127.0.0.1";
			bConsoleInput = 0;
		}
	}
	else
	{	//Script not specified
		szScript = "";//start without script
		host = "127.0.0.1";
		bConsoleInput = 0;
	}
	if (strlen(host) == 0)
		host = "127.0.0.1";
	CallNPCClient(szName, szScript, bConsoleInput, host);
	sq->pushbool(v, SQTrue);
	return 1;
}
_SQUIRRELDEF(SQ_ConnectNPCEx) {
	SQInteger iArgCount = sq->gettop(v);
	const SQChar* szName = NULL, * szScript = NULL, * host = NULL;
	sq->getstring(v, 2, &szName);
	float x, y, z, angle; SQInteger skinId, weaponId, classId;
	SQBool bConsoleInput;
	uint16_t flag = 0;
	const SQChar* szLoc = NULL;
	std::vector<const char*> params = {};
	if (iArgCount > 4) //means pos (x,y,z) specified
	{
		sq->getfloat(v, 3, &x);
		sq->getfloat(v, 4, &y);
		sq->getfloat(v, 5, &z);
		flag |= 1;
		if (iArgCount > 5)
		{
			sq->getfloat(v, 6, &angle);
			flag |= 2;
			if (iArgCount > 6)
			{
				sq->getinteger(v, 7, &skinId);
				flag |= 4;
				if (iArgCount > 7)
				{
					sq->getinteger(v, 8, &weaponId);
					flag |= 8;
					if (iArgCount > 8)
					{
						sq->getinteger(v, 9, &classId);
						flag |= 16;
						if (iArgCount > 9)
						{
							sq->getstring(v, 10, &szScript);
							flag |= 32;
							if (iArgCount > 10)
							{
								sq->getbool(v, 11, &bConsoleInput);
								flag |= 64;
								if(iArgCount > 11)
								{
									sq->getstring(v, 12, &host);
									flag |= 128;
									if (iArgCount > 12)
									{
										for (int i = 13; i <= iArgCount; i++)
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
										flag |= 256;
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
	if (!(flag & 32))
		szScript = "";
	if (!(flag & 64))
		bConsoleInput = 0;
	if (!(flag & 128)||strlen(host)==0)
		host = "127.0.0.1";
	if(flag & 16)
		sprintf(szArgs, "x%f y%f z%f a%f s%d w%d c%d", x, y, z, angle, (int)skinId, (int)weaponId, (int)classId);
	else if (flag & 8)
		sprintf(szArgs, "x%f y%f z%f a%f s%d w%d", x, y, z, angle, (int)skinId, (int)weaponId);
	else if (flag & 4)
		sprintf(szArgs, "x%f y%f z%f a%f s%d", x, y, z, angle, (int)skinId);
	else if(flag&2)
		sprintf(szArgs, "x%f y%f z%f a%f", x, y, z, angle);
	else 
		sprintf(szArgs, "x%f y%f z%f", x, y, z);
	if(flag & 256)
		CallNPCClient(szName, szScript, bConsoleInput, host, szArgs, params);
	else 
		CallNPCClient(szName, szScript, bConsoleInput, host, szArgs);
	
	sq->pushbool(v, SQTrue);
	return 1;
}
_SQUIRRELDEF(SQ_IsPlayerNPC) {//using UIDs
	char UID[255];
	SQInteger playerid;
	sq->getinteger(v, 2, &playerid);
	VCMP->GetPlayerUID((int32_t)playerid,UID,sizeof(UID));
	if (strcmp(UID, "0000000000000000000000000000000000000001") == 0)
	{
		sq->pushbool(v, SQTrue);
	}
	else sq->pushbool(v, SQFalse);
	return 1;
}
_SQUIRRELDEF(SQ_StartRecordingPlayerData) {
	//StartRecordingPlayerData(playerid, type, name);
	SQInteger playerid;
	sq->getinteger(v, 2, &playerid);
	SQInteger type;
	sq->getinteger(v, 3, &type);
	const SQChar* recname;
	sq->getstring(v, 4, &recname);
	std::string recordname = std::string(recname);
	if (VCMP->IsPlayerConnected((int32_t)playerid))
	{
		bool success = StartRecordingPlayerData((int32_t)playerid, (uint8_t)type, recordname);
		sq->pushbool(v, success);
	}
	else sq->pushbool(v, SQFalse);
	return 1;//1 because we are returning value
}
SQInteger RegisterSquirrelFunc(HSQUIRRELVM v, SQFUNCTION f, const SQChar* fname, char ucParams, const SQChar* szParams) {
	char szNewParams[32];

	sq->pushroottable(v);
	sq->pushstring(v, fname, -1);
	sq->newclosure(v, f, 0); /* create a new function */

	if (ucParams != 0) {
		if (ucParams < 0)ucParams--;
		else ucParams++; /* This is to compensate for the root table */
		sprintf(szNewParams, "t%s", szParams);
		sq->setparamscheck(v, ucParams, szNewParams); /* Add a param type check */
	}else 
		sq->setparamscheck(v, 1, "t"); /* Root table only */

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
void RegisterFuncs(HSQUIRRELVM v) {
	RegisterSquirrelFunc(v, SQ_ConnectNPC, "ConnectNPC", -1, "ssbs");
	RegisterSquirrelFunc(v, SQ_ConnectNPCEx, "ConnectNPCEx", -4, "sf|if|if|if|iiiisbs");
	RegisterSquirrelFunc(v, SQ_IsPlayerNPC, "IsPlayerNPC", 1, "i");
	RegisterSquirrelFunc(v, SQ_StartRecordingPlayerData, "StartRecordingPlayerData", 3, "iis");
	RegisterSquirrelFunc(v, SQ_StopRecordingPlayerData, "StopRecordingPlayerData", 1, "i");
#ifdef WIN32
	RegisterSquirrelFunc(v, SQ_ShowWindow, "ShowNPCConsole", 0, "");
	RegisterSquirrelFunc(v, SQ_HideWindow, "HideNPCConsole", 0, "");
#endif
}