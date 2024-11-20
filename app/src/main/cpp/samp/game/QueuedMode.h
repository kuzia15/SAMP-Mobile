/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "common.h"

class CQueuedMode {
public:
    uint16  m_nMode;
    uint8   pad_0[2];
    float   m_fDuration;
    uint16  m_nMinZoom;
    uint16  m_nMaxZoom;
};

VALIDATE_SIZE(CQueuedMode, 0xC);
