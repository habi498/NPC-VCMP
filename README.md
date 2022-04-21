# npc-vcmp
Implements Non Player Characters (NPC) for VCMP 0.4

The sub project npcclient uses three pre-built static libraries - libsquirrel_static, libsqstd_static and libnpc_static.
All libraries are included with the project. And first two libraries can be built again from https://github.com/albertodemichelis/squirrel.

The third library 'libnpc' contains a function to connect to server. It is made so not to expose some properties of vc-mp public.

Both Windows 32 and 64 bit systems use npcclient.exe, which is 32 bit. It might need vc_redist.x86

Note: The libs needed for linux x86 is yet to be made.
