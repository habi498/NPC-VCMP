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
#include "SYNC_DATA.h"
#include "PlayerRecording.h"

#ifdef __cplusplus
extern "C" {
#endif
	EXPORT	unsigned int			VcmpPluginInit					( PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo );
#ifdef __cplusplus
}
#endif

#endif