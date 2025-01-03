//
// Created by x1y2z on 16.09.2023.
//

#pragma once

#include "Automobile.h"

class CHeli : public CAutomobile {
#if VER_x32
    uint8 pad0[0x90];
#else
    uint8_t pad0[0x98];
#endif
};

VALIDATE_SIZE(CHeli, (VER_x32 ? 0xA2C : 0xC60));