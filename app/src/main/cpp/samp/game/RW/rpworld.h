//
// Created by x1y2z on 07.04.2023.
//

#pragma once

#include <stdint.h>
extern uintptr_t g_libGTASA;
extern uintptr_t g_libSAMP;

#define rpGEOMETRY 8

/**
 * \ingroup rpgeometry
 * RpGeometryFlag
 * Geometry type flags
 *
 * When creating a geometry, these flags can be OR'ed together to
 * specify the format along with the rpGEOMETRYTEXCOORDSETS(n) macro if more
 * than two sets of texture coordinates are required. See \ref RpGeometryCreate
 * for more details.
 *
 * \see RpGeometryCreate().
 */
enum RpGeometryFlag
{
    rpGEOMETRYTRISTRIP  = 0x00000001, /**<This geometry's meshes can be
                                          rendered as strips.
                                          \ref RpMeshSetTriStripMethod is
                                          used to change this method.*/
    rpGEOMETRYPOSITIONS = 0x00000002, /**<This geometry has positions */
    rpGEOMETRYTEXTURED  = 0x00000004, /**<This geometry has only one set of
                                          texture coordinates. Texture
                                          coordinates are specified on a per
                                          vertex basis */
    rpGEOMETRYPRELIT    = 0x00000008, /**<This geometry has pre-light colors */
    rpGEOMETRYNORMALS   = 0x00000010, /**<This geometry has vertex normals */
    rpGEOMETRYLIGHT     = 0x00000020, /**<This geometry will be lit */
    rpGEOMETRYMODULATEMATERIALCOLOR = 0x00000040, /**<Modulate material color
                                                      with vertex colors
                                                      (pre-lit + lit) */

    rpGEOMETRYTEXTURED2 = 0x00000080, /**<This geometry has at least 2 sets of
                                          texture coordinates. */

    /*
     * These above flags were stored in the flags field in an RwObject, they
     * are now stored in the flags file of the RpGeometry.
     */

    rpGEOMETRYNATIVE            = 0x01000000,
    rpGEOMETRYNATIVEINSTANCE    = 0x02000000,

    rpGEOMETRYFLAGSMASK         = 0x000000FF,
    rpGEOMETRYNATIVEFLAGSMASK   = 0x0F000000,

    rpGEOMETRYFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpGeometryFlag RpGeometryFlag;

/* Multi texture coordinate format specifier for RpGeometryCreate() */
#define rpGEOMETRYTEXCOORDSETS(_num) ((_num & 0xff) << 16)

/**
 * \ingroup rpgeometry
 * RpGeometryLockMode
 * Geometry lock flags
 */
enum RpGeometryLockMode
{
    rpGEOMETRYLOCKPOLYGONS   =   0x01, /**<Lock the polygons (triangle list) */
    rpGEOMETRYLOCKVERTICES   =   0x02, /**<Lock the vertex positional data */
    rpGEOMETRYLOCKNORMALS    =   0x04, /**<Lock the vertex normal data */
    rpGEOMETRYLOCKPRELIGHT   =   0x08, /**<Lock the pre-light values */
    rpGEOMETRYLOCKTEXCOORDS  =   0x10, /**<Lock the texture coordinates set 1*/
    rpGEOMETRYLOCKTEXCOORDS1 =   0x10, /**<Lock the texture coordinates set 1*/
    rpGEOMETRYLOCKTEXCOORDS2 =   0x20, /**<Lock the texture coordinates set 2*/
    rpGEOMETRYLOCKTEXCOORDS3 =   0x40, /**<Lock the texture coordinates set 3*/
    rpGEOMETRYLOCKTEXCOORDS4 =   0x80, /**<Lock the texture coordinates set 4*/
    rpGEOMETRYLOCKTEXCOORDS5 = 0x0100, /**<Lock the texture coordinates set 5*/
    rpGEOMETRYLOCKTEXCOORDS6 = 0x0200, /**<Lock the texture coordinates set 6*/
    rpGEOMETRYLOCKTEXCOORDS7 = 0x0400, /**<Lock the texture coordinates set 7*/
    rpGEOMETRYLOCKTEXCOORDS8 = 0x0800, /**<Lock the texture coordinates set 8*/
    rpGEOMETRYLOCKTEXCOORDSALL = 0x0ff0, /**<Lock all texture coordinate sets*/
    rpGEOMETRYLOCKALL        = 0x0fff, /**<Combination of all the above */

    rpGEOMETRYLOCKMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpGeometryLockMode RpGeometryLockMode;

/* Convenience macro for generating a tex coord lock flag */
#define rpGEOMETRYLOCKTEXCOORDSIDX(_idx) (rpGEOMETRYLOCKTEXCOORDS1 << (_idx))


#define RpGeometryGetMaterialMacro(_geometry, _num)             \
    (((_geometry)->matList.materials)[(_num)])

#define RpAtomicGetGeometryMacro(_atomic)                               \
    ((_atomic)->geometry)

#define RpMaterialGetColorMacro(_material)                          \
    (&((_material)->color))

#define RpMaterialSetSurfaceProperties(_material, _surfProps)       \
    RpMaterialSetSurfacePropertiesMacro(_material, _surfProps)

#define RpMaterialGetSurfaceProperties(_material)                   \
    RpMaterialGetSurfacePropertiesMacro(_material)

#define RpAtomicGetGeometry(_atomic) \
    RpAtomicGetGeometryMacro(_atomic)

#define RpMaterialGetColor(_material)                               \
    RpMaterialGetColorMacro(_material)

#define RpMaterialSetColorMacro(_material, _color)                  \
    (RwRGBAAssign(&((_material)->color), (_color)), (_material))

#define RpMaterialGetColorMacro(_material)                          \
    (&((_material)->color))

#define RpMaterialSetSurfacePropertiesMacro(_material, _surfProps)  \
    (RwSurfacePropertiesAssign(&((_material)->surfaceProps),        \
                               (_surfProps)), (_material))

#define RpMaterialGetSurfacePropertiesMacro(_material)              \
    (&((_material)->surfaceProps))

#define RpMaterialGetTextureMacro(_material)                        \
    ((_material)->texture)

#define RpMaterialAddRef(_material)                                 \
    RpMaterialAddRefMacro(_material)

#define RpMaterialSetColor(_material, _color)                       \
    RpMaterialSetColorMacro(_material, _color)

#define RpMaterialGetColor(_material)                               \
    RpMaterialGetColorMacro(_material)

#define RpMaterialSetSurfaceProperties(_material, _surfProps)       \
    RpMaterialSetSurfacePropertiesMacro(_material, _surfProps)

#define RpMaterialGetSurfaceProperties(_material)                   \
    RpMaterialGetSurfacePropertiesMacro(_material)

#define RpGeometryGetMaterial(_geometry, _num)                  \
    RpGeometryGetMaterialMacro(_geometry, _num)

#define RpMaterialGetTexture(_material)                             \
    RpMaterialGetTextureMacro(_material)

#define RpAtomicGetFrameMacro(_atomic)                                  \
    ((RwFrame *) rwObjectGetParent(_atomic))


#define RpAtomicGetFrame(_atomic) \
    RpAtomicGetFrameMacro(_atomic)


#define RpAtomicGetClumpMacro(_atomic)                                  \
    ((_atomic)->clump)

#define RpAtomicGetClump(_atomic) \
    RpAtomicGetClumpMacro(_atomic)

#define RpGeometryGetFlags(_geometry)                      \
    ((_geometry)->flags)

#define RpGeometrySetFlags(_geometry, _flags)              \
    (((_geometry)->flags = (_flags)), (_geometry))


/****************************** Object type ID ******************************/

/* Type IDs */

#define rpATOMIC 1
#define rpCLUMP 2

/* Interpolator flags */
enum RpInterpolatorFlag
{
    rpINTERPOLATORDIRTYINSTANCE = 0x01,
    rpINTERPOLATORDIRTYSPHERE = 0x02,
    rpINTERPOLATORNOFRAMEDIRTY = 0x04,
    rpINTERPOLATORFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpInterpolatorFlag rpInterpolatorFlag;

/**
 * \ingroup rpatomic
 * The bit-field type RpAtomicFlag specifies the options available for
 * controlling the behavior of atomics. See API function \ref RpAtomicSetFlags.
 *
 * \see RpAtomicSetFlags
 * \see RpAtomicGetFlags
 * \see RpWorldSectorForAllCollisionAtomics
 */

enum RpAtomicFlag
{
    rpATOMICCOLLISIONTEST = 0x01, /**<A generic collision flag to indicate
                                   * that the atomic should be considered
                                   * in collision tests.
                                   */
    rpATOMICRENDER = 0x04,      /**<The atomic is rendered if it is
                                 * in the view frustum.
                                 */

    rpATOMICFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpAtomicFlag RpAtomicFlag;

/**
 * \ingroup rpclump
 * \struct RpClump
 * Clump Geometry object. This should be
 * considered an opaque type. Use the RpClump API functions to access.
 */
typedef struct RpClump RpClump;

/**
 * \ingroup rpclump
 * RpClumpCallBack represents the function called from
 * \ref RwCameraForAllClumpsInFrustum when a clump lies inside the current
 * camera's view frustum. It is also the callback type used with
 * \ref RpWorldForAllClumps.
 *
 * This function should return a pointer to the current clump to indicate
 * success. The callback may return NULL to terminate further callbacks on
 * the clumps.
 *
 * \param  clump   Pointer to the current clump, supplied by
 *                 iterator.
 * \param  data    Pointer to developer-defined data structure.
 *
 * \return Pointer to the current clump.
 *
 * \see RpWorldForAllClumps
 * \see RpClumpGetCallBack
 * \see RpClumpSetCallBack
 * \see RwCameraForAllClumpsInFrustum
 */
typedef RpClump    *(*RpClumpCallBack) (RpClump * clump, void *data);

struct RpClump
{
    RwObject            object;

    /* Information about all the Atomics */
    RwLinkList          atomicList;

    /* Lists of lights and cameras */
    RwLinkList          lightList;
    RwLinkList          cameraList;

    /* The clump in a world */
    RwLLLink            inWorldLink;

    /* Clump frustum callback */
    RpClumpCallBack     callback;

    RwUInt16 renderFrame;
    RwUInt16 pad;
};

static_assert(sizeof(RpClump) == (VER_x32 ? 0x30 : 0x60));

/**
 * \ingroup rpatomic
 * \struct RpAtomic
 * Atomic Geometry object. This should be
 * considered an opaque type. Use the RpAtomic API functions to access.
 */
typedef struct RpAtomic RpAtomic;

typedef struct RpInterpolator RpInterpolator;

struct RpInterpolator
{
    RwInt32             flags; /**< flags */
    RwInt16             startMorphTarget; /**< startMorphTarget */
    RwInt16             endMorphTarget; /**< endMorphTarget */
    RwReal              time; /**< time */
    RwReal              recipTime; /**< recipTime */
    RwReal              position; /**< position */
};

typedef RpAtomic   *(*RpAtomicCallBackRender) (RpAtomic * atomic);

struct RpAtomic
{
    RwObjectHasFrame    object;

    /* Information for an instance */
    RwResEntry         *repEntry;

    /* Triangles making the object */
    RpGeometry         *geometry;

    /* Interpolated bounding sphere (in object space and world space) */
    RwSphere            boundingSphere;
    RwSphere            worldBoundingSphere;

    /* Connections to other atomics */
    RpClump            *clump;
    RwLLLink            inClumpLink;

    /* callbacks */
    RpAtomicCallBackRender renderCallBack;

    /* Interpolation animation pointer */
    RpInterpolator      interpolator;

    /* Counter for checks of "render has occurred already" */
    RwUInt16            renderFrame;
    RwUInt16            pad;

    /* Connections to sectors */
    RwLinkList          llWorldSectorsInAtomic;

    /* The Atomic object pipeline for this Atomic */
    RxPipeline         *pipeline;
};
static_assert(sizeof(RpAtomic) == (VER_x32 ? 0x70 : 0xA8));

/**
 * \ingroup rpatomic
 * \ref RpAtomicCallBack represents the function called from \ref RpWorldSectorForAllAtomics and
 * \ref RpClumpForAllAtomics for all atomics in a given world sector or clump.
 * This function should return a pointer to the current atomic to indicate
 * success. The callback may return NULL to terminate further callbacks on
 * the world sector.
 *
 * \param  atomic   Pointer to the current atomic, supplied by
 *                  iterator.
 * \param  data     Pointer to developer-defined data structure.
 *
 * \return Returns a pointer to the current atomic
 */
typedef RpAtomic   *(*RpAtomicCallBack) (RpAtomic * atomic, void *data);

#define RpAtomicRenderMacro(_atomic)                                    \
    ((_atomic)->renderCallBack(_atomic))

/* NB "RpAtomicSetRenderCallBack(atom++, callback)" will break it */
#define RpAtomicSetRenderCallBackMacro(_atomic, _callback)              \
MACRO_START                                                             \
{                                                                       \
    (_atomic)->renderCallBack = (_callback);                            \
    if (!(_atomic)->renderCallBack)                                     \
    {                                                                   \
        (_atomic)->renderCallBack = AtomicDefaultRenderCallBack;        \
    }                                                                   \
}                                                                       \
MACRO_STOP

/****************************************************************************
Global types
*/

typedef struct RpVertexNormal RpVertexNormal;

#if (!defined(DOXYGEN))
struct RpVertexNormal
{
    RwInt8          x;
    RwInt8          y;
    RwInt8          z;
    RwUInt8         pad; /* pad character to force alignment */
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpworldsector
 * \struct RpWorldSector
 * World Sector object. This should be
 * considered an opaque type. Use the RpWorldSector API functions to access.
 */
typedef struct RpWorldSector RpWorldSector;

#if (!defined(DOXYGEN))
struct RpWorldSector
{
    RwInt32                 type;

    RpTriangle              *triangles;             /* Triangles */

    RwV3d                   *vertices;              /* Vertex positions */
    RpVertexNormal          *normals;               /* Vertex normals */

    RwTexCoords             *texCoords[rwMAXTEXTURECOORDS]; /* Texture coordinates */

    RwRGBA                  *preLitLum;             /* Pre lit luminances */

    /* Pointer to memory allocated for vertex and poly pointers */
    RwResEntry              *repEntry;

    /* Atomics in this sectors */
    /* The pointers are frigged so they look like they are pointing to
       Atomics when they are pointing to here */
    RwLinkList              collAtomicsInWorldSector;       /* Coll priority */

    /* Lights in an atomic sector */
    RwLinkList              lightsInWorldSector;

    /* Outer bounding box of sector based on BSP planes */
    RwBBox                  boundingBox;

    /* Bounding box tightly enclosing geometry */
    RwBBox                  tightBoundingBox;

    /* The mesh which groups same material triangles together */
    RpMeshHeader            *mesh;

    /* The WorldSector object pipeline for this WorldSector */
    RxPipeline    *pipeline;

    /* Material list window base
     * (triangles in a given sector can "see"
     * the 256 materials from
     * MatList[matListWindowBase] -> MatList[matListWindowBase + 255])
     */
    RwUInt16                matListWindowBase;

    RwUInt16                numVertices;            /* Num vertices */
    RwUInt16                numTriangles;           /* Num triangles */
    RwUInt16                pad;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rpworldsector
 * \ref RpWorldSectorCallBack
 * represents the function called from \ref RpWorldForAllWorldSectors,
 * \ref RpAtomicForAllWorldSectors and \ref RpLightForAllWorldSectors for all
 * world sectors in a given world or world sectors a given atomic or light lies
 * in. This function should return a pointer to the current world sector to
 * indicate success. The callback may return NULL to terminate further
 * callbacks on the atomic or light.
 *
 * \param  sector   Pointer to the current world sector
 * \param  data     Pointer to developer-defined data structure.
 *
 * \return Pointer to the current world sector.
 *
 */
typedef RpWorldSector *(*RpWorldSectorCallBack)(RpWorldSector *worldSector, void *data);

typedef struct RpSector RpSector;

#if (!defined(DOXYGEN))
struct RpSector
{
    RwInt32                 type;
};

/* NOTE: The left and right pointers can point to an RpPlaneSector or
 *       an RpWorldSector
 * This is determined what the type is via the type value
 */

typedef struct RpPlaneSector RpPlaneSector;

struct RpPlaneSector
{
    RwInt32                 type;

    RwReal                  value;
    RpSector                *leftSubTree;   /* Sector 'left' (less) of the plane */
    RpSector                *rightSubTree;  /* Sector 'right' (more) of the plane */
    RwReal                  leftValue;
    RwReal                  rightValue;
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/world/balight.h ---*/

/*
 * Lighting 3D objects.
 * Lights are used to illuminate atomics and worlds
 *
 * Copyright (c) 1998 Criterion Software Ltd.
 */


/****************************************************************************
 Defines
 */

struct RpLightChunkInfo
{
    RwReal              radius; /**< radius */
    RwReal              red; /**< red */
    RwReal              green; /**< green */
    RwReal              blue; /**< blue */
    RwReal              minusCosAngle; /**< minusCosAngle */
    RwUInt32            typeAndFlags; /**< typeAndFlags */
};

/* Type ID */
#define rpLIGHT 3

/* Beyond this the lights must be positioned */
#define rpLIGHTPOSITIONINGSTART 0x80

/**
 * \ingroup rplight
 * \ref RpLightType are
 * light sub types.  This type represents the different
 * types of light source that can be created using the API function \ref RpLightCreate.
 * Note that lights of types rpLIGHTPOINT, rpLIGHTSPOT and rpLIGHTSPOTSOFT have linear
 * intensity fall-off with distance from the source, reducing to zero at the light's radius:*/
enum RpLightType
{
    rpNALIGHTTYPE = 0,

    /* These don't take part in the tie mechanism (no position) */
    rpLIGHTDIRECTIONAL, /**<Directional Light */
    rpLIGHTAMBIENT, /**<Ambient Light */

    /* These do take part in the tie mechanism (do have position) */
    rpLIGHTPOINT = rpLIGHTPOSITIONINGSTART, /**<Point Light */
    rpLIGHTSPOT, /**<Spot Light */
    rpLIGHTSPOTSOFT, /**<Soft Spot Light */
    rpLIGHTTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpLightType RpLightType;

#define rpLIGHTMINCONEANGLE ((RwReal)0.0f)

/**
 * \ingroup rplight
 * \ref RpLightFlag defines what geometry is influenced by the light.
 * The bit-field RpLightFlag specifies the options available for controlling the scope
 * of a light source (see API function \ref RpLightSetFlags):*/
enum RpLightFlag
{
    rpLIGHTLIGHTATOMICS = 0x01, /**<The light source illuminates all atomics in a world */
    rpLIGHTLIGHTWORLD = 0x02, /**<The light source illuminates all static geometry in a world */
    rpLIGHTFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpLightFlag RpLightFlag;

enum rpLightPrivateFlag
{
    rpLIGHTPRIVATENOCHROMA = 0x01,
    rpLIGHTPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum rpLightPrivateFlag rpLightPrivateFlag;

#if (!defined(DOXYGEN))
struct RpLight
{
    RwObjectHasFrame    object; /**< object */
    RwReal              radius; /**< radius */
    RwRGBAReal          color; /**< color */  /* Light color */
    RwReal              minusCosAngle; /**< minusCosAngle */
    RwLinkList          WorldSectorsInLight; /**< WorldSectorsInLight */
    RwLLLink            inWorld; /**< inWorld */
    RwUInt16            lightFrame; /**< lightFrame */
    RwUInt16            pad;
};
#endif /* (!defined(DOXYGEN)) */

/**
 * \ingroup rplight
 * \ref RpLightCallBack
 * represents the function called from \ref RpWorldForAllLights and
 * \ref RpWorld SectorForAllLights for all lights in a given world or world
 * sector. This function should return a pointer to the current light to
 * indicate success. The callback may return NULL to terminate further
 * callbacks on the world sector.
 *
 * \param  light  Pointer to the current light in the world
 *                sector.
 * \param  data   Pointer to developer-defined data structure.
 *
 * \return Pointer to the current light.
 */
typedef RpLight    *(*RpLightCallBack) (RpLight * light, void *data);

typedef struct RpLightTie RpLightTie;

/**
 * \ingroup rplight
 * \struct RpLightTie
 *
 * RpLightTie is a linked list of lights inside world sectors. These are
 * created when frame hierarchies are updated as part of an \ref RwCameraBeginUpdate.
 * This is used for determining which lights influence a world sector and the atomics inside it.
 * Creation and destruction of RpLightTies is internal to the world plugin.
 */
struct RpLightTie
{
    /* Information for an atomic sector */
    RwLLLink            lightInWorldSector; /**< Lights IN this ATOMIC SECTOR */
    RpLight            *light;              /**< A pointer to a light */

    /* Information for a atomic */
    RwLLLink            WorldSectorInLight; /**< Atomic sectors HOLDING this Light */
    RpWorldSector      *sect;               /**< A pointer to a world sector */
};

/****************************************************************************
 <macro/inline functionality
 */

#define RpLightGetRadiusMacro(_light)                       \
    ((_light)->radius)

#define RpLightGetColorMacro(_light)                        \
    (&((_light)->color))

#define RpLightSetFrameMacro(_light, _frame)                \
    (rwObjectHasFrameSetFrame((_light), (_frame)), (_light))

#define RpLightGetFrameMacro(_light)                        \
    ((RwFrame *)rwObjectGetParent((_light)))

#define RpLightGetTypeMacro(_light)                         \
    ((RpLightType)rwObjectGetSubType((_light)))

#define RpLightSetFlagsMacro(_light, _flags)                \
    ((rwObjectSetFlags((_light), (_flags))), (_light))

#define RpLightGetFlagsMacro(_light)                        \
    (rwObjectGetFlags((_light)))


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RpLightGetRadius(_light)                            \
    RpLightGetRadiusMacro(_light)

#define RpLightGetColor(_light)                             \
    RpLightGetColorMacro(_light)

#define RpLightSetFrame(_light, _frame)                     \
    RpLightSetFrameMacro(_light, _frame)

#define RpLightGetFrame(_light)                             \
    RpLightGetFrameMacro(_light)

#define RpLightGetType(_light)                              \
    RpLightGetTypeMacro(_light)

#define RpLightSetFlags(_light, _flags)                     \
    RpLightSetFlagsMacro(_light, _flags)

#define RpLightGetFlags(_light)                             \
    RpLightGetFlagsMacro(_light)

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */


/**
 * \ingroup rpmaterial
 * \ref RpMaterialCallBack
 * represents the function called from \ref RpGeometryForAllMaterials and
 * \ref RpWorldForAllMaterials for all materials referenced by polygons in a
 * given geometry. This function should return a pointer to the current
 * material to indicate success. The callback may return NULL to terminate
 * further callbacks on the materials.
 *
 * \param  material   Pointer to the current material
 * \param  data       Pointer to developer-defined data structure.
 *
 * \return Pointer to the current material.
 */
typedef RpMaterial *(*RpMaterialCallBack)(RpMaterial *material, void *data);

/*
 * World handling.
 * World give objects scope, and provide a mechanism for
 * efficiency static object rendering.
 *
 * Copyright (c) 1998 Criterion Software Ltd.
 *
 */

/****************************************************************************
 Defines
 */

#define RpAtomicGetRenderCallBackMacro(_atomic)                         \
    ((_atomic)->renderCallBack)

#define RpAtomicGetInterpolatorMacro(_atomic)                         \
    (&((_atomic)->interpolator))

#define RpInterpolatorGetStartMorphTargetMacro(_intrp)                  \
    ((_intrp)->startMorphTarget)

#define RpInterpolatorGetEndMorphTargetMacro(_intrp)                    \
    ((_intrp)->endMorphTarget)

#define RpInterpolatorGetValueMacro(_intrp)                             \
    ((_intrp)->position)

#define RpInterpolatorGetScaleMacro(_intrp)                             \
    ((_intrp)->time)

/* NB "RpInterpolatorSetStartMorphTarget(interp++, target)" will break it */
#define RpInterpolatorSetStartMorphTargetMacro(_intrp, _target, _atomic)\
    ((_intrp)->startMorphTarget = (RwInt16) (_target),                  \
     (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE |         \
                                  rpINTERPOLATORDIRTYSPHERE     ),      \
     ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))?                \
         ((RpAtomicGetFrame(_atomic))?                                  \
            (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))):          \
            (0)):                                                       \
         (0)),                                                          \
     (_intrp))

/* NB "RpInterpolatorSetEndMorphTarget(interp++, target)" will break it */
#define RpInterpolatorSetEndMorphTargetMacro(_intrp, _target, _atomic)  \
    ((_intrp)->endMorphTarget = (RwInt16) (_target),                    \
     (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE |         \
                                  rpINTERPOLATORDIRTYSPHERE     ),      \
     ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))?                \
         ((RpAtomicGetFrame(_atomic))?                                  \
            (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))):          \
            (0)):                                                       \
         (0)),                                                          \
     (_intrp))

/* NB "RpInterpolatorSetValue(interp++, value)" will break it */
#define RpInterpolatorSetValueMacro(_intrp, _value, _atomic)            \
    ((_intrp)->position = (_value),                                     \
     (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE |         \
                                  rpINTERPOLATORDIRTYSPHERE     ),      \
     ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))?                \
         ((RpAtomicGetFrame(_atomic))?                                  \
            (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))):          \
            (0)):                                                       \
         (0)),                                                          \
     (_intrp))

/* NB "RpInterpolatorSetScale(interp++, *(scale++))" will break it */
#define RpInterpolatorSetScaleMacro(_intrp, _scale, _atomic)            \
    ((_intrp)->time = (_scale),                                         \
     (_intrp)->recipTime = (RwReal) (1.0) / (_scale),                   \
     (_intrp)->flags |= (RwInt32)(rpINTERPOLATORDIRTYINSTANCE |         \
                                  rpINTERPOLATORDIRTYSPHERE     ),      \
     ((!((_intrp)->flags & rpINTERPOLATORNOFRAMEDIRTY))?                \
         ((RpAtomicGetFrame(_atomic))?                                  \
            (RwFrameUpdateObjects(RpAtomicGetFrame(_atomic))):          \
            (0)):                                                       \
         (0)),                                                          \
     (_intrp))

#define RpAtomicGetClumpMacro(_atomic)                                  \
    ((_atomic)->clump)

/* NB "RpAtomicGetBoundingSphere(atomic++)" will break it */
#define RpAtomicGetBoundingSphereMacro(_atomic)                         \
    ((((_atomic)->interpolator.flags & rpINTERPOLATORDIRTYSPHERE)?      \
      _rpAtomicResyncInterpolatedSphere(_atomic), 0: 0),                \
      &((_atomic)->boundingSphere))
#define RpAtomicGetFrameMacro(_atomic)                                  \
    ((RwFrame *) rwObjectGetParent(_atomic))

/* NB "RpClumpSetFrame(clump++, frame)" will break it */
#if (!defined(RpClumpSetFrameMacro))
#define RpClumpSetFrameMacro(_clump, _frame)                            \
    (rwObjectSetParent(_clump, _frame),                                 \
     (_clump))
#endif /* (!defined(RpClumpSetFrameMacro)) */

#if (!defined(RpClumpSetFrameVoidMacro))
#define RpClumpSetFrameVoidMacro(_clump, _frame)                        \
MACRO_START                                                             \
{                                                                       \
    rwObjectSetParent(_clump, _frame);                                  \
}                                                                       \
MACRO_STOP
#endif /* (!defined(RpClumpSetFrameVoidMacro)) */

#define RpClumpGetFrameMacro(_clump)                                    \
    ((RwFrame *) rwObjectGetParent(_clump))

/* NB "RpAtomicSetFlags(atomic++, flags)" will break it */
#if (!defined(RpAtomicSetFlagsMacro))
#define RpAtomicSetFlagsMacro(_atomic, _flags)                          \
    (rwObjectSetFlags(_atomic, _flags),                                 \
     (_atomic))
#endif /* (!defined(RpAtomicSetFlagsMacro)) */

#define RpAtomicGetFlagsMacro(_atomic)                                  \
    (rwObjectGetFlags(_atomic))

#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))

#define RpAtomicRender(_atomic) \
    RpAtomicRenderMacro(_atomic)

#define RpAtomicGetGeometry(_atomic) \
    RpAtomicGetGeometryMacro(_atomic)

#define RpAtomicSetRenderCallBack(_atomic, _callback) \
    RpAtomicSetRenderCallBackMacro(_atomic, _callback)

#define RpAtomicGetRenderCallBack(_atomic) \
    RpAtomicGetRenderCallBackMacro(_atomic)

#define RpAtomicGetInterpolator(_atomic) \
    RpAtomicGetInterpolatorMacro(_atomic)

#define RpInterpolatorGetStartMorphTarget(_intrp) \
    RpInterpolatorGetStartMorphTargetMacro(_intrp)

#define RpInterpolatorGetEndMorphTarget(_intrp) \
    RpInterpolatorGetEndMorphTargetMacro(_intrp)

#define RpInterpolatorGetValue(_intrp) \
    RpInterpolatorGetValueMacro(_intrp)

#define RpInterpolatorGetScale(_intrp) \
    RpInterpolatorGetScaleMacro(_intrp)

#define RpInterpolatorSetStartMorphTarget(_intrp, _target, _atomic) \
    RpInterpolatorSetStartMorphTargetMacro(_intrp, _target, _atomic)

#define RpInterpolatorSetEndMorphTarget(_intrp, _target, _atomic) \
    RpInterpolatorSetEndMorphTargetMacro(_intrp, _target, _atomic)

#define RpInterpolatorSetValue(_intrp, _value, _atomic) \
    RpInterpolatorSetValueMacro(_intrp, _value, _atomic)

#define RpInterpolatorSetScale(_intrp, _scale, _atomic) \
    RpInterpolatorSetScaleMacro(_intrp, _scale, _atomic)

#define RpAtomicGetClump(_atomic) \
    RpAtomicGetClumpMacro(_atomic)

#define RpAtomicGetBoundingSphere(_atomic) \
    RpAtomicGetBoundingSphereMacro(_atomic)

#define RpAtomicGetFrame(_atomic) \
    RpAtomicGetFrameMacro(_atomic)

#define RpClumpSetFrame(_clump, _frame) \
    RpClumpSetFrameMacro(_clump, _frame)

#define RpClumpGetFrame(_clump) \
    RpClumpGetFrameMacro(_clump)

#define RpAtomicSetFlags(_atomic, _flags) \
    RpAtomicSetFlagsMacro(_atomic, _flags)

#define RpAtomicGetFlags(_atomic) \
    RpAtomicGetFlagsMacro(_atomic)

#endif /* (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) )) */

/*
 * World handling.
 * World give objects scope, and provide a mechanism for
 * efficiency static object rendering.
 *
 * Copyright (c) 1998 Criterion Software Ltd.
 *
 */

/* Type ID */
#define rpWORLD 7

/* RpWorld private flags (in RwObject) */
enum RpWorldPrivateFlag
{
    rpWORLDSINGLEMALLOC = 0x01,
    rpWORLDPRIVATEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldPrivateFlag RpWorldPrivateFlag;

/**
 * \ingroup rpworldsub
 * The bit-field type \ref RpWorldFlag specifies the options available
 * for creating the static geometry component of a world (see API function \ref RpWorldSetFlags):
 */
enum RpWorldFlag
{
    rpWORLDTRISTRIP  = 0x01, /**<This world's meshes can be rendered
                                 as tri strips */
    rpWORLDPOSITIONS = 0x02, /**<This world has positions */
    rpWORLDTEXTURED  = 0x04, /**<This world has only one set of texture coordinates */
    rpWORLDPRELIT    = 0x08, /**<This world has luminance values */
    rpWORLDNORMALS   = 0x10, /**<This world has normals */
    rpWORLDLIGHT     = 0x20, /**<This world will be lit */
    rpWORLDMODULATEMATERIALCOLOR = 0x40,
    /**<Modulate material color with vertex colors
        (pre-lit + lit) */
    rpWORLDTEXTURED2 = 0x80, /**<This world has 2 or more sets of texture coordinates */

    /*
     * These above flags were stored in the flags field in an RwObject, they
     * are now stored in the flags file of the RpWorld.
     */

    rpWORLDNATIVE               = 0x01000000,
    rpWORLDNATIVEINSTANCE       = 0x02000000,

    rpWORLDFLAGSMASK            = 0x000000FF,
    rpWORLDNATIVEFLAGSMASK      = 0x0F000000,

    rpWORLDSECTORSOVERLAP       = 0x40000000,

    rpWORLDFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldFlag RpWorldFlag;

/* Multi texture coordinate format specifier */
#define rpWORLDTEXCOORDSETS(_num)  ((_num & 0xff) << 16)

/* Maximum depth of BSP tree */
#define rpWORLDMAXBSPDEPTH 64


typedef struct RpMesh RpMesh;

/**
 * \ingroup rpmesh
 * \struct RpMesh
 * This type represents a single polygon mesh.
 * A mesh is defined as a collection of triangles derived from an RpGeometry
 * or RpWorldSector which have a common material.
 *
 * See API functions \see RpGeometryForAllMeshes and
 * \see RpWorldSectorForAllMeshes and
 * the corresponding function callback types:
 */
struct RpMesh
{
    RxVertexIndex      *indices;    /**< vertex indices defining the mesh */
    RwUInt32            numIndices; /**< number of vertices in mesh */
    RpMaterial         *material;   /**< pointer to material used to
                                     *   render the mesh. */
};

/****************************************************************************
 Global types
 */

/**
 * \ingroup rpworldsub
 *
 * A pluginID is a unique identifiers for a plugin. This is a list of the IDs for
 * RpWorld plugin, which is not just one plugin, but a collection of plugins
 * that extend RenderWare Graphics Core. See \ref RwEngineRegisterPlugin
 *
 */
enum RwCriterionWorldID
{
    /* Guard value that should not be used. */
    rwID_NAWORLDID = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x00),

    /* The valid plugin IDs */
    /**< RpMaterial pluginID */
    rwID_MATERIALMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x01),
    /**< RpMesh pluginID */
    rwID_MESHMODULE          = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x02),
    /**< RpGeometry pluginID */
    rwID_GEOMETRYMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x03),
    /**< RpClump pluginID */
    rwID_CLUMPMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x04),
    /**< RpLight pluginID */
    rwID_LIGHTMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x05),
    /* Not used */
    rwID_COLLISIONMODULE     = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x06),
    /**< RpWorld pluginID */
    rwID_WORLDMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x07),
    /* Not used */
    rwID_RANDOMMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x08),
    /**< PluginID for RpWorld's objects */
    rwID_WORLDOBJMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x09),
    /**< RpWorldSector pluginID */
    rwID_SECTORMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0A),
    /**< Binary RpWorld pluginID */
    rwID_BINWORLDMODULE      = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0B),
    /**< RpWorld pipeline pluginID */
    rwID_WORLDPIPEMODULE     = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0D),
    /**< Binary RpMesh pluginID */
    rwID_BINMESHPLUGIN       = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0E),
    /**< RpWorld device pluginID */
    rwID_RXWORLDDEVICEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x0F),
    /**< PluginID for platform specific serialization data */
    rwID_NATIVEDATAPLUGIN    = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x10),
    /**< \if xbox Vertex format pluginID \endif */
    /**< \if gcn  Vertex format pluginID \endif */
    rwID_VERTEXFMTPLUGIN     = MAKECHUNKID(rwVENDORID_CRITERIONWORLD, 0x11),
    rwCRITERIONWORLDIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionWorldID RwCriterionWorldID;


/**
 * \ingroup rpworldsub
 * \ref RpWorldRenderOrder
 * represents the options available for
 * the rendering order of world sectors in the camera's view frustum (see
 * API function \ref RpWorldSetRenderOrder).
 */
enum RpWorldRenderOrder
{
    rpWORLDRENDERNARENDERORDER = 0,
    rpWORLDRENDERFRONT2BACK,  /**<Renders nearest sectors first */
    rpWORLDRENDERBACK2FRONT, /**<Renders furthest sectors first */
    rpWORLDRENDERORDERFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpWorldRenderOrder RpWorldRenderOrder;

/**
 * \ingroup rpworldsector
 * \ref RpWorldSectorCallBackRender represents the function called from
 * \ref RpWorldSectorRender when the specified world sector lies inside the
 * current camera's view frustum. The default callback initiates execution of
 * the world sector rendering pipeline. This function should return a pointer
 * to the world sector to indicate success
 *
 * \param  worldSector   Pointer to the world sector to be
 *                       rendered
 *
 * \return Pointer to the world sector.
 *
 */
typedef RpWorldSector *(*RpWorldSectorCallBackRender) (RpWorldSector *worldSector);

/**
 * \ingroup rpworldsub
 * \struct RpWorld
 * World object. This should be considered an opaque type.
 * Use the RpWorld API functions to access.
 */
typedef struct RpWorld RpWorld;

#if (!defined(DOXYGEN))
struct RpWorld
{
    RwObject            object;

    RwUInt32            flags;

    RpWorldRenderOrder  renderOrder;

    /* Materials */
    RpMaterialList      matList;

    /* The world stored as a BSP tree */
    RpSector           *rootSector;

    /* The number of texture coordinate sets in each sector */
    RwInt32             numTexCoordSets;

    /* Render frame used when last rendered */
    RwInt32             numClumpsInWorld;
    RwLLLink           *currentClumpLink;

    /* All the clumps in the world */
    RwLinkList          clumpList;

    /* All of the lights in the world */
    RwLinkList          lightList;

    /* Directional lights in the world */
    RwLinkList          directionalLightList;

    /* The worlds origin offset */
    RwV3d               worldOrigin;

    /* Bounding box around the whole world */
    RwBBox              boundingBox;

    /* The callbacks functions */
    RpWorldSectorCallBackRender renderCallBack;

    RxPipeline         *pipeline;
};
#endif /* (!defined(DOXYGEN)) */

RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* data);
RpGeometry* RpGeometryForAllMaterials(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* data);
RwBool RpClumpDestroy(RpClump* clump);
RpClump* RpClumpRender(RpClump* clump);
RpLight* RpLightCreate(RwInt32 type);
RwBool RpLightDestroy(RpLight* light);
RpWorld* RpWorldCreate(RwBBox* boundingBox);
RpWorld* RpWorldAddCamera(RpWorld* world, RwCamera* camera);
RpLight* RpLightSetColor(RpLight* light, const RwRGBAReal* color);
RpAtomic* AtomicDefaultRenderCallBack(RpAtomic* atomic);
RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light);
RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light);
RwBool RpAtomicDestroy(RpAtomic* atomic);
void RpClumpGtaCancelStream();