/*
    Copyright (C) 2023  habi

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
//NPCFunctions5.cpp : Pickups,
#include "main.h"
extern HSQUIRRELVM v;
extern CPlayerPool* m_pPlayerPool;
extern NPC* iNPC;
extern CFunctions* m_pFunctions;
extern CPlayer* npc;
extern CPickupPool* m_pPickupPool;
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
SQInteger fn_IsPickupStreamedIn(HSQUIRRELVM v)
{
    SQInteger pickupId;
    sq_getinteger(v, 2, &pickupId);
    PICKUP* pickup = m_pPickupPool->GetByID(pickupId);
    if (pickup)
    {
        sq_pushbool(v, SQTrue);
    }
    else
        sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_GetPickupModel(HSQUIRRELVM v)
{
    SQInteger pickupId;
    sq_getinteger(v, 2, &pickupId);
    PICKUP* pickup = m_pPickupPool->GetByID(pickupId);
    if (pickup)
    {
        sq_pushinteger(v, pickup->wModel);
    }
    else
        return sq_throwerror(v, "The pickup with given id not found");
    return 1;
}

SQInteger fn_GetPickupAlpha(HSQUIRRELVM v)
{
    SQInteger pickupId;
    sq_getinteger(v, 2, &pickupId);
    PICKUP* pickup = m_pPickupPool->GetByID(pickupId);
    if (pickup)
    {
        sq_pushinteger(v, pickup->bAlpha);
    }
    else
        return sq_throwerror(v, "The pickup with given id not found");
    return 1;
}

SQInteger fn_GetPickupQuantity(HSQUIRRELVM v)
{
    SQInteger pickupId;
    sq_getinteger(v, 2, &pickupId);
    PICKUP* pickup = m_pPickupPool->GetByID(pickupId);
    if (pickup)
    {
        sq_pushinteger(v, pickup->dwQuantity);
    }
    else
        return sq_throwerror(v, "The pickup with given id not found");
    return 1;
}

SQInteger fn_GetPickupPosition(HSQUIRRELVM v)
{
    SQInteger pickupId;
    sq_getinteger(v, 2, &pickupId);
    PICKUP* pickup = m_pPickupPool->GetByID(pickupId);
    if (pickup)
    {
        if (SQ_SUCCEEDED(sq_pushvector(v, pickup->vecPos)))
        {
            return 1;
        }
        else
            return sq_throwerror(v, "Error occured when pushing vector to stack");
    }
    else
        return sq_throwerror(v, "The pickup with given id not found");
}
SQInteger fn_GetPickupCount(HSQUIRRELVM v)
{
    sq_pushinteger(v, m_pPickupPool->GetCount());
    return 1;
}
void RegisterNPCFunctions5()
{
    register_global_func(v, ::fn_IsPickupStreamedIn, "IsPickupStreamedIn", 2, "ti");
    register_global_func(v, ::fn_GetPickupModel, "GetPickupModel", 2, "ti");
    register_global_func(v, ::fn_GetPickupAlpha, "GetPickupAlpha", 2, "ti");
    register_global_func(v, ::fn_GetPickupQuantity, "GetPickupQuantity", 2, "ti");
    register_global_func(v, ::fn_GetPickupPosition, "GetPickupPosition", 2, "ti");
    register_global_func(v, ::fn_GetPickupCount, "GetPickupCount", 1, "t");
}