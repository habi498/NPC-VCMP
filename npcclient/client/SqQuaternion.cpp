/*
    Copyright (C) 2023  habi

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
#include "main.h"
SQInteger fn_Quaternion_constructor(HSQUIRRELVM v)
{
    SQFloat x, y, z, w; SQInteger X, Y, Z, W;
    if (sq_gettype(v, 2) == OT_FLOAT)
        sq_getfloat(v, 2, &x);
    else if (sq_gettype(v, 2) == OT_INTEGER)
    {
        sq_getinteger(v, 2, &X);
        x = (SQFloat)X;
    }
    if (sq_gettype(v, 3) == OT_FLOAT)
        sq_getfloat(v, 3, &y);
    else if (sq_gettype(v, 3) == OT_INTEGER)
    {
        sq_getinteger(v, 3, &Y);
        y = (SQFloat)Y;
    }
    if (sq_gettype(v, 4) == OT_FLOAT)
        sq_getfloat(v, 4, &z);
    else if (sq_gettype(v, 4) == OT_INTEGER)
    {
        sq_getinteger(v, 4, &Z);
        z = (SQFloat)Z;
    }
    
    if (sq_gettype(v, 5) == OT_FLOAT)
        sq_getfloat(v, 5, &w);
    else if (sq_gettype(v, 5) == OT_INTEGER)
    {
        sq_getinteger(v, 5, &W);
        w = (SQFloat)W;
    }
    
    sq_pushstring(v, "x", -1);
    sq_pushfloat(v, x);
    sq_set(v, 1);
    sq_pushstring(v, "y", -1);
    sq_pushfloat(v, y);
    sq_set(v, 1);
    sq_pushstring(v, "z", -1);
    sq_pushfloat(v, z);
    sq_set(v, 1);
    sq_pushstring(v, "w", -1);
    sq_pushfloat(v, w);
    sq_set(v, 1);
    return 0;
}
SQInteger fn_Quaternion_add(HSQUIRRELVM v)
{
    SQInteger nargs = sq_gettop(v);
    //xx
    sq_pushstring(v, "x", -1);
    sq_get(v, -3); SQFloat x1;
    sq_getfloat(v, -1, &x1);
    sq_poptop(v);
    sq_pushstring(v, "y", -1);
    sq_get(v, -3); SQFloat y1;
    sq_getfloat(v, -1, &y1);
    sq_poptop(v);
    sq_pushstring(v, "z", -1);
    sq_get(v, -3); SQFloat z1;
    sq_getfloat(v, -1, &z1);
    sq_poptop(v);
    sq_pushstring(v, "w", -1);
    sq_get(v, -3); SQFloat w1;
    sq_getfloat(v, -1, &w1);
    sq_poptop(v);

    sq_pushstring(v, "x", -1);
    sq_get(v, -2); SQFloat x2;
    sq_getfloat(v, -1, &x2);
    sq_poptop(v);//pops the float value pushed to stack by sq_get
    sq_pushstring(v, "y", -1);
    sq_get(v, -2); SQFloat y2;
    sq_getfloat(v, -1, &y2);
    sq_poptop(v);
    sq_pushstring(v, "z", -1);
    sq_get(v, -2); SQFloat z2;
    sq_getfloat(v, -1, &z2);
    sq_poptop(v);
    sq_pushstring(v, "w", -1);
    sq_get(v, -2); SQFloat w2;
    sq_getfloat(v, -1, &w2);
    sq_poptop(v);
    sq_pop(v, 2);//pop two class instances a and b #a+b

    sq_pushroottable(v);
    sq_pushstring(v, "Quaternion", -1);
    sq_get(v, -2);
    sq_pushnull(v);
    sq_pushfloat(v, x1 + x2);
    sq_pushfloat(v, y1 + y2);
    sq_pushfloat(v, z1 + z2);
    sq_pushfloat(v, w1 + w2);
    //call Quaternion
    sq_call(v, 5, SQTrue, SQTrue);
    //now tyx
    return 1;
}
SQInteger fn_Quaternion_sub(HSQUIRRELVM v)
{
    SQInteger nargs = sq_gettop(v);
    //xx
    sq_pushstring(v, "x", -1);
    sq_get(v, -3); SQFloat x1;
    sq_getfloat(v, -1, &x1);
    sq_poptop(v);
    sq_pushstring(v, "y", -1);
    sq_get(v, -3); SQFloat y1;
    sq_getfloat(v, -1, &y1);
    sq_poptop(v);
    sq_pushstring(v, "z", -1);
    sq_get(v, -3); SQFloat z1;
    sq_getfloat(v, -1, &z1);
    sq_poptop(v);
    sq_pushstring(v, "w", -1);
    sq_get(v, -3); SQFloat w1;
    sq_getfloat(v, -1, &w1);
    sq_poptop(v);

    sq_pushstring(v, "x", -1);
    sq_get(v, -2); SQFloat x2;
    sq_getfloat(v, -1, &x2);
    sq_poptop(v);//pops the float value pushed to stack by sq_get
    sq_pushstring(v, "y", -1);
    sq_get(v, -2); SQFloat y2;
    sq_getfloat(v, -1, &y2);
    sq_poptop(v);
    sq_pushstring(v, "z", -1);
    sq_get(v, -2); SQFloat z2;
    sq_getfloat(v, -1, &z2);
    sq_poptop(v);
    sq_pushstring(v, "w", -1);
    sq_get(v, -2); SQFloat w2;
    sq_getfloat(v, -1, &w2);
    sq_poptop(v);
    sq_pop(v, 2);//pop two class instances a and b #a+b

    sq_pushroottable(v);
    sq_pushstring(v, "Quaternion", -1);
    sq_get(v, -2);
    sq_pushnull(v);
    sq_pushfloat(v, x1 - x2);
    sq_pushfloat(v, y1 - y2);
    sq_pushfloat(v, z1 - z2);
    sq_pushfloat(v, w1 - w2);
    //call Quaternion
    sq_call(v, 5, SQTrue, SQTrue);
    //now tyx
    return 1;
}
SQInteger fn_Quaternion_tostring(HSQUIRRELVM v)
{   //x
    sq_pushstring(v, "x", -1);
    sq_get(v, -2); SQFloat x;
    sq_getfloat(v, -1, &x);
    sq_poptop(v);
    sq_pushstring(v, "y", -1);
    sq_get(v, -2); SQFloat y;
    sq_getfloat(v, -1, &y);
    sq_poptop(v);
    sq_pushstring(v, "z", -1);
    sq_get(v, -2); SQFloat z;
    sq_getfloat(v, -1, &z);
    sq_poptop(v);
    sq_pushstring(v, "w", -1);
    sq_get(v, -2); SQFloat w;
    sq_getfloat(v, -1, &w);
    sq_poptop(v);
    char* buffer = (char*)malloc(10 * 4);//take 10 as size of x as string eg. 1000.33333
    if (buffer)
    {
        sprintf(buffer, "(%f,%f,%f,%f)", x, y, z, w);
        sq_pushstring(v, buffer, -1);
        free(buffer);
        return 1;
    }
    return 0;
}
SQInteger fn_Quaternion_mul(HSQUIRRELVM v)
{
    SQInteger nargs = sq_gettop(v);
    //xf|i
    sq_pushstring(v, "x", -1);
    sq_get(v, -3); SQFloat x;
    sq_getfloat(v, -1, &x);
    sq_poptop(v);
    sq_pushstring(v, "y", -1);
    sq_get(v, -3); SQFloat y;
    sq_getfloat(v, -1, &y);
    sq_poptop(v);
    sq_pushstring(v, "z", -1);
    sq_get(v, -3); SQFloat z;
    sq_getfloat(v, -1, &z);
    sq_poptop(v);
    sq_pushstring(v, "w", -1);
    sq_get(v, -3); SQFloat w;
    sq_getfloat(v, -1, &w);
    sq_poptop(v);
    SQFloat f = 0; SQInteger temp;
    if (sq_gettype(v, 2) == OT_FLOAT)
        sq_getfloat(v, 2, &f);
    else if (sq_gettype(v, 2) == OT_INTEGER)
    {
        sq_getinteger(v, 2, &temp);
        f = (SQFloat)temp;
    }

    sq_pushroottable(v);
    sq_pushstring(v, "Quaternion", -1);
    sq_get(v, -2);
    sq_pushnull(v);
    sq_pushfloat(v, x * f);
    sq_pushfloat(v, y * f);
    sq_pushfloat(v, z * f);
    sq_pushfloat(v, w * f);
    //call Quaternion
    sq_call(v, 5, SQTrue, SQTrue);

    return 1;
}
SQInteger fn_Quaternion_div(HSQUIRRELVM v)
{
    SQInteger nargs = sq_gettop(v);
    //xf|i
    sq_pushstring(v, "x", -1);
    sq_get(v, -3); SQFloat x;
    sq_getfloat(v, -1, &x);
    sq_poptop(v);
    sq_pushstring(v, "y", -1);
    sq_get(v, -3); SQFloat y;
    sq_getfloat(v, -1, &y);
    sq_poptop(v);
    sq_pushstring(v, "z", -1);
    sq_get(v, -3); SQFloat z;
    sq_getfloat(v, -1, &z);
    sq_poptop(v);
    sq_pushstring(v, "w", -1);
    sq_get(v, -3); SQFloat w;
    sq_getfloat(v, -1, &w);
    sq_poptop(v);
    SQFloat f = 0; SQInteger temp;
    if (sq_gettype(v, 2) == OT_FLOAT)
        sq_getfloat(v, 2, &f);
    else if (sq_gettype(v, 2) == OT_INTEGER)
    {
        sq_getinteger(v, 2, &temp);
        f = (SQFloat)temp;
    }
    if (f == 0)
    {
        return sq_throwerror(v, "Division by zero");
    }
    sq_pushroottable(v);
    sq_pushstring(v, "Quaternion", -1);
    sq_get(v, -2);
    sq_pushnull(v);
    sq_pushfloat(v, x / f);
    sq_pushfloat(v, y / f);
    sq_pushfloat(v, z / f);
    sq_pushfloat(v, w / f);
    //call Quaternion
    sq_call(v, 5, SQTrue, SQTrue);

    return 1;
}
SQRESULT register_quaternionlib(HSQUIRRELVM v)
{
    sq_pushroottable(v);
    sq_pushstring(v, "Quaternion", -1);
    //do some stuff with squirrel here
    sq_newclass(v, false);
    sq_pushstring(v, "x", -1);
    sq_pushfloat(v, 0.0);
    sq_createslot(v, -3);
    sq_pushstring(v, "y", -1);
    sq_pushfloat(v, 0.0);
    sq_createslot(v, -3);
    sq_pushstring(v, "z", -1);
    sq_pushfloat(v, 0.0);
    sq_createslot(v, -3);
    sq_pushstring(v, "w", -1);
    sq_pushfloat(v, 1.0);
    sq_createslot(v, -3);
    sq_pushstring(v, "constructor", -1);
    sq_newclosure(v, fn_Quaternion_constructor, 0);
    sq_setparamscheck(v, 5, "xf|if|if|if|i");
    sq_newslot(v, -3, SQFalse);

    sq_pushstring(v, "_add", -1);
    sq_newclosure(v, fn_Quaternion_add, 0);
    sq_setparamscheck(v, 2, "xx");
    sq_newslot(v, -3, SQFalse);

    sq_pushstring(v, "_sub", -1);
    sq_newclosure(v, fn_Quaternion_sub, 0);
    sq_setparamscheck(v, 2, "xx");
    sq_newslot(v, -3, SQFalse);

    sq_pushstring(v, "_tostring", -1);
    sq_newclosure(v, fn_Quaternion_tostring, 0);
    sq_setparamscheck(v, 1, "x");
    sq_newslot(v, -3, SQFalse);

    sq_pushstring(v, "_mul", -1);
    sq_newclosure(v, fn_Quaternion_mul, 0);
    sq_setparamscheck(v, 2, "xf|i");
    sq_newslot(v, -3, SQFalse);

    sq_pushstring(v, "_div", -1);
    sq_newclosure(v, fn_Quaternion_div, 0);
    sq_setparamscheck(v, 2, "xf|i");
    sq_newslot(v, -3, SQFalse);

    sq_createslot(v, -3);//create slot "Quaternion" in roottable
    sq_poptop(v);//pops roottable
    return SQTrue;
}


//Pushes the quaternion  to the stack
SQRESULT sq_pushquaternion(HSQUIRRELVM v, SQFloat x, SQFloat y, SQFloat z, SQFloat w)
{
    SQInteger top = sq_gettop(v);
    sq_pushroottable(v); //t
    sq_pushstring(v, "Quaternion", -1); //ts
    sq_get(v, -2); //ty
    sq_pushnull(v); //tyo
    sq_pushfloat(v, x);//tyof
    sq_pushfloat(v, y);//tyoff
    sq_pushfloat(v, z);//ty offf
    sq_pushfloat(v, w);//ty offff
    //call Quaternion
    SQRESULT res = sq_call(v, 5, SQTrue, SQTrue); //ty x
    if (SQ_SUCCEEDED(res))
    {
        sq_remove(v, -2); //t x
        sq_remove(v, -2); //x
        return 1;
    }
    else {
        sq_settop(v, top);
        return -1;
    }
}

SQRESULT sq_pushquaternion(HSQUIRRELVM v, QUATERNION quaternion)
{
    return sq_pushquaternion(v, quaternion.X, quaternion.Y, quaternion.Z, quaternion.W);
}
//get a quaternion from stack. idx must be positive
SQRESULT sq_getquaternion(HSQUIRRELVM v, SQInteger idx, QUATERNION* quat)
{
    SQInteger top = sq_gettop(v);
    sq_pushstring(v, "x", -1);
    if (!SQ_SUCCEEDED(sq_get(v, idx)))
    {
        sq_settop(v, top); return -1;
    }
    sq_getfloat(v, -1, &quat->X);
    sq_poptop(v);

    sq_pushstring(v, "y", -1);
    if (!SQ_SUCCEEDED(sq_get(v, idx)))
    {
        sq_settop(v, top); return -1;
    }
    sq_getfloat(v, -1, &quat->Y);
    sq_poptop(v);

    sq_pushstring(v, "z", -1);
    if (!SQ_SUCCEEDED(sq_get(v, idx)))
    {
        sq_settop(v, top); return -1;
    }
    sq_getfloat(v, -1, &quat->Z);
    sq_poptop(v);

    sq_pushstring(v, "w", -1);
    if (!SQ_SUCCEEDED(sq_get(v, idx)))
    {
        sq_settop(v, top); return -1;
    }
    sq_getfloat(v, -1, &quat->W);
    sq_poptop(v);
    return 1;
}