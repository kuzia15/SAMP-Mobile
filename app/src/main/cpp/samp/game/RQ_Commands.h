//
// Created by x1y2z on 25.05.2023.
//

#pragma once

#include "ES2VertexBuffer.h"

class CRQ_Commands {
public:
    static void rqVertexBufferSelect(uintptr_t **qData);

public:
    static void InjectHooks();
};
