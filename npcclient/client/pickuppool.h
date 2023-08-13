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


#ifndef PICKUPPOOL_H
#define PICKUPPOOL_H

#pragma pack(1)
typedef struct _PICKUP
{
	unsigned short wID;
	unsigned short wModel;
	unsigned int dwQuantity;
	unsigned int dwSerialNo;
	VECTOR vecPos;
	unsigned char bAlpha;
} PICKUP;

//----------------------------------------------------

class CPickupPool
{
private:

	PICKUP*  m_pPickups;
	int		m_dwPoolSize;
	

public:

	CPickupPool() {
		m_pPickups = NULL;
		m_dwPoolSize = 0;
	};

	~CPickupPool() {
		if (m_dwPoolSize)
			free(m_pPickups);
	};

	bool New(uint16_t wID, uint32_t dwSerialNo, uint16_t wModelId, uint16_t wQuantity, VECTOR vecPos, uint8_t byteAlpha) 
	{
		PICKUP* temp =(PICKUP*) realloc(m_pPickups, (m_dwPoolSize + 1) * sizeof(PICKUP));
		if (temp)
		{
			m_dwPoolSize += 1;
			m_pPickups = temp;
			m_pPickups[m_dwPoolSize - 1].bAlpha = byteAlpha;
			m_pPickups[m_dwPoolSize - 1].dwSerialNo = dwSerialNo;
			m_pPickups[m_dwPoolSize - 1].vecPos = vecPos;
			m_pPickups[m_dwPoolSize - 1].wID = wID;
			m_pPickups[m_dwPoolSize - 1].wModel = wModelId;
			m_pPickups[m_dwPoolSize - 1].dwQuantity = wQuantity;
			return true;
		}
		else printf("pickuppool: Error in allocating memory\n");
		return false;
	}
	void Destroy(uint16_t wPickupID)
	{
		for (int i = 0; i < m_dwPoolSize; i++)
		{
			if (m_pPickups[i].wID == wPickupID)
			{
				if (m_dwPoolSize > i + 1)
				{
					//Move memory one (PICKUP) block left
					memcpy((void*)&m_pPickups[i], (void*)&m_pPickups[i + 1], (m_dwPoolSize - (i + 1)) * sizeof(PICKUP));
					//Realloc
					PICKUP* temp = (PICKUP*)realloc(m_pPickups, (m_dwPoolSize - 1) * sizeof(PICKUP));
					if (temp)
					{
						m_pPickups = temp;
					}
					else printf("pickuppool: realloc failed\n");
				}
				//Reduce size of pool by 1
				m_dwPoolSize -= 1;
				break;
			}
		}
	}
	PICKUP* GetByID(uint16_t wPickupID)
	{
		for (int i = 0; i < m_dwPoolSize; i++)
		{
			if (m_pPickups[i].wID == wPickupID)
			{
				return &m_pPickups[i];
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