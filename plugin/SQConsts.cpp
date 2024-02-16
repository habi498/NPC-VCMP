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
	NPC04_RegisterSquirrelConst(v, "PLAYER_RECORDING_TYPE_ALL",3);
	NPC04_RegisterSquirrelConst(v, "REC_NAME", REC_NAME);
	NPC04_RegisterSquirrelConst(v, "REC_SPAWN", REC_SPAWN);
	NPC04_RegisterSquirrelConst(v, "REC_ONFOOT_NORMAL", REC_ONFOOT_NORMAL);
	NPC04_RegisterSquirrelConst(v, "REC_ONFOOT_AIM", REC_ONFOOT_AIM);
	NPC04_RegisterSquirrelConst(v, "REC_VEHICLE_DRIVER", REC_VEHICLE_DRIVER);
	NPC04_RegisterSquirrelConst(v, "REC_VEHICLE_PASSENGER", REC_VEHICLE_PASSENGER);
	NPC04_RegisterSquirrelConst(v, "REC_DEATH", REC_DEATH);
	NPC04_RegisterSquirrelConst(v, "REC_OBJECT_SHOT", REC_OBJECT_SHOT);
	NPC04_RegisterSquirrelConst(v, "REC_OBJECT_TOUCH", REC_OBJECT_TOUCH);
	NPC04_RegisterSquirrelConst(v, "REC_PICKUP_PICKED", REC_PICKUP_PICKED);
	NPC04_RegisterSquirrelConst(v, "REC_CHECKPOINT_ENTERED", REC_CHECKPOINT_ENTERED);
	NPC04_RegisterSquirrelConst(v, "REC_CHECKPOINT_EXITED", REC_CHECKPOINT_EXITED);
	NPC04_RegisterSquirrelConst(v, "REC_PLAYER_COMMAND", REC_PLAYER_COMMAND);
	NPC04_RegisterSquirrelConst(v, "REC_PLAYER_CHAT", REC_PLAYER_CHAT);
	NPC04_RegisterSquirrelConst(v, "REC_PLAYER_PM", REC_PLAYER_PM);
	NPC04_RegisterSquirrelConst(v, "REC_CLIENTSCRIPT_DATA", REC_CLIENTSCRIPT_DATA);
	NPC04_RegisterSquirrelConst(v, "REC_DISCONNETION", REC_DISCONNETION);
	
	NPC04_RegisterSquirrelConst(v, "REC_STANDARD", REC_STANDARD);
	NPC04_RegisterSquirrelConst(v, "REC_AFTERLIFE", REC_AFTERLIFE);
	NPC04_RegisterSquirrelConst(v, "REC_SPYMODE", REC_SPYMODE);
	NPC04_RegisterSquirrelConst(v, "REC_RACEMODE", REC_RACEMODE);
	NPC04_RegisterSquirrelConst(v, "REC_RAMPAGE", REC_RAMPAGE);
	NPC04_RegisterSquirrelConst(v, "REC_RIFLERANGE", REC_RIFLERANGE);
	NPC04_RegisterSquirrelConst(v, "REC_EVERYTHING", REC_EVERYTHING);
	NPC04_RegisterSquirrelConst(v, "REC_ALLNAME", REC_ALLNAME);
	

	NPC04_RegisterSquirrelConst(v, "HF_NONE", HF_NONE);
	NPC04_RegisterSquirrelConst(v, "HF_AUTOMATIC_PLAY", HF_AUTOMATIC_PLAY);
	NPC04_RegisterSquirrelConst(v, "HF_DISCONNECT_AT_END", HF_DISCONNECT_AT_END);
	NPC04_RegisterSquirrelConst(v, "HF_SHOW_CONSOLE", HF_SHOW_CONSOLE);

}