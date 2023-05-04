#pragma once
#ifndef _SQCONSTS_H
#define _SQCONSTS_H

#include "SQMain.h"
#include "squirrel.h"

#ifdef __cplusplus
extern "C" {
#endif
    SQInteger               NPC04_RegisterSquirrelConst               (HSQUIRRELVM v, const SQChar* cname, SQInteger cvalue);
	void					NPC04_RegisterConsts						(HSQUIRRELVM v);

#ifdef __cplusplus
}
#endif

#endif