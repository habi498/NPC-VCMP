#include "SQConsts.h"
#include <cstdio>


extern HSQAPI sq;

SQInteger NPC04_RegisterSquirrelConst(HSQUIRRELVM v, const SQChar* cname, SQInteger cvalue) {
	sq->pushconsttable(v);
	sq->pushstring(v, cname, -1);
	sq->pushinteger(v, cvalue);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 1);

	return 0;
}

void NPC04_RegisterConsts(HSQUIRRELVM v) {
	NPC04_RegisterSquirrelConst(v, "PLAYER_RECORDING_TYPE_ONFOOT",1);
	NPC04_RegisterSquirrelConst(v, "PLAYER_RECORDING_TYPE_DRIVER",2);
}