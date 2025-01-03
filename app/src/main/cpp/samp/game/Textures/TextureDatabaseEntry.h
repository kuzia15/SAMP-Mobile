//
// Created by x1y2z on 21.11.2023.
//

#pragma once

#include "../common.h"
#include "TextureListingContainer.h"

class __attribute__((packed)) __attribute__((aligned(1))) TextureDatabaseEntry
{
public:

    const char *name;
    uint32 nameHash;
    struct {
        uint16 format : 4;
        uint16 alphaFormat : 4;
        uint16 streamMode : 4;
        uint16 status : 4;
    };
    uint16 flags;
    uint8 category;
    uint8 detailTexture;
    uint8 detailTiling;
    uint16 width;
    uint16 height;
    union
    {
        RwTexture *instance;
        const char *matchingName;
    };

public:
    static void InjectHooks();

    RwTexture* LoadInstance(TextureListingContainer *container);
};
VALIDATE_SIZE(TextureDatabaseEntry, (VER_x32 ? 0x17 : 0x1F));
