#include "SQFuncs.h"
extern PluginFuncs* VCMP;
extern HSQAPI sq;
#define RPC_CALL 0x1
#define RPC_FUNCTION 0x2
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
SQRESULT Function333(HSQUIRRELVM v, SQInteger stackIndex, uint8_t*& data, size_t& size);
SQInteger SQ_RemoteFunction(HSQUIRRELVM v)
{
	SQInteger nargs =sq->gettop(v);
    SQInteger typeRPC; 
    sq->getinteger(v, nargs--, &typeRPC); 
    const SQChar* funcname; 
	sq->getstring(v, nargs-- , &funcname);
    SQInteger dwInteger;
    if (typeRPC == RPC_CALL)
        sq->getinteger(v, nargs--, &dwInteger);
    uint8_t* data = (uint8_t*)calloc(5, sizeof(char));
    size_t size = 5; uint8_t* temp;
    
    if (data)
    {
        //*(uint32_t*)(data) = RPC_STREAM_IDENTIFIER;
        data[0] = (char)('F');
        //fill *(uint32_t*)(data+1) with length of func data later.
        uint16_t len;
        len = (uint16_t)strlen(funcname);
        if (len < 0)
        {
            free(data); return sq->throwerror(v, "Unknown error when computing string length 1");
        }
        temp = (uint8_t*)realloc(data, size + 2 + len);
        if (!temp) { 
            free(data); 
            return sq->throwerror(v, "Error in allocating memory"); 
        }
        data = temp; size += 2 + len;
        *(uint16_t*)(data+size - len - 2) = swap2(len);
        
        memcpy((void*)&data[size - len], funcname, len);
        
        for (SQInteger n = 2; n <= nargs; n++)
        {
            SQRESULT res = Function333(v, n, data, size);
            if(SQ_FAILED(res)) {
                return res;//No need to free data as Function333 will do it
            }
        }
    }
    else return sq->throwerror(v, "Memory allocation failed\n");
    
    //Now data buffer is created !
    //size is the total size of buffer. Now size-4 is the total size of stream (excluding first four bytes)
    //write size of stream at beginning of data
    //*(uint32_t*)data = swap4(size - 4); printf("size is %d\n",size);
    //VCMP->SendClientScriptData(playerid, (const void*) data, 8); printf("--");
    //vcmpError e=VCMP->SendClientScriptData(playerid, (const void*)data, size); 
    *(uint32_t*)(data + 1) = swap4((uint32_t)size - 5);//customary to swap length of following item
    if (typeRPC == RPC_FUNCTION)
    {
        SQUserPointer p = sq->newuserdata(v, size);
        memcpy(p, data, size);
        free(data); 
        return 1;
    }
    else if (typeRPC == RPC_CALL)
    {
        SQInteger playerid = dwInteger;
        //sq->getinteger(v, sq->gettop(v) - 2, &playerid);
        temp = (uint8_t*)realloc(data, size + 4);
        if (!temp) {
            free(data);
            return sq->throwerror(v, "Error in allocating memory");
        }
        data = temp; size += 4;
        memmove(data + 4, data, size - 4);
        *(uint32_t*)(data) = RPC_STREAM_IDENTIFIER;
        vcmpError e=VCMP->SendClientScriptData((int32_t)playerid, (const void*)data, size);
        free(data);
        if (e == vcmpErrorNone)sq->pushbool(v, SQTrue);
        else sq->pushbool(v, SQFalse);
        return 1;
    }
    else return 0;
}
SQRESULT Function333(HSQUIRRELVM v, SQInteger stackIndex, uint8_t* &data, size_t &size)
{
    uint8_t* temp;
    temp = (uint8_t*)realloc(data, size + 1);
    if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
    data = temp; size += 1;
    SQUnsignedInteger sizeOfUserData, sizeOfUserDataActual; uint16_t len;
    SQUserPointer userdata;

    switch(sq->gettype(v, stackIndex))
    {
    case OT_NULL:
        data[size - 1] = (char)('o');
        break;
    case OT_INTEGER:
        data[size - 1] = (char)('i');
        temp = (uint8_t*)realloc(data, size + 4);
        if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
        data = temp; size += 4;
        sq->getinteger(v, stackIndex, (SQInteger*)&data[size - 4]);
        break;
    case OT_FLOAT:
        data[size - 1] = (char)('f');
        temp = (uint8_t*)realloc(data, size + 4);
        if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
        data = temp; size += 4;
        sq->getfloat(v, stackIndex, (SQFloat*)&data[size - 4]);
        break;
    case OT_STRING:
        data[size - 1] = (char)('s');
        const SQChar* str;
        sq->getstring(v, stackIndex, &str);
        len = (uint16_t)strlen(str);
        if (len < 0)
        {
            free(data); return sq->throwerror(v, "Unknown error when computing string length 2");
        }
        temp = (uint8_t*)realloc(data, size + 2 + len);
        if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
        data = temp; size += 2 + len;
        *(uint16_t*)&data[size - len - 2] = swap2(len);
        memcpy((void*)&data[size - len], str, len);
        break;
    case OT_BOOL:
        data[size - 1] = (char)('b');
        temp = (uint8_t*)realloc(data, size + 1);
        if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
        data = temp; size += 1;
        SQBool b;
        sq->getbool(v, stackIndex, &b);
        if (b == SQTrue)
            data[size - 1] = (uint8_t)1;
        else if (b == SQFalse)
            data[size - 1] = (uint8_t)0;
        else
        {
            free(data);
            return sq->throwerror(v, "boolean neither true nor false");
        }
        break;
    case OT_ARRAY:
    {
        SQInteger sizeOfArray = sq->getsize(v, stackIndex);
        data[size - 1] = 'a';
        temp = (uint8_t*)realloc(data, size + 4);
        if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
        data = temp; size += 4;
        size_t index = size;//used to write len later
        for (int i = 0; i < sizeOfArray; i++)
        {
            sq->pushinteger(v, i);
            sq->get(v, stackIndex);
            if (SQ_FAILED(Function333(v, sq->gettop(v), data, size)))
            {
                return sq->throwerror(v, "Error occured processing array");
            }
            sq->poptop(v);
        }
        *(uint32_t*)(data + index - 4) = swap4((uint32_t)(size - index));
    }
    break;
    case OT_USERDATA:
    {
        sq->getuserdata(v, stackIndex, &userdata, NULL);
        sizeOfUserData = sq->getsize(v, stackIndex);
        if (!userdata || *(char*)userdata != 'F')
        {
            free(data);
            return sq->throwerror(v, "Unknown userdata passed as parameter");
        }
        try
        {
            //special case. here just copying the bytes and not data[-]='some character here'
            //sizeOfUserData will be greater than or equal to its userdata's buffer size
            sizeOfUserDataActual = swap4(*(uint32_t*)((uint8_t*)userdata + 1)) + 5;
            temp = (uint8_t*)realloc(data, size - 1 + sizeOfUserDataActual);
            if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
            data = temp;
            memcpy(data + size - 1, userdata, sizeOfUserDataActual);
            size += sizeOfUserDataActual - 1;//-1 for allocation of 1 byte before switch
            break;
        }
        catch (...)
        {
            free(data);
            return sq->throwerror(v, "Error in copying userdata");
        };
    }
        break;
    case OT_INSTANCE:
    {
        sq->pushroottable(v); //pushes the global table

        sq->pushstring(v, _SC("Vector"), -1);
        if (SQ_SUCCEEDED(sq->get(v, -2))) {
            sq->pushroottable(v); //push the 'this' (in this case is the global table)
            sq->pushfloat(v, 0.0);
            sq->pushfloat(v, 0.0);
            sq->pushfloat(v, 0.0);
            sq->call(v, 4, 1, 1); //calls the function 
            if (SQ_SUCCEEDED(sq->getclass(v, -1))) //-1 class
            {
                sq->push(v, stackIndex);// -2 class,-1 instance 
                SQBool b;
                //if instance at -1 is instance of class at -2. squirrel documentation is wrong here
                b = sq->instanceof(v);
                sq->pop(v, 2);//pop both instance and class.
                //now stack is roottable, closure, instance of Vector
                sq->pop(v, 2);//roottable left.
                if (b == SQTrue || b == SQFalse)
                {
                    if (b == SQTrue)
                    {
                        //It is a Vector instance.
                        data[size - 1] = (char)('v');
                        temp = (uint8_t*)realloc(data, size + 12);
                        if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
                        data = temp; size += 12;
                        sq->pushstring(v, "x", -1);
                        sq->get(v, stackIndex);
                        sq->getfloat(v, -1, (SQFloat*)&data[size - 12]);
                        sq->poptop(v);
                        sq->pushstring(v, "y", -1);
                        sq->get(v, stackIndex);
                        sq->getfloat(v, -1, (SQFloat*)&data[size - 8]);
                        sq->poptop(v);
                        sq->pushstring(v, "z", -1);
                        sq->get(v, stackIndex);
                        sq->getfloat(v, -1, (SQFloat*)&data[size - 4]);
                        sq->poptop(v);

                        sq->pop(v, 1);//pops roottable.
                        break;
                    }
                    else {
                        //Check for Quaternion 
                        sq->pushstring(v, _SC("Quaternion"), -1);
                        if (SQ_SUCCEEDED(sq->get(v, -2))) {
                            sq->pushroottable(v); //push the 'this' (in this case is the global table)
                            sq->pushfloat(v, 0.0);
                            sq->pushfloat(v, 0.0);
                            sq->pushfloat(v, 0.0);
                            sq->pushfloat(v, 1.0);
                            sq->call(v, 5, 1, 1); //calls the function 
                            if (SQ_SUCCEEDED(sq->getclass(v, -1))) //-1 class
                            {
                                sq->push(v, stackIndex);// -2 class,-1 instance 
                                SQBool b2;
                                //if instance at -1 is instance of class at -2. squirrel documentation is wrong here
                                b2 = sq->instanceof(v);
                                sq->pop(v, 2);//pop both instance and class.
                                //now stack is roottable, closure, instance of Quaternion
                                sq->pop(v, 2);//roottable left.
                                if (b2 == SQTrue || b2 == SQFalse)
                                {
                                    if (b2 == SQTrue)
                                    {
                                        //It is a Quaternion instance.
                                        data[size - 1] = (char)('q');
                                        temp = (uint8_t*)realloc(data, size + 16);
                                        if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
                                        data = temp; size += 16;
                                        sq->pushstring(v, "x", -1);
                                        sq->get(v, stackIndex);
                                        sq->getfloat(v, -1, (SQFloat*)&data[size - 16]);
                                        sq->poptop(v);
                                        sq->pushstring(v, "y", -1);
                                        sq->get(v, stackIndex);
                                        sq->getfloat(v, -1, (SQFloat*)&data[size - 12]);
                                        sq->poptop(v);
                                        sq->pushstring(v, "z", -1);
                                        sq->get(v, stackIndex);
                                        sq->getfloat(v, -1, (SQFloat*)&data[size - 8]);
                                        sq->poptop(v);
                                        sq->pushstring(v, "w", -1);
                                        sq->get(v, stackIndex);
                                        sq->getfloat(v, -1, (SQFloat*)&data[size - 4]);
                                        sq->poptop(v);

                                        sq->pop(v, 1);//pops roottable
                                        break;
                                    }
                                    else
                                    {
                                        free(data);
                                        return sq->throwerror(v, "Instance provided is neither Vector nor Quaternion");
                                    }

                                }
                                else printf("Error: SQBool (2) is neither true nor false\n");
                            }
                            else printf("Error: getting Quaternion class\n");

                        }
                        else printf("Error getting constructor of Quaternion class\n");
                    }
                }
                else printf("Error: SQBool (1) is neither true nor false\n");

            }
            else printf("Error in getting Vector class\n");
        }
        else printf("Error in getting constructor of Vector class\n");

        free(data);
        return -1;//was not success with the instance provided.
    }// case OT_INSTANCE
    default:
    free(data);
    return sq->throwerror(v, "Unknown type of parameter given");
    }//switch gettype
    return 1;
}
SQInteger SQ_CreateFunction(HSQUIRRELVM v)
{
    SQInteger n = sq->gettop(v);
    const SQChar* remotefuncname;
	sq->getstring(v, n, &remotefuncname);
    
   
    
    
    if (n == 3)sq->pushinteger(v, RPC_CALL);
    else sq->pushinteger(v, RPC_FUNCTION);
    sq->pushstring(v, remotefuncname, -1);
    if (n == 3)
    {
        SQInteger playerid;
        sq->getinteger(v, 2, &playerid);
        sq->pushinteger(v, playerid);
    }
   	sq->newclosure(v, SQ_RemoteFunction, n);
	return 1;
}