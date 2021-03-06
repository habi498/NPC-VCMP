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
	ID_GAME_MESSAGE_CONNECT = 0x98,
	ID_GAME_MESSAGE_JOIN = 0xaf,
	ID_GAME_MESSAGE_DISCONNECT = 0x99,
	ID_GAME_MESSAGE_CLIENT_MESSAGE = 0xcb,
	ID_GAME_MESSAGE_REQUEST_CLASS = 0xa5,
	ID_GAME_MESSAGE_REQUEST_SPAWN = 0xa6,
	ID_GAME_MESSAGE_SPAWN = 0xa7,
	ID_GAME_MESSAGE_SPAWN_GRANTED = 0xb6,
	ID_GAME_MESSAGE_PLAYER_DEATH = 0x9b,
	ID_GAME_MESSAGE_PLAYER_TEXT = 0xcd,
	ID_GAME_MESSAGE_VEHICLE_ENTER = 0xa2,
	ID_GAME_MESSAGE_VEHICLE_EXIT=0xa4,
	ID_GAME_MESSAGE_NPC_VEHICLE_EXIT = 0xf8,
	//another message 0xf9 is also send by server on exiting vehicle, 
	//which appears to be same as 0xf8. 
	ID_GAME_MESSAGE_PLAYER_STREAM_IN = 0x9c,
	ID_GAME_MESSAGE_PLAYER_STREAM_OUT = 0x9d,
	ID_GAME_MESSAGE_VEHICLE_STREAM_IN = 0xea,
	//not done much investigation on vehicle stream in
	//0xec is also send along with 0xea
	ID_GAME_MESSAGE_VEHICLE_STREAM_OUT = 0xeb, // no doubt. but multiple times server may send
	ID_GAME_MESSAGE_ONFOOT_UPDATE = 0x93,
	ID_GAME_MESSAGE_ONFOOT_UPDATE_AIM = 0x94,
	ID_GAME_MESSAGE_PLAYERUPDATE_DRIVER = 0x95,
	ID_GAME_MESSAGE_PLAYERUPDATE_PASSENGER = 0x96,
	ID_GAME_MESSAGE_SET_HEALTH = 0xb4,
	ID_GAME_MESSAGE_SET_ARMOUR = 0xb5,
	ID_GAME_MESSAGE_SET_WEAPON = 0xc0,
	ID_GAME_MESSAGE_SET_POSITION = 0xb9,
	ID_GAME_MESSAGE_SET_ANGLE = 0xbc,
	ID_GAME_MESSAGE_PLAYER_SPAWN=0xc6,
	ID_SERVER_MESSAGE_CLIENT_CONNECT = 0x98,
	ID_GAME_MESSAGE_NAME_CHANGE = 0x9a,
	ID_GAME_MESSAGE_CHAT = 0xa2,
	ID_GAME_MESSAGE_COMMAND=0xa4//client
};
