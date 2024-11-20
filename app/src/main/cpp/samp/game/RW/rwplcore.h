#pragma once

/******************************************/
/*                                        */
/*    RenderWare(TM) Graphics Library     */
/*                                        */
/******************************************/

/*
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999. Criterion Software Ltd.
 * All Rights Reserved.
 */

/*************************************************************************
 *
 * Filename: <C:/daily/rwsdk/include/d3d9/rwplcore.h>
 * Automatically Generated on: Thu Feb 12 13:01:33 2004
 *
 ************************************************************************/

#include <stdint.h>
#include "types.h"

/* Structure alignment */
#define RWALIGN(type, x)   type /* nothing */
#define rwMATRIXALIGNMENT sizeof(RwUInt32)
#define rwFRAMEALIGNMENT sizeof(RwUInt32)
#define rwV4DALIGNMENT sizeof(RwUInt32)
/****************************************************************************
 Types needed everywhere
 */

/* Get components */
#define RwMatrixGetRight(m)    (&(m)->right)
#define RwMatrixGetUp(m)       (&(m)->up)
#define RwMatrixGetAt(m)       (&(m)->at)
#define RwMatrixGetPos(m)      (&(m)->pos)

/* Vector operations Macros */

#if (!defined(RwV2dAssignMacro))
#define RwV2dAssignMacro(_target, _source)                      \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwV2dAssignMacro)) */

#define RwV2dAddMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) + ( (b)->x));                            \
    (o)->y = (((a)->y) + ( (b)->y));                            \
}                                                               \
MACRO_STOP

#define RwV2dSubMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) - ( (b)->x));                            \
    (o)->y = (((a)->y) - ( (b)->y));                            \
}                                                               \
MACRO_STOP

#define RwV2dScaleMacro(o, i, s)                                \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((i)->x) * ( (s)));                               \
    (o)->y = (((i)->y) * ( (s)));                               \
}                                                               \
MACRO_STOP

#define RwV2dDotProductMacro(a,b)                               \
    (( ((((a)->x) * ( (b)->x))) +                               \
      ( (((a)->y) * ( (b)->y)))))

#define _rwV2dNormalizeMacro(_result, _out, _in)                \
MACRO_START                                                     \
{                                                               \
    RwReal length2 = RwV2dDotProductMacro((_in), (_in));        \
    rwInvSqrtMacro(&(_result), length2);                        \
    RwV2dScaleMacro((_out), (_in), (_result));                  \
}                                                               \
MACRO_STOP

#define RwV2dNormalizeMacro(_result, _out, _in)                 \
MACRO_START                                                     \
{                                                               \
    RwReal length2 = RwV2dDotProductMacro((_in), (_in));        \
    RwReal recip;                                               \
                                                                \
    rwSqrtInvSqrtMacro(&(_result), &recip, length2);            \
    RwV2dScaleMacro((_out), (_in), recip);                      \
}                                                               \
MACRO_STOP

#define RwV2dLengthMacro(_result, _in)                          \
MACRO_START                                                     \
{                                                               \
    (_result) = RwV2dDotProductMacro(_in, _in);                 \
    rwSqrtMacro(&(_result), (_result));                         \
}                                                               \
MACRO_STOP

#define RwV2dLineNormalMacro(_o, _a, _b)                        \
MACRO_START                                                     \
{                                                               \
    RwReal recip;                                               \
                                                                \
    (_o)->y = (((_b)->x) - ( (_a)->x));                         \
    (_o)->x = (((_a)->y) - ( (_b)->y));                         \
    _rwV2dNormalizeMacro(recip, _o,_o);                         \
}                                                               \
MACRO_STOP

#define RwV2dPerpMacro(o, a)                                    \
MACRO_START                                                     \
{                                                               \
    (o)->x = -(a)->y;                                           \
    (o)->y = (a)->x;                                            \
}                                                               \
MACRO_STOP

/* RwV3d */

#if (!defined(RwV3dAssignMacro))
#define RwV3dAssignMacro(_target, _source)                     \
    ( *(_target) = *(_source) )
#endif /* (!defined(RwV3dAssignMacro)) */


#define RwV3dAddMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) + ( (b)->x));                            \
    (o)->y = (((a)->y) + ( (b)->y));                            \
    (o)->z = (((a)->z) + ( (b)->z));                            \
}                                                               \
MACRO_STOP

#define RwV3dSubMacro(o, a, b)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) - ( (b)->x));                            \
    (o)->y = (((a)->y) - ( (b)->y));                            \
    (o)->z = (((a)->z) - ( (b)->z));                            \
}                                                               \
MACRO_STOP

#define RwV3dScaleMacro(o, a, s)                                \
MACRO_START                                                     \
{                                                               \
    (o)->x = (((a)->x) * ( (s)));                               \
    (o)->y = (((a)->y) * ( (s)));                               \
    (o)->z = (((a)->z) * ( (s)));                               \
}                                                               \
MACRO_STOP

#define RwV3dIncrementScaledMacro(o, a, s)                      \
MACRO_START                                                     \
{                                                               \
    (o)->x += (((a)->x) * ( (s)));                              \
    (o)->y += (((a)->y) * ( (s)));                              \
    (o)->z += (((a)->z) * ( (s)));                              \
}                                                               \
MACRO_STOP

#define RwV3dNegateMacro(o, a)                                  \
MACRO_START                                                     \
{                                                               \
    (o)->x = -(a)->x;                                           \
    (o)->y = -(a)->y;                                           \
    (o)->z = -(a)->z;                                           \
}                                                               \
MACRO_STOP

#define RwV3dDotProductMacro(a, b)                              \
    ((((( (((a)->x) * ((b)->x))) +                              \
        ( (((a)->y) * ((b)->y))))) +                            \
        ( (((a)->z) * ((b)->z)))))

#define RwV3dCrossProductMacro(o, a, b)                         \
MACRO_START                                                     \
{                                                               \
    (o)->x =                                                    \
        (( (((a)->y) * ( (b)->z))) -                            \
         ( (((a)->z) * ( (b)->y))));                            \
    (o)->y =                                                    \
        (( (((a)->z) * ( (b)->x))) -                            \
         ( (((a)->x) * ( (b)->z))));                            \
    (o)->z =                                                    \
        (( (((a)->x) * ( (b)->y))) -                            \
         ( (((a)->y) * ( (b)->x))));                            \
}                                                               \
MACRO_STOP

#define _rwV3dNormalizeMacro(_result, _out, _in)                \
MACRO_START                                                     \
{                                                               \
    RwReal length2 = RwV3dDotProductMacro(_in, _in);            \
    rwInvSqrtMacro(&(_result), length2);                        \
    RwV3dScaleMacro(_out, _in, _result);                        \
}                                                               \
MACRO_STOP

#define RwV3dNormalizeMacro(_result, _out, _in)                 \
MACRO_START                                                     \
{                                                               \
    RwReal length2 = RwV3dDotProductMacro((_in), (_in));        \
    RwReal recip;                                               \
                                                                \
    rwSqrtInvSqrtMacro(&(_result), &recip, length2);            \
    RwV3dScaleMacro((_out), (_in), recip);                      \
}                                                               \
MACRO_STOP

#define RwV3dLengthMacro(_result, _in)                          \
MACRO_START                                                     \
{                                                               \
    (_result) = RwV3dDotProductMacro(_in, _in);                 \
    rwSqrtMacro(&(_result), _result);                           \
}                                                               \
MACRO_STOP

#define RwV2dAssign(o, a)               RwV2dAssignMacro(o, a)
#define RwV2dAdd(o, a, b)               RwV2dAddMacro(o, a, b)
#define RwV2dSub(o, a, b)               RwV2dSubMacro(o, a, b)
#define RwV2dLineNormal(_o, _a, _b)     RwV2dLineNormalMacro(_o, _a, _b)
#define RwV2dScale(o, i, s)             RwV2dScaleMacro(o, i, s)
#define RwV2dDotProduct(a,b)            RwV2dDotProductMacro(a,b)
#define RwV2dPerp(o, a)                 RwV2dPerpMacro(o, a)
#define RwV3dAssign(o, a)               RwV3dAssignMacro(o, a)
#define RwV3dAdd(o, a, b)               RwV3dAddMacro(o, a, b)
#define RwV3dSub(o, a, b)               RwV3dSubMacro(o, a, b)
#define RwV3dScale(o, a, s)             RwV3dScaleMacro(o, a, s)
#define RwV3dIncrementScaled(o, a, s)   RwV3dIncrementScaledMacro(o, a, s)
#define RwV3dNegate(o, a)               RwV3dNegateMacro(o, a)
#define RwV3dDotProduct(a, b)           RwV3dDotProductMacro(a, b)
#define RwV3dCrossProduct(o, a, b)      RwV3dCrossProductMacro(o, a, b)

#include "stream.h"
#include "color.h"

/* Limits of types */
#define RwInt32MAXVAL       0x7FFFFFFF
#define RwInt32MINVAL       0x80000000
#define RwUInt32MAXVAL      0xFFFFFFFF
#define RwUInt32MINVAL      0x00000000
#define RwRealMAXVAL        (RwReal)(3.40282347e+38)
#define RwRealMINVAL        (RwReal)(1.17549435e-38)
#define RwInt16MAXVAL       0x7FFF
#define RwInt16MINVAL       0x8000
#define RwUInt16MAXVAL      0xFFFF
#define RwUInt16MINVAL      0x0000

/* The maximum number of texture coordinates */
#define rwMAXTEXTURECOORDS 8
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup fundtypesdatatypes
 * RwTextureCoordinateIndex
 *  This type represents the index for texture coordinates.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwTextureCoordinateIndex
{
    rwNARWTEXTURECOORDINATEINDEX = 0,
    rwTEXTURECOORDINATEINDEX0,
    rwTEXTURECOORDINATEINDEX1,
    rwTEXTURECOORDINATEINDEX2,
    rwTEXTURECOORDINATEINDEX3,
    rwTEXTURECOORDINATEINDEX4,
    rwTEXTURECOORDINATEINDEX5,
    rwTEXTURECOORDINATEINDEX6,
    rwTEXTURECOORDINATEINDEX7,
    rwTEXTURECOORDINATEINDEXFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureCoordinateIndex RwTextureCoordinateIndex;

typedef struct RwTexCoords RwTexCoords;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup fundtypesdatatypes
 * \struct RwTexCoords
 * This type represents the u and v texture
 * coordinates of a particular vertex.
 */
#endif /* RWADOXYGENEXTERNAL */
struct RwTexCoords
{
    RwReal u;   /**< U value */
    RwReal v;   /**< V value */
};


/*****************/

/* Complex types */

/*****************/
typedef struct RwTexCoords RwTexCoords;
/****************************************************************************
 Defines
 */

/* Set true depth information (for fogging, eg) */
#define RwIm2DVertexSetCameraX(vert, camx)          /* Nothing */
#define RwIm2DVertexSetCameraY(vert, camy)          /* Nothing */
#define RwIm2DVertexSetCameraZ(vert, camz)          /* Nothing */

#define RwIm2DVertexSetRecipCameraZ(vert, recipz)   ((vert)->rhw = recipz)

#define RwIm2DVertexGetCameraX(vert)                (cause an error)
#define RwIm2DVertexGetCameraY(vert)                (cause an error)
#define RwIm2DVertexGetCameraZ(vert)                (cause an error)
#define RwIm2DVertexGetRecipCameraZ(vert)           ((vert)->rhw)

/* Set screen space coordinates in a device vertex */
#define RwIm2DVertexSetScreenX(vert, scrnx)         ((vert)->x = (scrnx))
#define RwIm2DVertexSetScreenY(vert, scrny)         ((vert)->y = (scrny))
#define RwIm2DVertexSetScreenZ(vert, scrnz)         ((vert)->z = (scrnz))
#define RwIm2DVertexGetScreenX(vert)                ((vert)->x)
#define RwIm2DVertexGetScreenY(vert)                ((vert)->y)
#define RwIm2DVertexGetScreenZ(vert)                ((vert)->z)

/* Set texture coordinates in a device vertex */
#define RwIm2DVertexSetU(vert, texU, recipz)        ((vert)->u = (texU))
#define RwIm2DVertexSetV(vert, texV, recipz)        ((vert)->v = (texV))
#define RwIm2DVertexGetU(vert)                      ((vert)->u)
#define RwIm2DVertexGetV(vert)                      ((vert)->v)

/* Modify the luminance stuff */
#define RwIm2DVertexSetRealRGBA(vert, red, green, blue, alpha)  \
    ((vert)->emissiveColor =                                    \
     (((RwFastRealToUInt32(alpha)) << 24) |                        \
      ((RwFastRealToUInt32(red)) << 16) |                          \
      ((RwFastRealToUInt32(green)) << 8) |                         \
      ((RwFastRealToUInt32(blue)))))

#define RwIm2DVertexSetIntRGBA(vert, red, green, blue, alpha)   \
    ((vert)->emissiveColor =                                    \
     ((((RwUInt32)(alpha)) << 24) |                             \
      (((RwUInt32)(red)) << 16) |                               \
      (((RwUInt32)(green)) << 8) |                              \
      (((RwUInt32)(blue)))))

#define RwIm2DVertexGetRed(vert)    \
    (((vert)->emissiveColor >> 16) & 0xFF)

#define RwIm2DVertexGetGreen(vert)  \
    (((vert)->emissiveColor >> 8) & 0xFF)

#define RwIm2DVertexGetBlue(vert)   \
    ((vert)->emissiveColor & 0xFF)

#define RwIm2DVertexGetAlpha(vert)  \
    (((vert)->emissiveColor >> 24) & 0xFF)

#define RwIm2DVertexCopyRGBA(dst, src)  \
    ((dst)->emissiveColor = (src)->emissiveColor)

/****************************************************************************
 Global Types
 */
typedef struct RwD3D9Vertex RwD3D9Vertex;
/**
 * \ingroup rwcoredriverd3d9
 * \struct RwD3D9Vertex
 * D3D9 vertex structure definition for 2D geometry
 */
struct RwD3D9Vertex
{
    RwReal      x;              /**< Screen X */
    RwReal      y;              /**< Screen Y */
    RwReal      z;              /**< Screen Z */
    RwReal      rhw;            /**< Reciprocal of homogeneous W */

    RwUInt32    emissiveColor;  /**< Vertex color */

    RwReal      u;              /**< Texture coordinate U */
    RwReal      v;              /**< Texture coordinate V */
};

typedef RwD3D9Vertex    RwIm2DVertex;
typedef RwUInt32        RxVertexIndex;
typedef RxVertexIndex   RwImVertexIndex;

#include "immedi.h"

/* Expose Z buffer range */
extern RwReal (*RwIm2DGetNearScreenZ)(void);
extern RwReal (*RwIm2DGetFarScreenZ)(void);

extern RwBool (*RwRenderStateGet)(RwRenderState state, void *value);
extern RwBool (*RwRenderStateSet)(RwRenderState state, void *value);

extern RwBool (*RwIm2DRenderLine)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
extern RwBool (*RwIm2DRenderTriangle)(RwIm2DVertex *vertices, RwInt32 numVertices,
                                   RwInt32 vert1, RwInt32 vert2, RwInt32 vert3 );
extern RwBool (*RwIm2DRenderPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices);
extern RwBool (*RwIm2DRenderIndexedPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices,
                                                             RwImVertexIndex *indices, RwInt32 numIndices);

struct RpMaterial
{
	struct RwTexture*   texture; /**< texture */
	RwRGBA              color; /**< color */
	struct RxPipeline*  pipeline; /**< pipeline */
	RwSurfaceProperties surfaceProps; /**< surfaceProps */
	RwInt16             refCount;          /* C.f. rwsdk/world/bageomet.h:RpGeometry */
	RwInt16             pad;
};
static_assert(sizeof(RpMaterial) == (VER_x32 ? 0x1C : 0x28));

struct RpMaterialList
{
	RpMaterial**    materials;
	RwInt32         numMaterials;
	RwInt32         space;
};
static_assert(sizeof(RpMaterialList) == (VER_x32 ? 0xC : 0x10));

/* Doubly linked list. End marked as start (its a ring) */

/**
 * \ingroup rwresources
 * \struct RwResEntry
 * RwResEntry object. Instanced data block in resources arena.
 * This should be considered an opaque
 * type. Use the RwResEntry API functions to access.
 */

typedef struct RwResEntry RwResEntry;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwresources
 * \ref RwResEntryDestroyNotify type represents the function
 * called from \ref RwResourcesFreeResEntry (and indirectly from
 * \ref RwResourcesEmptyArena) immediately before the memory used by the
 * specified resources entry is released.
 *
 * \param  resEntry   Pointer to the instanced data.
 */
#endif /* RWADOXYGENEXTERNAL */
typedef void        (*RwResEntryDestroyNotify) (RwResEntry * resEntry);

struct RwResEntry
{
    RwLLLink            link;   /* Node in the list of resource elements */
    RwInt32             size;   /* Size of this node */
    void               *owner;  /* Owner of this node */
    RwResEntry        **ownerRef; /* Pointer to pointer to this (enables de-alloc) */
    RwResEntryDestroyNotify destroyNotify; /* This is called right before destruction */
};

struct RpGeometry
{
    RwObject            object;     /* Generic type */
    RwUInt32            flags;      /* Geometry flags */
    RwUInt16            lockedSinceLastInst; /* What has been locked since we last instanced - for re-instancing */
    RwInt16             refCount;   /* Reference count (for keeping track of atomics referencing geometry) */

    RwInt32             numTriangles; /* Quantity of various things (polys, verts and morph targets) */
    RwInt32             numVertices;
    RwInt32             numMorphTargets;
    RwInt32             numTexCoordSets;

    RpMaterialList      matList;

    struct RpTriangle* triangles;  /* The triangles */

    RwRGBA* preLitLum;  /* The pre-lighting values */

    RwTexCoords* texCoords[8]; /* Texture coordinates */
    uintptr_t   vertexBuffer;
    struct RpMeshHeader* mesh;   /* The mesh - groups polys of the same material */

    struct RwResEntry* repEntry;       /* Information for an instance */

    struct RpMorphTarget* morphTarget;    /* The Morph Target */
};
static_assert(sizeof(RpGeometry) == (VER_x32 ? 0x64 : 0xA8));

enum RwOpCombineType
{
    rwCOMBINEREPLACE = 0,   /**<Replace -
                                all previous transformations are lost */
    rwCOMBINEPRECONCAT,     /**<Pre-concatenation -
                                the given transformation is applied
                                before all others */
    rwCOMBINEPOSTCONCAT,    /**<Post-concatenation -
                                the given transformation is applied
                                after all others */
    rwOPCOMBINETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

/*
 * RwOpCombineType typedef for enum RwOpCombineType
 */
typedef enum RwOpCombineType RwOpCombineType;

/* External flags (bits 0-15) */

/* Internal flags (bits 16-31) */
enum RwMatrixType
{
    rwMATRIXTYPENORMAL = 0x00000001,
    rwMATRIXTYPEORTHOGONAL = 0x00000002,
    rwMATRIXTYPEORTHONORMAL = 0x00000003,
    rwMATRIXTYPEMASK = 0x00000003,
    rwMATRIXTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixType RwMatrixType;

enum RwMatrixFlag
{
    rwMATRIXINTERNALIDENTITY = 0x00020000,
    rwMATRIXFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixFlag RwMatrixFlag;

/* Flags describing what will optimize for */
enum RwMatrixOptimizations
{
    rwMATRIXOPTIMIZE_IDENTITY = 0x00020000,
    rwMATRIXOPTIMIZATIONSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixOptimizations RwMatrixOptimizations;

struct RwMatrixTag
{
    /* These are padded to be 16 byte quantities per line */
    RwV3d               right;
    RwUInt32            flags;
    RwV3d               up;
    RwUInt32            pad1;
    RwV3d               at;
    RwUInt32            pad2;
    RwV3d               pos;
    RwUInt32            pad3;
};
static_assert(sizeof(RwMatrixTag) == 0x40);

typedef RwMatrixTag RwMatrix;


RwMatrix* RwMatrixUpdate(RwMatrix* matrix);
RwBool RwMatrixDestroy(RwMatrix* mpMat);
RwUInt32 RwStreamRead(RwStream* stream, void* buffer, RwUInt32 length);
RwStream* RwStreamOpen(RwStreamType type, RwStreamAccessType accessType, const void* data);
RwBool RwStreamClose(RwStream* stream, void* data);
RwMatrix* RwMatrixTransform(RwMatrix* matrix, const RwMatrix* transform, RwOpCombineType combineOp);
RwMatrix* RwMatrixOrthoNormalize(RwMatrix* matrixOut, const RwMatrix* matrixIn);
RwMatrix* RwMatrixCreate();
RwV3d* RwV3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix);
RwV3d* RwV3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix);
RwMatrix* RwMatrixRotate(RwMatrix* pMat, class CVector* axis, float angle);
RwMatrix* RwMatrixTranslate(RwMatrix *matrix, const RwV3d *translation, RwOpCombineType combineOp);