/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CEntityGTA.h"


class CDummy : public CEntityGTA {
public:
//    CDummy();
//    ~CDummy() override = default;
//
    static void* operator new(size_t size);
    static void  operator delete(void* obj);
//
//public:
//    static void InjectHooks();
};

static_assert(sizeof(CDummy) == (VER_x32 ? 0x3C : 0x60));


bool IsDummyPointerValid(CDummy* dummy);
