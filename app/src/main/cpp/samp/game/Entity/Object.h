//
// Created by x1y2z on 04.05.2023.
//

#pragma once

#include "game/Entity/CPhysical.h"
#include "game/common.h"
#include "game/Collision/ColPoint.h"

enum eObjectType {
    OBJECT_UNKNOWN         = 0,
    OBJECT_GAME            = 1,
    OBJECT_MISSION         = 2,
    OBJECT_TEMPORARY       = 3, // AKA OBJECT_TYPE_FLYING_COMPONENT
    OBJECT_TYPE_CUTSCENE   = 4,
    OBJECT_TYPE_DECORATION = 5, // Hand object, projectiles, escalator step, water creatures, no clue what this enum value should be called
    OBJECT_MISSION2        = 6
};

#pragma pack(push, 1)
class CObjectGta : public CPhysical {
public:
    uintptr*            m_pControlCodeList;
    uint8               m_nObjectType; // see enum eObjectType
    uint8               m_nBonusValue;
    uint16              m_wCostValue;
    union {
        struct {
            uint32 bIsPickup : 1;               // 0x1
            uint32 b0x02 : 1;                   // 0x2 - collision related
            uint32 bPickupPropertyForSale : 1;  // 0x4
            uint32 bPickupInShopOutOfStock : 1; // 0x8
            uint32 bHasBrokenGlass : 1;         // 0x10
            uint32 bGlassBrokenAltogether : 1;  // 0x20
            uint32 bIsExploded : 1;             // 0x40
            uint32 bChangesVehColor : 1;        // 0x80

            uint32 bIsLampPost : 1;
            uint32 bIsTargetable : 1;
            uint32 bIsBroken : 1;
            uint32 bTrainCrossEnabled : 1;
            uint32 bIsPhotographed : 1;
            uint32 bIsLiftable : 1;
            uint32 bIsDoorMoving : 1;
            uint32 bIsDoorOpen : 1;

            uint32 bHasNoModel : 1;
            uint32 bIsScaled : 1;
            uint32 bCanBeAttachedToMagnet : 1;
            uint32 bDamaged : 1;
            uint32 b0x100000 : 1;
            uint32 b0x200000 : 1;
            uint32 bFadingIn : 1; // works only for objects with type 2 (OBJECT_MISSION)
            uint32 bAffectedByColBrightness : 1;

            uint32 bEnableDisabledAttractors : 1;
            uint32 bDoNotRender : 1;
            uint32 bFadingIn2 : 1;
            uint32 b0x08000000 : 1;
            uint32 b0x10000000 : 1;
            uint32 b0x20000000 : 1;
            uint32 b0x40000000 : 1;
            uint32 b0x80000000 : 1;
        } objectFlags;
        uint32 m_nObjectFlags;
    };
    uint8         m_nColDamageEffect;        // see eObjectColDamageEffect
    uint8         m_nSpecialColResponseCase; // see eObjectSpecialColResponseCases
    char          KeepieUppyCounter;
    int8          m_nGarageDoorGarageIndex;
    uint8         m_nLastWeaponDamage;
    tColLighting  m_nColLighting;
    int16         m_nRefModelIndex;
    uint8         m_nCarColor[4];  // this is used for detached car parts
    uint32        m_nRemovalTime;  // time when this object must be deleted
    float         m_fHealth;
    float         m_fDoorStartAngle; // this is used for door objects
    float         m_fScale;
    uintptr       *m_pObjectInfo;
    uintptr       *m_pFire;
    int16         m_wScriptTriggerIndex;
    uint16        pad1;
    const char*   m_wRemapTxd;     // this is used for detached car parts
    RwTexture*    m_pRemapTexture; // this is used for detached car parts
    uintptr       *m_pDummyObject;  // used for dynamic objects like garage doors, train crossings etc.
    uint32        m_nBurnTime;     // time when particles must be stopped
    float         m_fBurnDamage;
};
#pragma pack(pop)

VALIDATE_SIZE(CObjectGta, (VER_x32 ? 0x184 : 0x1F8));

