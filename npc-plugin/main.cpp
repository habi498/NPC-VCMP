// npc-plugin.cpp : Defines the entry point for the application.
//
#pragma warning( disable : 4103)
#include "main.h"
#include "stdio.h"
PluginFuncs* funcs;
HSQUIRRELVM v;
HSQAPI sq;
unsigned int PluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo)
{
	//printf("My Module Loaded\n");
	funcs = pluginFuncs;
	size_t size;
	const void** sqExports = funcs->GetSquirrelExports(&size);
	if (sqExports != NULL && size > 0)
	{
		SquirrelImports** sqdf = (SquirrelImports**)sqExports;
		SquirrelImports* sqFuncs = (SquirrelImports*)(*sqdf);
		if (sqFuncs)
		{
			v = *(sqFuncs->GetSquirrelVM());
			sq = *(sqFuncs->GetSquirrelAPI());
			/*int top = sq->gettop(v);
			sq->pushroottable(v);
			sq->pushstring(v, "print", -1);
			if (SQ_SUCCEEDED(sq->get(v, -2))) {
				sq->pushroottable(v);
				sq->pushstring(v, "Hello world!", -1);
				sq->call(v, 2, SQFalse, SQTrue);
			}
			sq->settop(v, top);*/
		}
	}
	return 1;
}
