#include "main.h"
#ifdef WIN32
#include <string>
#else
#include <cstring>
#endif
extern PluginFuncs* funcs;
extern HSQAPI sq;
#define RPC_CALL 0x1
#define RPC_CALL_RAW 0x4
#define RPC_FUNCTION 0x2
#define RPC_FUNCTION_RAW 0x3
uint32_t swap4(uint32_t i);
uint16_t swap2(uint16_t i);
enum class Type { STRING, USERDATA };
SQRESULT FormatBuffer(HSQUIRRELVM v, SQInteger stackIndex, uint8_t*& data, size_t& size);
SQInteger SQ_RemoteFunction(HSQUIRRELVM v)
{
    SQInteger nargs = sq->gettop(v); 
    SQInteger typeRPC; 
    sq->getinteger(v, nargs--, &typeRPC); 
    Type paramtype = Type::STRING;
    const SQChar* funcname; SQUserPointer userpointer; SQInteger sizeOfUserData;
    if (sq->gettype(v, nargs) == OT_STRING)
    {
        sq->getstring(v, nargs--, &funcname);
    }
    else if (sq->gettype(v, nargs) == OT_USERDATA)
    {
        paramtype = Type::USERDATA;
        sizeOfUserData = sq->getsize(v, nargs);
        if(SQ_FAILED(sq->getuserdata(v, nargs--, &userpointer, NULL)))
            return sq->throwerror(v, "Failed when getting userdata");
    }
    uint8_t* data = (uint8_t*)calloc(5, sizeof(char));
    size_t size = 5; uint8_t* temp;
    
    if (data)
    {
        if (paramtype == Type::STRING)
        {
            if (typeRPC == RPC_FUNCTION_RAW || typeRPC == RPC_CALL_RAW)
                data[0] = (char)('E');
            else
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
            *(uint16_t*)(data + size - len - 2) = swap2(len);

            memcpy((void*)&data[size - len], funcname, len);
        }
        else if (paramtype == Type::USERDATA)
        {
            if (typeRPC == RPC_FUNCTION_RAW || typeRPC == RPC_CALL_RAW)
                data[0] = 'D';
            else
                data[0] = 'G';
            if (!userpointer || (*(char*)userpointer != 'F'
                && *(char*)userpointer != 'G'
                && *(char*)userpointer != 'E'
                && *(char*)userpointer != 'D'
                && *(char*)userpointer != 'g'
                )) {
                free(data);
                return sq->throwerror(v, "Error when getting user data.\
                or invalid user data.");
            }

            try
            {

                //Using try because the pointer can be invalid 
                SQInteger sizeOfUserDataActual = swap4(*(uint32_t*)((uint8_t*)userpointer + 1)) + 5;//5 for G/F XXXX
                
                temp = (uint8_t*)realloc(data, size + sizeOfUserDataActual);
                if (!temp) { free(data); return sq->throwerror(v, "Error in allocating memory"); }
                data = temp;
                //printf("sizeOfUserDataActual is %d\n", sizeOfUserDataActual);
                memcpy(data + size, userpointer, sizeOfUserDataActual);
                size += sizeOfUserDataActual;
                //data[1-4] will be formatted in the end!
            }
            catch (...)
            {
                free(data);
                return sq->throwerror(v, "Error in copying userdata.");
            }
        }
        SQRESULT res;
        for (SQInteger n = 2; n <= nargs; n++)
        {
            res = FormatBuffer(v, n, data, size);
            if(SQ_FAILED(res)) {
                return res;//No need to free data as FormatBuffer will do it
            }
        }
    }
    else return sq->throwerror(v, "Memory allocation failed\n");
    
    *(uint32_t*)(data + 1) = swap4((uint32_t)size - 5);//customary to swap length of following item
    if (typeRPC == RPC_FUNCTION || typeRPC == RPC_FUNCTION_RAW)
    {
        SQUserPointer p = sq->newuserdata(v, size);
        memcpy(p, data, size);
        free(data); 
        return 1;
    }
    else if (typeRPC == RPC_CALL||typeRPC==RPC_CALL_RAW)
    {
        temp = (uint8_t*)realloc(data, size + 4);
        if (!temp) {
            free(data);
            return sq->throwerror(v, "Error in allocating memory");
        }
        data = temp; size += 4;
        memmove(data + 4, data, size - 4);
        *(uint32_t*)(data) = RPC_STREAM_IDENTIFIER;
        funcs->SendServerData((const void*)data, size);
        free(data);
        funcError e= funcs->GetLastError();
        if (e == funcError::NoError)sq->pushbool(v, SQTrue);
        else sq->pushbool(v, SQFalse);
        return 1;
    }
    else return 0;
}
/*Attaches payload to buffer according to squirrel data type at stackIndex
String: sxxthestring
Function ( in form of userdata): F...
Vector: v..., Quaternion: q...,
Array: a...,
Null: o, Integer: ixxxx, Float: fyyyy, Bool: bx (x=0 or 1),
*/
SQRESULT FormatBuffer(HSQUIRRELVM v, SQInteger stackIndex, uint8_t* &buffer, size_t &size)
{
    uint8_t* temp;
    temp = (uint8_t*)realloc(buffer, size + 1);
    if (!temp) { free(buffer); return sq->throwerror(v, "Error in allocating memory"); }
    buffer = temp; size += 1;
    SQUnsignedInteger sizeOfUserData, sizeOfUserDataActual; uint16_t len;
    SQUserPointer userdata;

    switch(sq->gettype(v, stackIndex))
    {
    case OT_NULL:
        buffer[size - 1] = (char)('o');
        break;
    case OT_INTEGER:
        buffer[size - 1] = (char)('i');
        temp = (uint8_t*)realloc(buffer, size + 4);
        if (!temp) { free(buffer); return sq->throwerror(v, "Error in allocating memory"); }
        buffer = temp; size += 4;
        sq->getinteger(v, stackIndex, (SQInteger*)&buffer[size - 4]);
        break;
    case OT_FLOAT:
        buffer[size - 1] = (char)('f');
        temp = (uint8_t*)realloc(buffer, size + 4);
        if (!temp) { free(buffer); return sq->throwerror(v, "Error in allocating memory"); }
        buffer = temp; size += 4;
        sq->getfloat(v, stackIndex, (SQFloat*)&buffer[size - 4]);
        break;
    case OT_STRING:
        buffer[size - 1] = (char)('s');
        const SQChar* str;
        sq->getstring(v, stackIndex, &str);
        len = (uint16_t)strlen(str);
        if (len < 0)
        {
            free(buffer); return sq->throwerror(v, "Unknown error when computing string length 2");
        }
        temp = (uint8_t*)realloc(buffer, size + 2 + len);
        if (!temp) { free(buffer); return sq->throwerror(v, "Error in allocating memory"); }
        buffer = temp; size += 2 + len;
        *(uint16_t*)&buffer[size - len - 2] = swap2(len);
        memcpy((void*)&buffer[size - len], str, len);
        break;
    case OT_BOOL:
        buffer[size - 1] = (char)('b');
        temp = (uint8_t*)realloc(buffer, size + 1);
        if (!temp) { free(buffer); return sq->throwerror(v, "Error in allocating memory"); }
        buffer = temp; size += 1;
        SQBool b;
        sq->getbool(v, stackIndex, &b);
        if (b == SQTrue)
            buffer[size - 1] = (uint8_t)1;
        else if (b == SQFalse)
            buffer[size - 1] = (uint8_t)0;
        else
        {
            free(buffer);
            return sq->throwerror(v, "boolean neither true nor false");
        }
        break;
    case OT_ARRAY:
    {
        SQInteger sizeOfArray = sq->getsize(v, stackIndex);
        buffer[size - 1] = 'a';
        temp = (uint8_t*)realloc(buffer, size + 4);
        if (!temp) { free(buffer); return sq->throwerror(v, "Error in allocating memory"); }
        buffer = temp; size += 4;
        size_t index = size;//used to write len later
        for (int i = 0; i < sizeOfArray; i++)
        {
            sq->pushinteger(v, i);
            sq->get(v, stackIndex);
            if (SQ_FAILED(FormatBuffer(v, sq->gettop(v), buffer, size)))
            {
                return sq->throwerror(v, "Error occured processing array");
            }
            sq->poptop(v);
        }
        *(uint32_t*)(buffer + index - 4) = swap4((uint32_t)(size - index));
    }
    break;
    case OT_USERDATA:
    {
        sq->getuserdata(v, stackIndex, &userdata, NULL);
        sizeOfUserData = sq->getsize(v, stackIndex);
        if (!userdata || 
            ( * (char*)userdata != 'F' &&
            *(char*)userdata!='G' &&
                *(char*)userdata != 'E' &&
                *(char*)userdata != 'D') &&
                *(char*)userdata != 'g')
        {
            free(buffer);
            return sq->throwerror(v, "Unknown userdata passed as parameter");
        }
        try
        {
            //special case. here just copying the bytes and not data[-]='some character here'
            //sizeOfUserData will be greater than or equal to its userdata's buffer size
            sizeOfUserDataActual = swap4(*(uint32_t*)((uint8_t*)userdata + 1)) + 5;
            temp = (uint8_t*)realloc(buffer, size - 1 + sizeOfUserDataActual);
            if (!temp) { free(buffer); return sq->throwerror(v, "Error in allocating memory"); }
            buffer = temp;
            memcpy(buffer + size - 1, userdata, sizeOfUserDataActual);
            size += sizeOfUserDataActual - 1;//-1 for allocation of 1 byte before switch
            break;
        }
        catch (...)
        {
            free(buffer);
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
                        buffer[size - 1] = (char)('v');
                        temp = (uint8_t*)realloc(buffer, size + 12);
                        if (!temp) { free(buffer); return sq->throwerror(v, "Error in allocating memory"); }
                        buffer = temp; size += 12;
                        sq->pushstring(v, "x", -1);
                        sq->get(v, stackIndex);
                        sq->getfloat(v, -1, (SQFloat*)&buffer[size - 12]);
                        sq->poptop(v);
                        sq->pushstring(v, "y", -1);
                        sq->get(v, stackIndex);
                        sq->getfloat(v, -1, (SQFloat*)&buffer[size - 8]);
                        sq->poptop(v);
                        sq->pushstring(v, "z", -1);
                        sq->get(v, stackIndex);
                        sq->getfloat(v, -1, (SQFloat*)&buffer[size - 4]);
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
                                        buffer[size - 1] = (char)('q');
                                        temp = (uint8_t*)realloc(buffer, size + 16);
                                        if (!temp) { free(buffer); return sq->throwerror(v, "Error in allocating memory"); }
                                        buffer = temp; size += 16;
                                        sq->pushstring(v, "x", -1);
                                        sq->get(v, stackIndex);
                                        sq->getfloat(v, -1, (SQFloat*)&buffer[size - 16]);
                                        sq->poptop(v);
                                        sq->pushstring(v, "y", -1);
                                        sq->get(v, stackIndex);
                                        sq->getfloat(v, -1, (SQFloat*)&buffer[size - 12]);
                                        sq->poptop(v);
                                        sq->pushstring(v, "z", -1);
                                        sq->get(v, stackIndex);
                                        sq->getfloat(v, -1, (SQFloat*)&buffer[size - 8]);
                                        sq->poptop(v);
                                        sq->pushstring(v, "w", -1);
                                        sq->get(v, stackIndex);
                                        sq->getfloat(v, -1, (SQFloat*)&buffer[size - 4]);
                                        sq->poptop(v);

                                        sq->pop(v, 1);//pops roottable
                                        break;
                                    }
                                    else
                                    {
                                        free(buffer);
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

        free(buffer);
        return -1;//was not success with the instance provided.
    }// case OT_INSTANCE
    default:
    free(buffer);
    return sq->throwerror(v, "Unknown type of parameter given");
    }//switch gettype
    return 1;
}

//Returns closure.
SQInteger SQ_Remote_Call(HSQUIRRELVM v)
{
    SQInteger n = sq->gettop(v); 
    const SQChar* remotefuncname; SQUserPointer userdata;
    Type paramtype = Type::STRING;
    if (sq->gettype(v, 2) == OT_STRING)
    {
        sq->getstring(v, 2, &remotefuncname);
    }
    else if (sq->gettype(v, 2) == OT_USERDATA)
    {
        paramtype = Type::USERDATA;
        sq->getuserdata(v, 2, &userdata, NULL);
    }
   
    if (n == 3)sq->pushinteger(v, RPC_CALL);
    else sq->pushinteger(v, RPC_FUNCTION);
    
    if (paramtype == Type::STRING)
        sq->pushstring(v, remotefuncname, -1);
    else
        sq->push(v, 2);

    sq->newclosure(v, SQ_RemoteFunction, 2);
	return 1;
}

//Returns closure
SQInteger SQ_Remote_Call_Raw(HSQUIRRELVM v)
{
    SQInteger n = sq->gettop(v);
    const SQChar* remotefuncname; SQUserPointer userdata;
    Type paramtype = Type::STRING;
    if (sq->gettype(v, 2) == OT_STRING)
    {
        sq->getstring(v, 2, &remotefuncname);
    }
    else if (sq->gettype(v, 2) == OT_USERDATA)
    {
        paramtype = Type::USERDATA;
        sq->getuserdata(v, 2, &userdata, NULL);
    }

    if (n == 3)sq->pushinteger(v, RPC_CALL_RAW);
    else sq->pushinteger(v, RPC_FUNCTION_RAW);

    if (paramtype == Type::STRING)
        sq->pushstring(v, remotefuncname, -1);
    else
        sq->push(v, 2);

    sq->newclosure(v, SQ_RemoteFunction, 2);
    return 1;
}

//Returns userdata gxxxxfirstsecond
SQInteger SQ_Remote_Get(HSQUIRRELVM v)
{
    SQInteger n = sq->gettop(v);
    uint8_t* data = (uint8_t*)calloc(5, sizeof(char));
    size_t size = 5; //uint8_t* temp;
    int top = sq->gettop(v);
    if (data)
    {
        data[0] = 'g';
        SQRESULT res;
        for (SQInteger n = 2; n <= top; n++)
        {
            res = FormatBuffer(v, n, data, size);
            if (SQ_FAILED(res)) {
                return res;//No need to free data as FormatBuffer will do it
            }
        }
        *(uint32_t*)(data + 1) = swap4((uint32_t)size - 5);
        SQUserPointer p = sq->newuserdata(v, size);
        memcpy(p, data, size);
        free(data);
        return 1;
    }
    else return sq->throwerror(v, "Memory allocation failed");
}
//Returns userdata //hxxxx
SQInteger SQ_Remote_Set(HSQUIRRELVM v)
{
    SQInteger n = sq->gettop(v);
    uint8_t* data = (uint8_t*)calloc(9, sizeof(char));
    *(uint32_t*)(data) = RPC_STREAM_IDENTIFIER;
    size_t size = 9; 
    int top = sq->gettop(v);
    if (data)
    {
        data[4] = 'h';
        SQRESULT res;
        for (SQInteger n = 2; n <= top; n++)
        {
            res = FormatBuffer(v, n, data, size);
            if (SQ_FAILED(res)) {
                return res;//No need to free data as FormatBuffer will do it
            }
        }
        *(uint32_t*)(data + 5) = swap4((uint32_t)size - 9);
        funcs->SendServerData((const void*)data, size);
        free(data); 
        return 0;
    }
    else return sq->throwerror(v, "Memory allocation failed");
}
