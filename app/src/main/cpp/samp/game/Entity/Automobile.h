//
// Created by x1y2z on 16.09.2023.
//

#pragma once

#include "CVehicleGTA.h"

class CAutomobile : public CVehicleGTA {
#if VER_x32
    uint8_t pad0[0x3E8];
#else
    uint8_t pad0[0x470];
#endif
};

VALIDATE_SIZE(CAutomobile, (VER_x32 ? 0x99C : 0xBC8));
