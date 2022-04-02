/*
	Copyright Stormeus 2014-2018
	modified by habi 2022
*/

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
#ifndef CTIMER_H
#define CTIMER_H
#define MAX_FUNC_NAME 30
#include <stdio.h>
#include <stdint.h>
#ifdef _WIN32
#include <Windows.h>
#endif
class CTimer
{
private:
	bool bWrapAroundZero;
	DWORD dw_TargetTime;
	DWORD dw_Interval;
	bool bRepeat;
	CHAR	m_szFuncName[MAX_FUNC_NAME + 1];
	bool _Process();
	void _Update()
	{
		DWORD dw_Now = GetTickCount();
		this->dw_TargetTime = dw_Now + dw_Interval;
		if (dw_TargetTime < dw_Now)
			bWrapAroundZero = true;
		else bWrapAroundZero = false;
	}
public:
	CTimer(DWORD dw_Interval, const char* szName, uint8_t repeat)
	{
		this->dw_Interval = dw_Interval;
		_Update();//Sets dw_TargetTime
		this->bRepeat = repeat;
		strcpy(m_szFuncName, szName);
	}
	~CTimer() {  };
	//bool Process(DWORD dw_TickCount). Return true to delete timer. false to retain.
	bool Process(DWORD dw_TickCount)
	{
		if (dw_TickCount >= dw_TargetTime)
		{
			if (!bWrapAroundZero)
				return this->_Process();
			else 
			{
				if (dw_TickCount - dw_TargetTime < sizeof(DWORD) / 2) //half of 49.7 days
				{
					return this->_Process();
				}//here else means target time not yet achieved. so wait.
				else return false;
			}
		}//else timer not yet matured. wait.
		else return false;
	}
};
void TimersInit();
int AddTimer(CTimer* pTimer);
void KillTimer(int TimerID);
void ProcessTimers(DWORD dw_TickCount);
#endif