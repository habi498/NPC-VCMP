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


#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#pragma pack(1)
typedef struct _OBJECT
{
	unsigned short wID;
	unsigned short wModel;
	VECTOR vecPos;
	QUATERNION quatRot;
	unsigned char byteAlpha;
	unsigned char byteMaskTrackingShotsBumps;
	bool IsTouchReportEnabled()
	{
		return (byteMaskTrackingShotsBumps & 2) == 2 ? true : false;
	}
	bool IsShotReportEnabled()
	{
		return (byteMaskTrackingShotsBumps & 1) == 1 ? true : false;
	}
}OBJECT;

//----------------------------------------------------

class CObjectPool
{
private:

	OBJECT* m_pObjects;
	int		m_dwPoolSize;


public:

	CObjectPool() {
		m_pObjects = NULL;
		m_dwPoolSize = 0;
	};

	~CObjectPool() {
		if (m_dwPoolSize)
			free(m_pObjects);
	};

	bool New(OBJECT obj)
	{
		OBJECT* temp = (OBJECT*)realloc(m_pObjects, (m_dwPoolSize + 1) * sizeof(OBJECT));
		if (temp)
		{
			m_dwPoolSize += 1;
			m_pObjects = temp;
			m_pObjects[m_dwPoolSize - 1].byteAlpha = obj.byteAlpha;
			m_pObjects[m_dwPoolSize - 1].vecPos = obj.vecPos;
			m_pObjects[m_dwPoolSize - 1].wID = obj.wID;
			m_pObjects[m_dwPoolSize - 1].quatRot = obj.quatRot;
			m_pObjects[m_dwPoolSize - 1].byteMaskTrackingShotsBumps = obj.byteMaskTrackingShotsBumps;
			m_pObjects[m_dwPoolSize - 1].wModel = obj.wModel;
			return true;
		}
		else printf("objectpool: Error in allocating memory\n");
		return false;
	}
	void Destroy(uint16_t wObjectID)
	{
		for (int i = 0; i < m_dwPoolSize; i++)
		{
			if (m_pObjects[i].wID == wObjectID)
			{
				if (m_dwPoolSize > i + 1)
				{
					//Move memory one (OBJECT) block left
					memcpy((void*)&m_pObjects[i], (void*)&m_pObjects[i + 1], (m_dwPoolSize - (i + 1)) * sizeof(OBJECT));
					//Realloc
					OBJECT* temp = (OBJECT*)realloc(m_pObjects, (m_dwPoolSize - 1) * sizeof(OBJECT));
					if (temp)
					{
						m_pObjects = temp;
					}
					else printf("objectpool: realloc failed\n");
				}
				//Reduce size of pool by 1
				m_dwPoolSize -= 1;
				break;
			}
		}
	}
	OBJECT* GetByID(uint16_t wObjectID)
	{
		for (int i = 0; i < m_dwPoolSize; i++)
		{
			if (m_pObjects[i].wID == wObjectID)
			{
				return &m_pObjects[i];
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