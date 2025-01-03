//
// Created by x1y2z on 11.01.2023.
//

#pragma once

#include <cstdint>
#include <queue>
#include <list>
#include "game/RW/RenderWare.h"
#include "OSFile.h"

#include "TextureDatabase.h"


class TextureDatabaseRuntime : public TextureDatabase {
public:
    TDBArray<uint32>   priorityStreamingQueue;
    TDBArray<uint32>   renderedStreamingQueue;
    TDBArray<uint32>   unrenderedStreamingQueue;
    TDBArray<uint32>   deletionQueue;
    OSFile             streamFile;
    TDBArray<uint32>   fullDataOffsets;
    TDBArray<uint32>   hashOffsets;
    uint32             numHashes;

public:
    static inline uint32 storedTexels{};
    static inline TDBArray<TextureDatabaseRuntime*> loaded;
    static inline TDBArray<TextureDatabaseRuntime*> registered;
    static inline TDBArray<RwTexture*> detailTextures;

public:
    static void InjectHooks();

    static TextureDatabaseRuntime* Load(const char *withName, bool fullyLoad, TextureDatabaseFormat forcedFormat);
    static TextureDatabaseRuntime* GetDatabase(const char *dbName);
    static void UpdateStreaming(float deltaTime, bool flush);

    static void Register(TextureDatabaseRuntime *thiz);
    static void Unregister(TextureDatabaseRuntime *toUnregister);

    static RwTexture *GetTexture(const char *name);
};
VALIDATE_SIZE(TextureDatabaseRuntime, (VER_x32 ? 0xC8 : 0x118));
VALIDATE_SIZE(TDBArray<TextureDatabaseRuntime*>, (VER_x32 ? 0xC : 0x10));
