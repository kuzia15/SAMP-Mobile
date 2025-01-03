//
// Created by x1y2z on 02.05.2024.
//

#pragma once

#include "types.h"

struct _rxOpenGLAllInOnePrivateData
{
    /* instance both display lists and vertex arrays, or make a sensible choice? */
    RwBool       instanceDLandVA;

    /* per mesh instance callback */
    uintptr_t*   instanceCB;

    /* per mesh reinstance callback */
    uintptr_t*  reinstanceCB;

    /* per atomic/worldsector lighting callback */
    uintptr_t*   lightingCB;

    /* per atomic/worldsector render callback */
    uintptr_t*    renderCB;
};
