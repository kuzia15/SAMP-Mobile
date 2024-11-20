#pragma once

#include "rwplcore.h"


#define RwFrameGetParentMacro(_f)   ((RwFrame *)rwObjectGetParent(_f))
#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))
#define RwFrameGetParent(_f)    RwFrameGetParentMacro(_f)
#endif

#define RwFrameGetMatrixMacro(_f)   (&(_f)->modelling)
#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))
#define RwFrameGetMatrix(_f)    RwFrameGetMatrixMacro(_f)
#endif

#define RwIm3DVertexGetPos(_vert) (&((_vert)->objVertex))
#define RwIm3DVertexGetNormal(vert) (&((vert)->objNormal))


//-----------------------------------------------------------

#define RW_FRAME_NAME_LENGTH      23
struct RwListEntry
{
    RwListEntry* next, * prev;
};

struct RwList
{
    RwListEntry root;
};

struct RwFrame
{
    RwObject        object;                 // 0
    RwLLLink        inDirtyListLink;
    RwMatrix        modelling;              // 16
    RwMatrix        ltm;                    // 32
    RwLinkList      objects;                // 48
    struct RwFrame* child;                  // 56
    struct RwFrame* next;                   // 60
    struct RwFrame* root;                   // 64

//    // Rockstar Frame extension (0x253F2FE) (24 bytes)
//    char          pluginData[8];                               // padding
//    char          szName[RW_FRAME_NAME_LENGTH + 1];            // name (as stored in the frame extension)
};
static_assert(sizeof(RwFrame) == (VER_x32 ? 0xA4 : 0xC8));


#include "raster.h"

#define RwIm3DVertexSetRGBA(_vert, _r, _g, _b, _a) \
MACRO_START \
{ \
    ((_vert)->color = ((_a) << 24) | \
                              ((_r) << 16) | \
                              ((_g) << 8) | \
                              ((_b))); \
} \
MACRO_STOP

class TextureDatabaseEntry;

/* Us and Vs */
#define RxObjSpace3DLitVertexGetU(_vert) \
    ((_vert)->u)
#define RxObjSpace3DLitVertexGetV(_vert) \
    ((_vert)->v)
#define RxObjSpace3DLitVertexSetU(_vert, _imu) \
    ((_vert)->u = (_imu))
#define RxObjSpace3DLitVertexSetV(_vert, _imv) \
    ((_vert)->v = (_imv))

#define RwCameraGetFrameMacro(_camera)                          \
    ((RwFrame *)rwObjectGetParent((_camera)))

#define RwCameraSetRasterMacro(_camera, _raster)                \
    (((_camera)->frameBuffer = (_raster)), (_camera))

#define RwCameraGetRaster(_camera)                         \
    ((_camera)->frameBuffer)

#define RwRasterGetDepthMacro(_raster) ((_raster)->depth)
#define RwRasterGetDepth(_raster) RwRasterGetDepthMacro(_raster)

#define RwCameraGetNearClipPlaneMacro(_camera) ((_camera)->nearPlane)
#define RwCameraGetNearClipPlane(_camera) RwCameraGetNearClipPlaneMacro(_camera)

/* LEGACY-SUPPORT for old objvert names - NB does NOT guarantee the
 * app will work, because the old IM3DVERTEX macros are NOT correctly
 * abstracted - 'Get' will return pointers to RwV3ds inside the
 * ObjVert, but you can't assume there are any RwV3ds inside an
 * opaque vertex type */

#define RwIm3DVertexSetU   RxObjSpace3DLitVertexSetU
#define RwIm3DVertexSetV   RxObjSpace3DLitVertexSetV
#define RwIm3DVertexGetNext(_vert)      ((_vert) + 1)

/****************************************************************************
 <macro/inline functionality
 */

#define RwIm3DVertexSetPos(_vert, _imx, _imy, _imz) \
MACRO_START \
{ \
    (_vert)->objVertex.x = _imx; \
    (_vert)->objVertex.y = _imy; \
    (_vert)->objVertex.z = _imz; \
} \
MACRO_STOP

#define RwCameraSetFrameMacro(_camera, _frame)                  \
    (_rwObjectHasFrameSetFrame((_camera), (_frame)), (_camera))


#define RwCameraGetZRaster(_camera)                        \
    ((_camera)->zBuffer)

#define RwCameraSetZRaster(_camera, _raster)               \
    (((_camera)->zBuffer = (_raster)), (_camera))


#define RwTextureGetRasterMacro(_tex)                       \
    ((_tex)->raster)

#define RwCameraGetFarClipPlaneMacro(_camera)                   \
    ((_camera)->farPlane)

#define RwRasterGetWidthMacro(_raster) \
    ((_raster)->width)

#define RwRasterGetHeightMacro(_raster) \
    ((_raster)->height)

#define RwRasterGetStrideMacro(_raster) \
    ((_raster)->stride)

#define RwRasterGetDepthMacro(_raster) \
    ((_raster)->depth)

#define RwCameraSetFrame(_camera, _frame)                       \
    RwCameraSetFrameMacro(_camera, _frame)

#define RwCameraGetFrame(_camera)                               \
    RwCameraGetFrameMacro(_camera)

#define RwRasterGetFormatMacro(_raster) \
    ((((_raster)->cFormat) & (rwRASTERFORMATMASK >> 8)) << 8)

#define RwRasterGetTypeMacro(_raster) \
    (((_raster)->cType) & rwRASTERTYPEMASK)

#define RwRasterGetParentMacro(_raster) \
    ((_raster)->parent)

#define RwCameraSetRaster(_camera, _raster)                     \
    RwCameraSetRasterMacro(_camera, _raster)


#define RwRasterGetWidth(_raster)                   \
    RwRasterGetWidthMacro(_raster)

#define RwRasterGetHeight(_raster)                  \
    RwRasterGetHeightMacro(_raster)

#define RwRasterGetStride(_raster)                  \
    RwRasterGetStrideMacro(_raster)

#define RwRasterGetDepth(_raster)                   \
    RwRasterGetDepthMacro(_raster)

#define RwRasterGetFormat(_raster)                  \
    RwRasterGetFormatMacro(_raster)

#define RwRasterGetType(_raster)                  \
    RwRasterGetTypeMacro(_raster)

#define RwRasterGetParent(_raster)                  \
    RwRasterGetParentMacro(_raster)

#define RwCameraGetFarClipPlane(_camera)                        \
    RwCameraGetFarClipPlaneMacro(_camera)

#define RwTextureGetRaster(_tex)                            \
    RwTextureGetRasterMacro(_tex)

/****************************************************************************
 Defines
 */

/**
 * \ingroup rwimage
 * \struct RwImage 
 * Image containing device-independent pixels. 
 * This should be considered an opaque type.
 * Use the RwImage API functions to access.
 */
typedef struct RwImage RwImage;

struct RwImage
{
        RwInt32             flags;

        RwInt32             width;  /* Device may have different ideas */
        RwInt32             height; /* internally !! */

        RwInt32             depth;  /* Of referenced image */
        RwInt32             stride;

        RwUInt8            *cpPixels;
        RwRGBA             *palette;
};

#define RwImageSetStrideMacro(_image, _stride)      \
    (((_image)->stride = (_stride)), (_image))

#define RwImageSetPixelsMacro(_image, _pixels)      \
    (((_image)->cpPixels = (_pixels)), (_image))

#define RwImageSetPaletteMacro(_image, _palette)    \
    (((_image)->palette = (_palette)), (_image))

#define RwImageGetWidthMacro(_image)                \
    ((_image)->width)

#define RwImageGetHeightMacro(_image)               \
    ((_image)->height)

#define RwImageGetDepthMacro(_image)                \
    ((_image)->depth)

#define RwImageGetStrideMacro(_image)               \
    ((_image)->stride)

#define RwImageGetPixelsMacro(_image)               \
    ((_image)->cpPixels)

#define RwImageGetPaletteMacro(_image)              \
    ((_image)->palette)


#define RwImageSetStride(_image, _stride)           \
    RwImageSetStrideMacro(_image, _stride)

#define RwImageSetPixels(_image, _pixels)           \
    RwImageSetPixelsMacro(_image, _pixels)

#define RwImageSetPalette(_image, _palette)         \
    RwImageSetPaletteMacro(_image, _palette)

#define RwImageGetWidth(_image)                     \
    RwImageGetWidthMacro(_image)

#define RwImageGetHeight(_image)                    \
    RwImageGetHeightMacro(_image)

#define RwImageGetDepth(_image)                     \
    RwImageGetDepthMacro(_image)

#define RwImageGetStride(_image)                    \
    RwImageGetStrideMacro(_image)

#define RwImageGetPixels(_image)                    \
    RwImageGetPixelsMacro(_image)

#define RwImageGetPalette(_image)                   \
    RwImageGetPaletteMacro(_image)

extern RwImage* (*RwImageCreate)(RwInt32 width, RwInt32 height,
                                  RwInt32 depth);
extern RwBool (*RwImageDestroy)(RwImage * image);

    /* Allocating */
extern RwImage* (*RwImageAllocatePixels)(RwImage * image);
extern RwImage* (*RwImageFreePixels)(RwImage * image);

    /* Converting images */
extern RwImage* (*RwImageCopy)(RwImage * destImage,
                                const RwImage * sourceImage);

    /* Resizing images */
extern RwImage* (*RwImageResize)(RwImage * image, RwInt32 width,
                                  RwInt32 height);

    /* Producing masks ! */
extern RwImage* (*RwImageApplyMask)(RwImage * image,
                                     const RwImage * mask);
extern RwImage* (*RwImageMakeMask)(RwImage * image);

    /* Helper functions */
extern RwImage* (*RwImageReadMaskedImage)(const RwChar * imageName,
                                           const RwChar * maskname);
extern RwImage* (*RwImageRead)(const RwChar * imageName);
extern RwImage* (*RwImageWrite)(RwImage * image,
                                 const RwChar * imageName);

/****************************************************************************
 Global types
 */

typedef struct RwBBox RwBBox;

struct RwBBox
{
    /* Must be in this order */
    RwV3d sup;   /**< Supremum vertex. */
    RwV3d inf;   /**< Infimum vertex. */
};

/****************************************************************************
 Function prototypes
 */
 /* Images from rasters */
extern RwImage* (*RwImageSetFromRaster)(RwImage *image, RwRaster *raster);

/* Rasters from images */
extern RwRaster* (*RwRasterSetFromImage)(RwRaster *raster, RwImage *image);

/* Read a raster */
extern RwRaster* (*RwRasterRead)(const RwChar *filename);
extern RwRaster* (*RwRasterReadMaskedRaster)(const RwChar *filename, const RwChar *maskname);

/* Finding appropriate raster formats */
extern RwImage* (*RwImageFindRasterFormat)(RwImage *ipImage,RwInt32 nRasterType,
                                        RwInt32 *npWidth,RwInt32 *npHeight,
                                        RwInt32 *npDepth,RwInt32 *npFormat);


#define RwFrameGetParentMacro(_f)   ((RwFrame *)rwObjectGetParent(_f))
#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))
#define RwFrameGetParent(_f)    RwFrameGetParentMacro(_f)
#endif

#define RwFrameGetMatrixMacro(_f)   (&(_f)->modelling)
#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))
#define RwFrameGetMatrix(_f)    RwFrameGetMatrixMacro(_f)
#endif

/*--- Automatically derived from: C:/daily/rwsdk/src/batypehf.h ---*/

typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);

#if (!defined(DOXYGEN))
struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};
#endif /* (!defined(DOXYGEN)) */
static_assert(sizeof(RwObjectHasFrame) == (VER_x32 ? 0x14 : 0x28));

/* Frames */
extern void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame);
extern void _rwObjectHasFrameReleaseFrame(void *object);

/* ObjectHASFRAME METHODS */
#define rwObjectHasFrameInitialize(o, type, subtype, syncFunc)  \
MACRO_START                                                     \
{                                                               \
    rwObjectInitialize(o, type, subtype);                       \
    ((RwObjectHasFrame *)o)->sync = syncFunc;                   \
}                                                               \
MACRO_STOP

#define rwObjectHasFrameSync(o) \
    ((RwObjectHasFrame *)(o))->sync(o)

/* Compatibility macros */

#define rwObjectHasFrameSetFrame(object, frame) \
        _rwObjectHasFrameSetFrame(object, frame)
#define rwObjectHasFrameReleaseFrame(object) \
        _rwObjectHasFrameReleaseFrame(object)

/*--- Automatically derived from: C:/daily/rwsdk/src/basync.h ---*/


/*
 * Object Types - these are used in the binary object
 * representations and in the debug library. They must
 * be unique.  They are the old system.
 */

#define rwID_DATABASE 0x64617462     /* datb */

#define MAKECHUNKID(vendorID, chunkID) (((vendorID & 0xFFFFFF) << 8) | (chunkID & 0xFF))
#define GETOBJECTID(chunkID) (chunkID & 0xFF)
#define GETVENDORID(chunkID) ((chunkID >> 8) & 0xFFFFFF)

/***
 *** These are the vendor IDs.  A customer must reserve a vendor ID in order
 *** to be able to write toolkits (this prevents clashes between toolkits).
 *** We reserve some for our own use as shown below.  These are all 24 bit.
 ***
 *** IMPORTANT NOTE: DO NOT UNDER ANY CIRCUMSTANCES CHANGE THESE VALUES. IF
 ***                 YOU ARE ADDING A NEW ONE, APPEND IT!
 ***
 *** They must all be unique.
 ***/

enum RwPluginVendor
{
    rwVENDORID_CORE             = 0x000000L,
    rwVENDORID_CRITERIONTK      = 0x000001L,
    rwVENDORID_REDLINERACER     = 0x000002L,
    rwVENDORID_CSLRD            = 0x000003L,
    rwVENDORID_CRITERIONINT     = 0x000004L,
    rwVENDORID_CRITERIONWORLD   = 0x000005L,
    rwVENDORID_BETA             = 0x000006L,
    rwVENDORID_CRITERIONRM      = 0x000007L,
    rwVENDORID_CRITERIONRWA     = 0x000008L, /* RenderWare Audio */
    rwVENDORID_CRITERIONRWP     = 0x000009L, /* RenderWare Physics */
    rwPLUGINVENDORFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPluginVendor RwPluginVendor;

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwcamera
 * RwCameraClearMode
 * Camera clear flags */
enum RwCameraClearMode
{
    rwCAMERACLEARIMAGE = 0x1,   /**<Clear the frame buffer */
    rwCAMERACLEARZ = 0x2,       /**<Clear the Z buffer */
    rwCAMERACLEARSTENCIL = 0x4, /**<\if xbox   Clear the stencil buffer \endif
                                  * \if d3d8   Clear the stencil buffer \endif
                                  * \if d3d9   Clear the stencil buffer \endif
                                  * \if opengl Clear the stencil buffer \endif
                                  */
    rwCAMERACLEARMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCameraClearMode RwCameraClearMode;

/**
 * \ingroup rwcamera
 * RwCameraProjection
 * This type represents the options available for
 * setting the camera projection model, either perspective projection or
* parallel projection (see API function \ref RwCameraSetProjection)*/
enum RwCameraProjection
{
    rwNACAMERAPROJECTION = 0,   /**<Invalid projection */
    rwPERSPECTIVE = 1,          /**<Perspective projection */
    rwPARALLEL = 2,             /**<Parallel projection */
    rwCAMERAPROJECTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCameraProjection RwCameraProjection;

/**
 * \ingroup rwcamera
 * RwFrustumTestResult
 * This type represents the results from a
 * camera frustum test on a given sphere (see API function
 * \ref RwCameraFrustumTestSphere)*/
enum RwFrustumTestResult
{
    rwSPHEREOUTSIDE = 0,    /**<Outside the frustum */
    rwSPHEREBOUNDARY = 1,   /**<On the boundary of the frustum */
    rwSPHEREINSIDE = 2,     /**<Inside the frustum */
    rwFRUSTUMTESTRESULTFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwFrustumTestResult RwFrustumTestResult;


/**
 * \ingroup rwcamera
 * \struct RwCamera
 * Camera object for rendering a view.
 * This should be considered an opaque type.
 * Use the RwCamera API functions to access.
 */
typedef struct RwCamera RWALIGN(RwCamera, rwMATRIXALIGNMENT);

/* This allows a world to overload functionality */
typedef RwCamera   *(*RwCameraBeginUpdateFunc) (RwCamera * camera);
typedef RwCamera   *(*RwCameraEndUpdateFunc) (RwCamera * camera);

typedef struct RwFrustumPlane RwFrustumPlane;

#if (!defined(DOXYGEN))
/*
 * Structure describing a frustrum plane.
 */
struct RwFrustumPlane
{
    RwPlane             plane;
    RwUInt8             closestX;
    RwUInt8             closestY;
    RwUInt8             closestZ;
    RwUInt8             pad;
};
static_assert(sizeof(RwFrustumPlane) == (VER_x32 ? 0x14 : 0x14));

struct RwCamera
{
    RwObjectHasFrame    object;

    /* Parallel or perspective projection */
    RwCameraProjection  projectionType;

    /* Start/end update functions */
    RwCameraBeginUpdateFunc beginUpdate;
    RwCameraEndUpdateFunc endUpdate;

    /* The view matrix */
    RwMatrix            viewMatrix;

    /* The cameras image buffer */
    RwRaster           *frameBuffer;

    /* The Z buffer */
    RwRaster           *zBuffer;

    /* Cameras mathmatical characteristics */
    RwV2d               viewWindow;
    RwV2d               recipViewWindow;
    RwV2d               viewOffset;
    RwReal              nearPlane;
    RwReal              farPlane;
    RwReal              fogPlane;

    /* Transformation to turn camera z or 1/z into a Z buffer z */
    RwReal              zScale, zShift;

    RwUInt16            renderFrame;
    RwUInt16            pad;
    /* The clip-planes making up the viewing frustum */
    RwFrustumPlane      frustumPlanes[6];
    RwBBox              frustumBoundBox;

    /* Points on the tips of the view frustum */
    RwV3d               frustumCorners[8];
};
#endif /* (!defined(DOXYGEN)) */
static_assert(sizeof(RwCamera) == (VER_x32 ? 0x188 : 0x1B0));

/**
 * \ingroup rwcamera
 * \ref RwCameraCallBack type represents a function called from any camera
 * iterator that may be implemented in plugins. This function should return a
 * pointer to the current camera to indicate success. The callback may return
 * NULL to terminate further callbacks on other cameras.
 *
 * \param  camera   Pointer to the current camera, supplied by iterator.
 * \param  data  Pointer to developer-defined data structure.
 *
 * \return Pointer to the current camera, or NULL If not found.
 */
typedef RwCamera *(*RwCameraCallBack)(RwCamera *camera, void *data);

/*--- Automatically derived from: C:/daily/rwsdk/src/bacamval.h ---*/


extern RwCamera*    (*RwCameraBeginUpdate)(RwCamera * camera);
extern RwCamera*    (*RwCameraShowRaster)(RwCamera * camera, void *pDev, RwUInt32 flags);

#define rwTEXTUREBASENAMELENGTH     32

struct RwTexture
{
    RwRaster           *raster; /** pointer to RwRaster with data */                      //+0
    void               *dict;   /* Dictionary this texture is in */                       //+4
    RwLLLink            lInDictionary; /* List of textures in this dictionary */          //+8

    RwChar              name[rwTEXTUREBASENAMELENGTH];  /* Name of the texture */         //+16
    RwChar              mask[rwTEXTUREBASENAMELENGTH];  /* Name of the textures mask */   //+48

    /* 31 [xxxxxxxx xxxxxxxx vvvvuuuu ffffffff] 0 */
    RwUInt32            filterAddressing; /* Filtering & addressing mode flags */         //+80

    RwInt32             refCount; /* Reference count, surprisingly enough */              //+84
};
static_assert(sizeof(RwTexture) == (VER_x32 ? 0x58 : 0x68));

/* Type ID */
#define rwTEXDICTIONARY 6

/* Mipmap Name generation - maximum number of RwChar characters which can
 * be appended to the root name.
 */
#define rwTEXTUREMIPMAPNAMECHARS    16

/* We define texture names to be a maximum of 16 ISO chars */
#define rwTEXTUREBASENAMELENGTH     32

#define rwTEXTUREFILTERMODEMASK     0x000000FF
#define rwTEXTUREADDRESSINGUMASK    0x00000F00
#define rwTEXTUREADDRESSINGVMASK    0x0000F000
#define rwTEXTUREADDRESSINGMASK     (rwTEXTUREADDRESSINGUMASK |  \
                                     rwTEXTUREADDRESSINGVMASK)

/**
 * \ingroup rwtexture
 * \ref RwTextureCallBackRead
 * represents the function used by \ref RwTextureRead to read the specified
 * texture from a disk file. This function should return a pointer to the
 * texture to indicate success.
 *
 * \param  name   Pointer to a string containing the name of
 * the texture to read.
 *
 * \param  maskName   Pointer to a string containing the name
 * of the mask to read and apply to the texture.
 *
 * \return Pointer to the texture
 *
 * \see RwTextureSetReadCallBack
 * \see RwTextureGetReadCallBack
 */
typedef RwTexture *(*RwTextureCallBackRead)(const RwChar *name,
                                            const RwChar *maskName);

/**
 * \ingroup rwtexture
 * \ref RwTextureCallBackFind
 * represents the function used by \ref RwTextureRead to search for a
 * texture in memory before attempting to read one from disk. This
 * may involve searching previously loaded texture dictionaries.
 *
 * \param  name   Pointer to a string containing the name of
 * the texture to find.
 *
 * \return Pointer to the texture, or NULL if not found.
 *
 * \see RwTextureSetFindCallBack
 * \see RwTextureGetFindCallBack
 */
typedef RwTexture *(*RwTextureCallBackFind)(const RwChar *name);

/**
 * \ingroup rwtexture
 * \ref RwTextureCallBack
 * represents the function called from \ref RwTexDictionaryForAllTextures
 * for all textures in a given texture dictionary. This function should
 * return the current texture to indicate success. The callback may return
 * NULL to terminate further callbacks on the texture dictionary.
 *
 * \param  texture   Pointer to the current texture.
 *
 * \param  data   User-defined data pointer.
 *
 * \return Pointer to the current texture
 *
 * \see RwTexDictionaryForAllTextures
 */
typedef RwTexture *(*RwTextureCallBack)(RwTexture *texture, void *data);


/**
 * \ingroup rwtexture
 * \ref RwTextureCallBackMipmapGeneration
 * is the callback function supplied to \ref RwTextureSetMipmapGenerationCallBack
 * and returned from \ref RwTextureGetMipmapGenerationCallBack.
 *
 * The supplied function will be passed a pointer to a raster and an image.
 * The raster is the target for the generated mipmap levels and the image
 * provides the base for their generation.
 *
 * \param  raster   Pointer to raster, the target for generated mipmap levels
 * \param  image    Pointer to image, used to generate mipmap levels.
 *
 * \return
 * Returns a pointer to the raster if successful or NULL if an error occurred.
 *
 * \see RwTextureSetMipmapGenerationCallBack
 * \see RwTextureGetMipmapGenerationCallBack
 * \see RwTextureSetAutoMipmapping
 * \see RwTextureGetAutoMipmapping
 */
typedef RwRaster *(*RwTextureCallBackMipmapGeneration)(RwRaster * raster,
                                                       RwImage * image);

/**
 * \ingroup rwtexture
 * \ref RwTextureCallBackMipmapName
 * is the callback function supplied to \ref RwTextureSetMipmapNameCallBack and
 * returned from \ref RwTextureGetMipmapNameCallBack.
 *
 * The supplied function will be passed a pointer to a root name, a maskName, a mipmap
 * level and a format. The function returns TRUE if successful and the root name will have been
 * modified to equal the mipmap name.
 *
 * \param  name       Pointer to a string containing the root name of the texture. The
 * mipmap level name is put here.
 * \param  maskName   Pointer to a string containing the root mask name of the texture or
 * NULL if no mask name is required.
 * \param  mipLevel   A value equal to the mipmap level for which the name is required.
 * \param  format     A value describing the mipmapping mode. A combination of the bit
 * flags rwRASTERFORMATMIPMAP and rwRASTERFORMATAUTOMIPMAP.
 *
 * \return
 * Returns TRUE if the name is generated successfully or FALSE if an error occurred.
 *
 * \see RwTextureGenerateMipmapName
 * \see RwTextureSetMipmapNameCallBack
 * \see RwTextureGetMipmapNameCallBack
 * \see RwTextureSetAutoMipmapping
 * \see RwTextureGetAutoMipmapping
 */
typedef RwBool (*RwTextureCallBackMipmapName)(RwChar *name,
                                              RwChar *maskName,
                                              RwUInt8 mipLevel,
                                              RwInt32 format);

/****************************************************************************
 <macro/inline functionality
 */

#define RwTextureGetRasterMacro(_tex)                       \
    ((_tex)->raster)

#define RwTextureAddRefMacro(_tex)                          \
    (((_tex)->refCount++), (_tex))

#define RwTextureAddRefVoidMacro(_tex)                      \
MACRO_START                                                 \
{                                                           \
    (_tex)->refCount++;                                     \
}                                                           \
MACRO_STOP

#define RwTextureGetNameMacro(_tex)                         \
    ((_tex)->name)

#define RwTextureGetMaskNameMacro(_tex)                     \
    ((_tex)->mask)

#define RwTextureGetDictionaryMacro(_tex)                   \
    ((_tex)->dict)

#define RwTextureSetFilterModeMacro(_tex, _filtering)                       \
    (((_tex)->filterAddressing =                                            \
      ((_tex)->filterAddressing & ~rwTEXTUREFILTERMODEMASK) |               \
      (((RwUInt32)(_filtering)) &  rwTEXTUREFILTERMODEMASK)),               \
     (_tex))

#define RwTextureGetFilterModeMacro(_tex)                                   \
    ((RwTextureFilterMode)((_tex)->filterAddressing &                       \
                           rwTEXTUREFILTERMODEMASK))

#define RwTextureSetAddressingMacro(_tex, _addressing)                      \
    (((_tex)->filterAddressing =                                            \
      ((_tex)->filterAddressing & ~rwTEXTUREADDRESSINGMASK) |               \
      (((((RwUInt32)(_addressing)) <<  8) & rwTEXTUREADDRESSINGUMASK) |     \
       ((((RwUInt32)(_addressing)) << 12) & rwTEXTUREADDRESSINGVMASK))),    \
     (_tex))

#define RwTextureSetAddressingUMacro(_tex, _addressing)                     \
    (((_tex)->filterAddressing =                                            \
      ((_tex)->filterAddressing & ~rwTEXTUREADDRESSINGUMASK) |              \
      (((RwUInt32)(_addressing) << 8) & rwTEXTUREADDRESSINGUMASK)),         \
     (_tex))

#define RwTextureSetAddressingVMacro(_tex, _addressing)                     \
    (((_tex)->filterAddressing =                                            \
      ((_tex)->filterAddressing & ~rwTEXTUREADDRESSINGVMASK) |              \
      (((RwUInt32)(_addressing) << 12) & rwTEXTUREADDRESSINGVMASK)),        \
     (_tex))

#define RwTextureGetAddressingMacro(_tex)                                   \
    (((((_tex)->filterAddressing & rwTEXTUREADDRESSINGUMASK) >>  8) ==      \
      (((_tex)->filterAddressing & rwTEXTUREADDRESSINGVMASK) >> 12)) ?      \
     ((RwTextureAddressMode)(((_tex)->filterAddressing &                    \
                              rwTEXTUREADDRESSINGVMASK) >> 12)) :           \
     rwTEXTUREADDRESSNATEXTUREADDRESS)

#define RwTextureGetAddressingUMacro(_tex)                                  \
    ((RwTextureAddressMode)(((_tex)->filterAddressing &                     \
                             rwTEXTUREADDRESSINGUMASK) >> 8))

#define RwTextureGetAddressingVMacro(_tex)                                  \
    ((RwTextureAddressMode)(((_tex)->filterAddressing &                     \
                             rwTEXTUREADDRESSINGVMASK) >> 12))


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

#define RwTextureGetRaster(_tex)                            \
    RwTextureGetRasterMacro(_tex)

#define RwTextureAddRef(_tex)                               \
    RwTextureAddRefMacro(_tex)

#define RwTextureGetName(_tex)                              \
    RwTextureGetNameMacro(_tex)

#define RwTextureGetMaskName(_tex)                          \
    RwTextureGetMaskNameMacro(_tex)

#define RwTextureGetDictionary(_tex)                        \
    RwTextureGetDictionaryMacro(_tex)

#define RwTextureSetFilterMode(_tex, _filtering)            \
    RwTextureSetFilterModeMacro(_tex, _filtering)

#define RwTextureGetFilterMode(_tex)                        \
    RwTextureGetFilterModeMacro(_tex)

#define RwTextureSetAddressing(_tex, _addressing)           \
    RwTextureSetAddressingMacro(_tex, _addressing)

#define RwTextureSetAddressingU(_tex, _addressing)          \
    RwTextureSetAddressingUMacro(_tex, _addressing)

#define RwTextureSetAddressingV(_tex, _addressing)          \
    RwTextureSetAddressingVMacro(_tex, _addressing)

#define RwTextureGetAddressing(_tex)                        \
    RwTextureGetAddressingMacro(_tex)

#define RwTextureGetAddressingU(_tex)                       \
    RwTextureGetAddressingUMacro(_tex)

#define RwTextureGetAddressingV(_tex)                       \
    RwTextureGetAddressingVMacro(_tex)

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */


/* CamNorms.csl */
/**
 * \ingroup rwcoregeneric
 * \ref RxCamNorm
 * typedef for \ref RwV3d used by the RxClVStep cluster */
typedef RwV3d RxCamNorm;

/*--- Automatically derived from: C:/daily/rwsdk/src/pipe/p2/baim3d.h ---*/

/**
 * \ingroup rwim3d
 * RwIm3DTransformFlags
 *  The bit-field type  RwIm3DTransformFlags
 * specifies options available for controlling execution of the 3D immediate
 * mode pipeline (see API function \ref RwIm3DTransform):*/
enum RwIm3DTransformFlags
{
    rwIM3D_VERTEXUV      = 1,   /**<Texture coordinates in source vertices should be used */
    rwIM3D_ALLOPAQUE     = 2,   /**<All source vertices are opaque (alpha is 255) */
    rwIM3D_NOCLIP        = 4,   /**<No clipping should be performed on the geometry (the
                                 * app may know it is all onscreen or within the guard band clipping
                                 * region for the current hardware, so clipping can be skipped) */
    rwIM3D_VERTEXXYZ     = 8,   /**<Vertex positions */
    rwIM3D_VERTEXRGBA    = 16,  /**<Vertex color */

    rwIM3DTRANSFORMFLAGSFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwIm3DTransformFlags RwIm3DTransformFlags;

/****************************************************************************
 Global Types
 */

/* We use D3D9 formats for the instanced versions, to allow hardware T&L */

/*
 * Typedef for an RxObjSpace3DVertex.
 */
typedef struct RxObjSpace3DVertex RxObjSpace3DVertex;

/**
 * \ingroup cored3d9
 * \struct RxObjSpace3DVertex
 * Structure representing object space vertex.
 */
struct RxObjSpace3DVertex
{
    RwV3d       objVertex;        /**< position */
    RwV3d       objNormal;        /**< normal */
    RwUInt32    color;            /**< emissive color*/
    RwReal      u;                /**< u */
    RwReal      v;                /**< v */
};
static_assert(sizeof(RxObjSpace3DVertex) == 0x24);

/* This vertex is non truncatable */
#define RxObjSpace3DVertexNoUVsNoNormalsSize (sizeof(RxObjSpace3DVertex))
#define RxObjSpace3DVertexNoUVsSize          (sizeof(RxObjSpace3DVertex))
#define RxObjSpace3DVertexFullSize           (sizeof(RxObjSpace3DVertex))

/**
 * \ingroup cored3d9
 * \ref RxObjSpace3DLitVertex
 * Typedef for an RxObjSpace3DLitVertex.
 */
typedef RxObjSpace3DVertex RxObjSpace3DLitVertex;

/**
 * \ingroup cored3d9
 * \ref RwIm3DVertex
 * Typedef for an RwIm3DVertex.
 */
typedef RxObjSpace3DLitVertex RwIm3DVertex;

/* LEGACY-SUPPORT macro */
/**
 * \ingroup cored3d9
 * \ref RxScrSpace2DVertex
 * Typedef for an RxScrSpace2DVertex structure
 */
typedef RwIm2DVertex RxScrSpace2DVertex;

/****************************************************************************
 Object-space 3D unlit vertex macros
 */

/* Vertex positions */
#define RxObjSpace3DVertexGetPos(_vert, _pos) \
    (*(_pos) = (_vert)->objVertex)
#define RxObjSpace3DVertexSetPos(_vert, _pos) \
    ((_vert)->objVertex = *(_pos))

/* Pre-lighting colours */
#define RxObjSpace3DVertexGetPreLitColor(_vert, _col) \
MACRO_START \
{ \
    (_col)->alpha = (RwUInt8)((_vert)->color >> 24) & 0xFF; \
    (_col)->red   = (RwUInt8)((_vert)->color >> 16) & 0xFF; \
    (_col)->green = (RwUInt8)((_vert)->color >>  8) & 0xFF; \
    (_col)->blue  = (RwUInt8)((_vert)->color      ) & 0xFF; \
} \
MACRO_STOP

#define RxObjSpace3DVertexSetPreLitColor(_vert, _col) \
    ((_vert)->color = (((RwUInt32)(_col)->alpha) << 24) | \
                              (((RwUInt32)(_col)->red)   << 16) | \
                              (((RwUInt32)(_col)->green) <<  8) | \
                              (((RwUInt32)(_col)->blue)       ))

/* This uses the same slot as color (they are mutually exclusive) */
#define RxObjSpace3DVertexGetColor RxObjSpace3DVertexGetPreLitColor

/* Normals */
#define RxObjSpace3DVertexGetNormal(_vert, _normal)             \
    (*(_normal) = (_vert)->objNormal)
#define RxObjSpace3DVertexSetNormal(_vert, _normal)             \
    ((_vert)->objNormal = *(_normal))

/* Us and Vs */
#define RxObjSpace3DVertexGetU(_vert)                          \
    ((_vert)->u)
#define RxObjSpace3DVertexGetV(_vert)                          \
    ((_vert)->v)
#define RxObjSpace3DVertexSetU(_vert, _imu)                     \
    ((_vert)->u = (_imu))
#define RxObjSpace3DVertexSetV(_vert, _imv)                     \
    ((_vert)->v = (_imv))

RwFrame* RwFrameUpdateObjects(RwFrame* frame);
RwTexture* RwTextureCreate(RwRaster* raster);
RwCamera* RwCameraCreate();
RwFrame* RwFrameCreate();
RwCamera* RwCameraClear(RwCamera* camera, RwRGBA* colour, RwInt32 clearMode);
RwCamera* RwCameraSetNearClipPlane(RwCamera* camera, RwReal nearClip);
RwCamera* RwCameraSetFarClipPlane(RwCamera* camera, RwReal farClip);
RwFrame* RwFrameTranslate(RwFrame* frame, const RwV3d* v, RwOpCombineType combine);
RwFrame* RwFrameRotate(RwFrame* frame, const RwV3d* axis, RwReal angle, RwOpCombineType combine);
RwCamera* RwCameraSetViewWindow(RwCamera* camera, const RwV2d* viewWindow);
RwCamera* RwCameraSetProjection(RwCamera* camera, RwCameraProjection projection);
RwMatrix* RwFrameGetLTM(RwFrame* frame);
RwCamera* RwCameraEndUpdate(RwCamera* camera);
RwBool RwIm3DEnd();
RwBool RwIm3DRenderPrimitive(RwPrimitiveType primType);
RwBool RwIm3DRenderIndexedPrimitive(RwPrimitiveType primType, RwImVertexIndex* indices, RwInt32 numIndices);
void* RwIm3DTransform(RwIm3DVertex* pVerts, RwUInt32 numVerts, RwMatrix* ltm, RwUInt32 flags);
RwTexture* RwTextureRead(const char* name, const char* maskName);
RwFrame* RwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, void* data);
RwBool RwFrameDestroy(RwFrame* frame);
RwTexture* RwTextureSetRaster(RwTexture* texture, RwRaster* raster);
RwBool RwCameraDestroy(RwCamera* camera);
RwBool RwIm3DRenderLine(RwInt32 vert1, RwInt32 vert2);
RwFrame* RwFrameOrthoNormalize(RwFrame* frame);
RwTexture* RwTextureSetName(RwTexture* texture, const RwChar* name);
RwBool RwTextureSetFindCallBack(RwTextureCallBackFind callBack);
RwBool RwTextureSetReadCallBack(RwTextureCallBackRead callBack);