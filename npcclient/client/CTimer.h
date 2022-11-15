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
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#endif

// Define structures for timer maps
typedef struct
{
	void* pData;
	SQObjectType datatype; // This is some shitty programming yo
} TimerParam;

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
	bool _ProcessEx();
public:

	// The interval to run it at
	DWORD intervalInTicks;

	// Number of times to run, or 0 for an infinite number of runs.
	DWORD maxNumberOfPulses;
	
	// How many times has this timer ran?
	unsigned int pulseCount;

	SQChar* pFunc;
	// The list of parameters
	std::vector<TimerParam> params;

	// The parameter count
	unsigned char paramCount;

	void SetAlarmParam() //This is for SetTimerEx
	{
		DWORD dw_Now = GetTickCount();
		this->dw_TargetTime = dw_Now + intervalInTicks;
		if (dw_TargetTime < dw_Now)
			bWrapAroundZero = true;
		else bWrapAroundZero = false;
	}
public:
	CTimer()
	{
		this->maxNumberOfPulses = 0;
		this->pulseCount = 0;
		this->pFunc = NULL;
		this->paramCount = 0;
		this->params.clear();
	}
	~CTimer()
	{
		if (this->params.size() > 0)
		{
			for (int i = 0; i < paramCount; i++)
			{
				switch (this->params[i].datatype)
				{
				case OT_STRING:
					free(this->params[i].pData);
					break;
				}
			}
			this->params.clear();
		}
	}

	CTimer(DWORD dw_Interval, const char* szName, uint8_t repeat)
	{
		this->dw_Interval = dw_Interval;
		_Update();//Sets dw_TargetTime
		this->bRepeat = repeat;
		strcpy(m_szFuncName, szName);
		this->pFunc = NULL;//compability for Ex timer.
	}
	//bool Process(DWORD dw_TickCount). Return true to delete timer. false to retain.
	bool Process(DWORD dw_TickCount)
	{
		if (dw_TickCount >= dw_TargetTime)
		{
			if (!bWrapAroundZero)
				return this->_Process();
			else 
			{
				if (dw_TickCount - dw_TargetTime < ((DWORD)~(DWORD)(0)) / 2) //half of 49.7 days
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