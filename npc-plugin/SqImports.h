#ifndef IMPORTS_H
#define IMPORTS_H
//Requires Sqrat's sqmodule.h for working of HSQAPI
#include "sqmodule.h"

typedef HSQAPI* (*Sq_GetSquirrelAPI)(void);
typedef HSQUIRRELVM* (*Sq_GetSquirrelVM)(void);
struct SquirrelImports
{
	unsigned int uStructSize;
	Sq_GetSquirrelAPI GetSquirrelAPI;
	Sq_GetSquirrelVM GetSquirrelVM;
};
#endif