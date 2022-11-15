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
#include <string>
#include <algorithm>
#include "tclap/CmdLine.h"
#include "main.h"
using namespace TCLAP;
using namespace std;
#define NPC_DIR "npcscripts"
#ifdef LINUX
#include <time.h>

long GetTickCount()
{
    tms tm;
    return (times(&tm) * 10);
}
#endif
//static BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType);
int main(int argc, char **argv) {
    // Wrap everything in a try block.  Do this every time,
    // because exceptions will be thrown for problems.
    try {
        // Define the command line object.
        CmdLine cmd("VCMP-Non Player Characters", ' ', "0.1b",false);

        // Define a value argument and add it to the command line.
        ValueArg<string> hostnameArg("h", "hostname", "IP address of host", false, "127.0.0.1",
                                 "IP address");
        ValueArg<int> portArg("p", "port", "Port to connect to", false, 8192,
            "port");
        ValueArg<string> npcnameArg("n", "name", "Name of the NPC", true, "NPC",
            "name");
        ValueArg<string> fileArg("m", "scriptfile", "Squirrel Script file to be used", true, "npc",
            "scriptfile");
        ValueArg<string> passwdArg("z", "password", "Password of the server to connect", false, "",
            "password");
        cmd.add(hostnameArg);
        cmd.add(portArg);
        cmd.add(npcnameArg);
        cmd.add(fileArg);
        cmd.add(passwdArg);
        // Parse the args.
        cmd.parse(argc, argv);
        // Get the value parsed by each arg.
        string hostname = hostnameArg.getValue();
        int port = portArg.getValue();
        std::string npcname = npcnameArg.getValue();
        std::string npcscript = fileArg.getValue();
        std::string password = passwdArg.getValue();
        TimersInit();
        bool success = StartSquirrel(std::string(NPC_DIR+std::string("/")+npcscript).c_str());
        if (success)
        {
             //SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
            ConnectToServer(hostname, port, npcname, password);
        }
        else {
            printf("Error in loading script file. Does it exist?");
            exit(0);
        }
        
    } catch (ArgException &e)  // catch any exceptions
    {
        cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    }
}
// Handler function will be called on separate thread!
/*Function Credit: https://asawicki.info/news_1465_handling_ctrlc_in_windows_console_application*/
/*static BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType)
{
   switch (dwCtrlType)
    {
    case CTRL_C_EVENT: // Ctrl+C  
    case CTRL_BREAK_EVENT: // Ctrl+Break
    case CTRL_CLOSE_EVENT: // Closing the console window
    case CTRL_LOGOFF_EVENT: // User logs off. Passed only to services!
    case CTRL_SHUTDOWN_EVENT: // System is shutting down. Passed only to services!
    StopSquirrel();
    }

    // Return TRUE if handled this message, further handler functions won't be called.
    // Return FALSE to pass this message to further handlers until default handler calls ExitProcess().
    return FALSE;
}*/