/*
	Copyright (C) 2022  habi

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
/* Class definitions, mimicked from SA-MP files at
github https://github.com/dashr9230/SA-MP just for fun*/
#ifndef PLAYERRECORDING_H
#define PLAYERRECORDING_H
#include <stdint.h>
#include <stdio.h>
#include <string>
#define RECDIR "recordings"
#define RECDIR_ALTERNATE "npcscripts/recordings"
#define SERVER_REC_DIR "recordings/a_server"
//#define NPC_RECFILE_IDENTIFIER_V6 1006 //Aug 2024
#define NPC_RECFILE_IDENTIFIER_V5 1005 //Dec 2023
//#define NPC_RECFILE_IDENTIFIER_V4 1004 //ref 25.August 2023
#define MAX_PLAYERS 100
#define PLAYER_RECORDING_TYPE_DRIVER	2
#define PLAYER_RECORDING_TYPE_ONFOOT	1
#define PLAYER_RECORDING_TYPE_ALL		3


class CPlayer
#define PI 3.1415926
{
private:
	uint8_t m_PlayerID;
	bool init;
	bool CreateRecordingFile(std::string file)
	{
		pFile = fopen(file.c_str(), "wb");
		if (pFile == NULL)return 0;
		else
		{
			return 1;
		}
	}
	bool WriteHeader(uint32_t identifier)
	{
		size_t count = fwrite(&identifier, sizeof(identifier), 1, pFile);
		if (count != 1)return 0;
		count = fwrite(&m_RecordingType, sizeof(m_RecordingType), 1, pFile);
		if (count != 1)return 0;
		return 1;
	}

public:
	FILE* pFile;
	uint32_t m_dwRecFlags = 0;
	uint32_t m_RecordingType;//uint32_t 4bytes 
	bool SpawnDataWritten = false;//otherwise onfoot data gets written before
	uint32_t sgbytePreviousClassID = 0;//the class of the player before death or {class selection screen previous}
	std::string szFilename = "";
	void ProcessUpdate(PluginFuncs* VCMP, vcmpPlayerUpdate updateType);
	void ProcessUpdate2(PluginFuncs* VCMP, vcmpPlayerUpdate updateType);
	void SetID(uint8_t playerId)
	{
		this->m_PlayerID = playerId;
	}
	CPlayer()
	{
		this->init = false;
		this->m_RecordingType = -1;
		this->pFile = NULL;
		this->m_PlayerID = 255;
	}
	~CPlayer() { if (this->IsRecording())this->Abort(); }
	bool IsRecording() { return init; }
	void Abort() { this->Finish(); }
	void Finish()
	{
		if (this->IsRecording())
		{
			if (pFile) {
				fclose(pFile); pFile = NULL;
			}
			this->init = false;
			this->SpawnDataWritten = false;
			this->szFilename = "";
		}
	}
	bool Start(uint32_t recordtype, std::string filename, uint32_t flags = 0);
	std::string GetFileName()
	{
		return szFilename;
	}
};
class CPlayerPool
{
private:
	CPlayer* m_pPlayers[MAX_PLAYERS];
	BOOL	m_bPlayerSlotState[MAX_PLAYERS];
public:
	CPlayerPool()
	{
		for (uint8_t playerID = 0; playerID < MAX_PLAYERS; playerID++) {
			m_bPlayerSlotState[playerID] = FALSE;
			m_pPlayers[playerID] = NULL;
		}
	}
	CPlayer* GetAt(uint8_t playerID)
	{
		if (playerID >= MAX_PLAYERS) { return NULL; }
		return m_pPlayers[playerID];
	}
	BOOL GetSlotState(BYTE playerID) {
		if (playerID >= MAX_PLAYERS) { return FALSE; }
		return m_bPlayerSlotState[playerID];
	};
	bool New(uint8_t playerID)
	{
		if (playerID > MAX_PLAYERS)return false;
		m_pPlayers[playerID] = new CPlayer();
		if (m_pPlayers[playerID])
		{
			m_pPlayers[playerID]->SetID(playerID);
			m_bPlayerSlotState[playerID] = TRUE;
			return true;
		}
		else return false;
	}
	bool Delete(uint8_t playerID)
	{
		if (!m_pPlayers[playerID])
			return false;
		delete m_pPlayers[playerID];
		m_bPlayerSlotState[playerID] = FALSE;
		return true;
	}
};
bool StartRecordingPlayerData(int32_t playerId, uint8_t recordtype=3, std::string recordname="", uint32_t flags=60);
uint8_t StartRecordingAllPlayerData(uint8_t recordtype=3, uint32_t flags=60, bool bRecordNewPlayers = false);
bool StopRecordingPlayerData(int32_t playerId);
uint8_t StopRecordingAllPlayerData();
bool IsPlayerRecording(uint8_t bytePlayerId);
uint16_t ConvertToUINT16_T(float value, float base);
#endif