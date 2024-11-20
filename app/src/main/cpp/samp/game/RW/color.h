#pragma once

#include "types.h"

typedef struct RwRGBAReal RwRGBAReal;
/**
 * \ingroup rwrgba
 * \struct RwRGBAReal
 * This structure represents an RGBA color which has 
 * components specified as real values. 
 *
 * A color component of an RwRGBA with the value 255 generally corresponds 
 * to the associated component in an RwRGBAReal with the value 1.0f. 
 * However, any values can be substituted to denormalize/normalize 
 * RwRGBAReal and create different effects. For example, while light colors
 * are expressed as normalized RGBA, interesting effects can be gained using 
 * larger values. 
 *
 * It should also be noted that a color component of an RwRGBA with the 
 * value 0 generally corresponds to the associcated component in an 
 * RwRGBAReal with the value 0.0.
 */
struct RwRGBAReal
{
    RwReal red;     /**< red component */
    RwReal green;   /**< green component */
    RwReal blue;    /**< blue component */
    RwReal alpha;   /**< alpha component */
};
static_assert(sizeof(RwRGBAReal) == 0x10);

#undef RwRGBARealAssign
#define RwRGBARealAssign(_target, _source) ( *(_target) = *(_source) )

typedef struct RwRGBA RwRGBA;
/**
 * \ingroup rwrgba
 * \struct RwRGBA
 * This structure represents an RGBA color 
 * which has integer components specified in the range 0 to 255. */
struct RwRGBA
{
    RwUInt8 red;    /**< red component */
    RwUInt8 green;  /**< green component */
    RwUInt8 blue;   /**< blue component */
    RwUInt8 alpha;  /**< alpha component */
};
static_assert(sizeof(RwRGBA) == 0x4);

#undef RwRGBAAssign
#define RwRGBAAssign(_target, _source) ( *(_target) = *(_source) )

#define RwRGBARealAddMacro(o,a,b)                                        \
MACRO_START                                                              \
{                                                                        \
    (o)->red   = (((a)->red) + (   (b)->red));                           \
    (o)->green = (((a)->green) + ( (b)->green));                         \
    (o)->blue  = (((a)->blue) + (  (b)->blue));                          \
    (o)->alpha = (((a)->alpha) + ( (b)->alpha));                         \
}                                                                        \
MACRO_STOP

#define RwRGBARealSubMacro(o,a,b)                                        \
MACRO_START                                                              \
{                                                                        \
    (o)->red   = (((a)->red) - (   (b)->red));                           \
    (o)->green = (((a)->green) - ( (b)->green));                         \
    (o)->blue  = (((a)->blue) - (  (b)->blue));                          \
    (o)->alpha = (((a)->alpha) - ( (b)->alpha));                         \
}                                                                        \
MACRO_STOP

#define RwRGBARealScaleMacro(o,a,scale)                                  \
MACRO_START                                                              \
{                                                                        \
    (o)->red   = (((a)->red) * (   scale));                              \
    (o)->green = (((a)->green) * ( scale));                              \
    (o)->blue  = (((a)->blue) * (  scale));                              \
    (o)->alpha = (((a)->alpha) * ( scale));                              \
}                                                                        \
MACRO_STOP

/* Conversion macros */
#define RwRGBAFromRwRGBARealMacro(o, i)                                  \
MACRO_START                                                              \
{                                                                        \
    RwInt32 quantize;                                                    \
                                                                         \
    quantize = RwInt32FromRealMacro( ((i)->red   * (RwReal)255.0)             \
                                + (RwReal)0.5 );                         \
    (o)->red   = (RwUInt8) quantize;                                     \
    quantize = RwInt32FromRealMacro( ((i)->green * (RwReal)255.0)             \
                                + (RwReal)0.5 );                         \
    (o)->green = (RwUInt8) quantize;                                     \
    quantize = RwInt32FromRealMacro( ((i)->blue  * (RwReal)255.0)             \
                                + (RwReal)0.5 );                         \
    (o)->blue  = (RwUInt8) quantize;                                     \
    quantize = RwInt32FromRealMacro( ((i)->alpha * (RwReal)255.0)             \
                                + (RwReal)0.5 );                         \
    (o)->alpha = (RwUInt8) quantize;                                     \
                                                                         \
}                                                                        \
MACRO_STOP

#define RwRGBARealFromRwRGBAMacro(o, i)                                  \
MACRO_START                                                              \
{                                                                        \
    (o)->red   =                                                         \
        (((RwReal)(((i)->red))) * (   (RwReal)((1.0/255.0))));           \
    (o)->green =                                                         \
        (((RwReal)(((i)->green))) * ( (RwReal)((1.0/255.0))));           \
    (o)->blue  =                                                         \
        (((RwReal)(((i)->blue))) * (  (RwReal)((1.0/255.0))));           \
    (o)->alpha =                                                         \
        (((RwReal)(((i)->alpha))) * ( (RwReal)((1.0/255.0))));           \
}                                                                        \
MACRO_STOP

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */


#if (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) ))

#define RwRGBARealAdd(o,a,b) \
        RwRGBARealAddMacro(o,a,b)

#define RwRGBARealSub(o,a,b) \
        RwRGBARealSubMacro(o,a,b)

#define RwRGBARealScale(o,a,scale) \
        RwRGBARealScaleMacro(o,a,scale)

#define RwRGBAFromRwRGBAReal(o, i) \
        RwRGBAFromRwRGBARealMacro(o, i)

#define RwRGBARealFromRwRGBA(o, i) \
        RwRGBARealFromRwRGBAMacro(o, i)

#else /* (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) )) */

/* Function versions for debug */
extern void RwRGBARealAdd(RwRGBAReal *result,
                          const RwRGBAReal *source1,
                          const RwRGBAReal *source2);

extern void RwRGBARealSub(RwRGBAReal *result,
                          const RwRGBAReal *source1,
                          const RwRGBAReal *source2);

extern void RwRGBARealScale(RwRGBAReal *result,
                            const RwRGBAReal *source,
                            RwReal scalar);

extern void RwRGBAFromRwRGBAReal(RwRGBA *result,
                                 const RwRGBAReal *source);

extern void RwRGBARealFromRwRGBA(RwRGBAReal *result,
                                 RwRGBA *source);

#endif /* (! ( defined(RWDEBUG) || defined(RWSUPPRESSINLINE) )) */

/* RWPUBLICEND */

/* Init/Term */
extern void *_rwColorOpen(void *instance, RwInt32 offset, RwInt32 size);
extern void *_rwColorClose(void *instance, RwInt32 offset, RwInt32 size);

/* RWPUBLIC */
#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

