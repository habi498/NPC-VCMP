#ifndef MESSAGEIDS_0471
#define MESSAGEIDS_0471
enum GameMessagesFromServer
{
	ID_GAME_MESSAGE_ONFOOT_UPDATE = 0x30, //both server and client //0x30
	ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM, //both s and c //0x31
	ID_GAME_MESSAGE_PLAYERUPDATE_DRIVER, //both s and c //0x32
	ID_GAME_MESSAGE_PLAYERUPDATE_PASSENGER, //both s, c //0x33
	ID_GAME_MESSAGE_SYNC_VEHICLE, //0x34
	ID_GAME_MESSAGE_PLAYER_CONNECTED, //both s,c same id //0x35
	ID_GAME_MESSAGE_DISCONNECT, // " //0x36
	ID_GAME_MESSAGE_NAME_CHANGE, //0x37
	ID_GAME_MESSAGE_PLAYER_DEATH, //0x38
	ID_GAME_MESSAGE_PLAYER_STREAM_IN, //0x39
	ID_GAME_MESSAGE_PLAYER_STREAM_OUT, //0x3A
	ID_GAME_MESSAGE_SET_SKIN, //0x3B
	ID_GAME_MESSAGE_SET_TEAM, //0x3C
	ID_GAME_MESSAGE_SET_COLOUR, //0x3D
	ID_GAME_MESSAGE_UNKNOWN1, //0x3E
	ID_GAME_MESSAGE_VEHICLE_ENTER, //0x3F
	ID_GAME_MESSAGE_UNKNOWN2, //0x40
	ID_GAME_MESSAGE_VEHICLE_EXIT_EVENT, //0x41
	ID_GAME_MESSAGE_UNKNOWN3, //0x42
	ID_GAME_MESSAGE_UNKNOWN4, //3-d arrow here //0x43
	ID_GAME_MESSAGE_UNKNOWN5, //0x44
	ID_GAME_MESSAGE_UNKNOWN6, //0x45
	ID_GAME_MESSAGE_UNKNOWN7, //0x46
	ID_GAME_MESSAGE_UNKNOWN8, //crouch enabled //0x47
	ID_GAME_MESSAGE_TIMEWEATHER_SYNC, //0x48
	ID_GAME_MESSAGE_UNKNOWN9, //0x49
	ID_GAME_MESSAGE_TICK, //0x4A
#ifdef _REL0470
	ID_GAME_MESSAGE_UNKNOWN10, // specific to _REL0470
#endif
	ID_GAME_MESSAGE_JOIN, //0x4B
	ID_GAME_MESSAGE_UNKNOWN11, //0x4C disconnect ?
	ID_GAME_MESSAGE_UNKNOWN12, //0x4D
	ID_GAME_MESSAGE_SPECIAL_SIGNAL, //0x4E
	ID_GAME_MESSAGE_UNKNOWN13, //0x4F
	ID_GAME_MESSAGE_SET_HEALTH, //0x50
	ID_GAME_MESSAGE_SET_ARMOUR, //0x51
	ID_GAME_MESSAGE_SPAWN_GRANTED, //0x52
	ID_GAME_MESSAGE_FORCEALL_SELECT, //0x53
	ID_GAME_MESSAGE_UNKNOWN14, //0x54
	ID_GAME_MESSAGE_SET_POSITION, //0x55
	ID_GAME_MESSAGE_UNKNOWN15, //0x56
	ID_GAME_MESSAGE_UNKNOWN16, //0x57
	ID_GAME_MESSAGE_SET_ANGLE, //0x58
	ID_GAME_MESSAGE_UNKNOWN17, //0x59
	ID_GAME_MESSAGE_REMOVE_ALL_WEAPONS, //0x5A
	ID_GAME_MESSAGE_SET_PLAYER_WEAPON_SLOT, //0x5B
	ID_GAME_MESSAGE_SET_WEAPON, //0x5C
	ID_GAME_MESSAGE_REMOVE_WEAPON, //0x5D
	ID_GAME_MESSAGE_FREEZE_PLAYER, //player options //0x5E
	ID_GAME_MESSAGE_SET_CAMERA, //0x5F
#ifdef _REL047
	ID_GAME_MESSAGE_INTERPOLATE_CAMERALOOKAT, //0x60
#endif
	ID_GAME_MESSAGE_RESTORE_CAMERA, //0x61
#ifdef _REL047
	ID_GAME_MESSAGE_SETDRUNK_HANDLINGANDVISUALS, //0x62
	ID_GAME_MESSAGE_UNKNOWNXX, //0x63
#endif
	ID_GAME_MESSAGE_UNKNOWN18, //0x64
	ID_GAME_MESSAGE_UNSTREAMED_PLAYERDATA, //spawned but not streamed //0x65
	ID_GAME_MESSAGE_SNIPER_RIFLE_FIRED, //0x66
	ID_GAME_MESSAGE_PROJECTILE_FIRED, //0x67
	ID_GAME_MESSAGE_EXPLOSION_CREATED, //0x68
	ID_GAME_MESSAGE_UNKNOWN19, //0x69
	ID_GAME_MESSAGE_CLIENT_MESSAGE, //0x6A
	ID_GAME_MESSAGE_UNKNOWN20, //0x6B
	ID_GAME_MESSAGE_PLAYER_TEXT, //0x6C
	ID_GAME_MESSAGE_UNKNOWN21, //0x6D
	ID_GAME_MESSAGE_SERVER_DATA_RCVD, //0x6E
#ifndef _REL004 
	ID_GAME_MESSAGE_REQUESTMODULELIST, //0x6F
#endif
#ifdef _REL047
	ID_GAME_MESSAGE_UNKNOWNYY, //0x70
#endif
	ID_GAME_MESSAGE_OBJECT_CREATED, //0x71
	ID_GAME_MESSAGE_OBJECT_DESTROYED, //0x72
	ID_GAME_MESSAGE_OBJECT_CHANGE_ALPHA, //0x73
	ID_GAME_MESSAGE_OBJECT_CHANGE_SHOTS_BUMPS, //0x74
	ID_GAME_MESSAGE_OBJECT_ROTATE_TO, //0x75
	ID_GAME_MESSAGE_OBJECT_MOVEDTO, //0x76
	ID_GAME_MESSAGE_UNKNOWN22, //0x77
	ID_GAME_MESSAGE_OBJECT_SETPOSITION, //0x78
	ID_GAME_MESSAGE_PICKUP_CREATED, //0x79
	ID_GAME_MESSAGE_PICKUP_DESTROYED, //0x7A
	ID_GAME_MESSAGE_PICKUP_CHANGE_ALPHA, //0x7B
	ID_GAME_MESSAGE_PICKUP_CHANGE_POS, //0x7C
	ID_GAME_MESSAGE_REFRESH_PICKUP, //0x7D
	ID_GAME_MESSAGE_UNKNOWN23, //0x7E
	ID_GAME_MESSAGE_SET_VEHICLE_HANDLING, //0x7F
	ID_GAME_MESSAGE_UNKNOWN25, //0x80
	ID_GAME_MESSAGE_UNKNOWN26, //0x81
	ID_GAME_MESSAGE_UNKNOWN27, //0x82
	ID_GAME_MESSAGE_UNKNOWN28, //0x83
	ID_GAME_MESSAGE_UNKNOWN29, //0x84
	ID_GAME_MESSAGE_CREATE_CHECKPOINT, // old = 0xe5, //0x85
	ID_GAME_MESSAGE_DESTROY_CHECKPOINT, //0x86
	ID_GAME_MESSAGE_CHECKPOINT_CHANGE_POS, //0x87
	ID_GAME_MESSAGE_CHECKPOINT_CHANGE_COLOUR, //0x88
	ID_GAME_MESSAGE_CHECKPOINT_CHANGE_RADIUS, //0x89
	ID_GAME_MESSAGE_VEHICLE_STREAM_IN, //0x8A
	ID_GAME_MESSAGE_VEHICLE_STREAM_OUT, //0x8B
	ID_GAME_MESSAGE_VEHICLE_SETPOSITION,

	//0x8C
	ID_GAME_MESSAGE_VEHICLE_SETROTATION, //0x8D
	ID_GAME_MESSAGE_UNKNOWN30, //0x8E
	ID_GAME_MESSAGE_VEHICLE_SETSPEED, //0x8F
	ID_GAME_MESSAGE_VEHICLE_ADDSPEED, //0x90
	ID_GAME_MESSAGE_VEHICLE_ADDRELSPEED, //0x91
	ID_GAME_MESSAGE_VEHICLE_SETTURNSPEED, //0x92
	ID_GAME_MESSAGE_VEHICLE_ADDTURNSPEED, //0x93
	ID_GAME_MESSAGE_VEHICLE_ADDRELTURNSPEED, //0x94
	ID_GAME_MESSAGE_VEHICLE_SETHEALTH, //0x95
	ID_GAME_MESSAGE_VEHICLE_SETDAMAGE, //0x96
	ID_GAME_MESSAGE_VEHICLE_ALARM_LIGHTS_IMMUNITY_3DARROW, //immunity=0,lights=1,alarm=3,isghost=4,police_siren=8,locked=0x23 //0x97
	ID_GAME_MESSAGE_VEHICLE_LOST_OWNERSHIP, //0x98
	ID_GAME_MESSAGE_VEHICLE_GAIN_OWNERSHIP, //0x99
	ID_GAME_MESSAGE_VEHICLE_SETEXPLODE, //0x9A
	ID_GAME_MESSAGE_VEHICLE_SETRADIO, // old = 0xfb, new 0x9b //0x9B
	ID_GAME_MESSAGE_UNKNOWN48,	//0x9c
	ID_GAME_MESSAGE_BULLET_FIRED //0x9d
};
#endif