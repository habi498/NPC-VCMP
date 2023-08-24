#include <functional>
#ifndef PLUGINS_H
#define PLUGINS_H
#ifndef WIN32
#define HINSTANCE void*
#endif
#include "pluginscommon.h"


typedef unsigned int (*PluginInit) (PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo);
struct PluginPool_s
{
	HINSTANCE hInstance;
	PluginInit Init;
	PluginInfo* pPluginInfo;
	PluginCallbacks* pPluginCalls;
	PluginFuncs* pPluginFuncs;
};
typedef std::vector<PluginPool_s*> PluginPoolVector;
class CPlugins
{
private:
	PluginPoolVector m_Plugins;
	BOOL LoadSinglePlugin(char* szPluginPath);
public:
	CPlugins();
	~CPlugins();
	void LoadPlugins(const char* szSearchPath, std::string PluginsList);
	DWORD GetPluginCount();
	PluginPool_s* GetPlugin(DWORD index);
};
#endif
