//
// Created by x1y2z on 25.05.2023.
//

#pragma once

#include <GLES3/gl32.h>
#include "RQVertexBuffer.h"

struct ES2VertexBuffer : RQVertexBuffer
{
    unsigned int    size;
    GLuint          bufferId;

public:
    static inline const void *curCPUBuffer;


public:
    static void InjectHooks();
};

static_assert(sizeof (ES2VertexBuffer) == 0xc);




