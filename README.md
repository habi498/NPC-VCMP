# npc-vcmp
Implements Non Player Characters (NPC) for VCMP 0.4

# Instructions for building the project. 

This is a Visual Studio 'CMake Project'.

The sub project npcclient uses three pre-built static libraries - libsquirrel, libsqstd and libnpc.
All libraries are included with the project. And first two libraries can be built again from [here](https://github.com/albertodemichelis/squirrel)

The third library 'libnpc' contains a function to connect to server. It is made so not to expose some properties of vc-mp public.

Both Windows 32 and 64 bit systems use npcclient.exe, which is 32 bit. It might need vc_redist.x86

In linux, 32 bit systems and 64 bit systems use different executables. They are distinguished as by appending the bitness at the end of it.
