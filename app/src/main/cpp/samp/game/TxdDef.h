/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "common.h"

class TxdDef {
public:
    uintptr*    m_pRwDictionary;
    int16       m_wRefsCount;
    int16       m_wParentIndex;
    uint32      m_hash;

    // only mobile
    char        name[20];
    char        dbname[20];
    bool        keepCPU;
};

VALIDATE_SIZE(TxdDef, VER_x32 ? 0x38 : 0x40);