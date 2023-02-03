//
// SQFuncs: These are the tools to register custom functions within the Squirrel VM.
//
//	Written for Liberty Unleashed by the Liberty Unleashed Team.
//

#pragma once
#ifndef _SQFUNCS_H
#define _SQFUNCS_H

#include "SQMain.h"
#include "squirrel.h"
#include <vector>

#define _SQUIRRELDEF(x) SQInteger x(HSQUIRRELVM v)

#ifdef __cplusplus
extern "C" {
#endif
	SQInteger				RegisterSquirrelFunc				(HSQUIRRELVM v, SQFUNCTION f, const SQChar* fname, char uiParams, const SQChar* szParams);
	void					RegisterFuncs						(HSQUIRRELVM v);
#ifdef __cplusplus
}
#endif
SQInteger SQ_CreateFunction(HSQUIRRELVM v);
bool CallNPCClient(const char* szName, const char* szScript, bool bConsoleInputEnabled ,
	const char* host , const char* plugins , const char* loc , std::vector<const char*>params );

bool IsPlayerNPC(uint8_t byteplayerId);
#define RPC_STREAM_IDENTIFIER 0x40000000
#endif
