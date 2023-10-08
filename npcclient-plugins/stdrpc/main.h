#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#include <stdio.h>
#include "plugin.h"
#include "SqImports.h"
#include "RPC.h"
#ifdef __cplusplus
extern "C" {
#endif
	EXPORT	unsigned int	PluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo);
#ifdef __cplusplus
}
#endif