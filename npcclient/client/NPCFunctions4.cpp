/*
    Copyright (C) 2022-2023  habi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
//NPCFunctions4.cpp : For writing and reading blob for Serverdata
#include "main.h"
#include "sqstdblobimpl.h"
#define SQSTD_BLOB_TYPE_TAG ((SQUnsignedInteger)(SQSTD_STREAM_TYPE_TAG | 0x00000002))
extern HSQUIRRELVM v;
extern CPlayerPool* m_pPlayerPool;
extern NPC* iNPC;
extern CPlayerPool* m_pPlayerPool;
extern CFunctions* m_pFunctions;
extern CPlayer* npc;
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress; 

SQInteger fn_ReadInt(HSQUIRRELVM v)
{
    SQUserPointer data; size_t size;
    if (SQ_SUCCEEDED(sqstd_getblob(v, 2, &data)))
    {
        size = sqstd_getblobsize(v, 2);
        SQBlob* blob;
        {
            if (SQ_FAILED(sq_getinstanceup(v, 2, (SQUserPointer*)&blob, (SQUserPointer)SQSTD_BLOB_TYPE_TAG, SQFalse)))
                return SQ_ERROR;
        }
        int integer;
        if (blob->Read(&integer, 4) == 4)
        {
            sq_pushinteger(v, integer);
            return 1;
        }
        else return sq_throwerror(v, "Cannot read int from current position of blob");
    }
    else
    {
        return sq_throwerror(v, "Blob not provided as parameter 1");
    }
}
SQInteger fn_ReadByte(HSQUIRRELVM v)
{
    SQUserPointer data; size_t size;
    if (SQ_SUCCEEDED(sqstd_getblob(v, 2, &data)))
    {
        size = sqstd_getblobsize(v, 2);
        SQBlob* blob;
        {
            if (SQ_FAILED(sq_getinstanceup(v, 2, (SQUserPointer*)&blob, (SQUserPointer)SQSTD_BLOB_TYPE_TAG, SQFalse)))
                return SQ_ERROR;
        }
        unsigned char byte;
        if (blob->Read(&byte, 1) == 1)
        {
            sq_pushinteger(v, byte);
            return 1;
        }
        else return sq_throwerror(v, "Cannot read byte from current position of blob");
    }
    else
    {
        return sq_throwerror(v, "Blob not provided as parameter 1");
    }
}
SQInteger fn_ReadFloat(HSQUIRRELVM v)
{
    SQUserPointer data; size_t size;
    if (SQ_SUCCEEDED(sqstd_getblob(v, 2, &data)))
    {
        size = sqstd_getblobsize(v, 2);
        SQBlob* blob;
        {
            if (SQ_FAILED(sq_getinstanceup(v, 2, (SQUserPointer*)&blob, (SQUserPointer)SQSTD_BLOB_TYPE_TAG, SQFalse)))
                return SQ_ERROR;
        }
        unsigned int integer;
        if (blob->Read(&integer, 4) == 4)
        {
            float f = *(float*)&integer;
            sq_pushfloat(v, f);
            return 1;
        }
        else return sq_throwerror(v, "Cannot read float from current position of blob");
    }
    else
    {
        return sq_throwerror(v, "Blob not provided as parameter 1");
    }
}
SQInteger fn_ReadString(HSQUIRRELVM v)
{
    SQUserPointer data; size_t size;
    if (SQ_SUCCEEDED(sqstd_getblob(v, 2, &data)))
    {
        size = sqstd_getblobsize(v, 2);
        SQBlob* blob;
        {
            if (SQ_FAILED(sq_getinstanceup(v, 2, (SQUserPointer*)&blob, (SQUserPointer)SQSTD_BLOB_TYPE_TAG, SQFalse)))
                return SQ_ERROR;
        }
        char sizeOfString[2];
        if (blob->Read(&sizeOfString, 2) == 2)
        {
            unsigned short strsize = sizeOfString[0] * 256 + sizeOfString[1];
            char* buffer = new char[strsize];
            if (blob->Read(buffer, strsize) == strsize)
            {
                sq_pushstring(v, buffer, strsize);
                delete[] buffer;
                return 1;
            }
            else return sq_throwerror(v, "Error when reading string");
            
        }
        else return sq_throwerror(v, "Cannot read string from current position of blob");
    }
    else
    {
        return sq_throwerror(v, "Blob not provided as parameter 1");
    }
}
SQInteger fn_WriteInt(HSQUIRRELVM v)
{
    SQUserPointer data; SQInteger integer;
    if (SQ_SUCCEEDED(sqstd_getblob(v, 2, &data)))
    {
        SQBlob* blob;
        {
            if (SQ_FAILED(sq_getinstanceup(v, 2, (SQUserPointer*)&blob, (SQUserPointer)SQSTD_BLOB_TYPE_TAG, SQFalse)))
                return SQ_ERROR;
        }
        sq_getinteger(v, 3, &integer);
        if (blob->Write(&integer, 4)==4)
        {
            sq_pushbool(v, SQTrue);
            return 1;
        }
        else return sq_throwerror(v, "Failed when writing integer");
    }
    else
    {
        return sq_throwerror(v, "Blob not provided as parameter 1");
    }
}

SQInteger fn_WriteByte(HSQUIRRELVM v)
{
    SQUserPointer data; SQInteger integer;
    if (SQ_SUCCEEDED(sqstd_getblob(v, 2, &data)))
    {
        SQBlob* blob;
        {
            if (SQ_FAILED(sq_getinstanceup(v, 2, (SQUserPointer*)&blob, (SQUserPointer)SQSTD_BLOB_TYPE_TAG, SQFalse)))
                return SQ_ERROR;
        }
        sq_getinteger(v, 3, &integer);
        unsigned char byte = (unsigned char)integer;
        if (blob->Write(&byte, 1) == 1)
        {
            sq_pushbool(v, SQTrue);
            return 1;
        }
        else return sq_throwerror(v, "Failed when writing byte");
    }
    else
    {
        return sq_throwerror(v, "Blob not provided as parameter 1");
    }
}
SQInteger fn_WriteFloat(HSQUIRRELVM v)
{
    SQUserPointer data; SQFloat f;
    if (SQ_SUCCEEDED(sqstd_getblob(v, 2, &data)))
    {
        SQBlob* blob;
        {
            if (SQ_FAILED(sq_getinstanceup(v, 2, (SQUserPointer*)&blob, (SQUserPointer)SQSTD_BLOB_TYPE_TAG, SQFalse)))
                return SQ_ERROR;
        }
        sq_getfloat(v, 3, &f);
        if (blob->Write(&f, 4) == 4)
        {
            sq_pushbool(v, SQTrue);
            return 1;
        }
        else return sq_throwerror(v, "Failed when writing float");
    }
    else
    {
        return sq_throwerror(v, "Blob not provided as parameter 1");
    }
}
SQInteger fn_WriteString(HSQUIRRELVM v)
{
    SQUserPointer data; const SQChar* string;
    if (SQ_SUCCEEDED(sqstd_getblob(v, 2, &data)))
    {
        SQBlob* blob;
        {
            if (SQ_FAILED(sq_getinstanceup(v, 2, (SQUserPointer*)&blob, (SQUserPointer)SQSTD_BLOB_TYPE_TAG, SQFalse)))
                return SQ_ERROR;
        }
        sq_getstring(v, 3, &string);
        unsigned short len = (unsigned short)strlen(string);//12 00
        unsigned char swappedlen[2];
        swappedlen[0] = len & 0xFF00;
        swappedlen[1] = len & 0x00FF;//perhaps 'both' are 'same', depends on operating system.
        
        if (blob->Write(&swappedlen, 2) == 2)
        {
            void* str = malloc(len + 1);
            if (!str)return sq_throwerror(v, "Error when allocating memory");
            memcpy(str, string, len + 1); 
            
            if (blob->Write(str, len) == len)
            {
                sq_pushbool(v, SQTrue);
                free(str);
                return 1;
            }else{ 
                free(str);
                return sq_throwerror(v, "Failed when writing string");
            }
        }
        else return sq_throwerror(v, "Failed when writing size of string");
    }
    else
    {
        return sq_throwerror(v, "Blob not provided as parameter 1");
    }
}

void RegisterNPCFunctions4()
{
	register_global_func(v, ::fn_ReadInt, "ReadInt", 2, "tx");
	register_global_func(v, ::fn_ReadByte, "ReadByte", 2, "tx");
	register_global_func(v, ::fn_ReadFloat, "ReadFloat", 2, "tx");
	register_global_func(v, ::fn_ReadString, "ReadString", 2, "tx");
    register_global_func(v, ::fn_WriteInt, "WriteInt", 3, "txi");
    register_global_func(v, ::fn_WriteByte, "WriteByte", 3, "txi");
    register_global_func(v, ::fn_WriteFloat, "WriteFloat", 3, "txf");
    register_global_func(v, ::fn_WriteString, "WriteString", 3, "txs");
}