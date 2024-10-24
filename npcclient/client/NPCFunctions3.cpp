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

#include "main.h"
extern HSQUIRRELVM v;
extern CPlayerPool* m_pPlayerPool;
extern NPC* iNPC;
extern CPlayerPool* m_pPlayerPool;
extern CFunctions* m_pFunctions;
extern CPlayer* npc;
extern RakNet::RakPeerInterface* peer;
extern RakNet::SystemAddress systemAddress;
uint8_t GetSlotIdFromWeaponId(uint8_t byteWeapon);

SQInteger fn_SendIcSyncDataEx(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    SQInteger dwKeys;
    sq_getinteger(v, 2, &dwKeys);
    SQInteger byteHealth, byteArmour, byteWeapon, wAmmo;
    sq_getinteger(v, 3, &byteHealth);
    sq_getinteger(v, 4, &byteArmour);
    sq_getinteger(v, 5, &byteWeapon);
    sq_getinteger(v, 6, &wAmmo);
    SQFloat fCarHealth;
    sq_getfloat(v, 7, &fCarHealth);
    SQUnsignedInteger dwDamage;
    sq_getinteger(v, 8, (SQInteger*)&dwDamage);
    VECTOR vecPos, vecSpeed;
    QUATERNION quatRotation;
    if (!SQ_SUCCEEDED(sq_getvector(v, 9, &vecPos)))
    {
        return sq_throwerror(v, "Parameter 8 must be vector");
    }
    if (!SQ_SUCCEEDED(sq_getquaternion(v, 10, &quatRotation)))
    {
        return sq_throwerror(v, "Parameter 9 must be quaternion");
    }
    if (!SQ_SUCCEEDED(sq_getvector(v, 11, &vecSpeed)))
    {
        return sq_throwerror(v, "Parameter 10 must be vector");
    }
    SQFloat fTurretx, fTurrety;
    sq_getfloat(v, 12, &fTurretx);
    sq_getfloat(v, 13, &fTurrety);

    m_pFunctions->SendInCarSyncData(dwKeys,
        byteHealth, byteArmour, byteWeapon,
        wAmmo, fCarHealth, dwDamage,
        vecPos, quatRotation, vecSpeed,
        fTurretx, fTurrety);
    sq_pushbool(v, SQTrue);
    return 1;
}

SQInteger fn_SendIcSyncData(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    SQInteger dwKeys;
    sq_getinteger(v, 2, &dwKeys);
    SQInteger byteHealth, byteArmour, byteWeapon, wAmmo;
    sq_getinteger(v, 3, &byteHealth);
    sq_getinteger(v, 4, &byteArmour);
    sq_getinteger(v, 5, &byteWeapon);
    sq_getinteger(v, 6, &wAmmo);
    SQFloat fCarHealth;
    sq_getfloat(v, 7, &fCarHealth);
    SQUnsignedInteger dwDamage;
    sq_getinteger(v, 8, (SQInteger*)&dwDamage);
    VECTOR vecPos, vecSpeed;
    QUATERNION quatRotation;
    sq_getfloat(v, 9, &vecPos.X);
    sq_getfloat(v, 10, &vecPos.Y);
    sq_getfloat(v, 11, &vecPos.Z);
    sq_getfloat(v, 12, &quatRotation.X);
    sq_getfloat(v, 13, &quatRotation.Y);
    sq_getfloat(v, 14, &quatRotation.Z);
    sq_getfloat(v, 15, &quatRotation.W);
    sq_getfloat(v, 16, &vecSpeed.X);
    sq_getfloat(v, 17, &vecSpeed.Y);
    sq_getfloat(v, 18, &vecSpeed.Z);
    SQFloat fTurretx, fTurrety;
    sq_getfloat(v, 19, &fTurretx);
    sq_getfloat(v, 20, &fTurrety);

    m_pFunctions->SendInCarSyncData(dwKeys,
        byteHealth, byteArmour, byteWeapon,
        wAmmo, fCarHealth, dwDamage,
        vecPos, quatRotation, vecSpeed,
        fTurretx, fTurrety);
    sq_pushbool(v, SQTrue);
    return 1;
}
SQInteger fn_SendOnFootSyncDataEx(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    //t i y f i i i i y y y b b
    SQInteger dwKeys;
    sq_getinteger(v, 2, &dwKeys);
    //Position
    VECTOR vecPos;
    if (!SQ_SUCCEEDED(sq_getvector(v, 3, &vecPos)))
        return sq_throwerror(v, "Parameter 2 should be Vector");

    SQFloat fAngle;
    sq_getfloat(v, 4, &fAngle);

    SQInteger byteHealth;
    sq_getinteger(v, 5, &byteHealth);

    SQInteger byteArmour;
    sq_getinteger(v, 6, &byteArmour);

    SQInteger byteCurrentWeapon;
    sq_getinteger(v, 7, &byteCurrentWeapon);

    SQInteger wAmmo;
    sq_getinteger(v, 8, &wAmmo);

    //Speed
    VECTOR vecSpeed;
    if (!SQ_SUCCEEDED(sq_getvector(v, 9, &vecSpeed)))
        return sq_throwerror(v, "Parameter 8 should be Vector");


    //Aim Position
    VECTOR vecAimPos;
    if (!SQ_SUCCEEDED(sq_getvector(v, 10, &vecAimPos)))
        return sq_throwerror(v, "Parameter 9 should be Vector");

    //Aim Direction
    VECTOR vecAimDir;
    if (!SQ_SUCCEEDED(sq_getvector(v, 11, &vecAimDir)))
        return sq_throwerror(v, "Parameter 10 should be Vector");

    //IsCrouching
    SQBool bIsCrouching;
    sq_getbool(v, 12, &bIsCrouching);

    //IsReloading
    SQBool bIsReloading;
    sq_getbool(v, 13, &bIsReloading);

    m_pFunctions->SendOnFootSyncDataEx(dwKeys, vecPos,
        fAngle, byteHealth, byteArmour, byteCurrentWeapon,
        wAmmo, vecSpeed, vecAimPos,vecAimDir, bIsCrouching,
        bIsReloading);
    sq_pushbool(v, SQTrue);
    return 1;//0 because we are returning nothing!
}
SQInteger fn_SendOnFootSyncData(HSQUIRRELVM v)
{
    if (!npc) {
        return 0;
    }
    //i fff f i i i fff fff fff
    SQInteger dwKeys;
    sq_getinteger(v, 2, &dwKeys);
    //Position
    SQFloat x, y, z;
    sq_getfloat(v, 3, &x);
    sq_getfloat(v, 4, &y);
    sq_getfloat(v, 5, &z);

    SQFloat fAngle;
    sq_getfloat(v, 6, &fAngle);

    SQInteger byteHealth;
    sq_getinteger(v, 7, &byteHealth);

    SQInteger byteArmour;
    sq_getinteger(v, 8, &byteArmour);

    SQInteger byteCurrentWeapon;
    sq_getinteger(v, 9, &byteCurrentWeapon);

    SQInteger wAmmo;
    sq_getinteger(v, 10, &wAmmo);

    //Speed
    SQFloat vx, vy, vz;
    sq_getfloat(v, 11, &vx);
    sq_getfloat(v, 12, &vy);
    sq_getfloat(v, 13, &vz);


    //Aim Position
    SQFloat s, t, u;
    sq_getfloat(v, 14, &s);
    sq_getfloat(v, 15, &t);
    sq_getfloat(v, 16, &u);

    //Aim Direction
    SQFloat p, q, r;
    sq_getfloat(v, 17, &p);
    sq_getfloat(v, 18, &q);
    sq_getfloat(v, 19, &r);

    //IsCrouching
    SQBool bIsCrouching;
    sq_getbool(v, 20, &bIsCrouching);

    //IsReloading
    SQBool bIsReloading;
    sq_getbool(v, 21, &bIsReloading);
    m_pFunctions->SendOnFootSyncData(dwKeys, x, y, z,
        fAngle, byteHealth, byteArmour, byteCurrentWeapon,
        wAmmo, vx, vy, vz, s, t, u, p, q, r, bIsCrouching, bIsReloading);
    
    return 0;//0 because we are returning nothing!
}
SQInteger fn_FireSniperRifleEx(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger weapon;
    VECTOR vecAimPos, vecDir;
    // x y x aiming position
    sq_getinteger(v, 2, &weapon);
    if (!(SQ_SUCCEEDED(sq_getvector(v, 3, &vecAimPos))))
    {
        return sq_throwerror(v, "Parameter 2 should be Vector");
    }
    if (!(SQ_SUCCEEDED(sq_getvector(v, 4, &vecDir))))
    {
        return sq_throwerror(v, "Parameter 3 should be Vector");
    }
    m_pFunctions->FireSniperRifle((char)weapon, vecAimPos.X, vecAimPos.Y,
        vecAimPos.Z, vecDir.X, vecDir.Y, vecDir.Z);
    sq_pushbool(v, SQTrue);
    return 1;
}
SQInteger fn_FireSniperRifle(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger weapon;
    float x, y, z, dx, dy, dz;
    // x y x aiming position
    sq_getinteger(v, 2, &weapon);
    sq_getfloat(v, 3, &x);
    sq_getfloat(v, 4, &y);
    sq_getfloat(v, 5, &z);
    sq_getfloat(v, 6, &dx);
    sq_getfloat(v, 7, &dy);
    sq_getfloat(v, 8, &dz);
    m_pFunctions->FireSniperRifle((char)weapon, x, y, z, dx, dy, dz);
   return 0;
}
SQInteger fn_SendShotInfo(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger bodypart, animation;
    sq_getinteger(v, 2, &bodypart);
    if (sq_gettype(v, 3) == OT_NULL)
    {
        switch (bodypart)
        {
        case static_cast<uint8_t>(bodyPart::Body): animation = 13; break;//13 KO_shot_front
        case static_cast<uint8_t>(bodyPart::Torso): animation = 18; break;//18 KO_shot_stom
        case static_cast<uint8_t>(bodyPart::LeftArm): animation = 19; break;//19 KO_shot_arml
        case static_cast<uint8_t>(bodyPart::RightArm): animation = 20; break;
        case static_cast<uint8_t>(bodyPart::LeftLeg): animation = 21; break;
        case static_cast<uint8_t>(bodyPart::RightLeg): animation = 22; break;
        case static_cast<uint8_t>(bodyPart::Head): animation = 17; break; //17 KO_shot_face
        /*http://wiki.thijn.ovh/index.php?title=Animations*/
        default:return sq_throwerror(v, "Invalid body part. Must be in range 0 - 6");
        }
    }
    else if (sq_gettype(v, 3) == OT_INTEGER)
        sq_getinteger(v, 3, &animation);
    else return sq_throwerror(v, "Animation neither integer nor null");
    SQBool Autodeath = SQFalse; SQInteger AutoDeathWep = 0, AutoDeathKiller = 255;
    if (bodypart >= 0 && bodypart <= 6)
    {
        if (sq_gettop(v) >= 4)
        {
            if (sq_gettype(v, 4) == OT_BOOL)
            {
                sq_getbool(v, 4, &Autodeath);
            }
            else return sq_throwerror(v, "boolean value expected for autodeath parameter");
        }
        if (sq_gettop(v) >= 5)
        {
            if (sq_gettype(v, 5) == OT_INTEGER)
            {
                sq_getinteger(v, 5, &AutoDeathWep);
            }
            else return sq_throwerror(v, "integer value expected for autodeathweapon paramter");
        }
        if (sq_gettop(v) >= 6)
        {
            if (sq_gettype(v, 6) == OT_INTEGER)
            {
                sq_getinteger(v, 6, &AutoDeathKiller);
            }
            else return sq_throwerror(v, "integer value expected for autodeathkiller paramter");
        }
        m_pFunctions->SendShotInfo(bodyPart(bodypart), animation,Autodeath==SQTrue,(uint8_t)AutoDeathWep,(uint8_t)AutoDeathKiller);
        if (m_pFunctions->GetLastError() == funcError::NoError)
        {
            sq_pushbool(v, SQTrue);
        }
        else sq_pushbool(v, SQFalse);
    }
    else return sq_throwerror(v, "Invalid body part. Must be in range 0 - 6");
    return 1;
}
SQInteger fn_GetLocalValue(HSQUIRRELVM v)
{
    if (!npc)return 0;
    SQInteger field;
    sq_getinteger(v, 2, &field);
    switch (field)
    {
    case I_KEYS: sq_pushinteger(v, npc->GetKeys());
        break;
    case I_HEALTH: sq_pushinteger(v, npc->m_byteHealth);
        break;
    case I_ARMOUR: sq_pushinteger(v, npc->m_byteArmour);
        break;
    case I_CURWEP: sq_pushinteger(v, npc->GetCurrentWeapon());
        break;
    case I_CURWEP_AMMO: sq_pushinteger(v, npc->GetSlotAmmo(GetSlotIdFromWeaponId(npc->GetCurrentWeapon())));
        break;
    case F_POSX: sq_pushfloat(v, npc->m_vecPos.X);
        break;
    case F_POSY: sq_pushfloat(v, npc->m_vecPos.Y);
        break;
    case F_POSZ: sq_pushfloat(v, npc->m_vecPos.Z);
        break;
    case V_ONFOOT_SPEED: if (SQ_FAILED(sq_pushvector(v, npc->GetONFOOT_SYNC_DATA()->vecSpeed)))
        return sq_throwerror(v, "Error when pushing vector");
        break;
    case V_AIMPOS: if (SQ_FAILED(sq_pushvector(v, npc->GetONFOOT_SYNC_DATA()->vecAimPos)))
        return sq_throwerror(v, "Error when pushing vector");
        break;
    case V_AIMDIR: if (SQ_FAILED(sq_pushvector(v, npc->GetONFOOT_SYNC_DATA()->vecAimDir)))
        return sq_throwerror(v, "Error when pushing vector");
        break;
    case V_POS: if (SQ_FAILED(sq_pushvector(v, npc->m_vecPos)))
        return sq_throwerror(v, "Error when pushing vector");
        break;
    case V_CAR_SPEED: if (SQ_FAILED(sq_pushvector(v, npc->GetINCAR_SYNC_DATA()->vecMoveSpeed)))
        return sq_throwerror(v, "Error when pushing vector");
        break;
    case F_CAR_HEALTH:sq_pushfloat(v, npc->GetINCAR_SYNC_DATA()->fCarHealth);
        break;
    case F_CAR_TURRETX: sq_pushfloat(v, npc->GetINCAR_SYNC_DATA()->Turretx);
        break;
    case F_CAR_TURRETY: sq_pushfloat(v, npc->GetINCAR_SYNC_DATA()->Turrety);
        break;
    case I_CAR_DAMAGE: sq_pushinteger(v, npc->GetINCAR_SYNC_DATA()->dDamage);
        break;
    case Q_CAR_ROTATION: if (SQ_FAILED(sq_pushquaternion(v, npc->GetINCAR_SYNC_DATA()->quatRotation)))
        return sq_throwerror(v, "Error when pushing quaternion");
        break;
    case F_ANGLE: sq_pushfloat(v, npc->m_fAngle);
        break;
    case I_ACTION: sq_pushinteger(v, npc->GetONFOOT_SYNC_DATA()->byteAction);
        break;
    case B_CROUCHING: sq_pushbool(v, npc->GetONFOOT_SYNC_DATA()->IsCrouching);
        break;
    case B_RELOADING: sq_pushbool(v, npc->GetONFOOT_SYNC_DATA()->bIsReloading);
        break;
    case B_AIM_UPDATE:sq_pushbool(v, npc->GetONFOOT_SYNC_DATA()->IsPlayerUpdateAiming);
        break;
    case F_CAR_ROTATIONX:
    if (npc->m_wVehicleId)
    {
        sq_pushfloat(v, npc->GetINCAR_SYNC_DATA()->quatRotation.X);
    }else return 0;
        break;
    case F_CAR_ROTATIONY:
        if (npc->m_wVehicleId)
        {
            sq_pushfloat(v, npc->GetINCAR_SYNC_DATA()->quatRotation.Y);
        }
        else return 0;
        break;
    case F_CAR_ROTATIONZ:
        if (npc->m_wVehicleId)
        {
            sq_pushfloat(v, npc->GetINCAR_SYNC_DATA()->quatRotation.Z);
        }
        else return 0;
        break;
    case F_CAR_ROTATIONW:
        if (npc->m_wVehicleId)
        {
            sq_pushfloat(v, npc->GetINCAR_SYNC_DATA()->quatRotation.W);
        }
        else return 0;
        break;
    default: return 0;
    }
    return 1;// we are returning a value;
}
SQInteger fn_SetLocalValue(HSQUIRRELVM v)
{
    if (!npc)return 0;
    SQInteger field;
    sq_getinteger(v, 2, &field);
    SQFloat fvalue; SQInteger dwvalue;
    VECTOR vec; QUATERNION quat;
    SQBool bIsVector=SQFalse, bIsQuaternion=SQFalse;
    if (sq_gettype(v, 3) == OT_INTEGER)
    {
        sq_getinteger(v, 3, &dwvalue);
        fvalue = static_cast<float>(dwvalue);
    }
    else if (sq_gettype(v, 3) == OT_FLOAT)
    {
        sq_getfloat(v, 3, &fvalue);
    }
    else if (sq_gettype(v, 3) == OT_BOOL)
    {
        SQBool bvalue;
        sq_getbool(v, 3, &bvalue);
        fvalue = static_cast<bool>(bvalue);
    }
    else if (sq_gettype(v, 3) == OT_INSTANCE)
    {
        if (SQ_SUCCEEDED(sq_isvector(v, 3, &bIsVector)))
        {
            if (bIsVector == SQTrue)
            {
                if (SQ_SUCCEEDED(sq_getvector(v, 3, &vec)))
                {
                    
                }
                else return sq_throwerror(v, "Error in getting vector");
            }
            else if (bIsVector == SQFalse)
            {
                if (SQ_SUCCEEDED(sq_isquaternion(v, 3, &bIsQuaternion)))
                {
                    if (bIsQuaternion == SQTrue)
                    {
                        if (SQ_SUCCEEDED(sq_getquaternion(v, 3, &quat)))
                        {

                        }
                        else return sq_throwerror(v, "Error in getting quaternion");
                    }
                    else return sq_throwerror(v, "The instance provided is neither vector nor quaternion");
                }
                else return sq_throwerror(v, "Error when checking for quaternion");
            }
            else return sq_throwerror(v, "Unknown Error.");
        }
        else return sq_throwerror(v, "Error when checking for vector");
    }
    else return 0;
    uint8_t byteNewWeapon, byteSlot;
    switch (field)
    {
    case I_KEYS: if (sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->SetKeys(static_cast<uint32_t>(fvalue));
        break;
    }
    else return sq_throwerror(v, "The value must be integer.");
    case I_HEALTH: if (sq_gettype(v, 3) == OT_INTEGER || sq_gettype(v, 3) == OT_FLOAT)
    {
        npc->m_byteHealth = static_cast<uint8_t> (fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be float or integer.");
    case I_ARMOUR:  if (sq_gettype(v, 3) == OT_INTEGER||sq_gettype(v,3)==OT_FLOAT)
    {
        npc->m_byteArmour = static_cast<uint8_t> (fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be float or integer.");
    case I_CURWEP:  if (sq_gettype(v, 3) == OT_INTEGER)
    {
        byteNewWeapon = static_cast<uint8_t>(fvalue);
        byteSlot = GetSlotIdFromWeaponId(byteNewWeapon);
        if (npc->GetSlotWeapon(byteSlot) == byteNewWeapon)
            npc->SetCurrentWeapon(byteNewWeapon);
        else return 0;
        break;
    }
    else return sq_throwerror(v, "The value must be integer.");
    case I_CURWEP_AMMO: if (sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->SetCurrentWeaponAmmo(static_cast<uint16_t>(fvalue));
        break;
    }
    else return sq_throwerror(v, "The value must be integer.");
    case F_POSX: if (sq_gettype(v, 3) == OT_FLOAT || sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->m_vecPos.X = static_cast<float>(fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be float or integer.");
    case F_POSY: if (sq_gettype(v, 3) == OT_FLOAT || sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->m_vecPos.Y = static_cast<float>(fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be float or integer.");
    case F_POSZ: if (sq_gettype(v, 3) == OT_FLOAT || sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->m_vecPos.Z = static_cast<float>(fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be float or integer.");
    case F_ANGLE: if (sq_gettype(v, 3) == OT_FLOAT || sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->m_fAngle = static_cast<float>(fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be float or integer.");
    case F_CAR_HEALTH: if (sq_gettype(v, 3) == OT_FLOAT || sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->GetINCAR_SYNC_DATA()->fCarHealth = static_cast<float>(fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be float or integer.");
    case I_CAR_DAMAGE:if (sq_gettype(v, 3) == OT_FLOAT || sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->GetINCAR_SYNC_DATA()->dDamage = static_cast<uint32_t>(fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be integer");
    case F_CAR_TURRETX:if (sq_gettype(v, 3) == OT_FLOAT || sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->GetINCAR_SYNC_DATA()->Turretx = static_cast<float>(fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be float or integer.");
    case F_CAR_TURRETY:if (sq_gettype(v, 3) == OT_FLOAT || sq_gettype(v, 3) == OT_INTEGER)
    {
        npc->GetINCAR_SYNC_DATA()->Turrety = static_cast<float>(fvalue);
        break;
    }
    else return sq_throwerror(v, "The value must be float or integer.");
    case V_ONFOOT_SPEED: 
        if (sq_gettype(v, 3) == OT_INSTANCE && bIsVector==SQTrue)
        {
            npc->GetONFOOT_SYNC_DATA()->vecSpeed = vec;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be a vector");
        break;
    case V_AIMPOS:
        if (sq_gettype(v, 3) == OT_INSTANCE&& bIsVector==SQTrue)
        {
            npc->GetONFOOT_SYNC_DATA()->vecAimPos = vec;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be a vector");
        break;
    case V_AIMDIR:
        if (sq_gettype(v, 3) == OT_INSTANCE&& bIsVector==SQTrue)
        {
            npc->GetONFOOT_SYNC_DATA()->vecAimDir = vec;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be a vector");
        break;
    case V_POS:
        if (sq_gettype(v, 3) == OT_INSTANCE && bIsVector == SQTrue)
        {
            npc->m_vecPos = vec;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be a vector");
        break;
    case V_CAR_SPEED:
        if (sq_gettype(v, 3) == OT_INSTANCE && bIsVector == SQTrue)
        {
            npc->GetINCAR_SYNC_DATA()->vecMoveSpeed = vec;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be a vector");
        break;
    case Q_CAR_ROTATION:
        if (sq_gettype(v, 3) == OT_INSTANCE && bIsQuaternion == SQTrue)
        {
            npc->GetINCAR_SYNC_DATA()->quatRotation = quat;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be a quaternion");
        break;
    case I_ACTION:
        if (sq_gettype(v, 3) == OT_INTEGER )
        {
            npc->GetONFOOT_SYNC_DATA()->byteAction = static_cast<uint8_t> (fvalue); break;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be a quaternion");
        break;
    case B_CROUCHING:
        if (sq_gettype(v, 3) == OT_BOOL)
        {
            npc->GetONFOOT_SYNC_DATA()->IsCrouching = static_cast<bool> (fvalue); break;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be boolean");
        break;
    case B_RELOADING:
        if (sq_gettype(v, 3) == OT_BOOL)
        {
            npc->GetONFOOT_SYNC_DATA()->bIsReloading = static_cast<bool> (fvalue); break;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be boolean");
        break;
    case B_AIM_UPDATE:
        if (sq_gettype(v, 3) == OT_BOOL)
        {
            npc->GetONFOOT_SYNC_DATA()->IsPlayerUpdateAiming = static_cast<bool> (fvalue); break;
        }
        else return sq_throwerror(v, "The value corresponding to this field must be boolean");
        break;
    default: return 0;
    }
    sq_pushbool(v, SQTrue);
    return 1;
}
SQInteger fn_SendOnFootSyncDataLV(HSQUIRRELVM v)
{
    if (!npc)return 0;
    SendNPCOfSyncDataLV();
    return 0;
}
SQInteger fn_SendInCarSyncDataLV(HSQUIRRELVM v)
{
    if (!npc)return 0;
    SendNPCIcSyncDataLV();
    return 0;
}

SQInteger fn_SendDeathInfo(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger weapon, killerid, bodypart;
    sq_getinteger(v, 2, &weapon);
    sq_getinteger(v, 3, &killerid);
    sq_getinteger(v, 4, &bodypart);
    if(bodypart>=0 && bodypart<=6)
        m_pFunctions->SendDeathInfo(weapon, killerid, bodyPart(bodypart));
    return 0;
}
SQInteger fn_FaceNPCToPlayer(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        //player is connected
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t state = player->GetState();
        if (state == PLAYER_STATE_ONFOOT ||
            state == PLAYER_STATE_AIM)
        {
            VECTOR vecPos = player->m_vecPos;
            VECTOR vecnpcPos = npc->m_vecPos;
            float angle = (float)atan2((float)(-(vecPos.X - vecnpcPos.X) ),
                (float)(vecPos.Y - vecnpcPos.Y));
            npc->m_fAngle = angle;
            SendNPCOfSyncDataLV();
            sq_pushbool(v, SQTrue);
            return 1;
        }
    }
    sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_WhereIsPlayer(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger playerId;
    sq_getinteger(v, 2, &playerId);
    if (m_pPlayerPool->GetSlotState(playerId))
    {
        //player is connected
        CPlayer* player = m_pPlayerPool->GetAt(playerId);
        uint8_t state = player->GetState();
        if (state == PLAYER_STATE_ONFOOT || 
            state == PLAYER_STATE_AIM )
        {
            VECTOR vecPos = player->m_vecPos;
            VECTOR vecnpcPos = npc->m_vecPos;
            float theta = (float)atan2((float)(-(vecPos.X - vecnpcPos.X)),
                (float)(vecPos.Y - vecnpcPos.Y)); //npcs angle to player
            float alpha = npc->m_fAngle;//current angle of npc;
            float change = theta - alpha; //this much is the difference
            float beta;
            if (change > 0 && change < PI)
                beta = change; //positive
            else if (change > 0 && change > PI)
                beta = change - 2 * (float) PI; //negative
            else if (change <0 && change > -PI)
                beta = change; //negative
            else if (change < 0 && change < -PI)
                beta = change + 2 * (float) PI;//positive
            else if (change == PI || change == -PI)
                beta = change; //does not matter
            else if (change == 0)
                beta = 0;
            /* beta is like npc's angle, but considering
             the player as north pole */
            /* Or simply, beta is zero  means player is in front.
            is PI or -PI means is in the back
            and if it is positive it means player is on the left
            and negative means player is on the right*/
            sq_pushfloat(v, beta);
            return 1;
        }
    }
    return 0;
}

SQInteger fn_SendPassengerSyncData(HSQUIRRELVM v)
{
    if(!npc)
        return 0;
    if (npc->m_wVehicleId != 0 &&
        npc->m_byteSeatId != -1)
    {
        SendPassengerSyncData();
        sq_pushbool(v, SQTrue);
    }
    else
        sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_SetPSLimit(HSQUIRRELVM v)
{
    SQInteger n;//Set -1 for disabling auto syncing
    sq_getinteger(v, 2, &n);
    iNPC->PSLimit = n;
    return 0;
}
SQInteger fn_EnterVehicle(HSQUIRRELVM v)
{
    SQInteger vehicleId, seatId = 0;
    sq_getinteger(v, 2, &vehicleId);
    if (sq_gettop(v) >= 3)
    {
        if (sq_gettype(v,3) == OT_INTEGER)
        {
            sq_getinteger(v, 3, &seatId);
        }
        else return sq_throwerror(v, "Seat Id must be integer.");
    }
    funcError e = m_pFunctions->RequestVehicleEnter(vehicleId, seatId);
    if (e == funcError::NoError)
    {
        sq_pushbool(v, SQTrue);
    }
    else sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_ExitVehicle(HSQUIRRELVM v)
{
    funcError e = m_pFunctions->ExitVehicleEx(true, 1,0,0);
    if (e == funcError::NoError)
    {
        sq_pushbool(v, SQTrue);
    }
    else sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_ExitVehicleEx(HSQUIRRELVM v)
{
    SQBool ofsd; SQInteger style, byte1, byte2;
    sq_getbool(v, 2, &ofsd);
    sq_getinteger(v, 3, &style);
    sq_getinteger(v, 4, &byte1);
    sq_getinteger(v, 5, &byte2);
    funcError e = m_pFunctions->ExitVehicleEx(ofsd, (uint8_t)style, (uint8_t)byte1,(uint8_t)byte2);
    if (e == funcError::NoError)
    {
        sq_pushbool(v, SQTrue);
    }
    else sq_pushbool(v, SQFalse);
    return 1;
}
SQInteger fn_SendServerData(HSQUIRRELVM v)
{
    SQUserPointer blob; size_t size;
    if (SQ_SUCCEEDED(sqstd_getblob(v, 2, &blob)))
    {
        size = sqstd_getblobsize(v, 2);
        m_pFunctions->SendServerData(blob, size);
        sq_pushbool(v, SQTrue);
        return 1;
    }
    else
    {
        sq_pushbool(v, SQFalse);
        return 1;
    }
}
SQInteger fn_FireProjectile(HSQUIRRELVM v)
{
    if (!npc)
        return 0;
    SQInteger weapon;
    VECTOR vecPos;
    // x y z aiming position
    sq_getinteger(v, 2, &weapon);
    if (!(SQ_SUCCEEDED(sq_getvector(v, 3, &vecPos))))
    {
        return sq_throwerror(v, "Parameter 2 should be Vector");
    }
    float r1, r2, r3, r4, r5, r6, r7;
    SQInteger integer;
    if (sq_gettype(v, 4) == OT_FLOAT)
        sq_getfloat(v, 4, &r1);
    else {
        sq_getinteger(v, 4, &integer); r1 = (float)integer;
    }
    if (sq_gettype(v, 5) == OT_FLOAT)
        sq_getfloat(v, 5, &r2);
    else {
        sq_getinteger(v, 5, &integer); r2 = (float)integer;
    }
    if (sq_gettype(v, 6) == OT_FLOAT)
        sq_getfloat(v, 6, &r3);
    else {
        sq_getinteger(v, 6, &integer); r3 = (float)integer;
    }
    if (sq_gettype(v, 7) == OT_FLOAT)
        sq_getfloat(v, 7, &r4);
    else {
        sq_getinteger(v, 7, &integer); r4 = (float)integer;
    }
    if (sq_gettype(v, 8) == OT_FLOAT)
        sq_getfloat(v, 8, &r5);
    else {
    sq_getinteger(v, 8, &integer); r5 = (float)integer;
    }
    if (sq_gettype(v, 9) == OT_FLOAT)
        sq_getfloat(v, 9, &r6);
    else {
    sq_getinteger(v, 9, &integer); r6 = (float)integer;
    }
    if (sq_gettype(v, 10) == OT_FLOAT)
        sq_getfloat(v, 10, &r7);
    else {
    sq_getinteger(v, 10, &integer); r7 = (float)integer;
    }
    m_pFunctions->FireProjectile((unsigned char)weapon, vecPos,
        r1, r2, r3, r4, r5, r6, r7);
    sq_pushbool(v, SQTrue);
    return 1;
}
void RegisterNPCFunctions3()
{
    register_global_func(v, ::fn_SendOnFootSyncData, "SendOnFootSyncData", 21, "tif|if|if|if|iiiiif|if|if|if|if|if|if|if|if|ibb");
    register_global_func(v, ::fn_SendOnFootSyncDataEx, "SendOnFootSyncDataEx", 13, "tixf|iiiiixxxbb");
    register_global_func(v, ::fn_FireSniperRifle, "FireSniperRifle", 8, "tif|if|if|if|if|if|i");
    register_global_func(v, ::fn_FireSniperRifleEx, "FireSniperRifleEx", 4, "tixx");
    //register_global_func(v, ::fn_FireProjectile, "FireProjectile", 7, "tifffff");
    register_global_func(v, ::fn_SendShotInfo, "SendShotInfo", -3, "tii|o");
    register_global_func(v, ::fn_SetLocalValue, "SetLocalValue", 3, "tii|f|x|b");
    register_global_func(v, ::fn_GetLocalValue, "GetLocalValue", 2, "ti");
    register_global_func(v, ::fn_SendOnFootSyncDataLV, "SendOnFootSyncDataLV", 1, "t");
    register_global_func(v, ::fn_SendDeathInfo, "SendDeathInfo", 4, "tiii");
    register_global_func(v, ::fn_FaceNPCToPlayer, "FaceNPCToPlayer", 2, "ti");
    register_global_func(v, ::fn_WhereIsPlayer, "WhereIsPlayer", 2, "ti");
    register_global_func(v, ::fn_SendPassengerSyncData, "SendPassengerSyncData", 1, "t");
    register_global_func(v, ::fn_SetPSLimit, "SetPSLimit", 2, "t");
    register_global_func(v, ::fn_SendIcSyncData, "SendInCarSyncData", 20, "tiiiiif|iif|if|if|if|if|if|if|if|if|if|if|if|i");
    register_global_func(v, ::fn_SendIcSyncDataEx, "SendInCarSyncDataEx",13 , "tiiiiif|iixxxf|if|i");
    register_global_func(v, ::fn_SendInCarSyncDataLV, "SendInCarSyncDataLV", 1, "t");
    register_global_func(v, ::fn_EnterVehicle, "EnterVehicle", -2, "ti");
    register_global_func(v, ::fn_ExitVehicle, "ExitVehicle", 1, "t");
    register_global_func(v, ::fn_ExitVehicleEx, "ExitVehicleEx", 5, "tbiii");
    register_global_func(v, ::fn_SendServerData, "SendServerData", 2, "tx");

    register_global_func(v, ::fn_FireProjectile, "FireProjectile", 10, "tixf|if|if|if|if|if|if|i");
    

}