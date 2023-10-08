// npc-plugin.cpp : Module to check attacks from player and reduce health and inform server.
//
#pragma warning( disable : 4103)
#if WIN32
#include <windows.h>
#else
#include <string.h>
#include <time.h>

long GetTickCount()
{
	struct timespec ts;
	long theTick = 0U;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	theTick = ts.tv_nsec / 1000000;
	theTick += ts.tv_sec * 1000;
	return theTick;
}
#endif
#include "main.h"
#include "stdio.h"
#include <map>
SQInteger SQ_Remote_Call(HSQUIRRELVM v);
SQInteger SQ_Remote_Call_Raw(HSQUIRRELVM v);
SQInteger SQ_Remote_Get(HSQUIRRELVM v);
SQInteger SQ_Remote_Set(HSQUIRRELVM v);
#define PI 3.1415926
uint32_t bytes_swap_u32(uint32_t i);
PluginFuncs* funcs;
HSQUIRRELVM v;
HSQAPI sq;
uint32_t swap4(uint32_t i) { //aa bb cc dd
	//00 00 00 aa | 00 aa bb cc | bb cc dd 00 | dd 00 00 00
	uint32_t u = (i >> 24) |
		((i >> 8) & 0x0000FF00) |
		((i << 8) & 0x00FF0000) |
		(i << 24);
	return u;
}
uint16_t swap2(uint16_t i) {
	uint16_t u = (i >> 8) | ((i << 8) & 0xFF00);
	return u;
}
SQRESULT CallFunction(const uint8_t* data, size_t size);
SQRESULT CallFunctionAdvanced(const uint8_t* data, size_t size);
void OnServerData(const uint8_t* data, size_t size)
{
	uint32_t header = *(uint32_t*)data;
	if (header == RPC_STREAM_IDENTIFIER)
	{
		int top = sq->gettop(v); 
		SQRESULT res = CallFunctionAdvanced(data + 4, size - 4);
		if (SQ_SUCCEEDED(res))
		{
			//printf("Success\n");
			sq->settop(v, top);
		}
		else printf("Function call attempt from distant failed\n");
		sq->settop(v, top);
	}
	//else printf("header different.\n");
	
}
//Increases index and push the value to the stack of VM.
SQRESULT ReadBuffer(HSQUIRRELVM v,const uint8_t* &buffer, const size_t &size, size_t &index)
{
	uint32_t dwval; float fval; VECTOR vec; QUATERNION quat;
	uint16_t wtemp; char* string; uint16_t subfunctionStreamLen; 
	uint8_t bytebool;	SQRESULT res; uint16_t arrayStreamLen;
	size_t t;
	switch (*(buffer + index++))
	{
	case 'G':
	{
		if (index + 4 <= size)
		{
			subfunctionStreamLen = swap4(*(uint32_t*)(buffer + index));
			index += 4;
			//G,PQRS,F,ABCD,00 05 w h a t i (s hello )
			if (index + subfunctionStreamLen <= size)
			{
				res = CallFunctionAdvanced(buffer + index - 5, subfunctionStreamLen + 5);
				if (SQ_FAILED(res))
				{
					return -1;
				}
				//Otherwise result is pushed into stack.
				index += subfunctionStreamLen;
			}
			else return -1;
		}
		else return -1;
		break;
	}
	case 'D':
	{
		if (index + 4 <= size)
		{
			subfunctionStreamLen = swap4(*(uint32_t*)(buffer + index));
			index += 4;
			//G,PQRS,F,ABCD,00 05 w h a t i (s hello )
			if (index + subfunctionStreamLen <= size)
			{
				res = CallFunctionAdvanced(buffer + index - 5, subfunctionStreamLen + 5);
				if (SQ_FAILED(res))
				{
					return -1;
				}
				//Otherwise result is pushed into stack.
				index += subfunctionStreamLen;
			}
			else return -1;
		}
		else return -1;
		break;
	}
	case 'E':
	{
		if (index + 4 <= size)
		{
			subfunctionStreamLen = swap4(*(uint32_t*)(buffer + index));
			index += 4;
			//F,ABCD,00 05 w h a t i (s hello )
			if (index + subfunctionStreamLen <= size)
			{
				res = CallFunctionAdvanced(buffer + index - 5, subfunctionStreamLen + 5);
				if (SQ_FAILED(res))
					return -1;
				//Otherwise result is pushed into stack.
				index += subfunctionStreamLen;
			}
			else return -1;
		}
		else return -1;
		break;
	}
	case 'F':
	{
		if (index + 4 <= size)
		{
			subfunctionStreamLen = swap4(*(uint32_t*)(buffer + index));
			index += 4;
			//F,ABCD,00 05 w h a t i (s hello )
			if (index + subfunctionStreamLen <= size)
			{
				res = CallFunctionAdvanced(buffer + index - 5, subfunctionStreamLen + 5);
				if (SQ_FAILED(res))
					return -1;
				//Otherwise result is pushed into stack.
				index += subfunctionStreamLen; 
			}
			else return -1;
		}
		else return -1;
		break;
	}
	case 'i':
	{
		if (index + 4 <= size)
		{
			dwval = *(uint32_t*)(buffer + index);
			index += 4;
			sq->pushinteger(v, dwval);
		}
		else return -1;
		break;
	}
	case 'f':
	{
		if (index + 4 <= size)
		{
			fval = *(float*)(buffer + index);
			index += 4;
			sq->pushfloat(v, fval); 
		}
		else return -1;
		break;
	}
	case 'o':
	{
		sq->pushnull(v); 
		break;
	}
	case 's':
	{
		if (index + 2 <= size)
		{
			wtemp = swap2(*(uint16_t*)(buffer + index));
			index += 2;
			//Now string length is wtemp;
			if (index + wtemp <= size)
			{
				string = (char*)(buffer + index);
				sq->pushstring(v, string, wtemp); 
				index += wtemp;
			}
			else return -1;

		}
		else return -1;
		break;
	}
	//boolean
	case 'b':
	{
		if (index + 1 <= size)
		{
			bytebool = *(buffer + index);
			if (bytebool == 1)
				sq->pushbool(v, SQTrue);
			else if (bytebool == 0)
				sq->pushbool(v, SQFalse);
			else return -1;
			index += 1;
		}
		else return -1;
		break;
	}
	case 'v':
	{
		if (index + 12 <= size)
		{
			vec.X = *(float*)(buffer + index);
			vec.Y = *(float*)(buffer + index + 4);
			vec.Z = *(float*)(buffer + index + 8);
			index += 12;
			if (SQ_SUCCEEDED(sq->pushvector(v, vec)))
			{
				return 1;
			}
			else return -1;
		}
		else return -1;
		break;
	}
	case 'q':
	{
		if (index + 16 <= size)
		{
			quat.X = *(float*)(buffer + index);
			quat.Y = *(float*)(buffer + index + 4);
			quat.Z = *(float*)(buffer + index + 8);
			quat.W = *(float*)(buffer + index + 12);
			index += 16;
			if (SQ_SUCCEEDED(sq->pushquaternion(v, quat)))
			{
				return 1;
			}
			else return -1;
		}
		else return -1;
		break;
	}
	case 'a':
	{
		if (index + 4 <= size)
		{
			arrayStreamLen = swap4(*(uint32_t*)(buffer + index));
			index += 4;
			t=index+arrayStreamLen;
			if (index + arrayStreamLen <= size)
			{
				sq->newarray(v, 0);
				while (index < t)
				{
					res = ReadBuffer(v, buffer, size, index);
					if (SQ_FAILED(res))
					{
						return -1;
					}
					sq->arrayappend(v, -2);
				}
			}
		}
		else return -1;
		break;
	}
	default:
		return sq->throwerror(v, "Unknown identifier in server data");
	}//end of switch
	return 1;
}
SQRESULT CallFunctionAdvanced(const uint8_t* data, size_t size)
{
	SQRESULT res0; size_t index; 
	bool bFunctionPushed = false;
	int top = sq->gettop(v); uint16_t nparams = 0;
	if (*data == 'G' && size >= 5 )
	{
		uint32_t GfuncStreamLen = swap4(*(uint32_t*)(data + 1));
		if (GfuncStreamLen + 5 > size)return -1;
		index = 5;
		res0=ReadBuffer(v, data, size, index);//this will push function into the stack
		if (SQ_FAILED(res0))
		{
			return -1;
		}
		if (sq->gettype(v, -1) == OT_CLOSURE || sq->gettype(v, -1) == OT_NATIVECLOSURE)
		{
			bFunctionPushed = true; 
		}
		else {
			printf("A function was not returned from squirrel while processing");
			return -1;
		}

	}else if (*data == 'D' && size >= 5)
	{
		//copied from section for 'G'
		uint32_t GfuncStreamLen = swap4(*(uint32_t*)(data + 1));
		if (GfuncStreamLen + 5 > size)return -1;
		index = 5;
		res0 = ReadBuffer(v, data, size, index);//this will push function into the stack
		if (SQ_FAILED(res0))
		{
			return -1;
		}
		if (sq->gettype(v, -1) == OT_CLOSURE || sq->gettype(v, -1) == OT_NATIVECLOSURE)
		{
			bFunctionPushed = true;
		}
		else {
			printf("A function was not returned from squirrel while processing");
			return -1;
		}

	}else if (*data == 'F' && size>=5 )
	{
		uint32_t funcStreamLen = swap4(*(uint32_t*)(data + 1));
		if (funcStreamLen + 5 > size)return -1;
		uint16_t lenfuncname = swap2(*(uint16_t*)(data + 5));
		sq->pushroottable(v);
		sq->pushstring(v, (char*)(data + 7), lenfuncname);
		if (SQ_FAILED(sq->get(v, -2)))return -1;
		sq->remove(v, -2);//remove root table pushed before function.
		//F(ABCD)(00 05)(w h a t i )i integer
		index = 5 + 2 + lenfuncname; 
	}else if (*data == 'E' && size >= 5)
	{
		uint32_t funcStreamLen = swap4(*(uint32_t*)(data + 1));
		if (funcStreamLen + 5 > size)return -1;
		uint16_t lenfuncname = swap2(*(uint16_t*)(data + 5));
		index = 5 + 2 + lenfuncname;
	}
	else return -1;
	//CallFunctionAdvanced: Now function is pushed into the stack.

	
	if (*data == 'F' || *data == 'G')
	{
		sq->pushroottable(v); nparams++;
	}

	SQRESULT res;
	while (index < size)
	{
		res = ReadBuffer(v, data, size, index);
		if (SQ_FAILED(res))
		{
			return res;
		}
		if (nparams == 0 && (*data == 'D' || *data == 'E'))
		{
			if (*data == 'E')
			{
				//Function has not been fetched
				//Do some calculations again
				uint16_t lenfuncname = swap2(*(uint16_t*)(data + 5));
				
				sq->pushstring(v, (char*)(data + 7), lenfuncname);
				if (SQ_FAILED(sq->get(v, -2)))return -1;
				//Now we have function on top and it's env on -2
				//Reorder it
				sq->push(v, -2);//env fn env
				sq->remove(v, -3);//fn env
			}
			else if (*data == 'D')
			{
				//Do nothing
			}
		}
		nparams++;
	}//end of while
	res = sq->call(v, nparams, 1, 1);
	if (SQ_SUCCEEDED(res))
	{
		sq->remove(v, -2);//the closure
	}
	else {
		sq->settop(v, top);
		sq->getlasterror(v);
		if (sq->gettype(v, -1) == OT_STRING)
		{
			const SQChar* str;
			sq->getstring(v, -1, &str);
			printf("Error: %s\n", str);
		}
	}
	return res;
}

/*Calls the squirrel function eg.F(ABCD)(00 08)(funcname)... and pushes
the result in the stack*/
SQRESULT CallFunction(const uint8_t* data, size_t size)
{
	if (*data != 'F')return -1;
	uint32_t funcStreamLen = swap4(*(uint32_t*)(data + 1));
	if (funcStreamLen + 5 > size)return -1;
	uint16_t lenfuncname = swap2(*(uint16_t*)(data + 5));
	int top = sq->gettop(v);
	sq->pushroottable(v);
	sq->pushstring(v, (char*)(data + 7), lenfuncname);
	if (SQ_FAILED(sq->get(v, -2)))return -1;
	
	uint16_t nparams = 0;
	sq->pushroottable(v); nparams++;
	
	//F(ABCD)(00 05)(w h a t i )i integer
	size_t index = 5 + 2 + lenfuncname; SQRESULT res;
	while (index < size)
	{
		res = ReadBuffer(v, data, size, index); 
		if (SQ_FAILED(res))
		{
			return res;
		}
		nparams++;
	}//end of while
	res= sq->call(v, nparams, 1, 1);
	if (SQ_SUCCEEDED(res))
	{
		sq->remove(v, -2);//the closure
		sq->remove(v, -2);//the roottable pushed in beginning.
		}
	else {
		sq->settop(v, top);
	}
	return res;
}

unsigned int PluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo)
{
	//printf("My Module Loaded\n");
	funcs = pluginFuncs;
	strcpy(pluginInfo->name, "LibRPC");
	pluginInfo->pluginVersion = 0x1101;
	pluginCalls->OnServerData = OnServerData;
	
	if (pluginInfo->apiMajorVersion != API_MAJOR ||
		pluginInfo->apiMinorVersion != API_MINOR)
		printf("	Warning: LibRPC is compiled for API: %d.%d. But the current API is %d.%d\n", API_MAJOR, API_MINOR
		, pluginInfo->apiMajorVersion, pluginInfo->apiMinorVersion);
		
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
			sq->pushroottable(v);
			sq->pushstring(v,"F",-1);
			sq->newclosure(v, SQ_Remote_Call, 0);
			sq->setparamscheck(v, 2, "ts|u");
			sq->setnativeclosurename(v, -1, "F");
			sq->newslot(v, -3, SQFalse);
			sq->pop(v, 1);

			sq->pushroottable(v);
			sq->pushstring(v, "RFC", -1);
			sq->pushnull(v);
			sq->newclosure(v, SQ_Remote_Call, 1);
			sq->setparamscheck(v, 2, "ts|u");
			sq->setnativeclosurename(v, -1, "RFC");
			sq->newslot(v, -3, SQFalse);
			sq->pop(v, 1);

			sq->pushroottable(v);
			sq->pushstring(v, "Fa", -1);
			sq->newclosure(v, SQ_Remote_Call_Raw, 0);
			sq->setparamscheck(v, 2, "ts|u");
			sq->setnativeclosurename(v, -1, "Fa");
			sq->newslot(v, -3, SQFalse);
			sq->pop(v, 1);

			sq->pushroottable(v);
			sq->pushstring(v, "RFCa", -1);
			sq->pushnull(v);
			sq->newclosure(v, SQ_Remote_Call_Raw, 1);
			sq->setparamscheck(v, 2, "ts|u");
			sq->setnativeclosurename(v, -1, "RFCa");
			sq->newslot(v, -3, SQFalse);
			sq->pop(v, 1);

			sq->pushroottable(v);
			sq->pushstring(v, "Z", -1);
			sq->newclosure(v, SQ_Remote_Get, 0);
			sq->setparamscheck(v, -2, "ts|u.");
			sq->setnativeclosurename(v, -1, "Z");
			sq->newslot(v, -3, SQFalse);
			sq->pop(v, 1);

			sq->pushroottable(v);
			sq->pushstring(v, "S", -1);
			sq->newclosure(v, SQ_Remote_Set, 0);
			sq->setparamscheck(v, -3, "ts|u..");
			sq->setnativeclosurename(v, -1, "S");
			sq->newslot(v, -3, SQFalse);
			sq->pop(v, 1);

		}
	}
	printf("	LibRPC module loaded\n");
	return 1;
}
uint32_t bytes_swap_u32(uint32_t i) { //aa bb cc dd
	//00 00 00 aa | 00 aa bb cc | bb cc dd 00 | dd 00 00 00
	uint32_t u = (i >> 24) |
		((i >> 8) & 0x0000FF00) |
		((i << 8) & 0x00FF0000) |
		(i << 24);
	return u;
}