/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CPedGTA.h"

enum eCopType : uint32_t { // TODO: Fix naming
    COP_TYPE_CITYCOP,
    COP_TYPE_LAPDM1,
    COP_TYPE_SWAT1,
    COP_TYPE_SWAT2,
    COP_TYPE_FBI,
    COP_TYPE_ARMY,
    COP_TYPE_CSHER = 7
};

class CCopPed : public CPedGTA {
public:
    bool        m_bRoadBlockCop;
    bool        m_bRemoveIfNonVisible;
    uint8_t       pad0[2];
    uint32_t       m_CopType;
    uint32_t       m_nStuckCounter;
    CCopPed*    m_pPartner;
    CPedGTA*    m_aCriminalsImAfter[5];
    bool        m_bIAmDriver;
    uint8_t        pad1[3];

public:

};

VALIDATE_SIZE(CCopPed, (VER_x32 ? 0x7CC : 0x9C8));

