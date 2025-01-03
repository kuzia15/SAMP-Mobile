//
// Created by x1y2z on 20.11.2023.
//

#pragma once

#include "game/common.h"
#include "TextureListingContainer.h"
#include "TextureDatabaseEntry.h"

template <class A>  struct TDBArray
{
    unsigned int numAlloced;
    unsigned int numEntries;
    A *dataPtr;
};

enum TextureDatabaseFormat
{
    DF_UNC = 0x0,
    DF_DXT = 0x1,
    DF_360 = 0x2,
    DF_PS3 = 0x3,
    DF_PVR = 0x4,
    DF_ETC = 0x5,
    DF_Default = 0x6,
    DF_ALL = 0x7,
};

struct TextureCategory
{
    const char *name;
    uint32 onFootPriority;
    uint32 slowCarPriority;
    uint32 fastCarPriority;
    uint16 defaultFormat;
    uint16 defaultStreamMode;
};
VALIDATE_SIZE(TextureCategory, (VER_x32 ? 0x14 : 0x18));


class TextureDatabase {
public:
    int (**_vptr$TextureDatabase)(void);
    const char *name;
    TDBArray<TextureCategory> categories;
    TDBArray<TextureDatabaseEntry> entries;
    TDBArray<TextureListingContainer> thumbs[6];
    TDBArray<void *> toFree;
    TextureDatabaseFormat loadedFormat;

public:
    static void InjectHooks();

    void LoadEntry(char *line, bool hashName);
    bool LoadThumbs(TextureDatabaseFormat forFormat, bool setEntries);
    bool LoadDataFile(const char *filename, std::vector<TextureListingContainer> *intoArray);
    void GetPNGFilename(char *pngBuffer, const char *entryName);
};
VALIDATE_SIZE(TextureDatabase, (VER_x32 ? 0x78 : 0xA8));