#ifndef PLUGINS_H
#define PLUGINS_H
typedef struct _Settings {
	uint32_t structSize;
	uint32_t flags;
} Settings;


typedef struct _PluginInfo {
	uint32_t structSize;
	char name[32];
} PluginInfo;

typedef struct _PluginFuncs {
	uint32_t structSize;
	uint32_t(*GetVersion) (void);
}PluginFuncs;

typedef struct _PluginCallbacks{
	uint32_t structSize;
	void (*OnPlayerDeath)(uint8_t bytePlayerId) {};
	void (*OnPlayerText)(uint8_t bytePlayerId, char* text, uint16_t length) {};
	void (*OnNPCEnterVehicle)(uint16_t vehicleid, uint8_t seatid);
	void (*OnNPCExitVehicle)();
	void (*OnPlayerStreamIn)(uint8_t bytePlayerId);
	void (*OnPlayerStreamOut)(uint8_t playerid);
	void (*OnVehicleStreamIn)(uint16_t vehicleid);
	void (*OnVehicleStreamOut)(uint16_t vehicleid);
	void (*OnNPCDisconnect)(uint8_t reason);
	void (*OnSniperRifleFired)(uint8_t playerid, uint8_t weapon, float x, float y, float z, float dx, float dy, float dz);
	void (*OnPlayerUpdate)(uint8_t bytePlayerId, vcmpPlayerUpdate updateType);
	void (*OnNPCConnect)(uint8_t byteId);
	void (*OnClientMessage)(uint8_t r, uint8_t g, uint8_t b, char* message, uint16_t len);
	void (*OnNPCSpawn)(); 
	void (*OnCycle) () {};
}PluginCallbacks;

//typedef unsigned int __declspec ( PluginInit)();// PluginFuncs pluginFuncs, PluginCallbacks pluginCalls, PluginInfo pluginInfo);
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
	void LoadPlugins(char* szSearchPath, std::string PluginsList);
	DWORD GetPluginCount();
	PluginPool_s* GetPlugin(DWORD index);
};
#endif
