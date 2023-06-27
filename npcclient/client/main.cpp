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
void start_consoleinput();
CFunctions* m_pFunctions;
CPlugins* m_pPlugins;
#define NPC_DIR "npcscripts"
#define NPC_PLUGINS_DIR "npcscripts/plugins"
#ifdef LINUX
#include <time.h>
long GetTickCount()
{
    struct timespec ts;
    long theTick = 0U;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    theTick = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}
#endif

//static BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType);
int main(int argc, char** argv) {
    // Wrap everything in a try block.  Do this every time,
    // because exceptions will be thrown for problems.
    try {
        // Define the command line object.
        CmdLine cmd("VCMP-Non Player Characters v1.6 (6.27)", ' ', "0.1b",false);

        // Define a value argument and add it to the command line.
        ValueArg<string> hostnameArg("h", "hostname", "IP address of host", false, "127.0.0.1",
                                 "string");
        ValueArg<int> portArg("p", "port", "Port to connect to", false, 8192,
            "integer");
        ValueArg<string> npcnameArg("n", "name", "Name of the NPC", true, "NPC",
            "string");
        ValueArg<string> fileArg("m", "scriptfile", "Squirrel Script file to be used", false, "",
            "string");
        ValueArg<string> passwdArg("z", "password", "Password of the server to connect", false, "",
            "string");
        ValueArg<string> pluginArg("q", "plugins", "List of plugins to be loaded", false, "",
            "string");
        cmd.add(hostnameArg);
        cmd.add(portArg);
        cmd.add(npcnameArg);
        cmd.add(fileArg);
        cmd.add(passwdArg);
        ValueArg<string> LocationArg("l", "location", "The location, skin, weapon and class to spawn eg. \"x__ y__ z__ s_ w_ c_\"", false, "",
            "string");
        MultiArg<string> scriptArg("w", "params", "The params to be passed to script", false, 
            "string");
        cmd.add(LocationArg);
        cmd.add(scriptArg);
        cmd.add(pluginArg);
        SwitchArg consoleInputSwitch("c", "consoleinput", "Use Console Input", cmd, false);

        // Parse the args.
        cmd.parse(argc, argv);
        // Get the value parsed by each arg.
        string hostname = hostnameArg.getValue();
        int port = portArg.getValue();
        
        std::string npcname = npcnameArg.getValue();
        std::string npcscript = fileArg.getValue();
        std::string password = passwdArg.getValue();
        std::string location = LocationArg.getValue();
        std::vector<string> params=scriptArg.getValue();
        TimersInit();
        std::string scriptpath = "";
        if (npcscript.length() > 0)
            scriptpath = std::string(NPC_DIR + std::string("/") + npcscript);
        bool success = StartSquirrel(scriptpath.c_str(), location, params);
        if (success)
        {
             //SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
            bool bUseConsoleInput = consoleInputSwitch.getValue();
            if (bUseConsoleInput)
                start_consoleinput();

            InitSquirrelExports();
            //Need to initialize CFunctions class, as CPlugins need it.
            
            m_pFunctions = new CFunctions();
            if (!m_pFunctions)
                exit(0);
            m_pPlugins = new CPlugins();
            if (!m_pPlugins)
                exit(0);
            if (pluginArg.getValue().length() > 0)
            {
                 m_pPlugins->LoadPlugins(NPC_PLUGINS_DIR, pluginArg.getValue());
            }
            int s= LoadScript(scriptpath.c_str(), params);
            if (s)
                ConnectToServer(hostname, port, npcname, password);
            else
                goto error;
        }
        else {
        error:
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