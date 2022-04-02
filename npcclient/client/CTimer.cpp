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
#include "main.h"
static const int maxTimers = 255;
CTimer* pTimerArray[maxTimers];
extern HSQUIRRELVM v;
void TimersInit()
{
	unsigned int i;
	for (i = 0; i < maxTimers; i++)
		pTimerArray[i] = NULL;
}
int AddTimer(CTimer* pTimer)
{
	for (int i = 0; i < maxTimers; i++)
	{
		if (pTimerArray[i] == NULL)
		{
			pTimerArray[i] = pTimer;
			return i;
		}
	}
	return maxTimers;
}
void ProcessTimers(DWORD dw_TickCount)
{
	for (int i = 0; i < maxTimers; i++)
	{
		if (pTimerArray[i] != NULL)
		{
			if (pTimerArray[i]->Process(dw_TickCount) == true)
			{
				KillTimer(i);
			}
		}
	}
}

void KillTimer(int TimerID)
{
	if (TimerID >= 0 && TimerID < maxTimers)
	{
		if (pTimerArray[TimerID] != NULL)
		{
			delete pTimerArray[TimerID];
			pTimerArray[TimerID] = NULL;
		}
	}
}
bool CTimer::_Process()
{
	bool bFuncExists = true;
	int top = sq_gettop(v); //saves the stack size before the call
	sq_pushroottable(v); //pushes the global table
	sq_pushstring(v, _SC(m_szFuncName), strlen(m_szFuncName));
	if (SQ_SUCCEEDED(sq_get(v, -2))) { //gets the field 'foo' from the global table
		sq_pushroottable(v); //push the 'this' (in this case is the global table)
		sq_call(v, 1, 0, 1); //calls the function 
	}
	else bFuncExists = false;
	sq_settop(v, top); //restores the original stack 
	if (!bFuncExists)return false;//Function does not exist. Kill the timer.
	if (!bRepeat)
		return true;
	else
	{
		_Update();
		return false;
	}
}