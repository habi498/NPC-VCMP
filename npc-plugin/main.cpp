// npc-plugin.cpp : Module to check attacks from player and reduce health and inform server.
//
#pragma warning( disable : 4103)
#if WIN32
#include <windows.h>
#else
#include <time.h>
long getTick() {
	
}
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
#include "main.h"
#include "stdio.h"
#include <map>
#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif
#define PI 3.1415926
#define MAX_PLAYERS 100
#define SOS_CMD "iamunderattack"
bool bSOS_SEND=false;
uint8_t PlayerAttackingNPC = 255;
float fLastAttackFromAnyPlayer = 0;
PluginFuncs* funcs;
HSQUIRRELVM v;
HSQAPI sq;
//true- if val is in [a,b] or [b,a]
bool IsInInterval(double val, double a, double b)
{
	return (a < b&& val >= a && val <= b) ||
		(b < a&& val >= b && val <= a) || (a == b && val == b);
}
//an angle is in an intervalAB if its cos is within interval
//formed by cosA, cosB and its sin is within the interval formed 
//by sinA sinB. eg. consider angle=3, angleA=-3.1, angle B=2.8
bool IsAngleInArcAB(float angle, float angleA, float angleB)
{
	return IsInInterval((double)cos(angle), (double)cos(angleA), (double)cos(angleB)) &&
		IsInInterval((double)sin(angle), (double)sin(angleA), (double)sin(angleB));
}
uint8_t GetWeaponDamage(uint8_t weaponId)
{
	switch (weaponId)
	{
	case 0: return 8;
	case 1: return 16;
	case 2: return 45;
	case 3: return 21;
	case 4: return 21;
	case 5: return 21;
	case 6: return 21;
	case 7: return 21;
	case 8: return 24;
	case 9: return 24;
	case 10: return 30;
	case 11: return 35;
	case 12: return 75;
	case 13: return 75;
	case 14: return 75;
	case 15: return 75;
	case 16: return 75;
	case 17: return 25;
	case 18: return 135;
	case 19: return 80;
	case 20: return 100;
	case 21: return 120;
	case 22: return 20;
	case 23: return 20;
	case 24: return 15;
	case 25: return 35;
	case 26: return 40;
	case 27: return 35;
	case 28: return 125;
	case 29: return 125;
	case 30: return 75;
	case 31: return 25;
	case 32: return 130;
	case 33: return 140;
	default:return 8;//weapon damage of fists
	}
}
float LastAttackTime[MAX_PLAYERS];
float AttackBeganTime[MAX_PLAYERS];
void InitAttackEniviron()
{
	for (uint8_t i = 0; i < MAX_PLAYERS; i++)
		LastAttackTime[i] = 0;
	for (uint8_t i = 0; i < MAX_PLAYERS; i++)
		AttackBeganTime[i] = 0;
}
float timeNPCDead = 0;
uint8_t killerId=255; bodyPart bpt=bodyPart::Body; uint8_t killerWeapon=0;
void OnCycle()
{
	if (timeNPCDead != 0)
	{
		if (GetTickCount() / 1000.f - timeNPCDead > 2.0)
		{
			funcs->SendDeathInfo(killerWeapon, killerId, bpt);
			timeNPCDead = 0;
			killerId = 255; killerWeapon = 0; bpt = bodyPart::Body;
		}
	}
	if (bSOS_SEND)
	{
		if (fLastAttackFromAnyPlayer-GetTickCount()/1000.f > 120.0)//60 seconds
		{
			//fLastAttackFromAnyPlayer = 0;
			bSOS_SEND = false;
		}
	}
}
void OnPlayerUpdate(uint8_t bytePlayerId, vcmpPlayerUpdate updateType)
{
	uint8_t npcId = funcs->GetNPCId(); 
	if (funcs->GetLastError() == funcError::NPCNotConnected)
		return; 
	if(updateType==vcmpPlayerUpdateAiming)
	{
		//player is aiming
		//Which Weapon..?
		uint8_t wep = funcs->GetPlayerWeapon(bytePlayerId);
		if (wep >= 12 && wep <= 15)return;//Grenades,motolovs
		VECTOR vecPos;
		funcs->GetPlayerPosition(bytePlayerId, &vecPos);
		VECTOR npcPos;
		funcs->GetPosition(&npcPos.X, &npcPos.Y, &npcPos.Z);
		float dis = (vecPos - npcPos).GetMagnitude();
		float now = GetTickCount() / 1000.f;
		if (wep <= 11 && dis<1.5)
		{
			
			if (LastAttackTime[bytePlayerId] != 0)
			{
				if (now - LastAttackTime[bytePlayerId] < 0.63)
					return;
			}
			if (AttackBeganTime[bytePlayerId] == 0)
			{
				AttackBeganTime[bytePlayerId] = now;
				return;
			}
			if (now - AttackBeganTime[bytePlayerId] < 0.17)
			{
				return;
			}
			float currentAngle= funcs->GetPlayerAngle(bytePlayerId);
			float angleToNPC = (float)atan2(-(double)(npcPos.X - vecPos.X), (double)(npcPos.Y - vecPos.Y));
			if (IsAngleInArcAB(currentAngle, (float)(angleToNPC - PI / 6), (float)(angleToNPC + PI / 6)))
			{
				//player is almost facing npc
				//Reduce health of npc
				//Using int16_t with a specific reason!
				int16_t byteHealth = (int16_t)funcs->GetPlayerHealth(npcId);
				int16_t byteArmour = (int16_t)funcs->GetPlayerArmour(npcId);
				byteArmour -= GetWeaponDamage(wep);
				if (byteArmour < 0)
				{
					byteHealth += byteArmour;
					byteArmour = 0;
				}
				if (timeNPCDead != 0)return;
				if (byteHealth < 0)
				{
					//npc is dead
					funcs->SetHealth(0, true);
					funcs->SendShotInfo(bodyPart::Body, 0xd);
					killerId = bytePlayerId;
					killerWeapon = wep;
					bpt = bodyPart::Body;
					timeNPCDead = now;
				}
				else
				{
					funcs->SetArmour((uint8_t)byteArmour, false);
					funcs->SetHealth((uint8_t)byteHealth, true);
					LastAttackTime[bytePlayerId] = now;
					fLastAttackFromAnyPlayer = now;
					
					if (!bSOS_SEND|| PlayerAttackingNPC != bytePlayerId)
					{
						PlayerAttackingNPC = bytePlayerId;
						char buffer[255];
						sprintf(buffer, "%s %u", SOS_CMD, PlayerAttackingNPC);
						funcs->SendCommandToServer(buffer);
						if(!bSOS_SEND)
							bSOS_SEND = true;
					}
				}
			}
		}
		else if (wep >= 17 && wep <= 27 && dis <= 60)
		{
			bool bPlayerFirstPerson = (funcs->GetPlayerKeys(bytePlayerId) & 1) ? true : false;
			//if it is shooting me
			uint32_t keys=funcs->GetPlayerKeys(bytePlayerId);
			if (keys & 64)
			{
				float currentAngle = funcs->GetPlayerAngle(bytePlayerId);
				float angleToNPC = (float)atan2(-(double)(npcPos.X - vecPos.X), (double)(npcPos.Y - vecPos.Y));
				if (IsAngleInArcAB(currentAngle, (float)(angleToNPC - PI / 6), (float)(angleToNPC + PI / 6)))
				{
					//player is almost facing npc
					if (timeNPCDead != 0)return;
					
					LastAttackTime[bytePlayerId] = now;
					fLastAttackFromAnyPlayer = now;

					if (!bSOS_SEND || PlayerAttackingNPC != bytePlayerId)
					{
						PlayerAttackingNPC = bytePlayerId;
						char buffer[255];
						sprintf(buffer, "%s %u", SOS_CMD, PlayerAttackingNPC);
						funcs->SendCommandToServer(buffer);
						if (!bSOS_SEND)
							bSOS_SEND = true;
					}
					
				}
			}
			
			//his ammo changed..?
			//yes, --> Reduce npc health or die (if die then get bodypart)
			//No, return.
		}
		return;
	}
	else if(updateType==vcmpPlayerUpdateNormal)
	{
		uint32_t keys = funcs->GetPlayerKeys(bytePlayerId);
		if ((keys & 64))
		{
			VECTOR npcPos, vecPos;
			funcs->GetPosition(&npcPos.X, &npcPos.Y, &npcPos.Z);
			funcs->GetPlayerPosition(bytePlayerId, &vecPos);
			float dis = (vecPos - npcPos).GetMagnitude();
			if (dis < 1.5)
			{
				float currentAngle = funcs->GetPlayerAngle(bytePlayerId);
				float angleToNPC = (float)atan2(-(double)(npcPos.X - vecPos.X), (double)(npcPos.Y - vecPos.Y));
				if (IsAngleInArcAB(currentAngle, (float)(angleToNPC - PI / 6), (float)(angleToNPC + PI / 6)))
				{
					int16_t byteHealth = (int16_t)funcs->GetPlayerHealth(npcId);
					int16_t byteArmour = (int16_t)funcs->GetPlayerArmour(npcId);
					uint8_t wep = funcs->GetPlayerWeapon(npcId);
					byteArmour -= GetWeaponDamage(wep);
					float now = GetTickCount() / 1000.f;
					if (LastAttackTime[bytePlayerId] != 0)
					{
						//Because with weapon 0,1 and 2 process one time only.
						//printf("%f\n",LastAttackTime[bytePlayerId]);
						return;
					}
					if (AttackBeganTime[bytePlayerId] == 0)
					{
						AttackBeganTime[bytePlayerId] = now;
						//printf("began attack\n");
						return;
					}
					if (now - AttackBeganTime[bytePlayerId] < 0.05)
					{
						//printf("the time was %f\n",now-AttackBeganTime[bytePlayerId]);
						return;
					}
					if (byteArmour < 0)
					{
						byteHealth += byteArmour;
						byteArmour = 0;
					}
					if (timeNPCDead != 0)return;
					if (byteHealth < 0)
					{
						//npc is dead
						funcs->SetHealth(0, true);
						funcs->SendShotInfo(bodyPart::Body, 0xd);
						killerId = bytePlayerId;
						killerWeapon = wep;
						bpt = bodyPart::Body;
						timeNPCDead = now;
					}
					else
					{
						funcs->SetArmour((uint8_t)byteArmour, false);
						funcs->SetHealth((uint8_t)byteHealth, true);
						LastAttackTime[bytePlayerId] = now;
						fLastAttackFromAnyPlayer = now;
						if (!bSOS_SEND || PlayerAttackingNPC != bytePlayerId) //old value
						{
							PlayerAttackingNPC = bytePlayerId;
							char buffer[255];
							sprintf(buffer, "%s %u", SOS_CMD, PlayerAttackingNPC);
							funcs->SendCommandToServer(buffer);
							if (!bSOS_SEND)
								bSOS_SEND = true;
						}
					}
				}
			}
			return;
		}
	}
	AttackBeganTime[bytePlayerId] = 0;
	LastAttackTime[bytePlayerId] = 0;
}
unsigned int PluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo)
{
	//printf("My Module Loaded\n");
	funcs = pluginFuncs;
	size_t size;
	pluginCalls->OnPlayerUpdate = OnPlayerUpdate;
	pluginCalls->OnCycle = OnCycle;
	const void** sqExports = funcs->GetSquirrelExports(&size);
	if (sqExports != NULL && size > 0)
	{
		SquirrelImports** sqdf = (SquirrelImports**)sqExports;
		SquirrelImports* sqFuncs = (SquirrelImports*)(*sqdf);
		if (sqFuncs)
		{
			v = *(sqFuncs->GetSquirrelVM());
			sq = *(sqFuncs->GetSquirrelAPI());
		}
	}
	InitAttackEniviron();
	return 1;
}
