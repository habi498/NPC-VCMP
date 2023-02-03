#include <vector>
typedef bool (*fn_StartRecordingPlayerData)(int32_t playerId, uint8_t recordtype, std::string recordname);
typedef bool (*fn_StopRecordingPlayerData)(int32_t playerId);
typedef bool (*fn_CallNPCClient)(const char* szName, const char* szScript, bool bConsoleInputEnabled,
	const char* host , const char* plugins, const char* loc, std::vector<const char*>params );
typedef bool (*fn_IsPlayerNPC)(uint8_t playerId);

struct NPCExports{
	unsigned int uStructSize;
	fn_StartRecordingPlayerData StartRecordingPlayerData;
	fn_StopRecordingPlayerData StopRecordingPlayerData;
	fn_CallNPCClient CallNPCClient;
	fn_IsPlayerNPC IsPlayerNPC;
};

