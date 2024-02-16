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
#define PLAY_IGNORE_PASSENGER_HEALTH 1
#define PLAY_IGNORE_SEATID    2
#define PLAY_IGNORE_VEHMODEL 4
#define PLAY_IGNORE_VEHICLEID 8
#define PLAY_IGNORE_CHECKPOINTS 16
#define PLAY_IGNORE_CHECKPOINT_POSITIONS 32
#define PLAY_IGNORE_UNSTREAMED_PICKUPS 64
#define PLAY_IGNORE_PICKUP_MODEL   128
#define PLAY_IGNORE_UNSTREAMED_OBJECTS   256
#define PLAY_IGNORE_OBJECT_MODEL   512
#define PLAY_AVOID_OBJECTSHOT_WEAPONCHECK   1024
bool GetNameFromRecFile(FILE* file, char* name, size_t size);
struct Playback
{
private:
    bool paused = false;
public:
    DWORD dw_pausedTime = 0;
    uint32_t identifier;
    FILE* pFile;
    uint8_t type;//PLAYER_RECORDING_TYPE_DRIVER or PLAYER_RECORDING_TYPE_ONFOOT or PLAYER_RECORDING_ALL
    bool running = false;
    DWORD nexttick;//When Gettickcount()>=nexttick, send packet
    DWORD prevtick;//storing tickcount of previous packet.
    DWORD dw_VehicleRequestedTime = 0;
    uint32_t dw_Flags = 0;
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