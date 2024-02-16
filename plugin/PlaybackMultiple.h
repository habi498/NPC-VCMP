
#ifndef PLAYBACKMULTIPLE_H
#ifndef WIN32
#include <climits>
#endif
#define PLAYBACKMULTIPLE_H

#define HREC_INITIALIZE_SUCCESS 0
#define HREC_PROCESSROW_SUCCESS 0
#define HREC_PLAY_IN_PROGRESS 1
#define HREC_ERROR_OPENING_FILE 2
#define HREC_ERROR_READING_MAGIC 3
#define HREC_INVALID_MAGIC 4
#define HREC_ERROR_READING_DATA 5
#define HREC_ERROR_ALLOCATING_MEMORY 6
#define HREC_ERROR_CONNECT_FAILED 7
#define HREC_ERROR_READING_NEW_ROW 8
#define HREC_ERROR_SEEKING_FILE 9

#define HF_NONE 0
#define HF_AUTOMATIC_PLAY   1
#define HF_DISCONNECT_AT_END   2
#define HF_SHOW_CONSOLE    4
#ifdef WIN32
#define HREC_PLAY_FLAG_STANDARD (HF_AUTOMATIC_PLAY|HF_DISCONNECT_AT_END|HF_SHOW_CONSOLE)
#else
#define HREC_PLAY_FLAG_STANDARD (HF_AUTOMATIC_PLAY|HF_DISCONNECT_AT_END)
#endif
struct PlaybackMultiple
{
private:
 
public:
    uint32_t identifier;
    FILE* pFile;
    bool running = false;
    DWORD ticksElapsed = 0;//One Tick is one ms
    DWORD IntervalInTicks = INT_MAX;
    std::string host = "";
    uint32_t flags = 0;
    std::string execArg = "";
    void Abort()
    {
        running = false;
        fclose(pFile);
        pFile = NULL;
        ticksElapsed = 0;
    }
    void Stop()
    {
        this->Abort();
    }
    uint8_t ProcessRow();
    uint8_t Start(std::string file, std::string host = "127.0.0.1", uint32_t flags = 7,std::string execArg = "")
    {
        if (this->running)return HREC_PLAY_IN_PROGRESS;
        this->host = host;
        this->execArg = execArg;
        this->flags = flags;
        pFile = fopen((SERVER_REC_DIR+std::string("/")+file+".hrec").c_str(), "rb");
        if (!pFile)return HREC_ERROR_OPENING_FILE;
        DWORD magic;
        size_t m=fread(&magic, sizeof(DWORD), 1, pFile);
        if (m != 1)return HREC_ERROR_READING_MAGIC;
        if (magic != SERVER_RECORDING_IDENTIFIER)
            return HREC_INVALID_MAGIC;
        uint8_t s = ProcessRow();
        if (s == HREC_PROCESSROW_SUCCESS)
            return HREC_INITIALIZE_SUCCESS;
        else
            return s;//return error code
    }
    void Process(float elapsedTime)
    {
        if (!this->running)return;
        DWORD ticks = (DWORD)(elapsedTime * 1000);
        this->ticksElapsed += ticks;
        if (this->ticksElapsed >= this->IntervalInTicks)
        {
            //read one row of 
            uint8_t s = ProcessRow(); //ProcessRow will also stop it when error or eof occured.
        }
        else return;//wait until balanced
    }
};
uint8_t ConnectMultipleNpcs(std::string filename, std::string host, uint32_t flags=7, std::string execArg="");
bool StopConnectingMultipleNpcs();
#endif
