#pragma once

#include "color.h"
#include "raster.h"
#include "immedi.h"


/**
 * \ingroup rwcoregeneric
 *  RxRenderStateFlag 
 *  Flags used in the \ref RxRenderStateVector structure */

enum RxRenderStateFlag
{
    rxRENDERSTATEFLAG_TEXTUREPERSPECTIVE   = 0x00000001, /**<Perspective texturing is to be enabled */
    rxRENDERSTATEFLAG_ZTESTENABLE          = 0x00000002, /**<Z-Buffer testing is to be performed */
    rxRENDERSTATEFLAG_ZWRITEENABLE         = 0x00000004, /**<Z-Buffer writing is to be enabled */
    rxRENDERSTATEFLAG_VERTEXALPHAENABLE    = 0x00000008, /**<Vertex alpha is to be enabled */
    rxRENDERSTATEFLAG_FOGENABLE            = 0x00000010, /**<Fog is to be enabled */
    rxRENDERSTATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxRenderStateFlag RxRenderStateFlag;

typedef struct RxRenderStateVector RxRenderStateVector;
/**
 * \ingroup rwcoregeneric
 * \struct RxRenderStateVector
 * Structure describing a render-state vector,
 * used by the RxClRenderState cluster */
struct RxRenderStateVector
{
    RwUInt32             Flags;         /**< A load of the boolean renderstate options */
    RwShadeMode          ShadeMode;     /**< Flat or Gouraud currently */
    RwBlendFunction      SrcBlend;      /**< Src  alpha, 1-src  alpha, etc */
    RwBlendFunction      DestBlend;     /**< Dest alpha, 1-dest alpha, etc */
    RwRaster            *TextureRaster; /**< texture raster */
    RwTextureAddressMode AddressModeU;  /**< U addressing mode - WRAP, MIRROR, CLAMP, BORDER */
    RwTextureAddressMode AddressModeV;  /**< V addressing mode - WRAP, MIRROR, CLAMP, BORDER */
    RwTextureFilterMode  FilterMode;    /**< filtering mode - combos of NEAREST LINEAR MIP */
    RwRGBA               BorderColor;   /**< Border color for texturing address mode border */
    RwFogType            FogType;       /**< Select the type of fogging to use */
    RwRGBA               FogColor;      /**< Color used for fogging */
};
static_assert(sizeof(RxRenderStateVector) == (VER_x32 ? 0x2C : 0x30));

#undef RxRenderStateVectorAssign
#define RxRenderStateVectorAssign(_target, _source) ( *(_target) = *(_source) )




#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern       RxRenderStateVector *RxRenderStateVectorSetDefaultRenderStateVector(RxRenderStateVector *rsvp);
extern const RxRenderStateVector *RxRenderStateVectorGetDefaultRenderStateVector(void);
extern       RxRenderStateVector *RxRenderStateVectorCreate(RwBool current);
extern       void                 RxRenderStateVectorDestroy(RxRenderStateVector *rsvp);
extern       RxRenderStateVector *RxRenderStateVectorLoadDriverState(RxRenderStateVector *rsvp);

#ifdef    __cplusplus
}
#endif /* __cplusplus */


