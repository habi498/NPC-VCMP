# npc-vcmp
Implements Non Player Characters (NPC) for VCMP 0.4

The sub project npcclient uses three pre-built static libraries - libsquirrel_static, libsqstd_static and libnpc_static.
The first two are built from https://github.com/albertodemichelis/squirrel. ( Windows 32 bit and Linux 64 bit are working. Linux 32 bit is still to be made. )

libnpc contains a function to connect to server. It is made so not to expose some properties of vc-mp public.

A note regarding bitness of outputs
Both Windows 32 and 64 bit systems use npcclient.exe, which is 32 bit. It might need vc_redist.x86
