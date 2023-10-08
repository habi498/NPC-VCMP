#include "main.h"
#include <stdio.h>
extern PluginFuncs* funcs;
extern HSQUIRRELVM v;
extern HSQAPI sq;
extern NPC npc;
SQInteger register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char* fname, SQInteger nparamscheck, const SQChar* typemask)
{
    sq->pushroottable(v);
    sq->pushstring(v, fname, -1);
    sq->newclosure(v, f, 0); //create a new function
    if (nparamscheck != 0) {

        sq->setparamscheck(v, nparamscheck, typemask); /* Add a param type check */
    }
    sq->setnativeclosurename(v, -1, fname);
    sq->newslot(v, -3, SQFalse);
    sq->pop(v, 1); //pops the root table
    return 0;
}
SQInteger fn_ShootPlayer(HSQUIRRELVM v)
{
    SQInteger playerId;
    sq->getinteger(v, 2, &playerId);
    if (funcs->IsPlayerStreamedIn(playerId))
    {
        uint8_t weapon = SelectGun();
        if (weapon != GUN_NOT_AVAILABLE)
        {
            npc.m_bHasTarget = true;
            npc.m_bIsReloadingWeapon = false;
            npc.m_bTargetOutOfRange = false;
            npc.m_byteTargetId = playerId;
            npc.m_dwLastFired = 0;
            npc.m_dwNextCycle = 0;
            sq->pushbool(v, SQTrue);
            return 1;
        }
    }
    sq->pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_ClearTarget(HSQUIRRELVM v)
{
    if (npc.m_bHasTarget)
    {
        npc.ClearTarget(REASON::INSTRUCTION_RECEIVED);
        sq->pushbool(v, SQTrue);
    }
    sq->pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_GetTarget(HSQUIRRELVM v)
{
    if (npc.m_bHasTarget)
    {
        sq->pushinteger(v, npc.m_byteTargetId);
        return 1;
    }
    else sq->pushinteger(v, -1);
    return 1;
}
void RegisterFunctions()
{
    register_global_func(v, ::fn_ShootPlayer, "ShootPlayer", 2, "ti");
    register_global_func(v, ::fn_ClearTarget, "ClearTarget", 1, "t");
    register_global_func(v, ::fn_GetTarget, "GetTarget", 1, "t");
}
SQInteger RegisterSquirrelConst(HSQUIRRELVM v, const SQChar* cname, SQInteger cvalue) 
{
    sq->pushconsttable(v);
    sq->pushstring(v, cname, -1);
    sq->pushinteger(v, cvalue);
    sq->newslot(v, -3, SQFalse);
    sq->pop(v, 1);
    return 0;
}