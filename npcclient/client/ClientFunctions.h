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
#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H
class CFunctions
{
private:

public:
	typedef enum {
		ErrorNoError = 0,
		ErrorEntityNotFound = 1,
		ErrorBufferShortage = 2,
		ErrorInputTooLarge = 3,
		ErrorArgumentIsOutOfBounds = 4,
		ErrorArgumentIsNull = 5,
		ErrorNameIsInvalid = 6,
		ErrorRequestIsDenied = 7,
		ErrorUnknown = INT32_MAX
	} funcError;
	void SendCommandToServer(const char* message);
	void SendChatMessage(const char* message);
	void FireSniperRifle(uint8_t weapon, float x, float y, float z, float dx, float dy, float dz);
	void SendShotInfo(SQInteger bodypart, SQInteger animation);
	void SendOnFootSyncData(uint32_t dwKeys, float x, float y, float z,
		float fAngle, uint8_t byteHealth, uint8_t byteArmour, uint8_t byteCurrentWeapon,
		uint16_t wAmmo, float speedx, float speedy, float speedz, 
		float aimposx, float aimposy, float aimposz, 
		float aimdirx, float aimdiry, float aimdirz, bool bIsCrouching, bool bIsReloading);
	void SendDeathInfo(uint8_t weapon, uint8_t killerid, uint8_t bodypart);

	uint32_t GetPlayerKeys(uint8_t playerId);

	funcError GetPlayerName(uint8_t playerId, char* buffer, size_t size);

	void SendPassengerSync();

	void GetPosition(float* x, float* y, float* z);

	void GetAngle(float* fAngle);

	void SetPosition(float x, float y, float z, bool sync=true);

	void SetAngle(float fAngle, bool sync=true);

	
	
};
#endif
