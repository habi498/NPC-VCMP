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

#include <cstdio>
void call_OnConsoleInput(char* input);

bool bConsoleInputEnabled = false;
void CheckForConsoleInput();
void start_consoleinput();
#ifdef WIN32
#include <windows.h>
#include <process.h>
LONG flag = FALSE;
char input[1024];
static void mynewthread(void* p)
{
    while (true)
    {
        char mystring[1024];
        if (fgets(mystring, sizeof(mystring), stdin) != NULL)
        {
            if (strlen(mystring) < sizeof(mystring) - 1)
                mystring[strlen(mystring) - 1] = 0;
            strcpy_s(input, sizeof(mystring), mystring);
            InterlockedExchange(&flag, TRUE);
        }
    }
}
void start_consoleinput()
{
    _beginthread(mynewthread, 0, NULL);
    bConsoleInputEnabled = true;
}

void CheckForConsoleInput()
{
    if (InterlockedCompareExchange(&flag, FALSE, TRUE))
    {
        call_OnConsoleInput(input);
    }
}
#else
/* Standard include files. stdio.h is required. */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

/* Used for select(2) */
#include <sys/types.h>
#include <sys/select.h>

#include <signal.h>
#include <errno.h>
#include <stdio.h>

/* Standard readline include files. */
#include <readline/readline.h>
#include <readline/history.h>
bool callback_handler_removed = false;
int sigwinch_received = false;
/* Handle SIGWINCH and window size changes when readline is not active and
   reading a character. */
static void sighandler(int sig)
{
    sigwinch_received = 1;
}
static void input_linehandler(char* line)
{
    if (line != NULL)
    {
        if (*line)
            add_history(line);
        call_OnConsoleInput(line);
        free(line);
    }
    else
    {
        rl_callback_handler_remove();
        callback_handler_removed = true;
    }
}
void start_consoleinput()
{
    /* Handle window size changes when readline is not active and reading
       characters. */
    signal(SIGWINCH, sighandler);

    /* Install the line handler. */
    rl_callback_handler_install(NULL, input_linehandler);

    bConsoleInputEnabled = true;
}
void CheckForConsoleInput()
{
    fd_set fds;
    int r; 
    if (callback_handler_removed)
        return;
    FD_ZERO(&fds);
    FD_SET(fileno(rl_instream), &fds);

    r = select(FD_SETSIZE, &fds, NULL, NULL, NULL);
    if (r < 0 && errno != EINTR)
    {
        perror("error on select");
        rl_callback_handler_remove();
        callback_handler_removed = true;
        return;
    }
    if (sigwinch_received)
    {
        rl_resize_terminal();
        sigwinch_received = 0;
    }
    if (r < 0)
        return;

    if (FD_ISSET(fileno(rl_instream), &fds))
        rl_callback_read_char();
}
void stop_consoleinput()
{
    rl_callback_handler_remove();
    callback_handler_removed = true;
}
#endif