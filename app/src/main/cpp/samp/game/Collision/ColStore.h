//
// Created by x1y2z on 01.07.2023.
//

#pragma once

#include "../Core/Pool.h"
#include "../common.h"
#include "../Core/Rect.h"

struct ColDef {
    CRect  m_Area;
    uint32 field_10;
    uint32 field_14;
    uint32 field_18;
    uint32 field_1C;
    uint16 field_20;
    int16  m_nModelIdStart;
    int16  m_nModelIdEnd;
    uint16 m_nRefCount;
    bool   m_bActive;
    bool   m_bCollisionIsRequired;
    bool   m_bProcedural;
    bool   m_bInterior;

    static void* operator new(size_t size);
    static void  operator delete(void* data);
};
VALIDATE_SIZE(ColDef, 0x2C);

typedef CPool<ColDef> CColPool;

class CColStore {
public:
    static void Initialise();
    static int32 AddColSlot(const char* name);
    static void RequestCollision(const CVector* pos, int32 areaCode);
    static void AddCollisionNeededAtPosn(const CVector* pos);
    static void RemoveCol(int32 colSlot);

    static void LoadCollision(CVector pos, bool bIgnorePlayerVeh);

    static void EnsureCollisionIsInMemory(const CVector *pos);
};

