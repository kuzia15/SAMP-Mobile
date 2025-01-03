//
// Created by x1y2z on 07.03.2023.
//

#pragma once

#include "ClumpModelInfo.h"
#include "game/Enums/ePedType.h"


struct CPedModelInfo : public CClumpModelInfo{
public:
    int         m_nAnimType;
    ePedType    m_nPedType;
    uint32_t    m_nStatType;
    uint16_t    m_nCarsCanDriveMask;
    uint16_t    m_nPedFlags;
    uintptr_t   *m_pHitColModel; // CColModel
    int8_t      m_nRadio1;
    int8_t      m_nRadio2;
    uint8_t     m_nRace;
    uint8_t     skip_1;
    int16_t     m_AudioPedType;
    int16_t     m_FirstVoice;
    int16_t     m_LastVoice;
    int16_t     m_NextVoice;
};


static_assert(sizeof(CPedModelInfo) == (VER_x32 ? 0x5c : 0x78));
