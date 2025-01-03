//
// Created by x1y2z on 21.11.2023.
//

#pragma once

#include "TextureListing.h"

class __attribute__((packed)) __attribute__((aligned(1))) TextureListingContainer
{
public:
    TextureListing  listing;
    void            *data;
    uint32          dataSize;
    bool            ownsData;

public:
    static void InjectHooks();

    RwRaster *CreateRaster(const TextureDatabaseEntry *forEntry);
};
VALIDATE_SIZE(TextureListingContainer, (VER_x32 ? 0x11 : 0x15));

