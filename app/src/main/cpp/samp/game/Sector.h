/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrListDoubleLink.h"

class CSector {
public:
    CPtrListDoubleLink m_buildings;
    CPtrListDoubleLink m_dummies;

public:
    CSector() = default;
    ~CSector() = default;
};

VALIDATE_SIZE(CSector, sizeof(CPtrListDoubleLink)*2);
