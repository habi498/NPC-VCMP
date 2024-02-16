#include "SQFuncs.h"
extern HSQUIRRELVM v;
extern HSQAPI sq;
extern PluginFuncs* VCMP;
extern CPlayerPool* m_pPlayerPool;
SQRESULT CallFunction(const uint8_t* data, size_t size);
SQRESULT CallFunctionAdvanced(const uint8_t* data, size_t size);
void NPC04_OnClientScriptData(int32_t playerId, const uint8_t* data, size_t size)
{
	uint32_t header = *(uint32_t*)data;
	if (header == RPC_STREAM_IDENTIFIER)
	{
		if (VCMP->IsPlayerAdmin(playerId))
		{
			SQInteger top = sq->gettop(v);
			SQRESULT res = CallFunctionAdvanced(data + 4, size - 4);
			if (SQ_SUCCEEDED(res))
			{
				//printf("Success\n");
				sq->settop(v, top);
			}
			else printf("Function call attempt from distant failed\n");
			sq->settop(v, top);
		}
		else VCMP->LogMessage("Remote Call: Player %d must be admin", playerId);
	}
	if (!m_pPlayerPool->GetSlotState(playerId))return;
	CPlayer* m_pPlayer = m_pPlayerPool->GetAt(playerId);
	if (!m_pPlayer->IsRecording())return;

	if (m_pPlayer->m_RecordingType == PLAYER_RECORDING_TYPE_ALL &&
		(
			m_pPlayer->m_dwRecFlags & REC_CLIENTSCRIPT_DATA
			))
	{
		GENERALDATABLOCK datablock;
		datablock.time = GetTickCount();
		datablock.bytePacketType = PACKET_CLIENTSCRIPT_DATA;
		size_t count = fwrite(&datablock, sizeof(datablock), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
		COMMANDDATA csdata;
		csdata.len = (uint16_t)size;
		count = fwrite(&csdata, sizeof(csdata), 1, m_pPlayer->pFile);
		if (count != 1)
		{
			m_pPlayer->Abort(); return;
		}
		count = fwrite(&data, sizeof(uint8_t), csdata.len, m_pPlayer->pFile);
		if (count != csdata.len)
		{
			m_pPlayer->Abort(); return;
		}
	}
}
uint32_t swap4(uint32_t i);
uint16_t swap2(uint16_t i);
//Pushes the vector  to the stack
SQRESULT sq_pushvector(HSQUIRRELVM v, SQFloat x, SQFloat y, SQFloat z)
{
	SQInteger top = sq->gettop(v);
	sq->pushroottable(v); //t
	sq->pushstring(v, "Vector", -1); //ts
	if (SQ_FAILED(sq->get(v, -2))) //ty
	{
		sq->settop(v, top);
		return -1;
	}
	sq->pushnull(v); //tyo
	sq->pushfloat(v, x);//tyof
	sq->pushfloat(v, y);//tyoff
	sq->pushfloat(v, z);//ty offf
	//call Vector
	SQRESULT res = sq->call(v, 4, SQTrue, SQTrue); //ty x
	if (SQ_SUCCEEDED(res))
	{
		sq->remove(v, -2); //t x
		sq->remove(v, -2); //x
		return 1;
	}
	else {
		sq->settop(v, top);
		return -1;
	}
}

//Pushes the quaternion  to the stack
SQRESULT sq_pushquaternion(HSQUIRRELVM v, SQFloat x, SQFloat y, SQFloat z, SQFloat w)
{
	SQInteger top = sq->gettop(v);
	sq->pushroottable(v); //t
	sq->pushstring(v, "Quaternion", -1); //ts
	if (SQ_FAILED(sq->get(v, -2))) //ty
	{
		sq->settop(v, top);
		return -1;
	}
	sq->pushnull(v); //tyo
	sq->pushfloat(v, x);//tyof
	sq->pushfloat(v, y);//tyoff
	sq->pushfloat(v, z);//ty offf
	sq->pushfloat(v, w);//ty offff
	//call Quaternion
	SQRESULT res = sq->call(v, 5, SQTrue, SQTrue); //ty x
	if (SQ_SUCCEEDED(res))
	{
		sq->remove(v, -2); //t x
		sq->remove(v, -2); //x
		return 1;
	}
	else {
		sq->settop(v, top);
		return -1;
	}
}

//Increases index and push the value to the stack of VM.
SQRESULT ReadBuffer(HSQUIRRELVM v, const uint8_t*& buffer, const size_t& size, size_t& index)
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
	case 'g'://get
	{
		if (index + 4 <= size)
		{
			uint32_t StreamLen = swap4(*(uint32_t*)(buffer + index));
			if (StreamLen + 5 > size)return -1;
			index += 4;
			size_t i = index + StreamLen;
			SQRESULT res0 = ReadBuffer(v, buffer, size, index);//this will push something into the stack
			if (SQ_FAILED(res0))
			{
				return -1;
			}
			//Special case below: if string, get it from roottable
			if (sq->gettype(v, -1) == OT_STRING)
			{
				sq->pushroottable(v);
				sq->push(v, -2);
				sq->remove(v, -3);//remove the string
				if (SQ_FAILED(sq->get(v, -2)))
				{
					return -1;
				}
				//now something got through 'get' is on top
				sq->remove(v, -2);//roottable
			}
			while (size > index && i>index) //more parameters
			{
				//Second parameter provided
				res0 = ReadBuffer(v, buffer, size, index);//this will push the key into the stack
				if (SQ_FAILED(res0))
				{
					return -1;
				}

				if (SQ_SUCCEEDED(sq->get(v, -2)))
				{
					sq->remove(v, -2);//the object at -2.
				}
				else return -1;
			}
			index = i;//actually index is already equal to i
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
			if (SQ_SUCCEEDED(sq_pushvector(v, vec.X, vec.Y, vec.Z)))
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
			if (SQ_SUCCEEDED(sq_pushquaternion(v, quat.X, quat.Y, quat.Z, quat.W)))
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
			t = index + arrayStreamLen;
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
	SQInteger top = sq->gettop(v); uint16_t nparams = 0;
	if (*data == 'G' && size >= 5)
	{
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

	}
	else if (*data == 'D' && size >= 5)
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

	}
	else if (*data == 'F' && size >= 5)
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
	}
	else if (*data == 'E' && size >= 5)
	{
		uint32_t funcStreamLen = swap4(*(uint32_t*)(data + 1));
		if (funcStreamLen + 5 > size)return -1;
		uint16_t lenfuncname = swap2(*(uint16_t*)(data + 5));
		index = 5 + 2 + lenfuncname;
	}
	else if (*data == 'h' && size >=5)
	{
		SQInteger top = sq->gettop(v);
		uint32_t StreamLen = swap4(*(uint32_t*)(data + 1));
		if (StreamLen + 5 > size)return -1;
		index = 5;
		SQRESULT res0 = ReadBuffer(v, data, size, index);//this will push something into the stack
		if (SQ_FAILED(res0))
		{
			return -1;
		}
		bool envIsRootTable = false;
		if (sq->gettype(v, -1) == OT_STRING)
		{
			envIsRootTable = true;
			sq->pushroottable(v);
			sq->push(v, -2);//the string as key
			sq->remove(v, -3);//the string before roottable
		}
		if (size <= index)
			return -1;
		res0 = ReadBuffer(v, data, size, index);//this will push the key or value(if envIsRootTable) into the stack
		if (SQ_FAILED(res0))
		{
				return -1;
		}
		if (!envIsRootTable)//read value
		{
			if (size <= index)
				return -1; 
			res0 = ReadBuffer(v, data, size, index);//this will push the value into the stack
			if (SQ_FAILED(res0))
			{
				return -1;
			}
		}
		//Store the key and value
		HSQOBJECT obj,key, val;
		sq->getstackobj(v, -3, &obj);
		sq->getstackobj(v, -2, &key);
		sq->getstackobj(v, -1, &val);
		if (SQ_FAILED(sq->set(v, -3)))
		{
			//Check if key exist
			sq->pushobject(v, obj);
			sq->pushobject(v, key);
			if (SQ_FAILED(sq->get(v, -2)))
			{
				sq->pushobject(v, key);
				sq->pushobject(v, val);
				if (SQ_FAILED(sq->newslot(v, -3, SQFalse)))
					return -1;
			}
			else 
				return -1;
		}
		sq->settop(v, top);//the object on which set operation was performed
		return 0;//unlike other cases here we return immediately
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
				//Do nothing. We have parameter one(env) on top
			}
		}
		nparams++;
	}//end of while
	if (sq->gettype(v, -nparams - 1) == OT_CLOSURE ||
		sq->gettype(v, -nparams - 1) == OT_NATIVECLOSURE)
	{
		res = sq->call(v, nparams, 1, 1);//Call it
//else if(nparams==1)

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
	}
	else {
		sq->settop(v, top); printf("Error: Not a function. ");
		return -1;
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
	SQInteger top = sq->gettop(v);
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
	res = sq->call(v, nparams, 1, 1);
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
