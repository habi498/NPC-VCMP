#include "SQMain.h"
extern CServerRecording* pServerRecording;
extern CPlayerPool* m_pPlayerPool;
extern PluginFuncs* VCMP;
bool StopServerInRecordingMode()
{
	if (!pServerRecording->IsRecording())return false;
	StopRecordingAllPlayerData();
	pServerRecording->Finish();
	return true;
}
bool IsServerInRecordingMode()
{
	return pServerRecording->IsRecording();
}
bool PutServerInRecordingMode(uint8_t recordtype, uint32_t flags, std::string filename)
{
	if (pServerRecording->IsRecording())
		return false;
	StopRecordingAllPlayerData();
	StartRecordingAllPlayerData(recordtype, flags, true);
	bool s=pServerRecording->Start(filename);
	if (!s)return false;
	char playername[30]{};
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (m_pPlayerPool->GetSlotState(i))
		{
			CPlayer* m_pPlayer = m_pPlayerPool->GetAt(i);
			if (m_pPlayer && m_pPlayer->IsRecording())
			{
				std::string filename = m_pPlayer->GetFileName();
				if (VCMP->GetPlayerName(i, playername, sizeof(playername)) == vcmpErrorNone)
				{
					if (filename != "")
					{
						DWORD dw_time = GetTickCount();

						pServerRecording->WriteNewData(dw_time, filename, playername);
					}
				}
			}
		}
	}
	return true;
}
