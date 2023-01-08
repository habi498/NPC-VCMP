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
#ifndef SQVECTOR_H
#define SQVECTOR_H

SQRESULT register_vectorlib(HSQUIRRELVM v);
SQRESULT sq_pushvector(HSQUIRRELVM v, VECTOR vector);
SQRESULT sq_pushvector(HSQUIRRELVM v, SQFloat x, SQFloat y, SQFloat z);
SQRESULT sq_getvector(HSQUIRRELVM v, SQInteger idx, VECTOR* vecOut);
SQRESULT register_quaternionlib(HSQUIRRELVM v);
SQRESULT sq_pushquaternion(HSQUIRRELVM v, QUATERNION quaternion);
SQRESULT sq_pushquaternion(HSQUIRRELVM v, SQFloat x, SQFloat y, SQFloat z, SQFloat w);
SQRESULT sq_getquaternion(HSQUIRRELVM v, SQInteger idx, QUATERNION* quatOut);

#endif