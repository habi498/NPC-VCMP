
#define PACKET_SPAWN 1
#define PACKET_DEATH 2
#define PACKET_VEHICLE_EXIT 3
#define PACKET_VEHICLE_REQ_ENTER	4
#define PACKET_VEHICLE_ENTER	5
#define PACKET_ONFOOT	6
#define PACKET_ONFOOT_AIM	7
#define PACKET_DRIVER 8
#define PACKET_PASSENGER	9
#define PACKET_PLAYER_COMMAND	10
#define PACKET_PLAYER_CHAT	11
#define PACKET_PLAYER_PM	12
#define PACKET_CLIENTSCRIPT_DATA	13
#define PACKET_PICKUP_PICKED	14
#define PACKET_CHECKPOINT_ENTERED	15
#define PACKET_CHECKPOINT_EXITED 16
#define PACKET_OBJECT_TOUCH	17
#define PACKET_OBJECT_SHOT	18
#define PACKET_DISCONNECT	19
#define PACKET_CHOOSECLASS 20


#define REC_NAME	1
#define REC_SPAWN	2
#define REC_ONFOOT_NORMAL	4
#define REC_ONFOOT_AIM		8
#define REC_VEHICLE_DRIVER	16
#define REC_VEHICLE_PASSENGER	32
#define REC_DEATH	64
#define REC_OBJECT_SHOT	128
#define REC_OBJECT_TOUCH 256
#define REC_PICKUP_PICKED	512
#define REC_CHECKPOINT_ENTERED	1024
#define REC_CHECKPOINT_EXITED	2048
#define REC_PLAYER_COMMAND	4096
#define REC_PLAYER_CHAT		8192	//0x2000
#define REC_PLAYER_PM	0x4000
#define REC_CLIENTSCRIPT_DATA	0x8000
#define REC_DISCONNETION	0x10000
#define REC_STANDARD	(REC_ONFOOT_NORMAL|REC_ONFOOT_AIM|REC_VEHICLE_DRIVER|REC_VEHICLE_PASSENGER)
#define REC_AFTERLIFE	(REC_ONFOOT_NORMAL|REC_ONFOOT_AIM|REC_VEHICLE_DRIVER|REC_VEHICLE_PASSENGER|REC_SPAWN|REC_DEATH)
#define REC_SPYMODE		(REC_ONFOOT_NORMAL|REC_ONFOOT_AIM|REC_VEHICLE_DRIVER|REC_VEHICLE_PASSENGER|REC_SPAWN|REC_DEATH|REC_PLAYER_COMMAND|REC_PLAYER_CHAT|REC_PLAYER_PM)
#define REC_RACEMODE	(REC_ONFOOT_NORMAL|REC_ONFOOT_AIM|REC_VEHICLE_DRIVER|REC_VEHICLE_PASSENGER|REC_SPAWN|REC_DEATH|REC_CHECKPOINT_ENTERED|REC_CHECKPOINT_EXITED)
#define REC_RAMPAGE (REC_ONFOOT_NORMAL|REC_ONFOOT_AIM|REC_VEHICLE_DRIVER|REC_VEHICLE_PASSENGER|REC_SPAWN|REC_DEATH|REC_PICKUP_PICKED)
#define REC_RIFLERANGE (REC_ONFOOT_NORMAL|REC_ONFOOT_AIM|REC_VEHICLE_DRIVER|REC_VEHICLE_PASSENGER|REC_SPAWN|REC_DEATH|REC_OBJECT_SHOT|REC_OBJECT_TOUCH)
#define REC_EVERYTHING (REC_ONFOOT_NORMAL|REC_ONFOOT_AIM|REC_VEHICLE_DRIVER|REC_VEHICLE_PASSENGER|REC_SPAWN|REC_DEATH|REC_OBJECT_SHOT|REC_OBJECT_TOUCH|REC_CHECKPOINT_ENTERED|REC_CHECKPOINT_EXITED|REC_PICKUP_PICKED|REC_PLAYER_COMMAND|REC_PLAYER_CHAT|REC_PLAYER_PM|REC_CLIENTSCRIPT_DATA|REC_DISCONNETION)
#define REC_ALLNAME (REC_NAME|REC_ONFOOT_NORMAL|REC_ONFOOT_AIM|REC_VEHICLE_DRIVER|REC_VEHICLE_PASSENGER|REC_SPAWN|REC_DEATH|REC_OBJECT_SHOT|REC_OBJECT_TOUCH|REC_CHECKPOINT_ENTERED|REC_CHECKPOINT_EXITED|REC_PICKUP_PICKED|REC_PLAYER_COMMAND|REC_PLAYER_CHAT|REC_PLAYER_PM|REC_CLIENTSCRIPT_DATA|REC_DISCONNETION)

#define CLASS_PREVIOUS 255
#define CLASS_CURRENT 0
#define CLASS_NEXT 1
#include <stdint.h>
#include <math.h>
#ifdef _WIN32
	#include <windows.h>
	
#else
	typedef unsigned int DWORD;
#endif
#pragma pack(1)
typedef struct _VECTOR
{
	float X;
	float Y;
	float Z;
	_VECTOR()
	{
		this->X = 0.0, this->Y = 0.0, this -> Z = 0.0;
	}
	_VECTOR(float x, float y, float z)
	{
		this->X = x;
		this->Y = y;
		this->Z = z;
	}
	_VECTOR operator+(const _VECTOR& vector)
	{
		_VECTOR result;
		result.X = this->X + vector.X;
		result.Y = this->Y + vector.Y;
		result.Z = this->Z + vector.Z;
		return result;
	}
	_VECTOR operator+=(const _VECTOR& vector)
	{
		this->X +=  vector.X;
		this->Y +=  vector.Y;
		this->Z +=  vector.Z;
		return *this;
	}
	_VECTOR operator-(const _VECTOR& vector)
	{
		_VECTOR result;
		result.X = this->X - vector.X;
		result.Y = this->Y - vector.Y;
		result.Z = this->Z - vector.Z;
		return result;
	}
	_VECTOR operator-=(const _VECTOR& vector)
	{
		this->X -= vector.X;
		this->Y -= vector.Y;
		this->Z -= vector.Z;
		return *this;
	}

	float GetMagnitude()
	{
		return (float)sqrt(
			pow(this->X, 2) + pow(this->Y, 2) + pow(this->Z, 2)
			);
	}
}VECTOR;

#pragma pack(1)
typedef struct _QUATERNION
{
	float X;
	float Y;
	float Z;
	float W;
	_QUATERNION()
	{
		this->X = 0;
		this->Y = 0;
		this->Z = 0;
		this->W = 1;
	}
	_QUATERNION(float x, float y, float z, float w)
	{
		this->X = x;
		this->Y = y;
		this->Z = z;
		this->W = w;
	}
}QUATERNION;

#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA
{
	uint32_t dwKeys;
	VECTOR vecPos;
	float fAngle;
	uint8_t byteHealth;
	uint8_t byteArmour;
	uint8_t byteCurrentWeapon;
	bool IsCrouching;
	VECTOR vecSpeed;
	bool IsPlayerUpdateAiming;
	VECTOR vecAimDir;
	VECTOR vecAimPos;
	bool bIsReloading; //1 byte
	uint16_t wAmmo; // 2 byte
	uint8_t byteAction;//added v1.7 beta 2
	uint8_t byteReserved;//for future use
	//VECTOR vecMatrixRight;
	//VECTOR vecMatrixUp;
	//VECTOR vecMatrixPosition;
	_ONFOOT_SYNC_DATA()
	{
		dwKeys = 0; vecPos = VECTOR(0, 0, 0);
		fAngle = 0.0; byteHealth = 100;
		byteArmour = 0; byteCurrentWeapon = 0;
		IsCrouching = false;
		vecSpeed = VECTOR(0, 0, 0);
		IsPlayerUpdateAiming = false;
		vecAimDir = VECTOR(0, 0, 0);
		vecAimPos = VECTOR(0, 0, 0);
		bIsReloading = false;
		wAmmo = 0; byteAction = 1; byteReserved = 0;
		//vecMatrixRight = VECTOR(0, 0, 0);
		//vecMatrixUp = VECTOR(0, 0, 0);
		//vecMatrixPosition = VECTOR(0, 0, 0);
	}
} ONFOOT_SYNC_DATA; 
//--new
// 102 bytes size ONFOOT_SYNC_DATA
#pragma pack(1)
typedef struct _INCAR_SYNC_DATA
{
	uint16_t VehicleID; //The ID of Car
	uint32_t dwKeys;	//The Keys the related player is pressing
	QUATERNION quatRotation;	//The rotation of car
	VECTOR vecPos;	//The position of car
	VECTOR vecMoveSpeed;	//The speed of car
	float fCarHealth;	//The health of car in floating points.
	uint8_t bytePlayerHealth; //The health of player
	uint8_t bytePlayerArmour; //The armour of player
	uint8_t byteCurrentWeapon; //The current weapon of player
	uint32_t dDamage;	//The damage of car
	float Turretx;	//The horizontal turret of car
	float Turrety;	//The vertical turret of car
	uint16_t wAmmo;	//The ammo of current weapon of player.
} INCAR_SYNC_DATA;

/*Used to save to .rec file*/
#pragma pack(1)
typedef struct _ONFOOT_DATABLOCK
{
	DWORD time;
	ONFOOT_SYNC_DATA  m_pOfSyncData;
} ONFOOT_DATABLOCK;

#pragma pack(1)
typedef struct _INCAR_DATABLOCK
{
	DWORD time;
	INCAR_SYNC_DATA  m_pIcSyncData;
} INCAR_DATABLOCK;


#pragma pack(1)
typedef struct _COMMANDDATA
{
	uint16_t len;
	//char msg[len];
}COMMANDDATA;

//MESSAGEDATA same as COMMANDDATA

#pragma pack(1)
typedef struct _PRIVMSGDATA
{
	char target[24] = {};
	uint16_t msglen;
	//char msg[len];
}PRIVMSGDATA;


//CLIENTSCRIPTDATA same as COMMANDDATA

#pragma pack(1)
typedef struct _PICKUPATTEMPTDATA
{
	uint16_t wPickupID; //2 bytes
	uint16_t wModel; //2 bytes;
}PICKUPATTEMPTDATA;

#pragma pack(1)
typedef struct _CHECKPOINTENTEREDDATA
{
	uint16_t wCheckpointID; //2 bytes
	VECTOR vecPos;
}CHECKPOINTENTEREDDATA;

//CHECKPOINTEXITED same as ENTERED

#pragma pack(1)
typedef struct _OBJECTSHOT_DATA
{
	uint16_t wObjectID; //2 bytes
	uint16_t wModel; //2 bytes
	uint8_t byteWeapon;
}OBJECTSHOT_DATA;

#pragma pack(1)
typedef struct _OBJECTTOUCH_DATA
{
	uint16_t wObjectID; //2 bytes
	uint16_t wModel; //2 bytes
}OBJECTTOUCH_DATA;

#pragma pack(1)
typedef struct _SPAWNDATA
{
	uint8_t byteTeam;
	uint16_t wSkin;
	uint8_t byteClassId;
}SPAWNDATA;

typedef struct _CLASSCHOOSEDATA
{
	uint8_t byteIndex = 0; //0, -1(255) for previous skin 1 for next
	uint8_t byteclassId = 0;
}CLASSCHOOSEDATA;
#pragma pack(1)
#pragma pack(1)
typedef struct _DEATHDATA
{
	char killername[24]{};
	uint8_t byteReason;
	uint8_t byteBodyPart;
}DEATHDATA;

#pragma pack(1)
typedef struct _VEHICLENETER_REQUEST
{
	uint16_t wVehicleID;
	uint16_t wModel;
	uint8_t SeatId;
}VEHICLEENTER_REQUEST;

//VEHICLEENTERED same as REQUEST
#pragma pack(1)
typedef struct _PASSENGERDATA
{
	uint16_t wVehicleID;
	uint16_t wModel;
	uint8_t byteHealth;
	uint8_t byteArmour;
	uint8_t SeatId;
}PASSENGERDATA;


typedef struct _GENERALDATABLOCK
{
	uint32_t time;
	uint8_t bytePacketType;
}GENERALDATABLOCK;

#pragma pack(1)
typedef struct _RGB
{
	uint8_t r;
	uint8_t g;
	uint8_t b;

	_RGB()
	{
		this->r = 0, this->g = 0; this->b = 0;
	}
	_RGB(uint8_t r, uint8_t g, uint8_t b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
}Color;