/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	Version: $Id: pickuppool.h,v 1.2 2006/03/20 17:59:34 kyeman Exp $

	Modified and Copyright (C) 2023  habi

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


#ifndef CHECKPOINTPOOL_H
#define CHECKPOINTPOOL_H

#pragma pack(1)
typedef struct _CHECKPOINT
{
	unsigned short wID;
	unsigned char byteIsSphere;
	VECTOR vecPos;
	unsigned char byteRed;
	unsigned char byteGreen;
	unsigned char byteBlue;
	unsigned char byteAlpha;
	float fRadius;
} CHECKPOINT;

//----------------------------------------------------

class CCheckpointPool
{
private:

	CHECKPOINT* m_pCheckpoints;
	int		m_dwPoolSize;


public:

	CCheckpointPool() {
		m_pCheckpoints = NULL;
		m_dwPoolSize = 0;
	};

	~CCheckpointPool() {
		if (m_dwPoolSize)
			free(m_pCheckpoints);
	};

	//bool New(uint16_t wID, uint8_t byteIsSphere, VECTOR vecPos, uint8_t byteRed, uint8_t byteGreen, uint8_t byteBlue, uint8_t byteAlpha, float fRadius)
	bool New(CHECKPOINT cp)
	{
		CHECKPOINT* temp = (CHECKPOINT*)realloc(m_pCheckpoints, (m_dwPoolSize + 1) * sizeof(CHECKPOINT));
		if (temp)
		{
			m_dwPoolSize += 1;
			m_pCheckpoints = temp;
			m_pCheckpoints[m_dwPoolSize - 1].byteRed = cp.byteRed;
			m_pCheckpoints[m_dwPoolSize - 1].byteGreen = cp.byteGreen;
			m_pCheckpoints[m_dwPoolSize - 1].byteBlue = cp.byteBlue;
			m_pCheckpoints[m_dwPoolSize - 1].byteAlpha = cp.byteAlpha;
			m_pCheckpoints[m_dwPoolSize - 1].vecPos = cp.vecPos;
			m_pCheckpoints[m_dwPoolSize - 1].wID = cp.wID;
			m_pCheckpoints[m_dwPoolSize - 1].fRadius = cp.fRadius;
			m_pCheckpoints[m_dwPoolSize - 1].byteIsSphere = cp.byteIsSphere;
			return true;
		}
		else printf("checkpointpool: Error in allocating memory\n");
		return false;
	}
	void Destroy(uint16_t wCheckpointID)
	{
		for (int i = 0; i < m_dwPoolSize; i++)
		{
			if (m_pCheckpoints[i].wID == wCheckpointID)
			{
				if (m_dwPoolSize > i + 1)
				{
					//Move memory one (CHECKPOINT) block left
					memcpy((void*)&m_pCheckpoints[i], (void*)&m_pCheckpoints[i + 1], (m_dwPoolSize - (i + 1)) * sizeof(CHECKPOINT));
					//Realloc
					CHECKPOINT* temp = (CHECKPOINT*)realloc(m_pCheckpoints, (m_dwPoolSize - 1) * sizeof(CHECKPOINT));
					if (temp)
					{
						m_pCheckpoints = temp;
					}
					else printf("checkpointpool: realloc failed\n");
				}
				//Reduce size of pool by 1
				m_dwPoolSize -= 1;
				break;
			}
		}
	}
	CHECKPOINT* GetByID(uint16_t wCheckpointID)
	{
		for (int i = 0; i < m_dwPoolSize; i++)
		{
			if (m_pCheckpoints[i].wID == wCheckpointID)
			{
				return &m_pCheckpoints[i];
			}
		}
		return NULL;
	}
	int GetCount()
	{
		return m_dwPoolSize;
	}
};

//----------------------------------------------------

#endif