#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#include "plugin.h"

#ifdef __cplusplus
extern "C" {
#endif
	EXPORT	unsigned int	PluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo);
#ifdef __cplusplus
}
#endif