//
// Created by x1y2z on 15.04.2023.
//

#pragma once


#include "common.h"
#include "CFileMgr.h"
#include "Quaternion.h"
#include "game/Entity/CEntityGTA.h"

class CFileObjectInstance {
public:
    CVector     m_vecPosition;
    CQuaternion m_qRotation;
    int32       m_nModelId;
    union {
        struct { // CFileObjectInstanceType
            uint32 m_nAreaCode : 8;
            uint32 m_bRedundantStream : 1;
            uint32 m_bDontStream : 1; // Merely assumed, no countercheck possible.
            uint32 m_bUnderwater : 1;
            uint32 m_bTunnel : 1;
            uint32 m_bTunnelTransition : 1;
            uint32 m_nReserved : 19;
        };
        uint32 m_nInstanceType;
    };
    int32 m_nLodInstanceIndex; // -1 - without LOD model
};

VALIDATE_SIZE(CFileObjectInstance, 0x28);

enum eSection : uint8 {
    UNDEFINED             = 0,
    OBJECT                = 1,
    TIME_OBJECT           = 3,
    WEAPON_OBJECT         = 4,
    CLUMP_OBJECT          = 5,
    ANIMATED_CLUMP_OBJECT = 6,
    VEHICLE_OBJECT        = 7,
    PED_OBJECT            = 8,
    PATH_NODE             = 9,
    TWO_D_EFFECT          = 10,
    TXD_PARENT            = 11,
};

enum eIDE : uint8 {
    IDE_NONE,
    IDE_OBJS,
    IDE_MLO, // ?
    IDE_TOBJ,
    IDE_WEAP,
    IDE_HIER,
    IDE_ANIM,
    IDE_CARS,
    IDE_PEDS,
    IDE_PATH,
    IDE_2DFX,
    IDE_TXDP,
};

enum eIPL : uint8 {
    IPL_NONE,
    IPL_PATH,
    IPL_INST,
    IPL_MULT,
    IPL_ZONE,
    IPL_CULL,
    IPL_OCCL,
    IPL_UNK7,
    IPL_GRGE,
    IPL_ENEX,
    IPL_PICK,
    IPL_CARS,
    IPL_JUMP,
    IPL_TCYC,
    IPL_AUZO,
};

class CFileLoader {
public:
    static inline char ms_line[512];

public:
    static void InjectHooks();

    static char* LoadLine(FILE* file);
    static char* LoadLine(char*& outLine, int32& outSize);

    static char *FindFirstNonNullOrWS(char *it);

    static int32 LoadObject(const char *line);
    static CEntityGTA* LoadObjectInstance(CFileObjectInstance* objInstance, const char* modelName);
    static CEntityGTA* LoadObjectInstance1(const char* line);
};

