#include "main.h"
#define SQAPI(sz) sq->sz = sq_ ## sz
extern HSQAPI sq;
extern HSQUIRRELVM v;
extern SquirrelExports* pExp;

void InitSQAPI()
{
	if (sq == NULL)
		sq = new sq_api();
    /*vm*/
    SQAPI(open);
    SQAPI(newthread);
    SQAPI(seterrorhandler);
    SQAPI(close);
    SQAPI(setforeignptr);
    SQAPI(getforeignptr);
#if SQUIRREL_VERSION_NUMBER >= 300
    SQAPI(setprintfunc);
#else
    SQAPI(setprintfunc);
#endif
    SQAPI(getprintfunc);
    SQAPI(suspendvm);
    SQAPI(wakeupvm);
    SQAPI(getvmstate);

    /*compiler*/
    SQAPI(compile);
    SQAPI(compilebuffer);
    SQAPI(enabledebuginfo);
    SQAPI(notifyallexceptions);
    SQAPI(setcompilererrorhandler);

    /*stack operations*/
    SQAPI(push);
    SQAPI(pop);
    SQAPI(poptop);
    SQAPI(remove);
    SQAPI(gettop);
    SQAPI(settop);
#if SQUIRREL_VERSION_NUMBER >= 300
    SQAPI(reservestack);
#else
    SQAPI(reservestack);
#endif
    SQAPI(cmp);
    SQAPI(move);

    /*object creation handling*/
    SQAPI(newuserdata);
    SQAPI(newtable);
    SQAPI(newarray);
    SQAPI(newclosure);
    SQAPI(setparamscheck);
    SQAPI(bindenv);
    SQAPI(pushstring);
    SQAPI(pushfloat);
    SQAPI(pushinteger);
    SQAPI(pushbool);
    SQAPI(pushuserpointer);
    SQAPI(pushnull);
    SQAPI(gettype);
    SQAPI(getsize);
    SQAPI(getbase);
    SQAPI(instanceof);
#if SQUIRREL_VERSION_NUMBER >= 300
    SQAPI(tostring);
#else
    SQAPI(tostring);
#endif
    SQAPI(tobool);
    SQAPI(getstring);
    SQAPI(getinteger);
    SQAPI(getfloat);
    SQAPI(getbool);
    SQAPI(getthread);
    SQAPI(getuserpointer);
    SQAPI(getuserdata);
    SQAPI(settypetag);
    SQAPI(gettypetag);
    SQAPI(setreleasehook);
    SQAPI(getscratchpad);
    SQAPI(getclosureinfo);
    SQAPI(setnativeclosurename);
    SQAPI(setinstanceup);
    SQAPI(getinstanceup);
    SQAPI(setclassudsize);
    SQAPI(newclass);
    SQAPI(createinstance);
    SQAPI(setattributes);
    SQAPI(getattributes);
    SQAPI(getclass);
    SQAPI(weakref);
    SQAPI(getdefaultdelegate);

    /*object manipulation*/
    SQAPI(pushroottable);
    SQAPI(pushregistrytable);
    SQAPI(pushconsttable);
    SQAPI(setroottable);
    SQAPI(setconsttable);
    SQAPI(newslot);
    SQAPI(deleteslot);
    SQAPI(set);
    SQAPI(get);
    SQAPI(rawget);
    SQAPI(rawset);
    SQAPI(rawdeleteslot);
    SQAPI(arrayappend);
    SQAPI(arraypop);
    SQAPI(arrayresize);
    SQAPI(arrayreverse);
    SQAPI(arrayremove);
    SQAPI(arrayinsert);
    SQAPI(setdelegate);
    SQAPI(getdelegate);
    SQAPI(clone);
    SQAPI(setfreevariable);
    SQAPI(next);
    SQAPI(getweakrefval);
    SQAPI(clear);

    /*calls*/
    SQAPI(call);
    SQAPI(resume);
    SQAPI(getlocal);
    SQAPI(getfreevariable);
    SQAPI(throwerror);
    SQAPI(reseterror);
    SQAPI(getlasterror);

    /*raw object handling*/
    SQAPI(getstackobj);
    SQAPI(pushobject);
    SQAPI(addref);
    SQAPI(release);
    SQAPI(resetobject);
    SQAPI(objtostring);
    SQAPI(objtobool);
    SQAPI(objtointeger);
    SQAPI(objtofloat);
    SQAPI(getobjtypetag);

    /*GC*/
    SQAPI(collectgarbage);

    /*serialization*/
    SQAPI(writeclosure);
    SQAPI(readclosure);

    /*mem allocation*/
    SQAPI(malloc);
    SQAPI(realloc);
    SQAPI(free);

    /*debug*/
    SQAPI(stackinfos);
    SQAPI(setdebughook);
}
HSQAPI* pfGetSquirrelAPI()
{
    return &sq;
}
HSQUIRRELVM* pfGetSquirrelVM()
{
    return &v;
}
void InitSquirrelExports()
{
    InitSQAPI();
    pExp = new SquirrelExports;
    pExp->GetSquirrelAPI = pfGetSquirrelAPI;
    pExp->GetSquirrelVM = pfGetSquirrelVM;
    pExp->uStructSize = sizeof(SquirrelExports);
}
const void** GetSquirrelExports(size_t* sizeofExports)
{
    *sizeofExports = sizeof(SquirrelExports);
    return (const void**)&pExp;
}