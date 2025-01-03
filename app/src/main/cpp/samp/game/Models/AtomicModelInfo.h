//
// Created by x1y2z on 07.03.2023.
//

#pragma once

#include "BaseModelInfo.h"


struct CAtomicModelInfo : public CBaseModelInfo {

};


void SetAtomicModelInfoFlags(CAtomicModelInfo* modelInfo, uint32 dwFlags);

VALIDATE_SIZE(CAtomicModelInfo, (VER_x32 ? 0x38 : 0x48));