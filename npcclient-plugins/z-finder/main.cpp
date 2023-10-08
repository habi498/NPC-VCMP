#include <fstream>

#include "main.h"
#include <time.h> //For printing time on console
#include <cstring> //For calculating length of 
#include <math.h> //For calculating distance between players

#define MAP_FILE_IDENTIFIER 2000
PluginFuncs* funcs;
HSQAPI sq;
HSQUIRRELVM v;
float* pZ;
unsigned char* pbitZExists;

int xMin = 0, xMaxPlusOne = 1, yMin = 0, yMaxPlusOne = 1;

int xWidth, yWidth;
bool init = false;

SQInteger r(int _xMin, int _xMaxPlusOne, int _yMin, int _yMaxPlusOne);


SQInteger fn_MiamiScale_Init(HSQUIRRELVM v)
{
	if (init)
	{
		return sq->throwerror(v, "One or more map is already loaded. It needs to be unloaded first.");
	}
	const SQChar* mapfile = NULL;
	sq->getstring(v, 2, &mapfile);
	std::ifstream infile(mapfile, std::ios::in | std::ios::binary);
	
	if (!infile.is_open())
	{
		return sq->throwerror(v, "Error opening given mapfile");
	}
	int header = 0, reserved = 0;
	infile.read((char*)&header, 4);
	infile.read((char*)&reserved, 4);
	if (header != MAP_FILE_IDENTIFIER)
		return sq->throwerror(v, "Invalid map file given");
	/*
	First four bytes (after header) x min, x max, y min, y max
	*/
	
	infile.read((char*)&xMin, 4);
	infile.read((char*)&xMaxPlusOne, 4);
	infile.read((char*)&yMin, 4);
	infile.read((char*)&yMaxPlusOne, 4);
	xWidth = xMaxPlusOne - xMin;
	yWidth = yMaxPlusOne - yMin;
	if (xWidth < 0 || yWidth < 0)
		return sq->throwerror(v, "Mapfile not valid");

	/*Now allocate memory */
	
	pZ = (float*)malloc(xWidth * yWidth*sizeof(float));
	pbitZExists = (unsigned char*)malloc(xWidth * yWidth / 8 );
	if (!pZ || !pbitZExists)
		return sq->throwerror(v, "Error when allocating memory");
	/*Now get sub file positions.*/
	int Location1 = 0, Location2 = 0, Location3 = 0;
	int Location1End = 0, Location2End = 0, Location3End = 0;
	
	infile.read((char*)&Location1, 4);
	infile.read((char*)&Location1End, 4);

	infile.read((char*)&Location2, 4);
	infile.read((char*)&Location2End, 4);

	infile.read((char*)&Location3, 4);
	infile.read((char*)&Location3End, 4);

	printf("MiamiScale Init began.");
	
	//Load part1 and part2 into memory.
	unsigned char* part1 = new unsigned char[Location1End-Location1+1];
	if (!part1)return sq->throwerror(v, "Error when allocating memory");
	infile.seekg(Location1, std::ios::beg);
	infile.read((char*)part1, Location1End - Location1+1);
	
	unsigned char* part2 = new unsigned char[Location2End - Location2+1];
	if (!part2)return sq->throwerror(v, "Error when allocating memory.");
	infile.seekg(Location2, std::ios::beg);
	infile.read((char*)part2, Location2End-Location2+1);

	char byteMap1[1], byteMap2[1];
	unsigned index = 0;
	char f[4]; float z;
	unsigned int counter = 0;
	
	//Now point to Location3 and read float one by one
	infile.seekg(Location3, std::ios::beg); unsigned int k = 0;
	for (int i = xMin; i < xMaxPlusOne; i++)
	{
		for (int j = yMin; j < yMaxPlusOne; j++)
		{
			if ((counter % 8) == 0)
			{
				*(unsigned char*)byteMap1 = part1[((i - xMin) * (yWidth)+(j - yMin)) / 8];
				pbitZExists[((i -xMin) * (yWidth) + (j -yMin ))/8] = *(unsigned char*)byteMap1;
			}
			counter++;
			if ((*(unsigned char*)byteMap1 & 0b10000000) == 0b10000000)
			{
				if (index % 8 == 0)
				{
					*(unsigned char*)byteMap2 = part2[index/8];
				}
				if ((*(unsigned char*)byteMap2 & 0b10000000) == 0b10000000)
				{
					infile.read((char*)f, 4);
					z = *(float*)f;
					pZ[(i -xMin) * yWidth + (j -yMin)] = z;
					k = (i - xMin) * yWidth + (j - yMin);
				}
				else
					pZ[(i - xMin) * yWidth + (j - yMin)] = pZ[k];
				*(unsigned char*)byteMap2 = (*(unsigned char*)byteMap2 << 1) & 0xFF;

				index += 1;
			}
			*(unsigned char*)byteMap1 = (*(unsigned char*)byteMap1 << 1) & 0xFF;
		}
	}
	delete[] part1;
	delete[] part2;
	init = true;
	printf("MiamiScale Init completed.\n");
	sq->pushbool(v, SQTrue);
	return 1;
}
SQInteger fn_MiamiScale_SaveCurrentMap(HSQUIRRELVM v)
{
	if (!init)
		return sq->throwerror(v, "Map not loaded\n");
	const SQChar* mapfile;
	sq->getstring(v, 2, &mapfile);
	std::ofstream outfile(mapfile, std::ios::out | std::ios::binary);
	unsigned int temp = MAP_FILE_IDENTIFIER;
	outfile.write((char*)&temp, 4);
	temp = 0;
	outfile.write((char*)&temp, 4);
	outfile.write((char*)&xMin, 4);
	outfile.write((char*)&xMaxPlusOne, 4);
	outfile.write((char*)&yMin, 4);
	outfile.write((char*)&yMaxPlusOne, 4);
	unsigned int Location1 = 0x30;//0x18
	//write location
	outfile.write((char*)&Location1, 4);
	unsigned int Location1End = Location1 + (unsigned int)ceil((xWidth) * (yWidth) / 8)-1;
	outfile.write((char*)&Location1End, 4);//0x1c
	
	outfile.write((char*)&temp, 4);//Location2 0x20
	outfile.write((char*)&temp, 4);//Location2End 0x24
	unsigned int Location3 = Location1End + 1; //Location3 0x28
	outfile.write((char*)&Location3, 4);
	outfile.write((char*)&temp, 4);//Location3End  0x2c
	unsigned int Location2End, Location3End;
	//writing first part
	outfile.write((char*)(pbitZExists), (unsigned int)ceil((xWidth) * (yWidth)/8));
	
	//writing second part
	unsigned char byte = 0; unsigned int index = 0;
	unsigned int nBytespart2 = 0; unsigned int k=0;
	bool flag = false;
	unsigned char* part2 = new unsigned char[(unsigned int)ceil((xWidth) * (yWidth) / 8)];
	for (int i = xMin; i < xMaxPlusOne; i++)
	{
		for (int j = yMin; j < yMaxPlusOne; j++)
		{
			if (!((pbitZExists[((i - xMin) * yWidth + (j - yMin)) / 8] & (unsigned char)pow(2, 7 - (((i - xMin) * yWidth + (j - yMin)) % 8))) == (unsigned char)pow(2, 7 - (((i - xMin) * yWidth + (j - yMin)) % 8))))
				continue;
			if (flag==false)
			{
				byte = 1; k = (i - xMin) * yWidth + (j - yMin);
				outfile.write((char*)(pZ + (i - xMin) * yWidth + (j - yMin)), 4);
				Location3End = Location3 + 3;
				flag = true; 
			}
			else
			{
				
				if (pZ[(i - xMin) * yWidth + (j - yMin)] != pZ[k])
				{
					byte = (byte << 1) | 1;
					k = (i - xMin) * yWidth + (j - yMin);
					outfile.write((char*)(pZ + (i - xMin) * yWidth + (j - yMin)), 4);
					Location3End += 4;
				}
				else
					byte = (byte << 1) | 0;
				if (index % 8 == 7)
				{
					part2[nBytespart2++] = byte;
					byte = 0;
				}
			}
			index += 1;
		}
	}
	if (index % 8 != 0)
	{
		part2[nBytespart2++] = byte;
	}
	unsigned int Location2 = Location3End + 1;
	outfile.write((char*)part2, nBytespart2);
	Location2End = Location2 + nBytespart2 - 1;
	
	outfile.seekp(0x20, std::ios::beg);//Location2
	outfile.write((char*)&Location2, 4);
	outfile.write((char*)&Location2End, 4);
	outfile.seekp(0x2c, std::ios::beg);
	outfile.write((char*)&Location3End, 4);
	delete[] part2;
	outfile.close();
	printf("done");
	sq->pushbool(v, SQTrue);
	return 1;
}
SQInteger fn_MiamiScale_Unload(HSQUIRRELVM v)
{
	if (!init)
	{
		sq->pushbool(v, SQFalse);
		return 1;
	}
	free(pZ);
	free(pbitZExists);
	init = false;
	sq->pushbool(v, SQTrue);
	return 1;
}
SQInteger fn_MiamiScale_SetZ(HSQUIRRELVM v)
{
	if (!init)
		return sq->throwerror(v, "A map needs to be loaded first\n");
	SQInteger x, y, temp; SQFloat z;
	sq->getinteger(v, 2, &x);
	sq->getinteger(v, 3, &y);
	if (sq->gettype(v, 4) == OT_INTEGER)
	{
		sq->getinteger(v, 4, &temp);
		z = (float)temp;
	}
	else sq->getfloat(v, 4, &z);
	if (x < xMin || x >= xMaxPlusOne || y < yMin || y >= yMaxPlusOne)
	{
		if (x < xMin)
			r(x, xMaxPlusOne, yMin, yMaxPlusOne);
		if (y < yMin)
			r(xMin, xMaxPlusOne, y, yMaxPlusOne);
		if(x>=xMaxPlusOne)
			r(xMin, x+1, yMin, yMaxPlusOne);
		if(y>=yMaxPlusOne)
			r(xMin, xMaxPlusOne, yMin, y+1);
	}
	unsigned char byte = pbitZExists[((x - xMin) * yWidth + (y - yMin)) / 8];
	unsigned char t = ((x - xMin) * yWidth + (y - yMin)) % 8;
	t = 7 - t;
	pbitZExists[((x - xMin) * yWidth + (y - yMin)) / 8] |= (unsigned char)pow(2, t);
	pZ[(x - xMin) * yWidth + (y - yMin)] = z;
	sq->pushbool(v, SQTrue);
return 1;
}

SQInteger r(int _xMin, int _xMaxPlusOne, int _yMin, int _yMaxPlusOne)
{
	if (!init)
		return sq->throwerror(v, "Map not loaded");
	if (_xMin <= xMin && _xMaxPlusOne >= xMaxPlusOne && _yMin <= yMin &&
		_yMaxPlusOne >= yMaxPlusOne)
	{
		int _xWidth = _xMaxPlusOne - _xMin;
		int _yWidth = _yMaxPlusOne - _yMin;
		//Reallocate memory
		int _m = _xWidth * _yWidth;
		float* temp1 = (float*)realloc(pZ, _xWidth * _yWidth * sizeof(float));
		unsigned char* temp2 = (unsigned char*)realloc(pbitZExists, _xWidth * _yWidth / 8);
		if (!temp1 || !temp2)
			return sq->throwerror(v, "Error when allocating memory\n");
		pZ = temp1; pbitZExists = temp2;
		int m = xWidth * yWidth;  int _x, _y; int k1;
		unsigned char byte = 0;
		for (int k2 = _m - 1; k2 >= 0; k2--)
		{
			_y = k2 % _yWidth + _yMin;
			_x = (k2 + _yMin - _y) / _yWidth + _xMin;
			
			if (_x >= xMaxPlusOne || _x < xMin ||
				_y >= yMaxPlusOne || _y < yMin)
			{
				pZ[k2] = 0;
				//No change to byte.
			}
			else
			{
				k1 = (_x - xMin) * yWidth + _y - yMin;
				if ((pbitZExists[k1 / 8] & (unsigned char)pow(2, 7 - (k1 % 8))) == (unsigned char)pow(2, 7 - k1 % 8))
				{
					pZ[k2] = pZ[k1];//k2 will be greater than k1
					byte = (1<< (7-(k2 % 8))) | byte;
				}
			}
			if ( k2 % 8 == 0)
			{
				pbitZExists[k2 / 8] = byte;
				byte = 0;
			}
		}
		xMin = _xMin; yMin = _yMin;
		xMaxPlusOne = _xMaxPlusOne; yMaxPlusOne = _yMaxPlusOne;
		xWidth = _xWidth; yWidth = _yWidth;
		return 1;
	}
	else return sq->throwerror(v, "New bounds do not contain existing one\n");
}
SQInteger fn_MiamiScale_GetAvgZ(HSQUIRRELVM v)
{
	if (!init)
		return sq->throwerror(v, "MiamiScale is not initted.\n");
	HSQOBJECT po;
	sq->getstackobj(v, 2, &po);
	SQFloat x = sq->objtofloat(&po);
	sq->getstackobj(v, 3, &po);
	SQFloat y = sq->objtofloat(&po);
	float zgrand = 0; int n = 0; float z;
	for (int i = (int)floor(x) - 2; i < (int)floor(x) + 3; i++)
	{
		for (int j = (int)floor(y) - 2; j < (int)floor(y) + 3; j++)
		{
			if (i < xMin || i >= xMaxPlusOne)continue;
			if (j < yMin || j >= yMaxPlusOne)continue;
			sq->pushroottable(v);
			sq->pushstring(v, "FindZFor2DCoord", -1);
			if (SQ_SUCCEEDED(sq->get(v, -2)))
			{
				sq->pushroottable(v);
				sq->pushinteger(v, i);
				sq->pushinteger(v, j);
				if (SQ_SUCCEEDED(sq->call(v, 3, 1, 1)))
				{
					if (sq->gettype(v, -1) == OT_FLOAT)
					{
						sq->getfloat(v, -1, &z);
						zgrand += z; n += 1;
					}
				}
			}
		}
	}
	if (n > 0)
	{
		sq->pushfloat(v, zgrand / n);
		return 1;
	}
	else sq->pushnull(v);
	return 1;
}
SQInteger fn_MiamiScale_GetZ(HSQUIRRELVM v)
{
	if (!init)
		return sq->throwerror(v, "MiamiScale is not Initted.\n");
	SQInteger x, y; unsigned char flag = 0; SQFloat X, Y;
	float z; bool failed = false; unsigned char flag2 = 0;
	unsigned char flag3 = 0; float zgrand=0; unsigned char flag4 = 0;
	
	if (sq->gettype(v, 2) == OT_INTEGER)
		flag |= 1;
	if (sq->gettype(v, 3) == OT_INTEGER)
		flag |= 2;
	if (flag == 3)
	{
		sq->getinteger(v, 2, &x);
		sq->getinteger(v, 3, &y);
	}else
	if (flag == 1)
	{
		sq->getinteger(v, 2, &x);
		sq->getfloat(v, 3, &Y);
		if ((int)ceil(Y) == (int)floor(Y))
			y = (SQInteger)Y;
		else goto end;
	}else
	if (flag == 2)
	{
		sq->getinteger(v, 3, &y);
		sq->getfloat(v, 2, &X);
		if ((int)ceil(X) == (int)floor(X))
			x = (SQInteger)X;
		else goto end;
	}else
	if (flag == 0)
	{
		sq->getfloat(v, 2, &X);
		sq->getfloat(v, 3, &Y);
		if ((int)ceil(Y) == (int)floor(Y))
			y = (SQInteger)Y;
		else goto end;
		if ((int)ceil(X) == (int)floor(X))
			x = (SQInteger)X;
		else goto end;
	}

	flag2 = 1;
	getz:
	if (x >= xMin && x < xMaxPlusOne && y >= yMin && y < yMaxPlusOne)
	{
		unsigned char byte = pbitZExists[((x -xMin) * yWidth + (y - yMin)) / 8];
		unsigned char t = ((x-xMin)*yWidth+(y-yMin)) % 8;
		t = 7 - t;
		if ((byte & (unsigned char)pow(2, t)) == (unsigned char)pow(2, t))
		{
			z = pZ[(x - xMin) * yWidth + (y - yMin)];
		}
		else
		{
			z = 0; failed = true;
		}
		if (flag2)
		{
			if (!failed)
				sq->pushfloat(v, z);
			else
				sq->pushnull(v);
			return 1;
		}
		else
			goto process;
	}
	else return sq->throwerror(v, "MiamiScale: given (x,y) out of bounds\n");
end:
	int x1, x2, y1, y2;
	if (flag == 0)
	{
		x1 = (int)floor(X); x2 = (int)ceil(X);
		y1 = (int)floor(Y); y2 = (int)ceil(Y);
	}
	else if (flag == 1)
	{
		x1 = x; x2 = x;
		y1 = (int)floor(Y); y2 = (int)ceil(Y);
	}
	else if (flag == 2)
	{
		x1 = (int)floor(X); x2 = (int)ceil(X);
		y1 = y; y2 = y;
	}
	
process:
	if (flag3 == 0)
	{
		if (x1 >= xMin && y1 >= yMin)
		{
			x = x1; y = y1; flag3 |= 1;
			goto getz;
		}
		else goto process2;
	}
	else if (flag3 == 1)
	{
		if (!failed)
			zgrand = z;
		else {
			sq->pushnull(v); return 1;
		}
		process2:
		if (x2 < xMaxPlusOne && y2 < yMaxPlusOne)
		{
			x = x2; y = y2; flag3|= 2;
			goto getz;
		}
		else goto process3;
	}
	else if (flag3<=3&&(flag3 &2) == 2)
	{
		if (!failed)
		{
			zgrand += z;
		}
		else {
			sq->pushnull(v); return 1;
		}
		process3:
		if (x1 >= xMin && y2 < yMaxPlusOne)
		{
			x = x1; y = y2; flag3 |= 4;
			goto getz;
		}
		else goto process4;
	}
	else if (flag3<=7&&(flag3 &4)== 4)
	{
		if (!failed)
		{
			zgrand += z;
		}
		else {
			sq->pushnull(v); return 1;
		}
		process4:
		if (x2 < xMaxPlusOne && y1 >= yMin)
		{
			x = x2; y = y1; flag3 |= 8;
			goto getz;
		}
		else goto processfinal;
	}
	else if ((flag3&8) == 8)
	{
		if (!failed)
		{
			zgrand += z;
		}
		else {
			sq->pushnull(v); return 1;
		}
	processfinal:
		if (flag3 == 0)//no z was possible
		{
			sq->pushnull(v); return 1;
		}
		else
		{
			unsigned char i = 0;
			if (flag3 & 1)i++;
			if (flag3 & 2)i++;
			if (flag3 & 4)i++;
			if (flag3 & 8)i++;
			if (i != 0)
			{
				sq->pushfloat(v, zgrand / i);
				return 1;
			}
			else
				return sq->throwerror(v, "Possible error in internal code\n");
		}
	}
	return 0;
}

uint8_t MiamiScale_OnServerInitialize()
{
	return 1;
}


unsigned int PluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo)
{
	funcs=pluginFuncs;
	// Plugin information
	pluginInfo->pluginVersion = 0x1;
	pluginInfo->apiMajorVersion = API_MAJOR;
	pluginInfo->apiMinorVersion = API_MINOR;
	memcpy(pluginInfo->name, "z-finder", 9); 
	size_t size;
	const void** sqExports = funcs->GetSquirrelExports(&size);
	if (sqExports != NULL && size > 0)
	{
		
			SquirrelImports** s = (SquirrelImports**)sqExports;
			SquirrelImports* f = (SquirrelImports*)(*s);
			if (f)
			{
				sq = *(f->GetSquirrelAPI());
				v = *(f->GetSquirrelVM());
				sq->pushroottable(v);
				sq->pushstring(v, "MiamiScale_Init", -1);
				sq->newclosure(v, fn_MiamiScale_Init, 0);
				sq->setparamscheck(v, 2, "ts");
				sq->setnativeclosurename(v, -1, "MiamiScale_Init");
				sq->newslot(v, -3, SQFalse);
				sq->pop(v, 1);

				sq->pushroottable(v);
				sq->pushstring(v, "FindZFor2DCoord", -1);
				sq->newclosure(v, fn_MiamiScale_GetZ, 0);
				sq->setparamscheck(v, 3, "tf|if|i");
				sq->setnativeclosurename(v, -1, "FindZFor2DCoord");
				sq->newslot(v, -3, SQFalse);
				sq->pop(v, 1);

				sq->pushroottable(v);
				sq->pushstring(v, "FindAverageZ", -1);
				sq->newclosure(v, fn_MiamiScale_GetAvgZ, 0);
				sq->setparamscheck(v, 3, "tf|if|i");
				sq->setnativeclosurename(v, -1, "FindAverageZ");
				sq->newslot(v, -3, SQFalse);
				sq->pop(v, 1);

				sq->pushroottable(v);
				sq->pushstring(v, "MiamiScale_Unload", -1);
				sq->newclosure(v, fn_MiamiScale_Unload, 0);
				sq->setparamscheck(v, 1, "t");
				sq->setnativeclosurename(v, -1, "MiamiScale_Unload");
				sq->newslot(v, -3, SQFalse);
				sq->pop(v, 1);

				sq->pushroottable(v);
				sq->pushstring(v, "MiamiScale_SaveCurrentMap", -1);
				sq->newclosure(v, fn_MiamiScale_SaveCurrentMap, 0);
				sq->setparamscheck(v, 2, "ts");
				sq->setnativeclosurename(v, -1, "MiamiScale_SaveCurrentMap");
				sq->newslot(v, -3, SQFalse);
				sq->pop(v, 1);

				sq->pushroottable(v);
				sq->pushstring(v, "SetZFor2DCoord", -1);
				sq->newclosure(v, fn_MiamiScale_SetZ, 0);
				sq->setparamscheck(v, 4, "tiif|i");
				sq->setnativeclosurename(v, -1, "SetZFor2DCoord");
				sq->newslot(v, -3, SQFalse);
				sq->pop(v, 1);
				
				
			}
			return 1;
	}
	
	return 0;
}
