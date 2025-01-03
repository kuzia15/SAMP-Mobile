//
// Created by x1y2z on 21.11.2023.
//

#pragma once

#include "../common.h"

#pragma pack(push, 1)
class TextureListing
{
    uint16 nameHash;
    uint16 format;
    uint16 width;
    struct {
        uint16 height: 15;
        uint16 hasMipChain: 1;
    };
};
#pragma pack(pop)

VALIDATE_SIZE(TextureListing, 0x8);
