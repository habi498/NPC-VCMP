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

extern PluginFuncs* VCMP;
extern HSQAPI sq;

_SQUIRRELDEF(SQ_ConnectNPC) {
    SQInteger iArgCount = sq->gettop(v);
	const SQChar* szName=NULL, *szScript=NULL, *host=NULL;
	if (iArgCount == 4) {
		sq->getstring(v, 2, &szName);
		sq->getstring(v, 3, &szScript);
		sq->getstring(v, 4, &host);
	}
	else if (iArgCount == 3)
	{
		sq->getstring(v, 2, &szName);
		sq->getstring(v, 3, &szScript);
		host = "127.0.0.1";
	}
	CHAR szCmd[256]; int iPort;
	ServerSettings s;
	VCMP->GetServerSettings(&s);
	iPort = s.port;
	sprintf(szCmd, "-h %s -p %d -n %s -m %s", host, iPort, szName, szScript);
	char szPassword[255];
	VCMP->GetServerPassword(szPassword, sizeof(szPassword));
	if (szPassword && szPassword[0] != '\0')
	{
		strcat(szCmd, " -z ");
		strcat(szCmd, szPassword);
	}
#ifdef WIN32
	ShellExecute(0, "open", "npcclient.exe", szCmd, NULL, SW_HIDE);
#else
	char szDir[MAX_PATH];
	getcwd(szDir, MAX_PATH);
	char szCmd2[256];
	char szBin[256];
	if (sizeof(long) == 4)
		sprintf(szBin, "npcclient%s", "32");
	else if(sizeof(long)==8)
		sprintf(szBin, "npcclient%s", "64");

	sprintf(szCmd2, "%s/%s %s &", szDir,szBin, szCmd);
	system(szCmd2);
#endif
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
void RegisterFuncs(HSQUIRRELVM v) {
	RegisterSquirrelFunc(v, SQ_ConnectNPC, "ConnectNPC", -2, "sss");
	RegisterSquirrelFunc(v, SQ_IsPlayerNPC, "IsPlayerNPC", 1, "i");
	RegisterSquirrelFunc(v, SQ_StartRecordingPlayerData, "StartRecordingPlayerData", 3, "iis");
	RegisterSquirrelFunc(v, SQ_StopRecordingPlayerData, "StopRecordingPlayerData", 1, "i");
	
}