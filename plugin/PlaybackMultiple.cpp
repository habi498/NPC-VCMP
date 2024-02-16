#include "SQMain.h"
#include "SQFuncs.h"
extern PlaybackMultiple* pPlaybackMultiple;
uint8_t ConnectMultipleNpcs(std::string filename, std::string host, uint32_t flags, std::string execArg)
{
    return pPlaybackMultiple->Start(filename, host,flags,execArg);
}
bool StopConnectingMultipleNpcs()
{
    if (pPlaybackMultiple->running)
    {
        pPlaybackMultiple->Abort();
        return true;
    }
    return false;
}
uint8_t PlaybackMultiple::ProcessRow()
{
    DWORD time; uint16_t filenamelen; char playerName[24]{};
    size_t m = fread(&time, sizeof(time), 1, pFile);

    if (m != 1)return HREC_ERROR_READING_DATA;
    m = fread(&filenamelen, sizeof(filenamelen), 1, pFile);
    if (m != 1)return HREC_ERROR_READING_DATA;
    std::string filename;
    char* szFileName = (char*)calloc(filenamelen + 1, sizeof(char));
    if (!szFileName)return HREC_ERROR_ALLOCATING_MEMORY;
    m = fread(szFileName, sizeof(char), filenamelen, pFile);
    if (m != filenamelen) {
        free(szFileName); return HREC_ERROR_READING_DATA;
    }
    m = fread(playerName, sizeof(char), 24, pFile);
    if (m != 24) {
        free(szFileName); return HREC_ERROR_READING_DATA;
    }
    std::string arg = "";
    bool bShowConsole = this->flags & HF_SHOW_CONSOLE ? true : false;
    if (flags & HF_AUTOMATIC_PLAY)
    {
        arg += "function OnNPCClassSelect() {if (!IsPlaybackRunning())StartRecordingPlayback(3, \\\"";
        arg += (szFileName + std::string("\\\")};"));
    }
    if (flags & HF_DISCONNECT_AT_END)
    {
        arg += "function OnRecordingPlaybackEnd(){QuitServer()};";
    }
    arg += this->execArg;
    this->running = true;
    bool s = CallNPCClient(playerName, "", bShowConsole, this->host.c_str(), "", arg.c_str(), "", {});
    if (!s)
    {
        free(szFileName);
        this->Abort();
        return HREC_ERROR_CONNECT_FAILED;
    }
    free(szFileName);
    DWORD time_next;
    m = fread(&time_next, sizeof(DWORD), 1, pFile);
    if (m != 1) {
        if (std::feof(pFile))
        {
            //end of playback reached. no more players to connect
            this->Stop();
            return 0;
        }
        else
        {
            this->Stop(); return HREC_ERROR_READING_NEW_ROW;
        }
    }
    int c = fseek(pFile, -sizeof(DWORD), SEEK_CUR);//go back 4 bytes
    if (c != 0)
    {
        this->Stop(); return HREC_ERROR_SEEKING_FILE;
    }
    DWORD interval = time_next - time;
    if (interval > 0)
    {
        IntervalInTicks = interval;
    }
    else
    {
        //time_next is 10 and time is UINT32_MAX-20
        //(10-UINT32_MAX+20 +UINT32_MAX) = -30
        DWORD n = interval + UINT32_MAX;
        IntervalInTicks = n;
    }
    ticksElapsed = 0;
    return HREC_PROCESSROW_SUCCESS;
}

