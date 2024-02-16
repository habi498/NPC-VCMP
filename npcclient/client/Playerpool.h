/*
	Copyright 2004-2005 SA:MP Team
	Version: $Id: playerpool.h,v 1.12 2006/04/09 09:54:46 kyeman Exp $

	Modified and Copyright (C) 2022  habi

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
#ifndef PLAYER_POOL_H
#define PLAYER_POOL_H
#define INVALID_PLAYER_ID						0xFF
#define MAX_PLAYER_NAME							24
class CPlayerPool
{
private:

	BOOL	m_bPlayerSlotState[MAX_PLAYERS];
	CPlayer* m_pPlayers[MAX_PLAYERS];
	CHAR	m_szPlayerName[MAX_PLAYERS][MAX_PLAYER_NAME + 1];
	int		m_iPlayerCount;
public:

	CPlayerPool();
	~CPlayerPool();
	BOOL New(BYTE bytePlayerID, PCHAR szPlayerName);
	BOOL Delete(BYTE bytePlayerID);

	// Retrieve a player
	CPlayer* GetAt(BYTE bytePlayerID) {
		if (bytePlayerID >= MAX_PLAYERS) { return NULL; }
		return m_pPlayers[bytePlayerID];
	};
	// Find out if the slot is inuse.
	BOOL GetSlotState(BYTE bytePlayerID) {
		if (bytePlayerID >= MAX_PLAYERS) { return FALSE; }
		return m_bPlayerSlotState[bytePlayerID];
	};

	PCHAR GetPlayerName(BYTE bytePlayerID) {
		if (bytePlayerID >= MAX_PLAYERS) { return FALSE; }
		return m_szPlayerName[bytePlayerID];
	};
	void SetPlayerName(BYTE bytePlayerID, PCHAR szName) {
		strcpy(m_szPlayerName[bytePlayerID], szName);
	}
	int GetPlayerCount() { return m_iPlayerCount; };
	BYTE GetPlayerId(PCHAR szPlayerName);
};
#endif