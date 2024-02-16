
#pragma pack(1)
typedef struct _SpawnClass
{
	int8_t signedbyteClassID = -1;
	uint8_t byteTeam = 0;
	uint8_t byteSkin = 0;
	Color m_Color = Color(0, 0, 0);
}SpawnClass;

class CSpawnClassPool
{
private:

	SpawnClass* m_pSpawnClasses;
	int		m_dwPoolSize;


public:

	CSpawnClassPool() {
		m_pSpawnClasses = NULL;
		m_dwPoolSize = 0;
	};

	~CSpawnClassPool() {
		if (m_dwPoolSize)
			free(m_pSpawnClasses);
	};

	bool New(int8_t signedbyteClassID,  uint8_t byteTeam, uint8_t byteSkin, Color color)
	{
		SpawnClass* temp = (SpawnClass*)realloc(m_pSpawnClasses, (m_dwPoolSize + 1) * sizeof(SpawnClass));
		if (temp)
		{
			m_dwPoolSize += 1;
			m_pSpawnClasses = temp;
			m_pSpawnClasses[m_dwPoolSize - 1].byteTeam = byteTeam;
			m_pSpawnClasses[m_dwPoolSize - 1].byteSkin = byteSkin;
			m_pSpawnClasses[m_dwPoolSize - 1].m_Color = color;
			m_pSpawnClasses[m_dwPoolSize - 1].signedbyteClassID = signedbyteClassID;
			return true;
		}
		else printf("spawnclasspool: Error in allocating memory\n");
		return false;
	}
	void Destroy(uint8_t signedbyteClassID)
	{
		for (int i = 0; i < m_dwPoolSize; i++)
		{
			if (m_pSpawnClasses[i].signedbyteClassID == signedbyteClassID)
			{
				if (m_dwPoolSize > i + 1)
				{
					//Move memory one SpawnClass block left
					memcpy((void*)&m_pSpawnClasses[i], (void*)&m_pSpawnClasses[i + 1], (m_dwPoolSize - (i + 1)) * sizeof(SpawnClass));
					//Realloc
					SpawnClass* temp = (SpawnClass*)realloc(m_pSpawnClasses, (m_dwPoolSize - 1) * sizeof(SpawnClass));
					if (temp)
					{
						m_pSpawnClasses = temp;
					}
					else printf("spawnclasspool: realloc failed\n");
				}
				//Reduce size of pool by 1
				m_dwPoolSize -= 1;
				break;
			}
		}
	}
	SpawnClass* GetByID(int8_t signedbyteClassID)
	{
		for (int i = 0; i < m_dwPoolSize; i++)
		{
			if (m_pSpawnClasses[i].signedbyteClassID == signedbyteClassID)
			{
				return &m_pSpawnClasses[i];
			}
		}
		return NULL;
	}
	int GetCount()
	{
		return m_dwPoolSize;
	}
	void ChangeSignToPositive(int8_t ntotalclasses)
	{
		for (int i = 0; i < m_dwPoolSize; i++)
		{
			if (m_pSpawnClasses[i].signedbyteClassID < 0)
			{
				m_pSpawnClasses[i].signedbyteClassID += ntotalclasses;
			}
		}
	}
};
