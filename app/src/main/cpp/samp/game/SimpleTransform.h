//
// Created by x1y2z on 09.04.2023.
//

#pragma once

#include "game/Core/Vector.h"


class CSimpleTransform {
public:
    CSimpleTransform() : m_vPosn(), m_fHeading(0.0F) {}

public:
    CVector m_vPosn; // SA name m_translate
    float m_fHeading;

    void UpdateRwMatrix(RwMatrix* out);
    void Invert(const CSimpleTransform& base);
    void UpdateMatrix(class CMatrix* out);
};


static_assert(sizeof(CSimpleTransform) == 0x10, "Invalid size CSimpleTransform");
