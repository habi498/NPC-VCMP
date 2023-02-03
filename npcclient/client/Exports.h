#ifndef EXPORTS_H
#define EXPORTS_H
#include <sqmodule2.h>
void InitSQAPI();

HSQAPI* pfGetSquirrelAPI();
HSQUIRRELVM* pfGetSquirrelVM();
void InitSquirrelExports();
const void** GetSquirrelExports(size_t* sizeofExports);
typedef HSQAPI* (*Sq_GetSquirrelAPI)(void);
typedef HSQUIRRELVM* (*Sq_GetSquirrelVM)(void);
struct SquirrelExports
{
	unsigned int uStructSize;
	Sq_GetSquirrelAPI GetSquirrelAPI;
	Sq_GetSquirrelVM GetSquirrelVM;
};
#endif