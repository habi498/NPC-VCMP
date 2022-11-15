# npc-vcmp
Implements Non Player Characters (NPC) in VC-MP 0.4 Servers

# Instructions for building the project. 

This is a Visual Studio 'CMake Project'.
## Visual Studio -> File -> Clone Repository
<img src="https://github.com/habi498/npc-vcmp/blob/master/pciture.png" width="510"></img>
## Project Configurations
1. WINODWS_32 (Release)
2. WINDOWS_64 (Release)
3. LINUX_32 (Release)
4. LINUX_64 (Release)
## Building on windows
As you can see below (section titled "There are two sub projects"), we have to build two binaries: npcclient.exe (The application which runs NPCs as client) and npc04relxx.dll (the VC-MP Server plugin).
The bitness of npcclient.exe is currently 32 bit always. This is made in view of the idea that 64 bit machines can run 32 bit applications in windows. Since, the bitness of **npcclient.exe is 32 bit**, it can be built only on the configuration WINDOWS_32.  

  Building on WINDOWS_**64** will produce the npc04rel64.dll in _out\build\WINDOWS_64\plugin_ folder.
  
  Building after selecting WINDOWS_32 will produce both npcclient.exe and the plugin npc04rel32.dll in _out\build\WINDOWS_32\npcclient_ folder and _out\build\WINDOWS_32\plugin_ folder respectively.
  
  This can be summarised on the following table:
  <img src="https://github.com/habi498/npc-vcmp/blob/master/binaries.png" width="510"></img>
## Building for LINUX on windows
You must have **Linux-development-workload** installed on Visual Studio.( See "Visual Studio Setup" and "Ubuntu on remote Linux systems" at [this](https://docs.microsoft.com/en-us/cpp/linux/download-install-and-setup-the-linux-development-workload?view=msvc-170) page.

For building binaries for the linux, you have to have login information for a linux system running remotely. On Visual Studio, go to Tools->Options->Cross Platform and Add or Edit your "connection information".
After this step, select "LINUX_32" or "LINUX_64" as the case may be and Build. This will first copy the files to remote machine after you selet configuration. Then when it is ready, and you click on build, it will build the binaries on the remote machine and copy them back to your computer. If you have selected "LINUX_32", you will have binaries (npc04rel32.so and npcclient32) on "out\build\LINUX_32". If you have selected "LINUX_64", the binaries will be npc04rel64.so and npcclient64 located on "out\build\LINUX_64".
(Note: On Linux, unlike windows the bitness problem of application npcclient does not arise, since the application can have 32 or 64 bitness )
## There are two sub projects. 
### 1. npcclient
This is the executable which uses Raknet to connect to a VC-MP Server.
### 2. plugin
This is a vc-mp server plugin - npc04relxx.dll/so
This plugin searches for another plugin squirrel04relxx.dll/so and connects with it, so that 
 1. Through squirrel scripts,  the NPCs can be created.
 2. Player actions can be recorded and written to recordings folder. This files are later copied to the recordings folder of npcscripts and they use it.
 3. Checks if a player is an NPC.

## Prebuilt static libraries
The sub project npcclient uses three pre-built static libraries - libsquirrel, libsqstd and libnpc.
All libraries are included with the project. And first two libraries can be built again from [here](https://github.com/albertodemichelis/squirrel)

The third library 'libnpc' contains a function to connect to server. It is made so not to expose some properties of vc-mp public.

## Difference in name of executables across platforms 
Both Windows 32 and 64 bit systems use npcclient.exe, which is 32 bit. It might need vc_redist.x86

In linux, 32 bit systems and 64 bit systems use different executables. They are distinguished as by appending the bitness at the end of it.

## Binaries can be downloaded from sourceforge.net
Select version 1.1 at the files over [here](https://sourceforge.net/projects/npc-for-vcmp/files/)

## NPC Application (npcclient.exe) Command-line arguments
The tclap libary has been made use and hence the the application can be run with the following parameters:
```
USAGE:

   npcclient  [-] [-h <IP address>] [-p <port>] [-z <password>] -m <scriptfile> -n <name>
   
Where:

   -h <IP address>,  --hostname <IP address>
     IP address of host

   -p <port>,  --port <port>
     Port to connect to

   -n <name>,  --name <name>
     (required) Name of the NPC

   -m <scriptfile>,  --scriptfile <scriptfile>
     (required) Squirrel Script file to be used

   -z <password>,  --password <password>
     Password of the server to connect

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   VCMP-Non Player Characters
```
Version 2.0 Major changes
Timer new implemented.
