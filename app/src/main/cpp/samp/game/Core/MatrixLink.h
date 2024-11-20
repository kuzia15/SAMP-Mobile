/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Matrix.h"

class CPlaceable;

class CMatrixLink : public CMatrix {
public:
    CMatrixLink() : CMatrix() {}
    CMatrixLink(float fScale) : CMatrix() { SetScale(fScale); }

public:
    CPlaceable*  m_pOwner;
    CMatrixLink* m_pPrev;
    CMatrixLink* m_pNext;

public:
    static void InjectHooks();

    void Insert(CMatrixLink* pWhere);
    void Remove();
};
static_assert(sizeof(CMatrixLink) == (VER_x32 ? 0x54 : 0x68));
