#pragma warning( disable : 4103)
#include "stdio.h"
#include "main.h"
PluginFuncs* pPluginFuncs;
void _OnPlayerDeath(uint8_t playerId) {
	printf("Player %d died\n", playerId);
}
extern "C" unsigned int PluginInit(PluginFuncs* pluginFuncs, PluginCallbacks*  pluginCalls, PluginInfo*  pluginInfo) {
		pluginCalls->OnPlayerDeath = _OnPlayerDeath;
		pPluginFuncs = pluginFuncs;
		printf("Module: PluginInit called\n");
		return 1;
}