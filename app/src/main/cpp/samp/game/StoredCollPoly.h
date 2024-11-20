/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once


#include "Core/Vector.h"
#include "Collision/ColPoint.h"


struct CStoredCollPoly {
    CVector      verts[3]{}; // triangle vertices
    bool         valid{};
    char         _pad[3];
    tColLighting ligthing{};
    char         _pad2[3];
};


VALIDATE_SIZE(CStoredCollPoly, 0x2c);
