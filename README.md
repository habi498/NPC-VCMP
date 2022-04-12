# npc-vcmp
Implements Non Player Characters (NPC) for VCMP 0.4

The project uses three pre-built static libraries - libsquirrel, libsqstd and libnpc.
The first two are built from https://github.com/albertodemichelis/squirrel with linker flag -m32  added for 32-bit version.

libnpc contains a function to connect to server. It is made so not to expose some properties of vc-mp public.

A note regarding bitness of outputs
The program npcclient.exe is supposed to be 32 bit. It might need vc_redist.x86.
In linux, npcclient32 is 32-bit and npcclient64 is 64 bit.
