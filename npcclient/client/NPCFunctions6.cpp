
#include "main.h"
#include <cmath>
extern HSQUIRRELVM v;
extern CPlayerPool* m_pPlayerPool;
extern NPC* iNPC;
extern CFunctions* m_pFunctions;
extern CPlayer* npc;
extern CPickupPool* m_pPickupPool;
extern CCheckpointPool* m_pCheckpointPool;
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
extern CVehiclePool* m_pVehiclePool;
extern uint32_t configvalue;
extern std::string store_download_location;
extern bool bDownloadStore;
extern bool bIsDownloadInProgress;
SQInteger fn_Suicide(HSQUIRRELVM v)
{
	if (m_pFunctions->Suicide() == funcError::NoError)
	{
		sq_pushbool(v, SQTrue);
	}
	sq_pushbool(v, SQFalse);
	return 1;
}
SQInteger fn_SetMyHealth(HSQUIRRELVM v)
{
	SQInteger health;
	if (SQ_SUCCEEDED(sq_getinteger(v, 2, &health)))
	{
		m_pFunctions->SetHealth((uint8_t)health, true);
		if (m_pFunctions->GetLastError() == funcError::NoError)
		{
			sq_pushbool(v, SQTrue);
			return 1;
		}
	}
	sq_pushbool(v, SQFalse);
	return 1;
}
SQInteger fn_GetMyHealth(HSQUIRRELVM v)
{
	uint8_t health = m_pFunctions->GetPlayerHealth(iNPC->GetID());
	sq_pushinteger(v, health);
	return 1;
}
SQInteger fn_SetConfig(HSQUIRRELVM v)
{
	SQInteger val;
	if (SQ_SUCCEEDED(sq_getinteger(v, 2, &val)))
	{
		m_pFunctions->SetConfig((uint32_t)val);
		sq_pushbool(v, SQTrue);
		return 1;
	}
	sq_pushbool(v, SQFalse);
	return 1;

}
SQInteger fn_LookAtPos(HSQUIRRELVM v)
{
	sq_pushbool(v, SQFalse);
	if (!npc)return 1; if (!iNPC||!iNPC->IsSpawned())return 1;
	if (npc->m_wVehicleId)return 1;
	VECTOR vecPos, myPos = npc->m_vecPos;
	if (SQ_SUCCEEDED(sq_getvector(v, 2, &vecPos)))
	{
		float myAngle = atan2(-(vecPos.X - myPos.X), vecPos.Y - myPos.Y);
		m_pFunctions->SetAngle(myAngle, true);
		sq_poptop(v); sq_pushbool(v, SQTrue);//pop false and push true
	}
	return 1;
}
SQInteger fn_LookAtPlayer(HSQUIRRELVM v)
{
	sq_pushbool(v, SQFalse);
	if (!npc)return 1; if (!iNPC || !iNPC->IsSpawned())return 1;
	if (npc->m_wVehicleId)return 1;
	SQInteger playerId;
	VECTOR vecPos, myPos = npc->m_vecPos;
	if (SQ_SUCCEEDED(sq_getinteger(v, 2, &playerId)))
	{
		if (m_pFunctions->IsPlayerConnected((uint8_t)playerId) && m_pFunctions->IsPlayerSpawned((uint8_t)playerId))
		{
			m_pFunctions->GetPlayerPosition(playerId, &vecPos);
			float myAngle = atan2(-(vecPos.X - myPos.X), vecPos.Y - myPos.Y);
			m_pFunctions->SetAngle(myAngle, true);
			sq_poptop(v); sq_pushbool(v, SQTrue);//pop false and push true
		}
	}
	return 1;
}
SQInteger fn_GetMyClass(HSQUIRRELVM v)
{
	if (iNPC && iNPC->Initialized())
	{
		sq_pushinteger(v, iNPC->PotentialClassID);
		return 1;
	}
	return 0;
}
SQInteger fn_RequestClassAbs(HSQUIRRELVM v)
{
	SQInteger classId;
	sq_getinteger(v, 2, &classId);
	if (classId >= 50 || classId < 0)return 0;// null will be returned
	if (iNPC )//&& !iNPC->IsSpawned())
	{
		iNPC->SetSpawnClass((uint8_t)classId);
		if (iNPC->IsSpawned() == false)
		{
			//Request class
			m_pFunctions->RequestClass(0);
			iNPC->AbsClassNotSpecified = false;
		}
		sq_pushbool(v, SQTrue);
		return 1;
	}
	return 0;
}
SQInteger fn_AmISpawned(HSQUIRRELVM v)
{
	if (!iNPC)return 0;
	if (iNPC->IsSpawned())
		sq_pushbool(v, SQTrue);
	else
		sq_pushbool(v, SQFalse);
	return 1;
}
SQInteger fn_GetMyArmour(HSQUIRRELVM v)
{
	uint8_t armour = m_pFunctions->GetPlayerArmour(iNPC->GetID());
	sq_pushinteger(v, armour);
	return 1;
}
SQInteger fn_GetMyTeam(HSQUIRRELVM v)
{
	uint8_t team = m_pFunctions->GetPlayerTeam(iNPC->GetID());
	sq_pushinteger(v, team);
	return 1;
}

SQInteger fn_GetMySkin(HSQUIRRELVM v)
{
	uint8_t team = m_pFunctions->GetPlayerSkin(iNPC->GetID());
	sq_pushinteger(v, team);
	return 1;
}
SQInteger fn_SendPrivMsg(HSQUIRRELVM v)
{
	SQInteger playerId; const SQChar* msg;
	sq_getinteger(v, 2, &playerId);
	sq_getstring(v, 3, &msg);
	m_pFunctions->SendPrivMessage(playerId, msg);
	if (m_pFunctions->GetLastError() == funcError::NoError)
	{
		sq_pushbool(v, SQTrue);
	}
	else sq_pushbool(v, SQFalse);
	return 1;
}

//For InPolyTest

struct point
{
	float x; float y;
};

bool InPolyTest(float px, float py, const std::vector<point>& polygon) {
	int n = polygon.size();
	//printf("n is %d\n", n);
	if (n < 3) return false; // A polygon must have at least 3 points
	
	//https://sidvind.com/wiki/Point-in-polygon:_Jordan_Curve_Theorem
	
	int crossings = 0;

	for (int i = 0; i < n; ++i) {
		// Get the points for the current line segment
		float x1, x2;
		if (polygon[i].x < polygon[(i + 1) % n].x) {
			x1 = polygon[i].x;
			x2 = polygon[(i + 1) % n].x;
		}
		else {
			x1 = polygon[(i + 1) % n].x;
			x2 = polygon[i].x;
		}

		// Check if the ray is possible to cross the line segment
		if (px > x1 && px <= x2 && (py < polygon[i].y || py <= polygon[(i + 1) % n].y)) {
			static const float eps = 0.000001f;

			// Calculate the equation of the line
			float dx = polygon[(i + 1) % n].x - polygon[i].x;
			float dy = polygon[(i + 1) % n].y - polygon[i].y;
			float k;

			if (std::fabs(dx) < eps) {
				k = INFINITY; // Vertical line
			}
			else {
				k = dy / dx;
			}

			float m = polygon[i].y - k * polygon[i].x;

			// Find if the ray crosses the line segment
			float y2 = k * px + m;
			if (py <= y2) {
				crossings++;
			}
		}
	}
	//printf("crossings is %d\n", crossings);
	// If crossings are odd, the point is inside the polygon
	return (crossings % 2 == 1);
}
SQInteger fn_InPolyTest(HSQUIRRELVM v)
{
	HSQOBJECT* _px, *_py;
	_px = (HSQOBJECT*)malloc(sizeof(HSQOBJECT));
	_py = (HSQOBJECT*)malloc(sizeof(HSQOBJECT));
	if (!_px || !_py)
		return sq_throwerror(v, "Error allocating memory");
	sq_resetobject(_px);
	sq_resetobject(_py);
	sq_getstackobj(v, 2, _px);
	sq_getstackobj(v, 3, _py);
	float px, py;
	px=sq_objtofloat(_px);
	py=sq_objtofloat(_py);
	free(_px); free(_py);
	int n = sq_gettop(v);
	if (n % 2 == 0)
		return sq_throwerror(v, "Error: y missing for one x point");
	int z = 4;// idx from 4 onwards
	
	//printf("Enumeriating vertices..\n");
	std::vector<point> vertices;
	point p;
	HSQOBJECT* __x, *__y;
	__x = (HSQOBJECT*)malloc(sizeof(HSQOBJECT));
	__y = (HSQOBJECT*)malloc(sizeof(HSQOBJECT));
	if (!__x || !__y)return sq_throwerror(v, "Error when allocating memory");
	sq_resetobject(__x);
	sq_resetobject(__y);
	//printf("Starting while loop..\n");
	while (z < n) //z+1 will be <= n because of throwerror above
	{
		sq_resetobject(__x);
		sq_resetobject(__y);
		//printf("While Loop. z is %d and n is %d\n", z, n);
		SQRESULT r1= sq_getstackobj(v, z, __x);
		SQRESULT r2 = sq_getstackobj(v, z + 1, __y);
		if (SQ_FAILED(r1) || SQ_FAILED(r2))
		{
			return sq_throwerror(v, "Error when getting stack object");
		}
		//printf("Got Stack object\n");
		p.x = sq_objtofloat(__x);
		p.y = sq_objtofloat(__y);
		//printf("p.x is %f and p.y is %f\n", p.x, p.y);
		vertices.push_back(p);
		//printf("Pushed into vector vertices\n");
		z += 2;
		//printf("z incremented by 2\n");
	}
	//printf("going to call InPolyTest\n");
	//We got px, py and vector<point> vertices. 
	bool b=InPolyTest(px, py, vertices);
	//printf("InPolyTest returned\n");
	free(__x); free(__y);
	if (b)
		sq_pushbool(v, SQTrue);
	else
		sq_pushbool(v, SQFalse);
	return 1;
}
SQInteger fn_FireBullet(HSQUIRRELVM v)
{
#ifdef _REL047
	SQInteger weaponId;
	sq_getinteger(v, 2, &weaponId);
	HSQOBJECT* x, * y, * z;
	x = (HSQOBJECT*)malloc(sizeof(HSQOBJECT));
	if (!x)return sq_throwerror(v, "Error. Memory allocation failed");
	y = (HSQOBJECT*)malloc(sizeof(HSQOBJECT));
	if (!y)return sq_throwerror(v, "Error. Memory allocation failed");
	z = (HSQOBJECT*)malloc(sizeof(HSQOBJECT));
	if (!z)return sq_throwerror(v, "Error. Memory allocation failed");
	sq_resetobject(x);
	sq_resetobject(y);
	sq_resetobject(z);
	sq_getstackobj(v, 3, x);
	sq_getstackobj(v, 4, y);
	sq_getstackobj(v, 5, z);
	SQFloat _x, _y, _z;
	_x=sq_objtofloat(x);
	_y=sq_objtofloat(y);
	_z=sq_objtofloat(z);
	free(x); free(y); free(z);
	m_pFunctions->FireBullet(weaponId, _x, _y, _z);
#endif
	return 0;
}
SQInteger fn_GetStoreLocation(HSQUIRRELVM v)
{
	if (bDownloadStore)
		sq_pushstring(v, store_download_location.c_str(), -1);
	else
		sq_pushnull(v);
	return 1;
}
SQInteger fn_IsStoreDownloadInProgress(HSQUIRRELVM v)
{
	if (!bDownloadStore)return 0;//return null
	if (bIsDownloadInProgress)
		sq_pushbool(v, SQTrue);
	else
		sq_pushbool(v, SQFalse);
	return 1;
}
SQInteger fn_GetStoreUrl(HSQUIRRELVM v)
{
	sq_pushstring(v, iNPC->storeURL.c_str(),-1);
	return 1;
}
void RegisterNPCFunctions6()
{
	register_global_func(v, ::fn_Suicide, "Suicide", 1, "t");
	register_global_func(v, ::fn_SetMyHealth, "SetMyHealth", 2, "ti");
	register_global_func(v, ::fn_GetMyHealth, "GetMyHealth", 1, "t");
	register_global_func(v, ::fn_SetConfig, "SetConfig", 2, "ti");
	register_global_func(v, ::fn_LookAtPos, "LookAtPos", 2, "tx");
	register_global_func(v, ::fn_LookAtPlayer, "LookAtPlayer", 2, "ti");
	register_global_func(v, ::fn_GetMyClass, "GetMyClass", 1, "t");
	register_global_func(v, ::fn_RequestClassAbs, "RequestClassAbs", 2, "ti");
	register_global_func(v, ::fn_AmISpawned, "AmISpawned", 1, "t");
	register_global_func(v, ::fn_GetMySkin, "GetMySkin", 1, "t");
	register_global_func(v, ::fn_GetMyTeam, "GetMyTeam", 1, "t");
	register_global_func(v, ::fn_GetMyArmour, "GetMyArmour", 1, "t");
	register_global_func(v, ::fn_SendPrivMsg, "SendPrivMsg", 3, "tis");
	register_global_func(v, ::fn_InPolyTest, "InPoly", -9, "tf|if|if|if|if|if|if|if|i");
	register_global_func(v, ::fn_FireBullet, "FireBullet", 5, "tif|if|if|i");
	register_global_func(v, ::fn_GetStoreLocation, "GetStoreLocation", 1, "t");
	register_global_func(v, ::fn_IsStoreDownloadInProgress, "IsStoreDownloading", 1, "t");
	register_global_func(v, ::fn_GetStoreUrl, "GetStoreURL", 1, "t");

}