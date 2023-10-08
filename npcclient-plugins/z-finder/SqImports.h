#ifndef IMPORTS_H
#define IMPORTS_H

#include "squirrel.h"
//Requires sqmodule2.h for working of HSQAPI
#include "sqmodule2.h"

typedef HSQAPI* (*Sq_GetSquirrelAPI)(void);
typedef HSQUIRRELVM* (*Sq_GetSquirrelVM)(void);
struct SquirrelImports
{
	unsigned int uStructSize;
	Sq_GetSquirrelAPI GetSquirrelAPI;
	Sq_GetSquirrelVM GetSquirrelVM;
};
#endif