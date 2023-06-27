/*
	Copyright (C) 2022  habi

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
enum GameMessages
{
	ID_GAME_MESSAGE_ONFOOT_UPDATE = 0x93, //both server and client
	ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM = 0x94, //both s and c
	ID_GAME_MESSAGE_PLAYERUPDATE_DRIVER = 0x95, //both s and c
	ID_GAME_MESSAGE_PLAYERUPDATE_PASSENGER = 0x96, //both s, c
	ID_GAME_MESSAGE_CONNECT = 0x98,//both s,c same id
	ID_GAME_MESSAGE_DISCONNECT = 0x99,// "
	ID_GAME_MESSAGE_NAME_CHANGE = 0x9a,
	ID_GAME_MESSAGE_PLAYER_DEATH = 0x9b,
	ID_GAME_MESSAGE_PLAYER_STREAM_IN = 0x9c,
	ID_GAME_MESSAGE_PLAYER_STREAM_OUT = 0x9d,
	ID_GAME_MESSAGE_SET_SKIN = 0x9e,
	ID_GAME_MESSAGE_SET_TEAM=0x9f,// two bytes. pid, team
	ID_GAME_MESSAGE_SET_COLOUR = 0xa0,

	ID_GAME_MESSAGE_VEHICLE_ENTER = 0xa2,
	ID_GAME_MESSAGE_VEHICLE_EXIT = 0xa4,
	ID_GAME_MESSAGE_TICK=0xad,
	ID_GAME_MESSAGE_JOIN = 0xaf,
	ID_GAME_MESSAGE_SPECIAL_SIGNAL=0xb2, //comes along with b6 also.
	ID_GAME_MESSAGE_SET_HEALTH = 0xb4,
	ID_GAME_MESSAGE_SET_ARMOUR = 0xb5,
	ID_GAME_MESSAGE_SPAWN_GRANTED = 0xb6,
	ID_GAME_MESSAGE_SET_POSITION = 0xb9,
	ID_GAME_MESSAGE_SET_ANGLE = 0xbc,
	ID_GAME_MESSAGE_REMOVE_ALL_WEAPONS = 0xbe,
	ID_GAME_MESSAGE_SET_PLAYER_WEAPON_SLOT = 0xbf,
	ID_GAME_MESSAGE_SET_WEAPON = 0xc0,
	ID_GAME_MESSAGE_REMOVE_WEAPON = 0xc1,
	ID_GAME_MESSAGE_FREEZE_PLAYER =0xc2,// 10 62 00 freeze, 10 63 00 unfreeze
	ID_GAME_MESSAGE_SET_CAMERA=0xc3,
	ID_GAME_MESSAGE_RESTORE_CAMERA=0xc4,//no payload after c4
	//ID_GAME_MESSAGE_PLAYER_SPAWN=0xc6,
	ID_GAME_MESSAGE_SNIPER_RIFLE_FIRED = 0xc7, 
	ID_GAME_MESSAGE_CLIENT_MESSAGE = 0xcb,
	ID_GAME_MESSAGE_PLAYER_TEXT = 0xcd,
	ID_GAME_MESSAGE_PROJECTILE_FIRED=0xc8,
	ID_GAME_MESSAGE_EXPLOSION_CREATED=0xc9,
	ID_GAME_MESSAGE_SERVER_DATA_RCVD = 0xcf,
#ifndef _REL004
	ID_GAME_MESSAGE_VEHICLE_STREAM_IN = 0xea,
	//not done much investigation on vehicle stream in
	//0xec is also sometimes send along with 0xea
	ID_GAME_MESSAGE_VEHICLE_STREAM_OUT = 0xeb, // no doubt. but multiple times server may send
	ID_GAME_MESSAGE_VEHICLE_LOST_OWNERSHIP = 0xf8,
	ID_GAME_MESSAGE_VEHICLE_GAIN_OWNERSHIP = 0xf9,
#else
	ID_GAME_MESSAGE_VEHICLE_STREAM_IN = 0xe9,
	ID_GAME_MESSAGE_VEHICLE_STREAM_OUT = 0xea,
	ID_GAME_MESSAGE_VEHICLE_LOST_OWNERSHIP = 0xf7,
	ID_GAME_MESSAGE_VEHICLE_GAIN_OWNERSHIP = 0xf8,
#endif
	
	
	//another message 0xf9 is also send by server on exiting vehicle, 
	//which appears to be same as 0xf8. 

	ID_SERVER_MESSAGE_CLIENT_CONNECT = 0x98, //client->server
	ID_GAME_MESSAGE_BEGIN_TYPING=0x9b,//from rel004
	ID_GAME_MESSAGE_END_TYPING=0x9c,//from rel004

	
#ifndef _REL004
	ID_GAME_MESSAGE_CHAT = 0xa2, //client
	ID_GAME_MESSAGE_ENTER_VEHICLE_REQUEST = 0xab, //client->server 
	ID_GAME_MESSAGE_VEHICLE_PUT_ACCEPTED = 0xac, //client->server needs more info on this one

	ID_GAME_MESSAGE_COMMAND = 0xa4,//client
	ID_GAME_MESSAGE_REQUEST_CLASS = 0xa5,
	ID_GAME_MESSAGE_REQUEST_SPAWN = 0xa6, //client
	ID_GAME_MESSAGE_SPAWN = 0xa7, //client
	ID_GAME_MESSAGE_DEATH_INFO = 0xa8, //client
	ID_GAME_MESSAGE_PLAYER_SHOT_BODYPART = 0xa9, //client->server

	ID_GAME_MESSAGE_SNIPERFIRE = 0xae, //client
	ID_GAME_MESSAGE_FIRE_PROJECTILE = 0xaf,//client
	ID_GAME_MESSAGE_SERVER_DATA = 0xbd,
#else 
	ID_GAME_MESSAGE_CHAT = 0xa1,
	ID_GAME_MESSAGE_ENTER_VEHICLE_REQUEST = 0xaa, //client->server 
	ID_GAME_MESSAGE_VEHICLE_PUT_ACCEPTED = 0xab, //client->server needs more info on this one

	ID_GAME_MESSAGE_COMMAND = 0xa3,//client
	ID_GAME_MESSAGE_REQUEST_CLASS = 0xa4,//client
	ID_GAME_MESSAGE_REQUEST_SPAWN = 0xa5, //client
	ID_GAME_MESSAGE_SPAWN = 0xa6, //client
	ID_GAME_MESSAGE_DEATH_INFO = 0xa7, //client
	ID_GAME_MESSAGE_PLAYER_SHOT_BODYPART = 0xa8, //client->server

	ID_GAME_MESSAGE_SNIPERFIRE = 0xad, //client
	ID_GAME_MESSAGE_FIRE_PROJECTILE = 0xae,//client
	ID_GAME_MESSAGE_SERVER_DATA = 0xbc
#endif
	
	
	

};
