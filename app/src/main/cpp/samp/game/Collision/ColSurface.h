#pragma once

#include "../Enums/eSurfaceType.h"
#include "ColPoint.h"

struct CColSurface {
    eSurfaceType m_nMaterial{};
    uint8        m_nPiece{}; // ePedPieceTypes, eCarPiece, etc
    tColLighting m_nLighting{};
    uint8        m_nLight{};
};
VALIDATE_SIZE(CColSurface, 0x4);
