/*
	Copyright 2004-2005 SA:MP Team
	Version: $Id: playerpool.cpp,v 1.19 2006/05/07 15:35:32 kyeman Exp $

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
#include "main.h"
CPlayerPool::CPlayerPool()
{
	// loop through and initialize all net players to null and slot states to false
	for (BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		m_bPlayerSlotState[bytePlayerID] = FALSE;
		m_pPlayers[bytePlayerID] = NULL;
	}
	m_iPlayerCount = 0;
}
CPlayerPool::~CPlayerPool()
{
	for (BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		Delete(bytePlayerID);
	}
	m_iPlayerCount = 0;
}
BOOL CPlayerPool::New(BYTE bytePlayerID, PCHAR szPlayerName)
{
	if (bytePlayerID > MAX_PLAYERS) return FALSE;
	if (strlen(szPlayerName) > MAX_PLAYER_NAME) return FALSE;

	m_pPlayers[bytePlayerID] = new CPlayer();

	if (m_pPlayers[bytePlayerID])
	{
		strcpy(m_szPlayerName[bytePlayerID], szPlayerName);
		m_pPlayers[bytePlayerID]->SetID(bytePlayerID);
		m_bPlayerSlotState[bytePlayerID] = TRUE;
		m_iPlayerCount++;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL CPlayerPool::Delete(BYTE bytePlayerID)
{
	if (!GetSlotState(bytePlayerID) || !m_pPlayers[bytePlayerID])
	{
		return FALSE; // Player already deleted or not used.
	}
	m_bPlayerSlotState[bytePlayerID] = FALSE;
	delete m_pPlayers[bytePlayerID];
	m_pPlayers[bytePlayerID] = NULL;
	m_iPlayerCount--;

	return TRUE;
}