//
// Created by x1y2z on 27.04.2024.
//

#pragma once

#include "../common.h"
#include "Pool.h"
#include "game/Pipelines/RxPipeline.h"

constexpr auto ENV_MAP_PLUGIN_ID = 0x253F2FC;
constexpr auto ENV_MAP_ATM_PLUGIN_ID = 0x253F2F4;
constexpr auto SPECULAR_MAP_PLUGIN_ID = 0x253F2F6;
constexpr auto CUSTOM_CAR_ENV_MAP_PIPELINE_PLUGIN_ID = 0x53F2009A;

enum RpMatFXMaterialFlags
{
    rpMATFXEFFECTNULL            = 0, /**<No material effect.           */
    rpMATFXEFFECTBUMPMAP         = 1, /**<Bump mapping                  */
    rpMATFXEFFECTENVMAP          = 2, /**<Environment mapping           */
    rpMATFXEFFECTBUMPENVMAP      = 3, /**<Bump and environment mapping  */
    rpMATFXEFFECTDUAL            = 4, /**<Dual pass                     */
    rpMATFXEFFECTUVTRANSFORM     = 5, /**<Base UV transform             */
    rpMATFXEFFECTDUALUVTRANSFORM = 6, /**<Dual UV transform (2 pass)    */

    rpMATFXEFFECTMAX,
    rpMATFXNUMEFFECTS       = rpMATFXEFFECTMAX - 1,

    rpMATFXFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

struct ReflectionMaterialStream {
    float scaleX;
    float scaleY;
    float transSclX;
    float transSclY;
    float shininess;
    RwTexture* texture;
};

struct CustomEnvMapPipeMaterialData {
    uint8 scaleX;
    uint8 scaleY;
    uint8 transSclX;
    uint8 transSclY;

    uint8 shininess;
    int16 renderFrameCounter;
    RwTexture* texture;

    void FromStream(const ReflectionMaterialStream& stream) {
        scaleX    = (uint8)(stream.scaleX * 8.0f);
        scaleY    = (uint8)(stream.scaleY * 8.0f);
        transSclX = (uint8)(stream.transSclX * 8.0f);
        transSclY = (uint8)(stream.transSclY * 8.0f);
        shininess = (uint8)(stream.shininess * 255.0f);
        texture   = stream.texture;
        renderFrameCounter = 0;
    };
};
VALIDATE_SIZE(CustomEnvMapPipeMaterialData, (VER_x32 ? 0xC : 0x10));

struct CustomEnvMapPipeAtomicData {
    float lastTrans;
    float posx;
    float posy;
};
VALIDATE_SIZE(CustomEnvMapPipeAtomicData, 0xC);

struct SpecMatBuffer {
    float specularity;
    char name[24];
};

struct CustomSpecMapPipeMaterialData {
    float specularity;
    RwTexture* texture;
};
VALIDATE_SIZE(CustomSpecMapPipeMaterialData, (VER_x32 ? 0x8 : 0x10));

typedef CPool<CustomEnvMapPipeMaterialData, CustomEnvMapPipeMaterialData, true>     CustomEnvMapPipeMaterialDataPool;
typedef CPool<CustomEnvMapPipeAtomicData, CustomEnvMapPipeAtomicData, true>         CustomEnvMapPipeAtomicDataPool;
typedef CPool<CustomSpecMapPipeMaterialData, CustomSpecMapPipeMaterialData, true>   CustomSpecMapPipeMaterialDataPool;

class CCustomCarEnvMapPipeline {
    static inline RxPipeline* ObjPipeline = nullptr;

    static inline CustomEnvMapPipeMaterialData          fakeEnvMapPipeMatData;
    static inline CustomEnvMapPipeMaterialDataPool*     m_gEnvMapPipeMatDataPool;
    static inline CustomEnvMapPipeAtomicDataPool*       m_gEnvMapPipeAtmDataPool;
    static inline CustomSpecMapPipeMaterialDataPool*    m_gSpecMapPipeMatDataPool;

public:
    static void InjectHooks();

    static bool RegisterPlugin();

    static bool CreatePipe();
    static void DestroyPipe();
    static RxPipeline* CreateCustomObjPipe();

    static RwBool CustomPipeInstanceCB(void* object, RxOpenGLMeshInstanceData* instanceData, const RwBool instanceDLandVA, const RwBool reinstance);
    static void CustomPipeRenderCB(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags);
};
