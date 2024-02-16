#pragma once
#ifndef _SQMAIN_H
#define _SQMAIN_H

#ifdef _WIN32
	#define WIN32_LEANANDMEAN
	#include <windows.h>

	#define EXPORT __declspec( dllexport )
#else
	#include <unistd.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/stat.h> 
	#include<sys/times.h>
	#define EXPORT
	#define MAX_PATH 250
	#define stricmp strcasecmp
#endif
#include "plugin.h"
#include "SQImports.h"
#include "ConsoleUtils.h"
#include "system.h"
#include <Utils.h>
#include "PlayerRecording.h"
#include "HideNPCImports.h"
#include "ReadCFG.h"
#include "ServerRecording.h"
#include "PlaybackMultiple.h"
#define CMD_ECHO 0x3b457A00
#define START_RECORDING_PLAYERDATA 0x3b457A01
#define STOP_RECORDING_PLAYER_DATA 0x3b457A02
#define START_RECORDING_ALL_PLAYERDATA 0x3b457A03
#define STOP_RECORDING_ALL_PLAYER_DATA  0x3b457A04
#define PUT_SERVER_IN_RECORDING_MODE 0x3b457A05
#define STOP_SERVER_IN_RECORDING_MODE 0x3b457A06
#define CONNECT_MULTIPLE_NPCS 0x3b457A07
#define STOP_CONNECTING_MULTIPLE_NPCS 0x3b457A08
#define CALL_NPCCLIENT 0x3b457A09
#ifdef __cplusplus
extern "C" {
#endif
	EXPORT	unsigned int			VcmpPluginInit					( PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo );
#ifdef __cplusplus
}
#endif

#endif