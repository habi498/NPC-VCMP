// npc-plugin.cpp : Module to check attacks from player and reduce health and inform server.
//
#pragma warning( disable : 4103)
#include "main.h"
#include "stdio.h"

#ifdef WIN32
#else
long GetTickCount()
{
	struct timespec ts;
	long theTick = 0U;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	theTick = ts.tv_nsec / 1000000;
	theTick += ts.tv_sec * 1000;
	return theTick;
}
#endif

#define PI 3.1415926
PluginFuncs* funcs;
HSQAPI sq;
HSQUIRRELVM v;


void NPC::ClearTarget(REASON reason)
{
	VECTOR pos; uint8_t npcid = funcs->GetNPCId();
	funcs->GetPlayerPosition(npcid, &pos);
	float angle;
	uint8_t health, armour, curwep; WORD ammo;
	VECTOR vecSpeed, vecAimPos, vecAimDir;
	bool IsCrouching;
	funcs->GetAngle(&angle);
	health = funcs->GetPlayerHealth(npcid);
	armour = funcs->GetPlayerArmour(npcid);
	curwep = funcs->GetPlayerWeapon(npcid);
	uint8_t slotId = GetSlotIdFromWeaponId(curwep);
	if (slotId != 255)
		ammo = funcs->GetPlayerAmmoAtSlot(npcid, slotId);
	else
		ammo = 0;
	funcs->GetPlayerSpeed(npcid, &vecSpeed);
	funcs->GetPlayerAimPos(npcid, &vecAimPos);
	funcs->GetPlayerAimDir(npcid, &vecAimDir);
	IsCrouching = funcs->IsPlayerCrouching(npcid);
	funcs->SendOnFootSyncDataEx(0, pos, angle, health, armour, curwep,
		ammo, vecSpeed, vecAimPos, vecAimDir, IsCrouching, false);
	int top = sq->gettop(v);
	sq->pushroottable(v);
	sq->pushstring(v, "OnTargetCleared", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		sq->pushroottable(v);
		sq->pushinteger(v, m_byteTargetId);
		sq->pushinteger(v, static_cast<uint8_t>(reason));
		sq->call(v, 3, 0, 1);
	}
	m_bHasTarget = false;
	m_byteTargetId = 255;
	m_dwLastFired = 0;
	sq->settop(v, top);
}
NPC npc;
extern WeaponData wepdata[34];
void fn_CheckWeaponChange(uint8_t newweapon)
{
	if (funcs->GetCurrentWeapon() != newweapon)
	{
		int top = sq->gettop(v);
		sq->pushroottable(v);
		sq->pushstring(v, "OnWeaponChange", -1);
		if (SQ_SUCCEEDED(sq->get(v, -2)))
		{
			sq->pushroottable(v);
			sq->pushinteger(v, funcs->GetCurrentWeapon());
			sq->pushinteger(v, newweapon);
			sq->call(v, 3, 0, 1);
		}
		sq->settop(v, top);
	}
}
bool fn_CallOnPullingTrigger(ONFOOT_SYNC_DATA* pOfSyncData)
{
	int top = sq->gettop(v); bool returnvalue = true;
	sq->pushroottable(v);
	sq->pushstring(v, "OnPullTrigger", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		sq->pushroottable(v);
		sq->pushinteger(v, pOfSyncData->dwKeys);
		sq->pushvector(v, pOfSyncData->vecPos);
		sq->pushfloat(v, pOfSyncData->fAngle);
		sq->pushinteger(v, pOfSyncData->byteHealth);
		sq->pushinteger(v, pOfSyncData->byteArmour);
		sq->pushinteger(v, pOfSyncData->byteCurrentWeapon);
		sq->pushinteger(v, pOfSyncData->wAmmo);
		sq->pushvector(v, pOfSyncData->vecSpeed);
		sq->pushvector(v, pOfSyncData->vecAimPos);
		sq->pushvector(v, pOfSyncData->vecAimDir);
		if (pOfSyncData->IsCrouching)
			sq->pushbool(v, SQTrue);
		else
			sq->pushbool(v, SQFalse);

		if (pOfSyncData->bIsReloading)
			sq->pushbool(v, SQTrue);
		else
			sq->pushbool(v, SQFalse);
		//printf("action calculated is %d\n", pOfSyncData->byteAction);
		//sq->pushinteger(v, pOfSyncData->byteAction);
		sq->call(v, 13, 1, 1);
		if (sq->gettype(v, -1) == OT_BOOL)
		{
			SQBool retval;
			sq->getbool(v, -1, &retval);
			if (retval == SQFalse)
				returnvalue= false;
		}
		else if (sq->gettype(v, -1) == OT_INTEGER)
		{
			SQInteger retval;
			sq->getinteger(v, -1, &retval);
			if (retval == 0)
				returnvalue= false;
		}
		else if (sq->gettype(v, -1) == OT_NULL)
		{
			returnvalue= false;
		}
	}
	sq->settop(v, top);
	return returnvalue;
}
void OnCycle()
{
	if (npc.m_bHasTarget)
	{
		DWORD now = GetTickCount();
		if (now < npc.m_dwNextCycle) {
			 return;
		}
		uint8_t weapon = SelectGun();
		if (weapon)//which will check valid weapon and ammo
		{
			VECTOR tPos;
			playerState s = funcs->GetPlayerState(npc.m_byteTargetId);
			if (s == playerState::Wasted)
			{
				npc.ClearTarget(REASON::PLAYER_WASTED);
				return;
			}
			funcError e=funcs->GetPlayerPosition(npc.m_byteTargetId, &tPos);
			if (e != funcError::NoError)return;
			VECTOR npcPos;
			funcs->GetPlayerPosition(funcs->GetNPCId(), &npcPos);
			VECTOR dis = npcPos - tPos;
			if (weapon > 33)return;
			
			if (dis.GetMagnitude() > wepdata[weapon].fRange )
			{
				if(npc.m_bTargetOutOfRange == false)
				{
					int top = sq->gettop(v);
					sq->pushroottable(v);
					sq->pushstring(v, "OnTargetOutOfRange", -1);
					if (SQ_SUCCEEDED(sq->get(v, -2)))
					{
						sq->pushroottable(v);
						sq->pushinteger(v, npc.m_byteTargetId);
						sq->call(v, 2, 0, 1);
					}
					sq->settop(v, top);
					npc.m_bTargetOutOfRange = true;
				}
				return;//player out of range;
			}
			npc.m_bTargetOutOfRange = false;
			
			//Now, shoot it
			
			uint8_t slotId = GetSlotIdFromWeaponId(weapon);
			WORD ammo;
			uint8_t npcid = funcs->GetNPCId();
			if (slotId != 255)
				ammo = funcs->GetPlayerAmmoAtSlot(npcid, slotId);
			else
				ammo = 0;
			float fAngle = (float)atan2(-(tPos.X - npcPos.X), tPos.Y - npcPos.Y);
			VECTOR vecAimPos = VECTOR(tPos.X, tPos.Y, tPos.Z);
			uint8_t t = funcs->GetPlayerAction(npc.m_byteTargetId);
			if (t == 42||t==43)//lying on ground/getting up, credits: vitovc
			{
				vecAimPos.Z -= 0.5;
			}
			VECTOR vecAimDir = VECTOR((float)PI, (float)PI, -fAngle);
			uint32_t keys = 512 | 64;
			ONFOOT_SYNC_DATA* OfSyncData;
			funcs->GetOnFootSyncData(&OfSyncData);
			
			OfSyncData->IsPlayerUpdateAiming = true;
			if (!npc.m_bIsReloadingWeapon)
			{
				if ((now-npc.m_dwLastFired)>=wepdata[weapon].wFiringrate)
				{
					fn_CheckWeaponChange(weapon);
					OfSyncData->vecAimPos = vecAimPos;
					OfSyncData->vecAimDir = vecAimDir;
					OfSyncData->fAngle = fAngle;
					funcs->SetAmmoAtSlot(slotId, --ammo);
					OfSyncData->wAmmo = ammo;
					OfSyncData->dwKeys = 512 | 64;
					OfSyncData->bIsReloading = false;
					OfSyncData->byteCurrentWeapon = weapon;
					if (fn_CallOnPullingTrigger(OfSyncData))
					{
						funcs->SendOnFootSyncDataEx2(*OfSyncData);
						funcs->FireBullet(weapon, 0,0,0);
					}
					npc.m_dwLastFired = now;
					npc.m_bIsFiringBullet = true;
					if ((ammo % wepdata[weapon].byteClipsize) == 0)
					{
						npc.m_bIsReloadingWeapon = true;
					}
				}
				else
				{
					//Send same packet again (ammo not decreased)
					OfSyncData->dwKeys = 512 | 64;
					OfSyncData->bIsReloading = false;
					funcs->SendOnFootSyncDataEx2(*OfSyncData);
				}

			}
			else
			{
				//npc is reloading weapon
				if ((now - npc.m_dwLastFired)
					>= ( wepdata[weapon].wReloadTimeMs))
				{
					fn_CheckWeaponChange(weapon);
					OfSyncData->vecAimPos = vecAimPos;
					OfSyncData->vecAimDir = vecAimDir;
					OfSyncData->fAngle = fAngle;
					//reloading complete
					npc.m_bIsReloadingWeapon = false;
					OfSyncData->dwKeys = 512 | 64;
					OfSyncData->byteCurrentWeapon = weapon;
					OfSyncData->bIsReloading = false;
					funcs->SetAmmoAtSlot(slotId, --ammo);//SelectGun ensures ammo >0
					OfSyncData->wAmmo = ammo;
														 //Decrease ammo first due to some reasons.
					if (fn_CallOnPullingTrigger(OfSyncData))
					{
						funcs->SendOnFootSyncDataEx2(*OfSyncData);
						funcs->FireBullet(weapon, 0,0,0);
					}
					npc.m_dwLastFired = now;
					if ((ammo % wepdata[weapon].byteClipsize) == 0)
					{
						npc.m_bIsReloadingWeapon = true;
					}
				}
				else
				{
					//reloading not complete. Send reloading action packet again
					OfSyncData->dwKeys = 64;
					OfSyncData->bIsReloading = true;
					funcs->SendOnFootSyncDataEx2(*OfSyncData);
				}
			}
			npc.m_dwNextCycle = now + 100;//Send packet in every 100 ms
		}
		else
		{
			npc.ClearTarget(REASON::AMMO_SHORTAGE);//ran out of weapon/ammo.
		}
	}
}


//TODO: Add support for custom weapons
bool IsProperWeapon(uint8_t weaponId)
{
	if (weaponId >= 17 && weaponId <= 27 || weaponId == 32 || weaponId == 33)
	{
		return true;
	}
	return false;
}
bool HasEnoughAmmo(uint8_t weaponId)
{
	int top = sq->gettop(v);
	sq->pushroottable(v);
	sq->pushstring(v, "GetSlotFromWeaponId", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		sq->pushroottable(v);
		sq->pushinteger(v, weaponId);
		if (SQ_SUCCEEDED(sq->call(v, 2, 1, 1)))
		{
			SQInteger SlotId;
			sq->getinteger(v, -1, &SlotId);
			WORD Ammo = funcs->GetPlayerAmmoAtSlot(funcs->GetNPCId(), SlotId);
			if (Ammo > 0)
			{
				sq->pop(v, 3);//roottable, function, ret value
				return true;
			}
		}
	}
	sq->settop(v, top);
	return false;
}
//TODO
bool IsTargetInRangeOfGun()
{
	return true;
}
uint8_t SelectGun()
{
	uint8_t weapon=funcs->GetPlayerWeapon(funcs->GetNPCId());
	if (IsProperWeapon(weapon) && HasEnoughAmmo(weapon))
		return weapon;
	uint8_t npcid = funcs->GetNPCId();
	for (uint8_t i = 0; i < 9; i++)
	{
		weapon = funcs->GetPlayerWeaponAtSlot(npcid, i);
		if (IsProperWeapon(weapon) && HasEnoughAmmo(weapon))
			return weapon;
	}
	return GUN_NOT_AVAILABLE;
}

uint8_t GetSlotIdFromWeaponId(uint8_t weaponId)
{
	int top = sq->gettop(v);
	sq->pushroottable(v);
	sq->pushstring(v, "GetSlotFromWeaponId", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		sq->pushroottable(v);
		sq->pushinteger(v, weaponId);
		if (SQ_SUCCEEDED(sq->call(v, 2, 1, 1)))
		{
			SQInteger SlotId;
			if (SQ_SUCCEEDED(sq->getinteger(v, -1, &SlotId)))
			{
				sq->settop(v, top);
				return SlotId;
			}
		}
	}
	sq->settop(v, top);
	return 255;
}
void OnPlayerStreamOut(uint8_t playerid)
{
	if (npc.m_bHasTarget && npc.m_byteTargetId == playerid)
	{
		npc.ClearTarget(REASON::PLAYER_STREAMEDOUT);
	}
}

unsigned int PluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo)
{
	funcs = pluginFuncs;
	strcpy(pluginInfo->name, "LibAction");
	pluginInfo->pluginVersion = 0x00000001;
	pluginCalls->OnCycle = OnCycle;
	pluginCalls->OnPlayerStreamOut = OnPlayerStreamOut;
	if (pluginInfo->apiMajorVersion != API_MAJOR ||
		pluginInfo->apiMinorVersion != API_MINOR)
	{
		if (pluginInfo->apiMajorVersion == 1 && 
			pluginInfo->apiMinorVersion == 0)
		{
			printf("	Error: LibAction requires API: 1.1\n");
			return 0;
		}
		printf("	Warning: LibAction is compiled for API: %d.%d. But the current API is %d.%d\n", API_MAJOR, API_MINOR
			, pluginInfo->apiMajorVersion, pluginInfo->apiMinorVersion);
	}
		
	size_t size;
	const void** sqExports = funcs->GetSquirrelExports(&size);
	if (sqExports != NULL && size > 0)
	{
		SquirrelImports** sqdf = (SquirrelImports**)sqExports;
		SquirrelImports* sqFuncs = (SquirrelImports*)(*sqdf);
		if (sqFuncs)
		{
			v = *(sqFuncs->GetSquirrelVM());
			sq = *(sqFuncs->GetSquirrelAPI());
			RegisterFunctions();
			RegisterSquirrelConst(v,"AMMO_SHORTAGE", static_cast<int>(REASON::AMMO_SHORTAGE));
			RegisterSquirrelConst(v, "PLAYER_WASTED", static_cast<int>(REASON::PLAYER_WASTED));
			RegisterSquirrelConst(v, "PLAYER_STREAMEDOUT", static_cast<int>(REASON::PLAYER_STREAMEDOUT));
			RegisterSquirrelConst(v, "INSTRUCTION_RECEIVED", static_cast<int>(REASON::INSTRUCTION_RECEIVED));
			LoadWeaponData();
			printf("	LibAction module loaded\n");
			return 1;
		}
	}
	return 0;
}
