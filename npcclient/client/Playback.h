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
#ifndef PLAYBACK_H
#define PLAYBACK_H
struct Playback
{
private:
    bool paused = false;
public:
    DWORD dw_pausedTime = 0;
    uint32_t identifier;
    FILE* pFile;
    uint8_t type;//PLAYER_RECORDING_TYPE_DRIVER or PLAYER_RECORDING_TYPE_ONFOOT
    bool running = false;
    DWORD nexttick;//When Gettickcount()>=nexttick, send packet
    DWORD prevtick;//storing tickcount of previous packet.
    bool IsPaused()
    {
        return this->paused;
    }
    void Pause();
    void Resume()
    {
        this->paused = false;
    }
    void Abort()
    {
        running = false;
        paused = false;
        fclose(pFile);
        pFile = NULL;
    }
    void Stop()
    {
        this->Abort();
    }
};
#endif