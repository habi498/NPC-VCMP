# npc-vcmp
Implements Non Player Characters (NPC) for VCMP 0.4

# Instructions for building the project. 

This is a Visual Studio 'CMake Project'.
## There are two sub projects. 
### 1. npcclient
This is the executable which uses Raknet to connect to a VC-MP Server.
### 2. plugin
This is a vc-mp server plugin - npc04relxx.dll/so
This plugin searches for another plugin squirrel04relxx.dll/so and connects with it, so that 
##### 1. Through squirrel scripts,  the NPCs can be created.
##### 2. Player actions can be recorded and written to recordings folder. This files are later copied to the recordings folder of npcscripts and they use it.
##### 3. Checks if a player is an NPC.

## Prebuilt static libraries
The sub project npcclient uses three pre-built static libraries - libsquirrel, libsqstd and libnpc.
All libraries are included with the project. And first two libraries can be built again from [here](https://github.com/albertodemichelis/squirrel)

The third library 'libnpc' contains a function to connect to server. It is made so not to expose some properties of vc-mp public.

## Difference in name of executables across platforms 
Both Windows 32 and 64 bit systems use npcclient.exe, which is 32 bit. It might need vc_redist.x86

In linux, 32 bit systems and 64 bit systems use different executables. They are distinguished as by appending the bitness at the end of it.

## Project Configurations
1. WINODWS_32 (Release)
2. WINDOWS_64 (Release)
3. LINUX_32 (Release)
4. LINUX_64 (Release)
