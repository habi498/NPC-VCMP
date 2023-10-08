# npc-vcmp
Implements Non Player Characters (NPC)  also called bots in VC-MP 0.4 Servers

# Instructions for building the project. For Visual Studio 
Open a command prompt in the directory containing this README.md
mkdir out
cd out
cmake ../ -A Win32
Now you can see visual studio project in out directory. Edit it and you can build it also.

But the above process will not build 64-bit plugin called npc04rel64.dll.
To do so, again open a command prompt in the directory containing this README.md
mkdir out-64
cd out-64
cmake ../

#Instructions for building the project. For VSCode


## Difference in name of executables across platforms 
Both Windows 32 and 64 bit systems use npcclient.exe, which is 32 bit. 

In linux, 32 bit systems and 64 bit systems use different executables. They are distinguished as by appending the bitness at the end of it.

## Binaries can be downloaded from sourceforge.net
Select latest version( 1.6 beta 5 as on July.2023) at the files over [here](https://sourceforge.net/projects/npc-for-vcmp/files/)

## NPC Application (npcclient.exe) Command-line arguments
The tclap libary has been made use and hence the the application can be run with the following parameters:
```
USAGE:

   npcclient  [-c] [-h <string>] [-l <string>] [-m <string>] [-p <integer>]
              [-q <string>] [-w <string> ...] [-z <string>] -n <string>


Where:

   -h <string>,  --hostname <string>
     IP address of host

   -p <integer>,  --port <integer>
     Port to connect to

   -n <string>,  --name <string>
     (required) Name of the NPC

   -m <string>,  --scriptfile <string>
     Squirrel Script file to be used

   -z <string>,  --password <string>
     Password of the server to connect

   -l <string>,  --location <string>
     The location, skin, weapon and class to spawn eg. "x__ y__ z__ s_ w_
     c_"

   -w <string>,  --params <string>  (accepted multiple times)
     The params to be passed to script

   -q <string>,  --plugins <string>
     List of plugins to be loaded

   -c,  --consoleinput
     Use Console Input

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   VCMP-Non Player Characters v1.6 (6.27)
```

## Version Changes since November 2022  
**1.2**  
--Implemented function SendOnFootSyncData through which NPCs can shoot by normal weapons.  
--Only windows builds were made  

**1.3** (15.11.2022)  
--Timer new implemented SetTimerEx  
--Fixed npc running unwantedly due to an error while writing 4 bits through raknet.  
--Fixed potential issue of not killing Timer if function not found at proper time.  
--Added ability for npc to shoot through sniper rifle.  
--Added boolean parameter for crouching in SendOnFootSyncData.  
--Windows builds were made.  

**Version 1.5** ( 05.12.2022 )
1. NPCs can be called with (l flag) -l "xPos yPos zPos aAngle sSkin wWeapon cClass" where
   Pos and Angle are floating points. The npc after connecting
   will spawn at the Pos specified with angle Angle.
2. Arguments can be passed to the npc script via w flag. (-w "Sniper" ). See (3)
3. OnNPCScriptLoad() is changed to OnNPCScriptLoad(params)
    where params is an array of strings and whose values are the arguments used in calling the program 
    with w flag. See (2). 
4. Has made -m scriptfile argument optional. Now NPCs can be connected
without script.
wiki available at https://npc-for-vcmp.sourceforge.io/wikiw/index.php

**Version 1.5 patch 1** (23.Dec.2022)
1. Added Support for npc as passenger.
2. Support for getting Team and Skin of players.

**Version 1.6 alpha**(8.Jan.2023)  
--Support for both rel004 and rel006 servers  
--NPC now can send vehicle syncs (as driver)  
--Introduced npc-plugins which can be loaded with the program.  
--Added Vector, Quaternion classes and updated parameters of few functions which are using them.  

**Version 1.6 beta**(4.Feb.2023)  
--Supports reading and writing ServerData(Streams)  
--LibRPC (Remote Procedure Calls) module available and included in npcscripts/plugins so that functions of NPC can be called from Server side( See RFC, F )  
--Fixed a major bug while using ConsoleInput in linux( program would wait indefinitely unless Enter key is pressed )  
--The functions of npc04relxx are exported.  
--NPC dies if health set to zero from server  

**Version 1.6 beta2 and patch1**(14-16.Feb.2023)  
--Now Vector.Length() and Vector.Normalized() functions to get magnitude and normalized copy of vector respectively.  
--Added fields for SetLocalValue, GetLocalValue: V_ONFOOT_SPEED, V_AIMPOS, V_AIMDIR, V_POS, Q_CAR_ROTATION,
F_CAR_HEALTH, I_CAR_DAMAGE, V_CAR_SPEED, F_CAR_TURRETX, F_CAR_TURRETY.  
--LibRPC and npc04relxx update to call function returned by remote function.  
--Bug fixed for function 'EnterVehicle'.  
--Bug fixed npc getting kicked for 'acquiring weapon' when exiting vehicle.  

**Version 1.6 beta3** (20.Feb 2023)  
--Plugin API v1.1  
--New module which has inbuilt timer and other functions which
enables npc to load ammo and shoot at any player. LibAction.

**Version 1.6 beta 4** (05. May. 2023)  
--fixed Q_CAR_ROTATION returning float instead of Quaternion.  
--fixed SendInCarSyncData function not available.  
--Earlier plugin npc04relxx do not work when some other plugins are loaded. It is fixed.  
--New functions and events of sending Projectiles and Explosions.  
--LibRPC now supports not only calling of functions of npcscripts from server, but reverse also provided npc is given admin status (for security reasons).  
--Script will load after plugins are loaded. (patch1)  
--API is updated to 1.3 (patch2) from 1.2 (patch1)  
--New functions RequestClass, RequestSpawn and event OnNPCClassSelect  

**Version 1.6 beta 5**( 27. June. 2023)  
--ConnectNPCEx takes position as Vector instead of x,y,z.  
--LibRPC functions extended to perform sq_get and sq_set on server vm from npcclient.  
--Fixed bug when onPlayerRequestClass return 0 on server. 

**Version 1.7** 

--Pickup Support 

--Bug fixes 

  1. npc getting kicked if shooting another player and npc has armour 
  2. npc crashing if Vector class tostring method used.
  3. many more

--Events added: OnTimeWeatherSync, OnPickupStreamIn, OnPickupDestroyed  
--Functions new: IsPickupStreamedIn, GetPickupModel, GetPickupAlpha, GetPickupQuantity, GetPickupPos, GetPickupCount

