
#include "main.h"
extern HSQUIRRELVM v;
extern CPlayerPool* m_pPlayerPool;
extern NPC* iNPC;
extern CFunctions* m_pFunctions;
extern CPlayer* npc;
extern CPickupPool* m_pPickupPool;
extern CCheckpointPool* m_pCheckpointPool;
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
extern CVehiclePool* m_pVehiclePool;
extern uint32_t configvalue;
SQInteger fn_Suicide(HSQUIRRELVM v)
{
	if (m_pFunctions->Suicide() == funcError::NoError)
	{
		sq_pushbool(v, SQTrue);
	}
	sq_pushbool(v, SQFalse);
	return 1;
}
SQInteger fn_SetMyHealth(HSQUIRRELVM v)
{
	SQInteger health;
	if (SQ_SUCCEEDED(sq_getinteger(v, 2, &health)))
	{
		m_pFunctions->SetHealth((uint8_t)health, true);
		if (m_pFunctions->GetLastError() == funcError::NoError)
		{
			sq_pushbool(v, SQTrue);
			return 1;
		}
	}
	sq_pushbool(v, SQFalse);
	return 1;
}
SQInteger fn_GetMyHealth(HSQUIRRELVM v)
{
	uint8_t health = m_pFunctions->GetPlayerHealth(iNPC->GetID());
	sq_pushinteger(v, health);
	return 1;
}
SQInteger fn_SetConfig(HSQUIRRELVM v)
{
	SQInteger val;
	if (SQ_SUCCEEDED(sq_getinteger(v, 2, &val)))
	{
		m_pFunctions->SetConfig((uint32_t)val);
		sq_pushbool(v, SQTrue);
		return 1;
	}
	sq_pushbool(v, SQFalse);
	return 1;

}
SQInteger fn_LookAtPos(HSQUIRRELVM v)
{
	sq_pushbool(v, SQFalse);
	if (!npc)return 1; if (!iNPC||!iNPC->IsSpawned())return 1;
	if (npc->m_wVehicleId)return 1;
	VECTOR vecPos, myPos = npc->m_vecPos;
	if (SQ_SUCCEEDED(sq_getvector(v, 2, &vecPos)))
	{
		float myAngle = atan2(-(vecPos.X - myPos.X), vecPos.Y - myPos.Y);
		m_pFunctions->SetAngle(myAngle, true);
		sq_poptop(v); sq_pushbool(v, SQTrue);//pop false and push true
	}
	return 1;
}
SQInteger fn_LookAtPlayer(HSQUIRRELVM v)
{
	sq_pushbool(v, SQFalse);
	if (!npc)return 1; if (!iNPC || !iNPC->IsSpawned())return 1;
	if (npc->m_wVehicleId)return 1;
	SQInteger playerId;
	VECTOR vecPos, myPos = npc->m_vecPos;
	if (SQ_SUCCEEDED(sq_getinteger(v, 2, &playerId)))
	{
		if (m_pFunctions->IsPlayerConnected((uint8_t)playerId) && m_pFunctions->IsPlayerSpawned((uint8_t)playerId))
		{
			m_pFunctions->GetPlayerPosition(playerId, &vecPos);
			float myAngle = atan2(-(vecPos.X - myPos.X), vecPos.Y - myPos.Y);
			m_pFunctions->SetAngle(myAngle, true);
			sq_poptop(v); sq_pushbool(v, SQTrue);//pop false and push true
		}
	}
	return 1;
}
SQInteger fn_GetMyClass(HSQUIRRELVM v)
{
	if (iNPC && iNPC->Initialized())
	{
		sq_pushinteger(v, iNPC->PotentialClassID);
		return 1;
	}
	return 0;
}
SQInteger fn_RequestClassAbs(HSQUIRRELVM v)
{
	SQInteger classId;
	sq_getinteger(v, 2, &classId);
	if (classId >= 50 || classId < 0)return 0;// null will be returned
	if (iNPC )//&& !iNPC->IsSpawned())
	{
		iNPC->SetSpawnClass((uint8_t)classId);
		if (iNPC->IsSpawned() == false)
		{
			//Request class
			m_pFunctions->RequestClass(0);
			iNPC->AbsClassNotSpecified = false;
		}
		sq_pushbool(v, SQTrue);
		return 1;
	}
	return 0;
}
SQInteger fn_AmISpawned(HSQUIRRELVM v)
{
	if (!iNPC)return 0;
	if (iNPC->IsSpawned())
		sq_pushbool(v, SQTrue);
	else
		sq_pushbool(v, SQFalse);
	return 1;
}
SQInteger fn_GetMyArmour(HSQUIRRELVM v)
{
	uint8_t armour = m_pFunctions->GetPlayerArmour(iNPC->GetID());
	sq_pushinteger(v, armour);
	return 1;
}
SQInteger fn_GetMyTeam(HSQUIRRELVM v)
{
	uint8_t team = m_pFunctions->GetPlayerTeam(iNPC->GetID());
	sq_pushinteger(v, team);
	return 1;
}

SQInteger fn_GetMySkin(HSQUIRRELVM v)
{
	uint8_t team = m_pFunctions->GetPlayerSkin(iNPC->GetID());
	sq_pushinteger(v, team);
	return 1;
}
SQInteger fn_SendPrivMsg(HSQUIRRELVM v)
{
	SQInteger playerId; const SQChar* msg;
	sq_getinteger(v, 2, &playerId);
	sq_getstring(v, 3, &msg);
	m_pFunctions->SendPrivMessage(playerId, msg);
	if (m_pFunctions->GetLastError() == funcError::NoError)
	{
		sq_pushbool(v, SQTrue);
	}
	else sq_pushbool(v, SQFalse);
	return 1;
}
void RegisterNPCFunctions6()
{
	register_global_func(v, ::fn_Suicide, "Suicide", 1, "t");
	register_global_func(v, ::fn_SetMyHealth, "SetMyHealth", 2, "ti");
	register_global_func(v, ::fn_GetMyHealth, "GetMyHealth", 1, "t");
	register_global_func(v, ::fn_SetConfig, "SetConfig", 2, "ti");
	register_global_func(v, ::fn_LookAtPos, "LookAtPos", 2, "tx");
	register_global_func(v, ::fn_LookAtPlayer, "LookAtPlayer", 2, "ti");
	register_global_func(v, ::fn_GetMyClass, "GetMyClass", 1, "t");
	register_global_func(v, ::fn_RequestClassAbs, "RequestClassAbs", 2, "ti");
	register_global_func(v, ::fn_AmISpawned, "AmISpawned", 1, "t");
	register_global_func(v, ::fn_GetMySkin, "GetMySkin", 1, "t");
	register_global_func(v, ::fn_GetMyTeam, "GetMyTeam", 1, "t");
	register_global_func(v, ::fn_GetMyArmour, "GetMyArmour", 1, "t");
	register_global_func(v, ::fn_SendPrivMsg, "SendPrivMsg", 3, "tis");

}