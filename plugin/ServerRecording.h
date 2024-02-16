#ifndef SERVERRECORDING_H
#define  SERVERRECORDING_H
#define SERVER_RECORDING_IDENTIFIER 9000
extern void CreateHRecFolder();
class CServerRecording
{
	bool bIsRunning = false;
	DWORD dw_StartTime = 0;
	FILE* pFile = NULL;
	std::string szFilename;
private:
	bool CreateHRecFile(std::string file)
	{
		pFile = fopen(file.c_str(), "wb");
		if (pFile == NULL)return 0;
		else
		{
			szFilename = file;
			return 1;
		}
	}
public:
	bool IsRecording() { return bIsRunning; }
	void Finish()
	{
		if (this->bIsRunning)
		{
			if (this->pFile)
				fclose(this->pFile);
			bIsRunning = false;
		}
	}
	bool WriteNewData(DWORD dw_time, std::string filename, char* playername)
	{
		if (!this->bIsRunning)return false;
		size_t count = fwrite(&dw_time, sizeof(dw_time), 1, pFile);
		if (count != 1)return 0;
		uint16_t namelen = filename.length();
		count = fwrite(&namelen, sizeof(namelen), 1, pFile);
		if (count != 1)return 0;
		count = fwrite(filename.c_str(), sizeof(char), namelen, pFile);
		if (count != namelen)return 0;
		uint16_t len = strlen(playername);
		if (len >= 0 && len <= 24)
		{
			count=fwrite(playername, sizeof(char), 24, pFile);
			if (count != 24)return 0;
		}
		else return 0;
		return 1;
	}
	bool Start(std::string hrecfilename)
	{
		CreateHRecFolder();
		std::string fullpath= std::string(SERVER_REC_DIR + std::string("/") + hrecfilename + std::string(".hrec"));
		bool success=CreateHRecFile(fullpath);
		if (!success)return 0;
		this->bIsRunning = true;
		this->dw_StartTime = GetTickCount();
		//write magic
		DWORD magic = SERVER_RECORDING_IDENTIFIER;
		size_t count = fwrite(&magic, sizeof(DWORD), 1, pFile);
		if (count != 1)
		{
			this->Finish();
			return 0;
		}
		return 1;
	}
};
bool StopServerInRecordingMode();
bool PutServerInRecordingMode(uint8_t recordtype, uint32_t flags, std::string filename);
bool IsServerInRecordingMode();
#endif