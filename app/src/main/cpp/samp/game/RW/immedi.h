/***************************************************************************
 *                                                                         *
 * Module  : baimmedi.h                                                    *
 *                                                                         *
 * Purpose : 2D immediate mode interface                                   *
 *                                                                         *
 **************************************************************************/

#ifndef RWIMMEDI_H
#define RWIMMEDI_H

/****************************************************************************
 Includes
 */

#include "types.h"
//#include "drvmodel.h"

/* RWPUBLIC */

/****************************************************************************
 Defines
 */


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                        Immediate mode interface V2.0

 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwRenderState represents the global state variables that control
 * rendering. These may be set and queried using the
 * \ref RwRenderStateSet and \ref RwRenderStateGet functions respectively.
 *
 * Refer to the \ref rwrenderstateoverview for an overview of this system.
 *
 * \note The texture render states (raster, address & filter modes) would
 * normally just be used when rendering in immediate mode and should be
 * specificied completely every time a texture is used. Retained mode
 * pipelines will frequently set theses states internally, usually based on
 * \ref RwTexture objects.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwRenderState
{
    rwRENDERSTATENARENDERSTATE = 0,

    rwRENDERSTATETEXTURERASTER,
        /**<Raster used for texturing (normally used in immediate mode). 
         *  The value is a pointer to an \ref RwRaster.
         * Default: NULL.
         */
    rwRENDERSTATETEXTUREADDRESS,
        /**<\ref RwTextureAddressMode: wrap, clamp, mirror or border.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREADDRESSU,
        /**<\ref RwTextureAddressMode in u only.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREADDRESSV,
        /**<\ref RwTextureAddressMode in v only.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREPERSPECTIVE,
        /**<Perspective correction on/off (always enabled on many platforms).
         */
    rwRENDERSTATEZTESTENABLE,
        /**<Z-buffer test on/off.
         * Default: TRUE.
         */
    rwRENDERSTATESHADEMODE,
        /**<\ref RwShadeMode: flat or gouraud shading.
         * Default: rwSHADEMODEGOURAUD.
         */
    rwRENDERSTATEZWRITEENABLE,
        /**<Z-buffer write on/off.
         * Default: TRUE.
         */
    rwRENDERSTATETEXTUREFILTER,
        /**<\ref RwTextureFilterMode: point sample, bilinear, trilinear, etc.
         * Default: rwFILTERLINEAR.
         */
    rwRENDERSTATESRCBLEND,
        /**<\ref RwBlendFunction used to modulate the source pixel color
         *  when blending to the frame buffer.
         * Default: rwBLENDSRCALPHA.
         */
    rwRENDERSTATEDESTBLEND,
        /**<\ref RwBlendFunction used to modulate the destination pixel
         *  color in the frame buffer when blending. The resulting pixel
         *  color is given by the formula 
         *  (SRCBLEND * srcColor + DESTBLEND * destColor) for each RGB
         *  component. For a particular platform, not all combinations
         *  of blend function are allowed (see platform specific
         *  restrictions).
         * Default: rwBLENDINVSRCALPHA.
         */
    rwRENDERSTATEVERTEXALPHAENABLE,
        /**<Alpha blending on/off (always enabled on some platforms). 
         *  This is normally used in immediate mode to enable alpha blending 
         *  when vertex colors or texture rasters have transparency. Retained
         *  mode pipelines will usually set this state based on material colors 
         *  and textures.
         * Default: FALSE.
         */
    rwRENDERSTATEBORDERCOLOR,
        /**<Border color for \ref RwTextureAddressMode 
         *  \ref rwTEXTUREADDRESSBORDER. The value should be a packed 
         *  RwUInt32 in a platform specific format. The macro 
         *  RWRGBALONG(r, g, b, a) may be used to construct this using 
         *  8-bit color components.
         * Default: RWRGBALONG(0, 0, 0, 0).
         */
    rwRENDERSTATEFOGENABLE,
        /**<Fogging on/off (all polygons will be fogged).  
         * Default: FALSE.
         */
    rwRENDERSTATEFOGCOLOR,
        /**<Color used for fogging. The value should be a packed RwUInt32 
         *  in a platform specific format. The macro RWRGBALONG(r, g, b, a)
         *  may be used to construct this using 8-bit color components. 
         * Default: RWRGBALONG(0, 0, 0, 0).
         */
    rwRENDERSTATEFOGTYPE,
        /**<\ref RwFogType, the type of fogging to use. 
         * Default: rwFOGTYPELINEAR.
         */
    rwRENDERSTATEFOGDENSITY,
        /**<Fog density for \ref RwFogType of 
         *  \ref rwFOGTYPEEXPONENTIAL or \ref rwFOGTYPEEXPONENTIAL2. 
         *  The value should be a pointer to an RwReal in the 
         *  range 0 to 1.  
         * Default: 1.
         */
    rwRENDERSTATECULLMODE = 20,
        /**<\ref RwCullMode, for selecting front/back face culling, or
         *  no culling.
         * Default: rwCULLMODECULLBACK.
         */
    rwRENDERSTATESTENCILENABLE,
        /**<Stenciling on/off.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: FALSE.
         */
    rwRENDERSTATESTENCILFAIL,
        /**<\ref RwStencilOperation used when the stencil test passes.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILZFAIL,
        /**<\ref RwStencilOperation used when the stencil test passes and 
         *  the depth test (z-test) fails. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILPASS,
        /**<\ref RwStencilOperation used when both the stencil and the depth 
         *  (z) tests pass. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILFUNCTION,
        /**<\ref RwStencilFunction for the stencil test. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILFUNCTIONALWAYS.
         */
    rwRENDERSTATESTENCILFUNCTIONREF,
        /**<Integer reference value for the stencil test. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0.
         */
    rwRENDERSTATESTENCILFUNCTIONMASK,
        /**<Mask applied to the reference value and each stencil buffer 
         *  entry to determine the significant bits for the stencil test. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0xffffffff.
         */
    rwRENDERSTATESTENCILFUNCTIONWRITEMASK,
        /**<Write mask applied to values written into the stencil buffer. 
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0xffffffff.
         */
    rwRENDERSTATEALPHATESTFUNCTION,
        /**<\ref RwAlphaTestFunction for the alpha test. When a pixel fails,
         * neither the frame buffer nor the Z-buffer are updated.
         * Default: rwALPHATESTFUNCTIONGREATER (GameCube, Xbox, D3D8, D3D9
         * and OpenGL). The default PS2 behaviour is to always update the
         * frame buffer and update the Z-buffer only if a greater than or
         * equal test passes.
         */
    rwRENDERSTATEALPHATESTFUNCTIONREF,
        /**<Integer reference value for the alpha test. 
         *  <i> Range is 0 to 255, mapped to the platform's actual range </i>
         * Default: 128 (PS2) 0 (GameCube, Xbox, D3D8, D3D9 and OpenGL).
         */

    rwRENDERSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRenderState RwRenderState;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwShadeMode represents the available shading modes that may be
 * set using the \ref RwRenderState \ref rwRENDERSTATESHADEMODE.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwShadeMode
{
    rwSHADEMODENASHADEMODE = 0,
    rwSHADEMODEFLAT,                /**<Flat shading */
    rwSHADEMODEGOURAUD,             /**<Gouraud shading */
    rwSHADEMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwShadeMode RwShadeMode;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwTextureFilterMode represents the texture filtering modes that may
 * be set using the \ref RwRenderState \ref rwRENDERSTATETEXTUREFILTER in
 * immediate mode, or \ref RwTextureSetFilterMode in retained mode.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwTextureFilterMode
{
    rwFILTERNAFILTERMODE = 0,
    rwFILTERNEAREST,                /**<Point sampled */
    rwFILTERLINEAR,                 /**<Bilinear */
    rwFILTERMIPNEAREST,             /**<Point sampled per pixel mip map */
    rwFILTERMIPLINEAR,              /**<Bilinear per pixel mipmap */
    rwFILTERLINEARMIPNEAREST,       /**<MipMap interp point sampled */
    rwFILTERLINEARMIPLINEAR,        /**<Trilinear */
    rwTEXTUREFILTERMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureFilterMode RwTextureFilterMode;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwFogType represents the fog modes available when setting the
 * \ref RwRenderState \ref rwRENDERSTATEFOGTYPE. Note that a particular
 * platform may not support all of these modes (see platform specific
 * restrictions).
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwFogType
{
    rwFOGTYPENAFOGTYPE = 0,
    rwFOGTYPELINEAR,            /**<Linear fog */
    rwFOGTYPEEXPONENTIAL,       /**<Exponential fog */
    rwFOGTYPEEXPONENTIAL2,      /**<Exponential^2 fog */
    rwFOGTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwFogType RwFogType;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwBlendFunction represents the options available when changing the
 * \ref RwRenderState setting for \ref rwRENDERSTATESRCBLEND and 
 * \ref rwRENDERSTATEDESTBLEND. The values are factors used to modulate
 * either the source or destination pixel color when blending to
 * the frame buffer. Note the some  combinations are not allowed on
 * certain platforms (see platform specific restrictions).
 *
 * In the following list, the factors applied to each color component are 
 * listed explicitly. A subscript s refers to a source value while a subscript 
 * d refers to a destination value. Note that blending in the alpha channel is 
 * only applicable when the frame buffer actually contains alpha, and that
 * the precise operation is platform specific. For instance, the
 * source alpha value might simply be written to the alpha channel with
 * no blending.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwBlendFunction
{
    rwBLENDNABLEND = 0,
    rwBLENDZERO,            /**<(0,    0,    0,    0   ) */
    rwBLENDONE,             /**<(1,    1,    1,    1   ) */
    rwBLENDSRCCOLOR,        /**<(Rs,   Gs,   Bs,   As  ) */
    rwBLENDINVSRCCOLOR,     /**<(1-Rs, 1-Gs, 1-Bs, 1-As) */
    rwBLENDSRCALPHA,        /**<(As,   As,   As,   As  ) */
    rwBLENDINVSRCALPHA,     /**<(1-As, 1-As, 1-As, 1-As) */
    rwBLENDDESTALPHA,       /**<(Ad,   Ad,   Ad,   Ad  ) */
    rwBLENDINVDESTALPHA,    /**<(1-Ad, 1-Ad, 1-Ad, 1-Ad) */
    rwBLENDDESTCOLOR,       /**<(Rd,   Gd,   Bd,   Ad  ) */
    rwBLENDINVDESTCOLOR,    /**<(1-Rd, 1-Gd, 1-Bd, 1-Ad) */
    rwBLENDSRCALPHASAT,     /**<(f,    f,    f,    1   )  f = min (As, 1-Ad) */
    rwBLENDFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwBlendFunction RwBlendFunction;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwTextureAddressMode represents the addressing modes available 
 * when mapping textures to polygons using UV texture coordinates.
 * This may be set in immediate mode via the \ref RwRenderState
 * \ref rwRENDERSTATETEXTUREADDRESS, or via \ref RwTextureSetAddressing
 * for an \ref RwTexture object.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwTextureAddressMode
{
    rwTEXTUREADDRESSNATEXTUREADDRESS = 0,
    rwTEXTUREADDRESSWRAP,      /**<UV wraps (tiles) */
    rwTEXTUREADDRESSMIRROR,    /**<Alternate UV is flipped */
    rwTEXTUREADDRESSCLAMP,     /**<UV is clamped to 0-1 */
    rwTEXTUREADDRESSBORDER,    /**<Border color takes effect outside of 0-1 */
    rwTEXTUREADDRESSMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureAddressMode RwTextureAddressMode;
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwStencilOperation represents the stencil operations that may be
 * performed depending on the results of stencil/z-buffer tests.
 * The operation may be set for the various tests 
 * via the \ref RwRenderState settings \ref rwRENDERSTATESTENCILFAIL,
 * \ref rwRENDERSTATESTENCILZFAIL and \ref rwRENDERSTATESTENCILPASS.
 */ 
#endif /* RWADOXYGENEXTERNAL */
enum RwStencilOperation
{
    rwSTENCILOPERATIONNASTENCILOPERATION = 0,

    rwSTENCILOPERATIONKEEP,     
        /**<Do not update the entry in the stencil buffer */
    rwSTENCILOPERATIONZERO,     
        /**<Set the stencil-buffer entry to 0 */
    rwSTENCILOPERATIONREPLACE,  
        /**<Replace the stencil-buffer entry with reference value */
    rwSTENCILOPERATIONINCRSAT,  
        /**<Increment the stencil-buffer entry, clamping to the 
         *  maximum value */
    rwSTENCILOPERATIONDECRSAT,  
        /**<Decrement the stencil-buffer entry, clamping to zero */    
    rwSTENCILOPERATIONINVERT,   
        /**<Invert the bits in the stencil-buffer entry */
    rwSTENCILOPERATIONINCR,     
        /**<Increment the stencil-buffer entry, wrapping to zero if 
         *  the new value exceeds the maximum value */
    rwSTENCILOPERATIONDECR,     
        /**<Decrement the stencil-buffer entry, wrapping to the maximum
         *  value if the new value is less than zero */
 
    rwSTENCILOPERATIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStencilOperation RwStencilOperation;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwStencilFunction represents the comparison functions available for
 * a stencil test. The function may be selected via the 
 * \ref RwRenderState setting \ref rwRENDERSTATESTENCILFUNCTION.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwStencilFunction
{
    rwSTENCILFUNCTIONNASTENCILFUNCTION = 0,

    rwSTENCILFUNCTIONNEVER,         
        /**<Always fail the test */
    rwSTENCILFUNCTIONLESS,          
        /**<Accept the new pixel if its value is less than the value of
         *  the current pixel */
    rwSTENCILFUNCTIONEQUAL,         
        /**<Accept the new pixel if its value equals the value of the
         *  current pixel */
    rwSTENCILFUNCTIONLESSEQUAL,     
        /**<Accept the new pixel if its value is less than or equal to 
         *  the value of the current pixel */
    rwSTENCILFUNCTIONGREATER,       
        /**<Accept the new pixel if its value is greater than the value 
         *  of the current pixel */
    rwSTENCILFUNCTIONNOTEQUAL,      
        /**<Accept the new pixel if its value does not equal the value of 
         *  the current pixel */
    rwSTENCILFUNCTIONGREATEREQUAL,  
        /**<Accept the new pixel if its value is greater than or equal
         *  to the value of the current pixel */
    rwSTENCILFUNCTIONALWAYS,        
        /**<Always pass the test */

    rwSTENCILFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStencilFunction RwStencilFunction;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwAlphaTestFunction represents the comparison functions available for
 * an alpha test. The function may be selected via the 
 * \ref RwRenderState setting \ref rwRENDERSTATEALPHATESTFUNCTION.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwAlphaTestFunction
{
    rwALPHATESTFUNCTIONNAALPHATESTFUNCTION = 0,

    rwALPHATESTFUNCTIONNEVER,         
        /**<Always fail the test */
    rwALPHATESTFUNCTIONLESS,          
        /**<Accept the new pixel if its alpha value is less than the value of
         *  the reference value */
    rwALPHATESTFUNCTIONEQUAL,         
        /**<Accept the new pixel if its alpha value equals the value of the
         *  reference value */
    rwALPHATESTFUNCTIONLESSEQUAL,     
        /**<Accept the new pixel if its alpha value is less than or equal to 
         *  the value of the reference value */
    rwALPHATESTFUNCTIONGREATER,       
        /**<Accept the new pixel if its alpha value is greater than the value 
         *  of the reference value */
    rwALPHATESTFUNCTIONNOTEQUAL,      
        /**<Accept the new pixel if its alpha value does not equal the value of 
         *  the reference value */
    rwALPHATESTFUNCTIONGREATEREQUAL,  
        /**<Accept the new pixel if its alpha value is greater than or equal
         *  to the value of the reference value */
    rwALPHATESTFUNCTIONALWAYS,        
        /**<Always pass the test */

    rwALPHATESTFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwAlphaTestFunction RwAlphaTestFunction;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwrenderstate
 * RwCullMode represents the options available for culling polygons
 * during rendering. The cull mode may be set via the \ref RwRenderState
 * setting \ref rwRENDERSTATECULLMODE.
 */
#endif /* RWADOXYGENEXTERNAL */
enum RwCullMode
{
    rwCULLMODENACULLMODE = 0,

    rwCULLMODECULLNONE, 
        /**<Both front and back-facing triangles are drawn. */
    rwCULLMODECULLBACK, 
        /**<Only front-facing triangles are drawn */
    rwCULLMODECULLFRONT,
        /**<Only back-facing triangles are drawn */

    rwCULLMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCullMode RwCullMode;

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup immediatemodedatatypes
 * RwPrimitiveType
 *  This type represents the different types of indexed
 * line and indexed triangle primitives that are available when rendering 2D
 * and 3D immediate mode objects (see API functions \ref RwIm2DRenderIndexedPrimitive,
 * \ref RwIm2DRenderPrimitive, \ref RwIm3DRenderIndexedPrimitive and \ref RwIm3DRenderPrimitive).
 * Indices are into a vertex list and must be defined in a counter-clockwise order
 * (as seen from the camera) to be visible.*/
#endif /* RWADOXYGENEXTERNAL */
enum RwPrimitiveType
{
    rwPRIMTYPENAPRIMTYPE = 0,   /**<Invalid primative type */
    rwPRIMTYPELINELIST = 1,     /**<Unconnected line segments, each line is specified by
                                 * both its start and end index, independently of other lines
                                 * (for example, 3 segments specified as 0-1, 2-3, 4-5) */
    rwPRIMTYPEPOLYLINE = 2,     /**<Connected line segments, each line's start index
                                 * (except the first) is specified by the index of the end of
                                 * the previous segment (for example, 3 segments specified as
                                 * 0-1, 1-2, 2-3) */
    rwPRIMTYPETRILIST = 3,      /**<Unconnected triangles: each triangle is specified by
                                 * three indices, independently of other triangles (for example,
                                 * 3 triangles specified as 0-1-2, 3-4-5, 6-7-8) */
    rwPRIMTYPETRISTRIP = 4,     /**<Connected triangles sharing an edge with, at most, one
                                 * other forming a series (for example, 3 triangles specified
                                 * as 0-2-1, 1-2-3-, 2-4-3) */
    rwPRIMTYPETRIFAN = 5 ,      /**<Connected triangles sharing an edge with, at most,
                                 * two others forming a fan (for example, 3 triangles specified
                                 * as 0-2-1, 0-3-2, 0-4-3) */
    rwPRIMTYPEPOINTLIST = 6,    /**<Points 1, 2, 3, etc. This is not
                                 * supported by the default RenderWare
                                 * immediate or retained-mode pipelines
                                 * (except on PlayStation 2), it is intended
                                 * for use by user-created pipelines */
    rwPRIMITIVETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPrimitiveType RwPrimitiveType;

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */


#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))

#else /* (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) )) */

/* Expose Z buffer range */
extern RwReal RwIm2DGetNearScreenZ(void);
extern RwReal RwIm2DGetFarScreenZ(void);

extern RwBool RwRenderStateGet(RwRenderState state, void *value);
extern RwBool RwRenderStateSet(RwRenderState state, void *value);

extern RwBool RwIm2DRenderLine(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
extern RwBool RwIm2DRenderTriangle(RwIm2DVertex *vertices, RwInt32 numVertices,
                                   RwInt32 vert1, RwInt32 vert2, RwInt32 vert3 );
extern RwBool RwIm2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices);
extern RwBool RwIm2DRenderIndexedPrimitive(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices,
                                                             RwImVertexIndex *indices, RwInt32 numIndices);

#endif /* (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) )) */


#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

/* RWPUBLICEND */

#endif /* RWIMMEDI_H */

