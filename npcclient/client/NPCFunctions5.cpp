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
//NPCFunctions5.cpp : Pickups, ModuleList, Vehicle, Checkpoint, Object, FPS
#include "main.h"
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
SQInteger fn_ClaimPickup(HSQUIRRELVM v)
{
    SQInteger pickupId;
    sq_getinteger(v, 2, &pickupId);
    PICKUP* pickup = m_pPickupPool->GetByID(pickupId);
    if (pickup)
    {
        funcError e= m_pFunctions->ClaimPickup(pickupId);
        if (e == funcError::NoError)
        {
            sq_pushbool(v, SQTrue);
        }
        else
            sq_pushbool(v, SQFalse);
        return 1;
    }
    else
        return sq_throwerror(v, "The pickup with given id not found");
}
SQInteger fn_GetVehicleDriver(HSQUIRRELVM v)
{
    SQInteger vehicleId;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &vehicleId)))
    {
        uint8_t driver=m_pFunctions->GetVehicleDriver(vehicleId);
        if (m_pFunctions->GetLastError() == funcError::NoError)
        {
            sq_pushinteger(v, driver);
        }
        else sq_pushinteger(v, INVALID_PLAYER_ID);
        return 1;
    }else 
        return sq_throwerror(v, "error getting vehicleid");
}
SQInteger fn_GetVehicleRotation(HSQUIRRELVM v)
{
    SQInteger vehicleId;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &vehicleId)))
    {
        QUATERNION rot;
        funcError e = m_pFunctions->GetVehicleRotation(vehicleId, &rot);
        if(e==funcError::NoError)
        {
            if (SQ_SUCCEEDED(sq_pushquaternion(v, rot)))
            {
                return 1;
            }
            else return sq_throwerror(v, "error when pushing quaternion");
        }
        else sq_pushquaternion(v, QUATERNION(0,0,0,1));
        return 1;
    }
    else return sq_throwerror(v, "error getting vehicleid");
}
SQInteger fn_GetVehiclePosition(HSQUIRRELVM v)
{
    SQInteger vehicleId;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &vehicleId)))
    {
        VECTOR pos;
        funcError e = m_pFunctions->GetVehiclePosition(vehicleId, &pos);
        if (e == funcError::NoError)
        {
            if (SQ_SUCCEEDED(sq_pushvector(v, pos)))
            {
                return 1;
            }
            else return sq_throwerror(v, "error when pushing vector");
        }
        else sq_pushvector(v, VECTOR(0, 0, 0));
        return 1;
    }
    else return sq_throwerror(v, "error getting vehicleid");
}
SQInteger fn_GetVehicleSpeed(HSQUIRRELVM v)
{
    SQInteger vehicleId;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &vehicleId)))
    {
        VECTOR speed;
        funcError e = m_pFunctions->GetVehicleSpeed(vehicleId, &speed);
        if (e == funcError::NoError)
        {
            if (SQ_SUCCEEDED(sq_pushvector(v, speed)))
            {
                return 1;
            }
            else return sq_throwerror(v, "error when pushing vector");
        }
        else sq_pushvector(v, VECTOR(0, 0, 0));
        return 1;
    }
    else return sq_throwerror(v, "error getting vehicleid");
}
SQInteger fn_GetVehicleHealth(HSQUIRRELVM v)
{
    SQInteger vehicleId;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &vehicleId)))
    {
        float health = m_pFunctions->GetVehicleHealth(vehicleId);
        if (m_pFunctions->GetLastError() == funcError::NoError)
        {
            sq_pushfloat(v, health);
            return 1;
        }
        else sq_pushfloat(v, 0.0);
        return 1;
    }
    else return sq_throwerror(v, "error getting vehicleid");
}
SQInteger fn_GetVehicleDamage(HSQUIRRELVM v)
{
    SQInteger vehicleId;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &vehicleId)))
    {
        uint32_t damage = m_pFunctions->GetVehicleDamage(vehicleId);
        if (m_pFunctions->GetLastError() == funcError::NoError)
        {
            sq_pushinteger(v, damage);//largest value of damage=536,870,912 https://forum.vc-mp.org/index.php?topic=7924
            return 1;
        }
        else sq_pushinteger(v, -1);
        return 1;
    }
    else return sq_throwerror(v, "error getting vehicleid");
}
SQInteger fn_GetVehicleModel(HSQUIRRELVM v)
{
    SQInteger vehicleId;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &vehicleId)))
    {
        uint16_t model = m_pFunctions->GetVehicleModel(vehicleId);
        if (m_pFunctions->GetLastError() == funcError::NoError)
        {
            sq_pushinteger(v, model);
            return 1;
        }
        else sq_pushinteger(v, 0);
        return 1;
    }
    else return sq_throwerror(v, "error getting vehicleid");
}
SQInteger fn_GetVehicleTurretRot(HSQUIRRELVM v)
{
    SQInteger vehicleId; float hx, hy;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &vehicleId)))
    {
        funcError e = m_pFunctions->GetVehicleTurretRotation(vehicleId,&hx,&hy);
        if (e == funcError::NoError)
        {
            sq_pushvector(v, VECTOR(hx, hy, 0));
            return 1;
        }
        else sq_pushvector(v, VECTOR(0, 0, 0));
        return 1;
    }
    else return sq_throwerror(v, "error getting vehicleid");
}
SQInteger fn_GetPlayerSpeed(HSQUIRRELVM v)
{
    SQInteger bytePlayerId;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &bytePlayerId)))
    {
        VECTOR vecSpeed;
        funcError e = m_pFunctions->GetPlayerSpeed(bytePlayerId, &vecSpeed);
        if (e == funcError::NoError)
        {
            sq_pushvector(v, vecSpeed);
        }
        else sq_pushvector(v, VECTOR(0, 0, 0));
        return 1;
    }
    else return sq_throwerror(v, "error getting player id");
}
SQInteger fn_GetPlayerAction(HSQUIRRELVM v)
{
    SQInteger bytePlayerId;
    if (SQ_SUCCEEDED(sq_getinteger(v, 2, &bytePlayerId)))
    {
        uint8_t byteAction = m_pFunctions->GetPlayerAction(bytePlayerId);
        if (m_pFunctions->GetLastError() == funcError::NoError)
        {
            sq_pushinteger(v, byteAction);
        }
        else sq_pushinteger(v, 0);
        return 1;
    }
    else return sq_throwerror(v, "error getting player id");
}
SQInteger fn_IsCheckpointStreamedIn(HSQUIRRELVM v)
{
    SQInteger cpId;
    sq_getinteger(v, 2, &cpId);
    CHECKPOINT* checkpoint = m_pCheckpointPool->GetByID(cpId);
    if (checkpoint)
    {
        sq_pushbool(v, SQTrue);
    }
    else
        sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_GetCheckpointRadius(HSQUIRRELVM v)
{
    SQInteger cpId;
    sq_getinteger(v, 2, &cpId);
    CHECKPOINT* checkpoint = m_pCheckpointPool->GetByID(cpId);
    if (checkpoint)
    {
        sq_pushfloat(v, checkpoint->fRadius);
    }
    else
        return sq_throwerror(v, "The checkpoint with the given id does not exist");
    return 1;
}
SQInteger fn_GetCheckpointPosition(HSQUIRRELVM v)
{
    SQInteger cpId;
    sq_getinteger(v, 2, &cpId);
    CHECKPOINT* checkpoint = m_pCheckpointPool->GetByID(cpId);
    if (checkpoint)
    {
        sq_pushvector(v, checkpoint->vecPos);
    }
    else
        return sq_throwerror(v, "The checkpoint with the given id does not exist");
    return 1;
}
SQInteger fn_IsCheckpointSphere(HSQUIRRELVM v)
{
    SQInteger cpId;
    sq_getinteger(v, 2, &cpId);
    CHECKPOINT* checkpoint = m_pCheckpointPool->GetByID(cpId);
    if (checkpoint)
    {
        sq_pushbool(v, checkpoint->byteIsSphere?SQTrue:SQFalse);
    }
    else
        return sq_throwerror(v, "The checkpoint with the given id does not exist");
    return 1;
}
SQInteger fn_GetCheckpointColor(HSQUIRRELVM v)
{
    SQInteger cpId;
    sq_getinteger(v, 2, &cpId);
    CHECKPOINT* checkpoint = m_pCheckpointPool->GetByID(cpId);
    if (checkpoint)
    {
        sq_newtable(v);
        sq_pushstring(v, "r", -1); sq_pushinteger(v, checkpoint->byteRed);
        if (SQ_FAILED(sq_newslot(v, -3, SQFalse)))return sq_throwerror(v, "Error creating slot for colour");
        sq_pushstring(v, "g", -1); sq_pushinteger(v, checkpoint->byteGreen);
        if (SQ_FAILED(sq_newslot(v, -3, SQFalse)))return sq_throwerror(v, "Error creating slot for colour");
        sq_pushstring(v, "b", -1); sq_pushinteger(v, checkpoint->byteBlue);
        if (SQ_FAILED(sq_newslot(v, -3, SQFalse)))return sq_throwerror(v, "Error creating slot for colour");
        sq_pushstring(v, "a", -1); sq_pushinteger(v, checkpoint->byteAlpha);
        if (SQ_FAILED(sq_newslot(v, -3, SQFalse)))return sq_throwerror(v, "Error creating slot for colour");
        return 1;//returns the table {r,g,b,a} formatted with values.
    }
    else
        return sq_throwerror(v, "The checkpoint with the given id does not exist");
    return 1;
}
SQInteger fn_ClaimEnterCheckpoint(HSQUIRRELVM v)
{
    SQInteger cpId;
    sq_getinteger(v, 2, &cpId);
    funcError e=m_pFunctions->ClaimEnterCheckpoint((uint16_t)cpId);
    if (e == funcError::NoError)
    {
        sq_pushbool(v, SQTrue);
    }
    else sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_ClaimExitCheckpoint(HSQUIRRELVM v)
{
    SQInteger cpId;
    sq_getinteger(v, 2, &cpId);
    funcError e = m_pFunctions->ClaimExitCheckpoint((uint16_t)cpId);
    if (e == funcError::NoError)
    {
        sq_pushbool(v, SQTrue);
    }
    else sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_IsObjectStreamedIn(HSQUIRRELVM v)
{
    SQInteger objectId;
    sq_getinteger(v, 2, &objectId);
    if (m_pFunctions->IsObjectStreamedIn(objectId))
        sq_pushbool(v, SQTrue);
    else
        sq_pushbool(v, SQFalse);
    return 1;
}


SQInteger fn_GetObjectModel(HSQUIRRELVM v)
{
    SQInteger objectId; uint16_t model;
    sq_getinteger(v, 2, &objectId);
    funcError e = m_pFunctions->GetObjectModel(objectId, &model);
    if (e == funcError::NoError)
    {
        sq_pushinteger(v, (uint32_t)model);
    }
    else return sq_throwerror(v, "The object with that ID does not exist!");
    return 1;
}

SQInteger fn_GetObjectPos(HSQUIRRELVM v)
{
    SQInteger objectId; VECTOR vecPos;
    sq_getinteger(v, 2, &objectId);
    funcError e = m_pFunctions->GetObjectPos(objectId, &vecPos);
    if (e == funcError::NoError)
    {
        sq_pushvector(v, vecPos);
    }
    else return sq_throwerror(v, "The object with that ID does not exist!");
    return 1;
}

SQInteger fn_GetObjectRotation(HSQUIRRELVM v)
{
    SQInteger objectId; QUATERNION quatRot;
    sq_getinteger(v, 2, &objectId);
    funcError e = m_pFunctions->GetObjectRotation(objectId, &quatRot);
    if (e == funcError::NoError)
    {
        sq_pushquaternion(v, quatRot);
    }
    else return sq_throwerror(v, "The object with that ID does not exist!");
    return 1;
}

SQInteger fn_GetObjectAlpha(HSQUIRRELVM v)
{
    SQInteger objectId; uint8_t byteAlpha;
    sq_getinteger(v, 2, &objectId);
    funcError e = m_pFunctions->GetObjectAlpha(objectId, &byteAlpha);
    if (e == funcError::NoError)
    {
        sq_pushinteger(v, (uint32_t)byteAlpha);
    }
    else return sq_throwerror(v, "The object with that ID does not exist!");
    return 1;
}

SQInteger fn_IsObjectTouchReportEnabled(HSQUIRRELVM v)
{
    SQInteger objectId; 
    sq_getinteger(v, 2, &objectId);
    bool bIsEnabled = m_pFunctions->IsObjectTouchReportEnabled(objectId);
    if (m_pFunctions->GetLastError() == funcError::NoError)
    {
        sq_pushbool(v, bIsEnabled?SQTrue:SQFalse);
    }
    else return sq_throwerror(v, "The object with that ID does not exist!");
    return 1;
}

SQInteger fn_IsObjectShotReportEnabled(HSQUIRRELVM v)
{
    SQInteger objectId; 
    sq_getinteger(v, 2, &objectId);
    bool bIsEnabled = m_pFunctions->IsObjectShotReportEnabled(objectId);
    if (m_pFunctions->GetLastError() == funcError::NoError)
    {
        sq_pushbool(v, bIsEnabled ? SQTrue : SQFalse);
    }
    else return sq_throwerror(v, "The object with that ID does not exist!");
    return 1;
}

SQInteger fn_ClaimObjectTouch(HSQUIRRELVM v)
{
    SQInteger objectId; 
    sq_getinteger(v, 2, &objectId);
    funcError e = m_pFunctions->ClaimObjectTouch(objectId);
    if (e == funcError::NoError)
    {
        sq_pushbool(v, SQTrue);
    }
    else return sq_throwerror(v, "The object with that ID does not exist!");
    return 1;
}

SQInteger fn_ClaimObjectShot(HSQUIRRELVM v)
{
    SQInteger objectId; SQInteger weaponId;
    sq_getinteger(v, 2, &objectId);
    sq_getinteger(v, 3, &weaponId);
    funcError e = m_pFunctions->ClaimObjectShot(objectId, weaponId);
    if (e == funcError::NoError)
    {
        sq_pushbool(v, SQTrue);
    }
    else return sq_throwerror(v, "The object with that ID does not exist!");
    return 1;
}

SQInteger fn_GetStreamedCheckpointCount(HSQUIRRELVM v)
{
    sq_pushinteger(v, m_pFunctions->GetStreamedCheckpointCount());
    return 1;
}

SQInteger fn_GetStreamedObjectCount(HSQUIRRELVM v)
{
    sq_pushinteger(v, m_pFunctions->GetStreamedObjectCount());
    return 1;
}
void RegisterNPCFunctions5()
{
    register_global_func(v, ::fn_IsPickupStreamedIn, "IsPickupStreamedIn", 2, "ti");
    register_global_func(v, ::fn_GetPickupModel, "GetPickupModel", 2, "ti");
    register_global_func(v, ::fn_GetPickupAlpha, "GetPickupAlpha", 2, "ti");
    register_global_func(v, ::fn_GetPickupQuantity, "GetPickupQuantity", 2, "ti");
    register_global_func(v, ::fn_GetPickupPosition, "GetPickupPos", 2, "ti");
    register_global_func(v, ::fn_GetPickupCount, "GetPickupCount", 1, "t");
    register_global_func(v, ::fn_ClaimPickup, "ClaimPickup", 2, "ti");
    //register_global_func(v, ::fn_SendModuleList, "SendModuleList", 2, "ts");
    register_global_func(v, ::fn_GetVehicleDriver, "GetVehicleDriver", 2, "ti");
    register_global_func(v, ::fn_GetVehicleModel, "GetVehicleModel", 2, "ti");
    register_global_func(v, ::fn_GetVehicleTurretRot, "GetVehicleTurretRot", 2, "ti");
    register_global_func(v, ::fn_GetVehicleRotation, "GetVehicleRotation", 2, "ti");
    register_global_func(v, ::fn_GetVehiclePosition, "GetVehiclePos", 2, "ti");
    register_global_func(v, ::fn_GetVehicleSpeed, "GetVehicleSpeed", 2, "ti");
    register_global_func(v, ::fn_GetVehicleHealth, "GetVehicleHealth", 2, "ti");
    register_global_func(v, ::fn_GetVehicleDamage, "GetVehicleDamage", 2, "ti");
    register_global_func(v, ::fn_GetPlayerSpeed, "GetPlayerSpeed", 2, "ti");
    register_global_func(v, ::fn_GetPlayerAction, "GetPlayerAction", 2, "ti");
    register_global_func(v, ::fn_IsCheckpointStreamedIn, "IsCheckpointStreamedIn", 2, "ti");
    register_global_func(v, ::fn_GetCheckpointRadius, "GetCheckpointRadius", 2, "ti");
    register_global_func(v, ::fn_GetCheckpointPosition, "GetCheckpointPos", 2, "ti");
    register_global_func(v, ::fn_GetCheckpointColor, "GetCheckpointColor", 2, "ti");
    register_global_func(v, ::fn_IsCheckpointSphere, "IsCheckpointSphere", 2, "ti");
    register_global_func(v, ::fn_ClaimEnterCheckpoint, "ClaimEnterCheckpoint", 2, "ti");
    register_global_func(v, ::fn_ClaimExitCheckpoint, "ClaimExitCheckpoint", 2, "ti");

    register_global_func(v, ::fn_IsObjectStreamedIn, "IsObjectStreamedIn", 2, "ti");
    register_global_func(v, ::fn_GetObjectModel, "GetObjectModel", 2, "ti");
    register_global_func(v, ::fn_GetObjectPos, "GetObjectPos", 2, "ti");
    register_global_func(v, ::fn_GetObjectRotation, "GetObjectRotation", 2, "ti");
    register_global_func(v, ::fn_GetObjectAlpha, "GetObjectAlpha", 2, "ti");
    register_global_func(v, ::fn_IsObjectTouchReportEnabled, "IsObjectTouchReportEnabled", 2, "ti");
    register_global_func(v, ::fn_IsObjectShotReportEnabled, "IsObjectShotReportEnabled", 2, "ti");
    register_global_func(v, ::fn_ClaimObjectTouch, "ClaimObjectTouch", 2, "ti");
    register_global_func(v, ::fn_ClaimObjectShot, "ClaimObjectShot", 2, "ti");
    register_global_func(v, ::fn_GetStreamedCheckpointCount, "GetCheckpointCount", 1, "t");
    register_global_func(v, ::fn_GetStreamedObjectCount, "GetObjectCount", 1, "t");


}