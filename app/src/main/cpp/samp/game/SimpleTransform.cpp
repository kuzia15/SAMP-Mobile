//
// Created by x1y2z on 09.04.2023.
//

#include "SimpleTransform.h"
#include "main.h"

void CSimpleTransform::UpdateRwMatrix(RwMatrix* out)
{
    const float heading = m_fHeading;
    const float sinHeading = sinf(heading);
    const float cosHeading = cosf(heading);

    out->right = { cosHeading, sinHeading, 0.0 };
    out->up = { -sinHeading, cosHeading, 0.0 };
    out->at = { 0.0, 0.0, 1.0 };
    out->pos = m_vPosn;

    RwMatrixUpdate(out);
}


void CSimpleTransform::Invert(const CSimpleTransform& base)
{
    const float cosHeading = cosf(base.m_fHeading);
    const float sinHeading = sinf(base.m_fHeading);

    m_vPosn.x = -(cosHeading * base.m_vPosn.x) - (sinHeading * base.m_vPosn.y);
    m_vPosn.y = (sinHeading * base.m_vPosn.x) - (cosHeading * base.m_vPosn.y);
    m_vPosn.z = -base.m_vPosn.z;
    m_fHeading = -base.m_fHeading;
}

#include "Matrix.h"
void CSimpleTransform::UpdateMatrix(CMatrix* out)
{
   // Log("UpdateMatrix");
    out->SetTranslate(m_vPosn);
    out->SetRotateZOnly(m_fHeading);
}