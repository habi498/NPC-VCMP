#ifndef MAIN_H
#define MAIN_H
#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#if WIN32
#include <windows.h>
#else
#include <string.h>
#include <time.h>
typedef unsigned int DWORD;
typedef unsigned short WORD;
long GetTickCount();
#endif

#include "plugin.h"
#include "SqImports.h"
#include "weapon.h"
#define GUN_NOT_AVAILABLE 0
#ifdef __cplusplus
extern "C" {
#endif
	EXPORT	unsigned int	PluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo);
#ifdef __cplusplus
}
#endif
void RegisterFunctions();
SQInteger RegisterSquirrelConst(HSQUIRRELVM v, const SQChar* cname, SQInteger cvalue);

uint8_t GetSlotIdFromWeaponId(uint8_t weaponId);
uint8_t SelectGun();
enum class REASON
{
	AMMO_SHORTAGE,
	PLAYER_WASTED,
	PLAYER_STREAMEDOUT,
	INSTRUCTION_RECEIVED
};

class NPC
{
public:
	bool m_bHasTarget;
	uint8_t m_byteTargetId;
	DWORD m_dwLastFired;
	DWORD m_dwNextCycle;
	bool m_bTargetOutOfRange;
	bool m_bIsReloadingWeapon;
	bool m_bIsFiringBullet;
	NPC()
	{
		m_bHasTarget = false;
		m_byteTargetId = 255;
		m_dwLastFired = 0;
		m_dwNextCycle = 0;
		m_bTargetOutOfRange = false;
		m_bIsReloadingWeapon = false;
		m_bIsFiringBullet = false;
	}
	void ClearTarget(REASON reason);
};
#endif