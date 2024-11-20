#pragma once

#include "memory.h"
#include "stream.h"
#include "color.h"
#include "tkreg.h"

/* If this bit is set then the image has been allocated by the user */

enum RwImageFlag
{
    rwNAIMAGEFLAG = 0x00,
    rwIMAGEALLOCATED = 0x1,
    rwIMAGEGAMMACORRECTED = 0x2,
    rwIMAGEFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwImageFlag RwImageFlag;

/****************************************************************************
 Global Types
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
static_assert(sizeof(RwImage) == (VER_x32 ? 0x1C : 0x28));

/**
 * \ingroup rwimage
 * \ref RwImageCallBackRead 
 * is the function registered with \ref RwImageRegisterImageFormat that is used,
 * for example by \ref RwImageRead and \ref RwImageReadMaskedImage,
 * to read images of a specific format from a disk file.
 * 
 * \param  imageName   Pointer to a string containing the file name of the image.
 *
 * \return Returns a pointer to the image read.
 *
 * \see RwImageRegisterImageFormat
 *
 */
typedef RwImage *(*RwImageCallBackRead)(const RwChar * imageName);

/**
 * \ingroup rwimage
 * \ref RwImageCallBackWrite 
 * is the function registered with \ref RwImageRegisterImageFormat that is used,
 * for example by \ref RwImageWrite, 
 * to write images of a specific format to a disk file.
 * 
 * \param  image   Pointer to the image.
 *
 * \param  imageName   Pointer to a string containing the file name
 * of the image.
 *
 *
 * \return Pointer to the written image.
 *
 * \see RwImageRegisterImageFormat
 *
 */
typedef RwImage *(*RwImageCallBackWrite)(RwImage *image, const RwChar *imageName);

/* RWPUBLICEND */

typedef struct rwImageFormat rwImageFormat;
struct rwImageFormat
{
        RwChar              lcExtension[20]; /* Lower case version */
        RwChar              ucExtension[20]; /* Upper case version */
        RwImageCallBackRead readImage;
        RwImageCallBackWrite writeImage;
        rwImageFormat      *nextFormat;
};
static_assert(sizeof(rwImageFormat) == (VER_x32 ? 0x34 : 0x40));

typedef struct rwImageGlobals rwImageGlobals;
struct rwImageGlobals
{
        RwFreeList         *imageFreeList;
        RwChar             *imagePath;
        RwInt32             imagePathSize;

        RwUInt8             gammaTable[256];
        RwUInt8             invGammaTable[256];
        RwReal              gammaVal;

        RwUInt8            *scratchMem;
        RwInt32             scratchMemSize;

        RwFreeList         *imageFormatFreeList;
        rwImageFormat      *imageFormats;
};
static_assert(sizeof(rwImageFormat) == (VER_x32 ? 0x220 : 0x238));

/* RWPUBLIC */

/****************************************************************************
 <macro/inline functionality
 */

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


#if !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE))

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

#endif /* !(defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */

    /* Creating and destroying */

extern void RwImageSetFreeListCreateParams( RwInt32 blockSize, RwInt32 numBlocksToPrealloc );

extern void RwImageFormatSetFreeListCreateParams( RwInt32 blockSize, RwInt32 numBlocksToPrealloc );

extern RwImage     *RwImageCreate(RwInt32 width, RwInt32 height,
                                  RwInt32 depth);
extern RwBool       RwImageDestroy(RwImage * image);

    /* Allocating */
extern RwImage     *RwImageAllocatePixels(RwImage * image);
extern RwImage     *RwImageFreePixels(RwImage * image);

    /* Converting images */
extern RwImage     *RwImageCopy(RwImage * destImage,
                                const RwImage * sourceImage);

    /* Resizing images */
extern RwImage     *RwImageResize(RwImage * image, RwInt32 width,
                                  RwInt32 height);

    /* Producing masks ! */
extern RwImage     *RwImageApplyMask(RwImage * image,
                                     const RwImage * mask);
extern RwImage     *RwImageMakeMask(RwImage * image);

    /* Helper functions */
extern RwImage     *RwImageReadMaskedImage(const RwChar * imageName,
                                           const RwChar * maskname);
extern RwImage     *RwImageRead(const RwChar * imageName);
extern RwImage     *RwImageWrite(RwImage * image,
                                 const RwChar * imageName);

    /* Setting and getting the default path for images */
extern RwChar      *RwImageGetPath(void);
extern const RwChar *RwImageSetPath(const RwChar * path);
    /* Fast image path change */
extern void        _rwImageSwapPath(RwChar **path, RwInt32 *size);

    /* Setting */
#if (defined(RWDEBUG) || defined(RWSUPPRESSINLINE))
extern RwImage     *RwImageSetStride(RwImage * image, RwInt32 stride);
extern RwImage     *RwImageSetPixels(RwImage * image, RwUInt8 * pixels);
extern RwImage     *RwImageSetPalette(RwImage * image, RwRGBA * palette);

    /* Getting */
extern RwInt32      RwImageGetWidth(const RwImage * image);
extern RwInt32      RwImageGetHeight(const RwImage * image);
extern RwInt32      RwImageGetDepth(const RwImage * image);
extern RwInt32      RwImageGetStride(const RwImage * image);
extern RwUInt8     *RwImageGetPixels(const RwImage * image);
extern RwRGBA      *RwImageGetPalette(const RwImage * image);
#endif /* (defined(RWDEBUG) || defined(RWSUPPRESSINLINE)) */

    /* Get device dependent pixel value */
extern RwUInt32     RwRGBAToPixel(RwRGBA * rgbIn, RwInt32 rasterFormat);
extern RwRGBA      *RwRGBASetFromPixel(RwRGBA * rgbOut,
                                       RwUInt32 pixelValue,
                                       RwInt32 rasterFormat);

    /* Gamma correction */
extern RwBool       RwImageSetGamma(RwReal gammaValue);
extern RwReal       RwImageGetGamma(void);
extern RwImage     *RwImageGammaCorrect(RwImage * image);

    /* Adding and removing gamma correction */
extern RwRGBA      *RwRGBAGammaCorrect(RwRGBA * rgb);

    /* Attaching toolkits */
extern RwInt32      RwImageRegisterPlugin(RwInt32 size, RwUInt32 pluginID,
                                          RwPluginObjectConstructor
                                          constructCB,
                                          RwPluginObjectDestructor
                                          destructCB,
                                          RwPluginObjectCopy copyCB);
extern RwInt32      RwImageGetPluginOffset(RwUInt32 pluginID);
extern RwBool       RwImageValidatePlugins(const RwImage * image);

extern RwBool       RwImageRegisterImageFormat(const RwChar * extension,
                                               RwImageCallBackRead
                                               imageRead,
                                               RwImageCallBackWrite
                                               imageWrite);

    /* Finding an extension for an image to load */
extern const RwChar *RwImageFindFileType(const RwChar * imageName);

    /* Reading and writing images to streams */
extern RwInt32      RwImageStreamGetSize(const RwImage * image);
extern RwImage     *RwImageStreamRead(RwStream * stream);
extern const RwImage *RwImageStreamWrite(const RwImage * image,
                                         RwStream * stream);

    /* RWPUBLICEND */

    /* Gamma correction */
extern void         _rwImageGammaCorrectArrayOfRGBA(RwRGBA * rgbaOut,
                                                    RwRGBA * rgbaIn,
                                                    RwInt32 numEls);
extern void         _rwImageGammaUnCorrectArrayOfRGBA(RwRGBA * rgbaOut,
                                                      RwRGBA * rgbaIn,
                                                      RwInt32 numEls);

    /* Opening and closing libraries */
extern void        *_rwImageOpen(void *instance, RwInt32 offset,
                                 RwInt32 size);
extern void        *_rwImageClose(void *instance, RwInt32 offset,
                                  RwInt32 size);

    /* RWPUBLIC */

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */
/* RWPUBLICEND */

#endif                          /* RWIMAGE_H */

