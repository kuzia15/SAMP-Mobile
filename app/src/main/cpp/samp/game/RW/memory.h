/***************************************************************************
 *                                                                         *
 * Module  : bamemory.h                                                    *
 *                                                                         *
 * Purpose : Memory handling                                               *
 *                                                                         *
 **************************************************************************/

#ifndef RWMEMORY_H
#define RWMEMORY_H

/****************************************************************************
 Includes
 */

#include <stdlib.h>
#include "types.h"

/* RWPUBLIC */
#if (defined(RWMEMDEBUG))
#ifdef _XBOX
/* Need OutputDebugString macros */
#include <xtl.h>
#endif
#endif

/****************************************************************************
 Defines
 */

/*
 * Debug fill bytes for compatibility with MSVC/C++ debug heap
 * See
 * \Program Files\Microsoft Visual Studio\VC98\CRT\SRC\DBGHEAP.C:
 * static unsigned char _bNoMansLandFill = 0xFD;
 *   // fill no-man's land with this
 * static unsigned char _bDeadLandFill   = 0xDD;
 *   // fill free objects with this
 * static unsigned char _bCleanLandFill  = 0xCD;
 *   // fill new objects with this
 */

#if (!defined(rwFREELISTNOMANSLANDFILL))
#define rwFREELISTNOMANSLANDFILL    0xFD
#endif /* (!defined(rwFREELISTNOMANSLANDFILL)) */

#if (!defined(rwFREELISTDEADLANDFILL))
#define rwFREELISTDEADLANDFILL      0xDD
#endif /* (!defined(rwFREELISTDEADLANDFILL)) */

#if (!defined(rwFREELISTCLEANLANDFILL))
#define rwFREELISTCLEANLANDFILL     0xCD
#endif /* (!defined(rwFREELISTCLEANLANDFILL)) */

#define RWFREELISTALIGNED(_pData, _freelist) \
  (! (((RwUInt32)(_pData)) & ((_freelist)->alignment - 1)) )

/*****************************
 * REGULAR MEMORY ALLOCATION *
 *****************************/

/**
 * \ingroup memoryfileinterface
 * \def RwMalloc
 * RwMalloc(_s, _h) is a macro that calls the currently registered RwMalloc function.
 * You can access all registered memory functions through an
 * \ref RwOsGetMemoryInterface() call which returns a pointer to an
 * \ref RwMemoryFunctions structure. You can also override the default
 * RenderWare Graphics functions by passing a structure populated with
 * your own RwMemoryFunctions structure to \ref RwEngineInit().
 *
 * For \c RWDEBUG applications, the default \ref RwMalloc records the file and line
 * on which it was called. These records are shared between \ref RwCalloc and
 * \ref RwRealloc, and does not store any additional history. To retrieve the last
 * file and line, use \ref RwMemoryGetLastAllocFile and \ref RwMemoryGetLastAllocLine.
 *
 * \see RwMemoryFunctions for more specific function documentation.
 */

/**
 * \ingroup memoryfileinterface
 * \def RwFree
 * RwFree(_p) is a macro that calls the currently registered RwFree function.
 * You can access all registered memory functions through an
 * \ref RwOsGetMemoryInterface() call which returns a pointer to an
 * \ref RwMemoryFunctions structure. You can also override the default
 * RenderWare Graphics functions by passing a structure populated with
 * your own RwMemoryFunctions structure to \ref RwEngineInit(). 
 *
 * \see RwMemoryFunctions for more specific function documentation.
 */

/**
 * \ingroup memoryfileinterface
 * \def RwCalloc
 * RwCalloc(_n, _s, _h) is a macro that calls the currently registered RwCalloc
 * function. You can access all registered memory functions
 * through an \ref RwOsGetMemoryInterface() call which returns a pointer
 * to an \ref RwMemoryFunctions structure. You can also override the
 * default RenderWare Graphics functions by passing a structure populated
 * with your own RwMemoryFunctions structure to \ref RwEngineInit(). 
 *
 * For \c RWDEBUG applications, the default \ref RwCalloc records the file and line
 * on which it was called. These records are shared between \ref RwMalloc and
 * \ref RwRealloc, and does not store any additional history. To retrieve the last
 * file and line, use \ref RwMemoryGetLastAllocFile and \ref RwMemoryGetLastAllocLine.
 *
 * \see RwMemoryFunctions for more specific function documentation.
 */

/**
 * \ingroup memoryfileinterface
 * \def RwRealloc
 * RwRealloc(_p, _s, _h) is a macro that calls the currently registered RwRealloc
 * function. You can access all registered memory functions
 * through an \ref RwOsGetMemoryInterface() call which returns a pointer
 * to an \ref RwMemoryFunctions structure. You can also override the
 * default RenderWare Graphics functions by passing a structure populated
 * with your own RwMemoryFunctions structure to \ref RwEngineInit(). 
 *
 * For \c RWDEBUG applications, the default \ref RwRealloc records the file and line
 * on which it was called. These records are shared between \ref RwMalloc and
 * \ref RwCalloc, and does not store any additional history. To retrieve the last
 * file and line, use \ref RwMemoryGetLastAllocFile and \ref RwMemoryGetLastAllocLine.
 *
 * \see RwMemoryFunctions for more specific function documentation.
 */

#if ( (defined(RWMEMDEBUG)) && defined(RWDEBUG) )

#if (!defined(RWNOFREELISTS))
#define RWNOFREELISTS
#endif /* (!defined(RWNOFREELISTS)) */

#if (defined(rwPLUGIN_ID))
#define _CLIENT_TAG \
    ( 0xFFFF & (rwPLUGIN_ID) )
#endif /* (defined(rwPLUGIN_ID)) */

#if (!defined(_CLIENT_TAG))
#define _CLIENT_TAG \
    ( 0xFFFF & (MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x00) ) )
#endif /* (!defined(_CLIENT_TAG)) */

# if (defined(_MSC_VER))
#  if ((_MSC_VER>=1000) && defined(_DEBUG))

#if (defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC))
#define _CRTDBG_MAP_ALLOC
#endif /* defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC)) */
#include <crtdbg.h>

#define RwMalloc(_s, _h)                                \
    (_rwMemoryLastAllocFile = RWSTRING(__FILE__),                 \
     _rwMemoryLastAllocLine = __LINE__,                 \
     _malloc_dbg((_s),                                  \
                _CLIENT_BLOCK | ((_CLIENT_TAG)<<16),    \
                __FILE__,                               \
                __LINE__))

#define RwFree(_p)                                      \
   _free_dbg((_p),                                      \
               _CLIENT_BLOCK | ((_CLIENT_TAG)<<16))

#define RwCalloc(_n, _s, _h)                            \
   (_rwMemoryLastAllocFile = RWSTRING(__FILE__),                  \
    _rwMemoryLastAllocLine = __LINE__,                  \
    _calloc_dbg((_n), (_s),                             \
                _CLIENT_BLOCK | ((_CLIENT_TAG)<<16),    \
                __FILE__,                               \
                __LINE__))

#define RwRealloc(_p, _s, _h)                           \
   (_rwMemoryLastAllocFile = __FILE__,                  \
    _rwMemoryLastAllocLine = __LINE__,                  \
    _realloc_dbg((_p),                                  \
                 (_s),                                  \
                _CLIENT_BLOCK | ((_CLIENT_TAG)<<16),    \
                __FILE__,                               \
                __LINE__))

#define     RWCRTORDBGFLAG(_flag)                       \
    do                                                  \
    {                                                   \
        int            _DbgFlag;                        \
                                                        \
        _DbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); \
        _DbgFlag |= (_flag);                            \
        _CrtSetDbgFlag(_DbgFlag);                       \
    }   while(0)

#define VALID_HEAP_STR \
  __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): valid heap\n"

#define     RWCRTCHECKMEMORY()                          \
    do                                                  \
    {                                                   \
        int             valid_heap;                     \
                                                        \
        valid_heap = _CrtCheckMemory();                 \
        _ASSERTE(valid_heap);                           \
    }   while(0)

/*
 *      if (valid_heap)                                 \
 *          OutputDebugString(VALID_HEAP_STR);          \
 */

#define NO_LEAKS_FOUND_STR \
   __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): no heap leaks found\n"

#define     RWCRTDUMPMEMORYLEAKS()                      \
    do                                                  \
    {                                                   \
        int             leaks_found;                    \
                                                        \
        leaks_found = _CrtDumpMemoryLeaks();            \
        _ASSERTE(!leaks_found);                         \
        if (!leaks_found)                               \
            OutputDebugString(NO_LEAKS_FOUND_STR);      \
    }   while(0)

#define HEAP_DIFFERENCES_FOUND_STR \
   __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): heap differences found\n"

#define NO_DIFFERENCES_FOUND_STR \
   __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): no heap differences found\n"

#define RWCRTHEAPDIFFERENCESINCE(_Then)                                   \
    do                                                                    \
    {                                                                     \
        /* only dump differences when                                     \
         * there are in fact differences */                               \
        _CrtMemState _Now;                                                \
        _CrtMemState _Delta;                                              \
        const int _DbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);         \
        int Differences;                                                  \
                                                                          \
        _CrtMemCheckpoint(&_Now);                                         \
        _CrtMemDifference(&_Delta, _Then, &_Now);                         \
                                                                          \
        (Differences) = ( ( 0 != _Delta.lCounts[_CLIENT_BLOCK] ) ||       \
                          ( 0 != _Delta.lCounts[_NORMAL_BLOCK] ) ||       \
                          ( (_DbgFlag & _CRTDBG_CHECK_CRT_DF) &&          \
                            ( 0 != _Delta.lCounts[_CRT_BLOCK]) ) );       \
                                                                          \
        if ( (Differences) )                                              \
        {                                                                 \
            /* difference detected: dump objects since _Then. */          \
            OutputDebugString(HEAP_DIFFERENCES_FOUND_STR);                \
            _CrtMemDumpAllObjectsSince(_Then);                            \
            _CrtMemDumpStatistics(&_Delta);                               \
        }                                                                 \
        else                                                              \
        {                                                                 \
            OutputDebugString(NO_DIFFERENCES_FOUND_STR);                  \
        }                                                                 \
    }   while (0)

#define RWCRTDBGBREAK() \
    _CrtDbgBreak()

#define RWCRTDOFORALLCLIENTOBJECTS(_f, _c) \
    _CrtDoForAllClientObjects(_f, _c)

#define RWCRTISMEMORYBLOCK(_p, _t, _r, _f, _l) \
    _CrtIsMemoryBlock(_p, _t, _r, _f, _l)

#define RWCRTISVALIDHEAPPOINTER(_p) \
    _CrtIsValidHeapPointer(_p)

#define RWCRTISVALIDPOINTER(_p, _n, _r) \
    _CrtIsValidPointer(_p, _n, _r)

#define RWCRTMEMCHECKPOINT(_s) \
    _CrtMemCheckpoint(_s)

#define RWCRTMEMDIFFERENCE(_s1, _s2, _s3) \
    _CrtMemDifference(_s1, _s2, _s3)

#define RWCRTMEMDUMPALLOBJECTSSINCE(_s) \
    _CrtMemDumpAllObjectsSince(_s)

#define RWCRTMEMDUMPSTATISTICS(_s) \
    _CrtMemDumpStatistics(_s)

#define RWCRTSETALLOCHOOK(_f) \
    _CrtSetAllocHook(_f)

#define RWCRTSETBREAKALLOC(_a) \
    _CrtSetBreakAlloc(_a)

#define RWCRTSETDBGFLAG(_f) \
    _CrtSetDbgFlag(_f)

#define RWCRTSETDUMPCLIENT(_f) \
    _CrtSetDumpClient(_f)

#define RWCRTSETREPORTFILE(_t, _f) \
    _CrtSetReportFile(_t, _f)

#define RWCRTSETREPORTHOOK(_f) \
    _CrtSetReportHook(_f)

#define RWCRTSETREPORTMODE(_t, _f) \
    _CrtSetReportMode(_t, _f)

#if (!defined(_CRTDBG_FLAGS))
#define _CRTDBG_FLAGS                                           \
    ( (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF |       \
       _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF) &          \
     ~(_CRTDBG_CHECK_ALWAYS_DF |_CRTDBG_RESERVED_DF) )
#endif /* (!defined(_CRTDBG_FLAGS)) */

#  endif /* ((_MSC_VER>=1000) && defined(_DEBUG)) */
# endif /* (defined(_MSC_VER)) */



#if (!defined(rwDEADPTRFILL))
#define rwDEADPTRFILL      ((void *)0xDDDDDDDD)
#endif /* (!defined(rwDEADPTRFILL)) */

#endif /* (defined(RWDEBUG) && (defined(RWMEMDEBUG))) */

#if (!defined(rwDEADPTRFILL))
#define rwDEADPTRFILL      (NULL)
#endif /* (!defined(rwDEADPTRFILL)) */

#if (!defined(RwMalloc))
#if (!defined(RWDEBUG) || defined(DOXYGEN))
#define RwMalloc(_s, _h)                                    \
    ((RWSRCGLOBAL(memoryFuncs).rwmalloc)((_s), (_h)))
#else /* (!defined(RWDEBUG) || defined(DOXYGEN)) */
#define RwMalloc(_s, _h)                                    \
    (_rwMemoryLastAllocFile = RWSTRING(__FILE__),                     \
     _rwMemoryLastAllocLine = __LINE__,                     \
     ((RWSRCGLOBAL(memoryFuncs).rwmalloc)((_s), (_h))))
#endif /* (!defined(RWDEBUG) || defined(DOXYGEN)) */
#endif /* (!defined(RwMalloc)) */

#if (!defined(RwFree))
#define RwFree(_p)                                          \
    ((RWSRCGLOBAL(memoryFuncs).rwfree)((_p)))
#endif /* (!defined(RwFree)) */

#if (!defined(RwCalloc))
#if (!defined(RWDEBUG) || defined(DOXYGEN))
#define RwCalloc(_n, _s, _h)                                \
    ((RWSRCGLOBAL(memoryFuncs).rwcalloc)((_n), (_s), (_h)))
#else /* (!defined(RWDEBUG) || defined(DOXYGEN)) */
#define RwCalloc(_n, _s, _h)                                \
    (_rwMemoryLastAllocFile = RWSTRING(__FILE__),                     \
     _rwMemoryLastAllocLine = __LINE__,                     \
     ((RWSRCGLOBAL(memoryFuncs).rwcalloc)((_n), (_s), (_h))))
#endif /* (!defined(RWDEBUG) || defined(DOXYGEN)) */
#endif /* (!defined(RwCalloc)) */

#if (!defined(RwRealloc))
#if (!defined(RWDEBUG) || defined(DOXYGEN))
#define RwRealloc(_p, _s, _h)                               \
    ((RWSRCGLOBAL(memoryFuncs).rwrealloc)((_p),(_s), (_h)))
#else /* (!defined(RWDEBUG) || defined(DOXYGEN)) */
#define RwRealloc(_p, _s, _h)                               \
    (_rwMemoryLastAllocFile = RWSTRING(__FILE__),                     \
     _rwMemoryLastAllocLine = __LINE__,                     \
     ((RWSRCGLOBAL(memoryFuncs).rwrealloc)((_p),(_s), (_h))))
#endif /* (!defined(RWDEBUG) || defined(DOXYGEN)) */
#endif /* (!defined(RwRealloc)) */

#if (!defined(RWCRTORDBGFLAG))
#define     RWCRTORDBGFLAG(_flag) /* No op */
#endif /* (!defined(RWCRTORDBGFLAG)) */

#if (!defined(RWCRTCHECKMEMORY))
#define RWCRTCHECKMEMORY()     /* No Op */
#endif /* (!defined(RWCRTCHECKMEMORY)) */

#if (!defined(RWCRTDBGBREAK))
#define RWCRTDBGBREAK()        /* No Op */
#endif /* (!defined(RWCRTDBGBREAK)) */

#if (!defined(RWCRTDOFORALLCLIENTOBJECTS))
#define RWCRTDOFORALLCLIENTOBJECTS(_f, _c) /* No Op */
#endif /* (!defined(RWCRTDOFORALLCLIENTOBJECTS)) */

#if (!defined(RWCRTDUMPMEMORYLEAKS))
#define RWCRTDUMPMEMORYLEAKS() /* No Op */
#endif /* (!defined(RWCRTDUMPMEMORYLEAKS)) */

#if (!defined(RWCRTHEAPDIFFERENCESINCE))
#define RWCRTHEAPDIFFERENCESINCE(_Then)           /* No Op */
#endif /* (!defined(RWCRTHEAPDIFFERENCESINCE)) */

#if (!defined(RWCRTISMEMORYBLOCK))
#define RWCRTISMEMORYBLOCK(_p, _t, _r, _f, _l)    (NULL != (_p))
#endif /* (!defined(RWCRTISMEMORYBLOCK)) */

#if (!defined(RWCRTISVALIDHEAPPOINTER))
#define RWCRTISVALIDHEAPPOINTER(_p)               (NULL != (_p))
#endif /* (!defined(RWCRTISVALIDHEAPPOINTER)) */

#if (!defined(RWCRTISVALIDPOINTER))
#define RWCRTISVALIDPOINTER(_p, _n, _r)           (NULL != (_p))
#endif /* (!defined(RWCRTISVALIDPOINTER)) */

#if (!defined(RWCRTMEMCHECKPOINT))
#define RWCRTMEMCHECKPOINT(_s) /* No Op */
#endif /* (!defined(RWCRTMEMCHECKPOINT)) */

#if (!defined(RWCRTMEMDIFFERENCE))
#define RWCRTMEMDIFFERENCE(_s1, _s2, _s3) /* No Op */
#endif /* (!defined(RWCRTMEMDIFFERENCE)) */

#if (!defined(RWCRTMEMDUMPALLOBJECTSSINCE))
#define RWCRTMEMDUMPALLOBJECTSSINCE(_s) /* No Op */
#endif /* (!defined(RWCRTMEMDUMPALLOBJECTSSINCE)) */

#if (!defined(RWCRTMEMDUMPSTATISTICS))
#define RWCRTMEMDUMPSTATISTICS(_s)                (NULL)
#endif /* (!defined(RWCRTMEMDUMPSTATISTICS)) */

#if (!defined(RWCRTSETALLOCHOOK))
#define RWCRTSETALLOCHOOK(_f)                     (NULL)
#endif /* (!defined(RWCRTSETALLOCHOOK)) */

#if (!defined(RWCRTSETBREAKALLOC))
#define RWCRTSETBREAKALLOC(_a)                    (0)
#endif /* (!defined(RWCRTSETBREAKALLOC)) */

#if (!defined(RWCRTSETDBGFLAG))
#define RWCRTSETDBGFLAG(_f)                       (0)
#endif /* (!defined(RWCRTSETDBGFLAG)) */

#if (!defined(RWCRTSETDUMPCLIENT))
#define RWCRTSETDUMPCLIENT(_f)                    (NULL)
#endif /* (!defined(RWCRTSETDUMPCLIENT)) */

#if (!defined(RWCRTSETREPORTFILE))
#define RWCRTSETREPORTFILE(_t, _f)                (NULL)
#endif /* (!defined(RWCRTSETREPORTFILE)) */

#if (!defined(RWCRTSETREPORTHOOK))
#define RWCRTSETREPORTHOOK(_f)                    (NULL)
#endif /* (!defined(RWCRTSETREPORTHOOK)) */

#if (!defined(RWCRTSETREPORTMODE))
#define RWCRTSETREPORTMODE(_t, _f)                (0)
#endif /* (!defined(RWCRTSETREPORTMODE)) */

#if (!defined(RWREGSETBREAKALLOC))
#define RWREGSETBREAKALLOC(_name) /* No op */
#endif /* (!defined(RWREGSETBREAKALLOC)) */

#if (!defined(RWREGSETASSERTPRINT))
#define RWREGSETASSERTPRINT(_name) /* No op */
#endif /* (!defined(RWREGSETASSERTPRINT)) */

#if (!defined(RWGETWINREGDWORD))
#define  RWGETWINREGDWORD(_env_var, _match) /* No op */
#endif /* (!defined(RWGETWINREGDWORD)) */

#if (!defined(RWGETWINREGBINARY))
#define  RWGETWINREGBINARY(_env_var, _match) /* No op */
#endif /* (!defined(RWGETWINREGBINARY)) */

#if (!defined(RWGETWINREGSTRING))
#define  RWGETWINREGSTRING(_env_var, _match) /* No op */
#endif /* (!defined(RWGETWINREGSTRING)) */

#if (!defined(_CRTDBG_FLAGS))
#define  _CRTDBG_FLAGS 0x33
#endif /* (!defined(_CRTDBG_FLAGS)) */

/**
 * \ingroup memoryfileinterface
 * Macro for extracting chunkID from a hint. This is the id of either
 * the object for which a memory request came, or of the module a call
 * for allocation belongs to. Refer to \ref RwCorePluginID for a list of
 * all object IDs and to \ref rwengine, \ref RwCriterionCoreID
 * and \ref RwCriterionWorldID for a list of all module IDs. Refer to
 * RwMemoryFunctions for more details about the hints.
 */
#define  RwMemoryHintGetChunkID(_h) ((_h) & 0xFFFF)
/**
 * \ingroup memoryfileinterface
 * Macro for extracting duration from a hint. Refer to RwMemoryFunctions
 * for more details about the hints.
 * \see RwMemoryHintDuration
 */
#define  RwMemoryHintGetDuration(_h) ((_h) & rwMEMHINTDUR_MASK)
/**
 * \ingroup memoryfileinterface
 * Macro for extracting flags from a hint. Refer to RwMemoryFunctions
 * for more details about the hints.
 * \see RwMemoryHintFlag
 */
#define  RwMemoryHintGetFlags(_h)    ((_h) & rwMEMHINTFLAG_MASK)

/****************************************************************************
 Global Types
 */

/**
 * \ingroup memoryfileinterface
 * This type represents the memory hint flags.
 */
enum RwMemoryHintFlag
{
    /** Indicates that memory being allocated will potentially be
     *  resized with calls to \ref RwRealloc
     */
    rwMEMHINTFLAG_RESIZABLE = 0x01000000,
    rwMEMHINTFLAG_MASK      = 0xFF000000,
    rwMEMHINTFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintFlag RwMemoryHintFlag;

/**
 * \ingroup memoryfileinterface
 * This type represents the duration of a memory.
 */
enum RwMemoryHintDuration
{
    rwMEMHINTDUR_NADURATION = 0x00000000,
    /** Is used for allocating some temporary memory which
     *  will be freed inside the function scope.
     */
    rwMEMHINTDUR_FUNCTION   = 0x00010000,
    /** Is used for per frame memory allocations.
     */
    rwMEMHINTDUR_FRAME      = 0x00020000,
    /** Is used for allocations which persist for longer
     *  than per frame, but are not global. This can be per
     *  level or per event.
     */
    rwMEMHINTDUR_EVENT      = 0x00030000,
    /** Is used for all global allocations, happening on and
     *  before RwEngineStart() and which are freed on and after
     *  RwEngineStop()
     */
    rwMEMHINTDUR_GLOBAL     = 0x00040000,
    rwMEMHINTDUR_MASK       = 0x00FF0000,
    rwMEMHINTDURFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintDuration RwMemoryHintDuration;

/**
 * \ingroup memoryfileinterface
 * \struct RwMemoryFunctions
 * This type represents the memory functions used by RenderWare. The
 * application may override the default functions by passing a
 * RwMemoryFunctions structure populated with a user defined functions
 * to \ref RwEngineInit().
 *
 * All RenderWare Graphics memory-management functions that allocate
 * memory take an additional RwUInt32 parameter which is a memory hint.
 * The main purpose of the hints is to help improve memory management,
 * such as preventing memory fragmentation. To use the memory hints, you
 * should have provided your own memory-management function through the
 * RwEngineInit() function. For example, you might want to have a
 * separate memory heap just for the RwMatrix or RpGeometry objects, or
 * a separate heap for all temporary allocations inside RenderWare
 * Graphics. As always, memory-management is very specific to each
 * application and has to be organized and tuned for each one.

 * A memory hint contains the following information:
 * RwMemoryHintDuration, RwMemoryHintFlag and chunkID. ChunkID is the id
 * of either the object for which a memory request came, or of the module
 * a call for allocation belongs to. Refer to \ref RwCorePluginID for a
 * list of all object IDs and to \ref rwengine, \ref RwCriterionCoreID
 * and \ref RwCriterionWorldID for a list of all module IDs. You can extend
 * all of these fields using specific values for duration, object IDs,
 * flags and create custom hints that can be passed to all the memory
 * allocation calls you make. A tipical call to RwMalloc looks like:
 * \code RwMalloc(size, rwID_MATRIX | rwMEMHINTDUR_FUNCTION);\endcode
 *
 * \note Note that once you override these function you will be responsible
 * for aligning the memory properly. Memory alignment differs between the
 * various platforms and it can also affect the application performance.
 *
 * \see RwMemoryHintGetDuration, RwMemoryHintGetChunkID,
 *      RwMemoryHintGetFlags, RwEngineInit.
 */
struct RwMemoryFunctions
{
    /* c.f.
     * Program Files/Microsoft Visual Studio/VC98/Include/MALLOC.H
     */
    void *(*rwmalloc)(size_t size, RwUInt32 hint);
        /**< Allocates memory blocks.
         *  \param size Number of bytes to allocate. Should be greater
         *         then zero.
         *  \param hint A RwUInt32 value representing a memory hint.
         *  \return A void pointer to the allocated space, or NULL if
         *          there is insufficient memory available.
         */
    void  (*rwfree)(void *mem);
        /**< Deallocates or frees a memory block.
         *  \param mem Previously allocated memory block to be freed.
         *         Shouldn't be NULL pointer.
         */
    void *(*rwrealloc)(void *mem, size_t newSize, RwUInt32 hint);
        /**< Reallocate memory blocks.
         *  \param mem Pointer to previously allocated memory block.
         *  \param size New size in bytes. Should be greater then zero.
         *  \param hint A RwUInt32 value representing a memory hint.
         *  \return A void pointer to the allocated space, or NULL if
         *          there is insufficient memory available.
         */
    void *(*rwcalloc)(size_t numObj, size_t sizeObj, RwUInt32 hint);
        /**< Allocates an array in memory with elements initialized to 0.
         *  \param numObj Non-zero number of elements.
         *  \param sizeObj Non-zero length in bytes of each element.
         *  \param hint A RwUInt32 value representing a memory hint.
         *  \return A void pointer to the allocated space, or NULL if
         *          there is insufficient memory available.
         */
};
static_assert(sizeof(RwMemoryFunctions) == (VER_x32 ? 0x10 : 0x20));

typedef struct RwMemoryFunctions RwMemoryFunctions;

 /**
 * \ingroup rwfreelist
 * The free list was statically allocated
 *
 * \see RwFreeListSetFlags
 */
#define rwFREELISTFLAG_STATIC 0x00000001

/**
 * \ingroup rwfreelist
 * \hideinitializer
 * Free blocks as soon as they are empty.
 *
 * \see RwFreeListSetFlags
 */
#define rwFREELISTFLAG_FREEBLOCKS 0x00000002


typedef struct RwFreeList RwFreeList;

/**
 * \ingroup rwfreelist
 * Holds free list info, should be considered opaque. Use API functions to access.
 */
struct RwFreeList
{
    RwUInt32   entrySize;       /**<Size of an entry in the free list. */
#if (defined(RWDEBUG) && !defined(DOXYGEN))
    RwUInt32   nonAlignedEntrySize;
#endif /* (defined(RWDEBUG) && !defined(DOXYGEN)) */
    RwUInt32   entriesPerBlock; /**<Number of entries per free list block. */
    RwUInt32   heapSize;        /**<Size of the heap. */
    RwUInt32   alignment;       /**<Alignment of a free list entry. */
    RwLinkList blockList;       /**<List of data blocks. */
    RwUInt32   flags;           /**<Flags which affect the behavior of the
                                    free list. <BR>
                                    rwFREELISTFLAG_FREEBLOCKS */
    RwLLLink   link;            /**<Link to the free list linked list. */

#if (defined(RWDEBUG) && !defined(DOXYGEN))
    const RwChar       *fileCreate;
    RwUInt32            lineCreate;
#endif /* (defined(RWDEBUG) && !defined(DOXYGEN)) */

//@{ 20050513 DDonSS : Threadsafe
#if defined USE_THREADSAFE_FREELIST
#if defined WIN32
	CRITICAL_SECTION	criticalSection;
#else // defined WIN32
	void*				criticalSection;
#endif // defined WIN32
#endif // defined USE_THREADSAFE_FREELIST
//@} DDonSS
};
static_assert(sizeof(RwFreeList) == (VER_x32 ? 0x24 : 0x38));

#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwfreelist
 * \ref RwFreeListCallBack represents
 * the function called from \ref RwFreeListForAllUsed for all used entries in a
 * given free list.
 *
 * \param  pMem   Pointer to the start of the current entries.
 *
 * \param  pData   User-defined data pointer.
 *
 * \see RwFreeListForAllUsed
 *
 */
#endif /* RWADOXYGENEXTERNAL */
typedef void        (*RwFreeListCallBack) (void *pMem, void *pData);
typedef void       *(*RwMemoryAllocFn)    (RwFreeList * fl, RwUInt32 hint);
typedef RwFreeList *(*RwMemoryFreeFn)     (RwFreeList * fl, void *pData);

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */

#if defined(RWDEBUG)

#if defined(_RWDLL)
__declspec(dllimport) extern const RwChar   *_rwMemoryLastAllocFile;
__declspec(dllimport) extern RwUInt32       _rwMemoryLastAllocLine;
#else /* defined(_RWDLL) */
extern const RwChar *_rwMemoryLastAllocFile;
extern RwUInt32     _rwMemoryLastAllocLine;
#endif /* defined(_RWDLL) */

extern const RwChar *
RwMemoryGetLastAllocFile( void );

extern RwUInt32
RwMemoryGetLastAllocLine( void );

#endif /* defined(RWDEBUG) */

extern RwMemoryFunctions *RwOsGetMemoryInterface(void);

/*************
 * FREELISTS *
 *************/

/* Allocation and freeing */
#if (defined(RWDEBUG) && !defined(DOXYGEN))

extern RwFreeList  *_rwFreeListCreate(RwInt32 entrySize,
                                      RwInt32 entriesPerBlock,
                                      RwInt32 alignment,
                                      RwUInt32 hint,
                                      const RwChar *fileCreate,
                                      RwUInt32 lineCreate );

#define RwFreeListCreate(entrySize, entriesPerBlock,            \
                         alignment, hint)                       \
                        _rwFreeListCreate(entrySize,            \
                                      entriesPerBlock,          \
                                      alignment,                \
                                      hint,                     \
                                      __FILE__,                 \
                                      __LINE__)
#else /* (defined(RWDEBUG) && !defined(DOXYGEN)) */

/* legacy freelist create */

extern RwFreeList  *RwFreeListCreate(RwInt32 entrySize,
                                     RwInt32 entriesPerBlock,
                                     RwInt32 alignment,
                                     RwUInt32 hint);
#endif /* (defined(RWDEBUG) && !defined(DOXYGEN)) */

extern RwFreeList* 
RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize,
                                    RwInt32 entriesPerBlock,
                                    RwInt32 alignment,
                                    RwInt32 numBlocksToPreallocate,
                                    RwFreeList *inPlaceSpaceForFreeListStruct,
                                    RwUInt32 hint);

extern RwBool       RwFreeListDestroy(RwFreeList * freelist);

extern void RwFreeListSetFlags( RwFreeList *freeList, RwUInt32 flags );
extern RwUInt32 RwFreeListGetFlags( RwFreeList *freeList );

/* RWPUBLICEND */

extern void        *_rwFreeListAllocReal(RwFreeList * freelist,
                                         RwUInt32 hint);
extern RwFreeList  *_rwFreeListFreeReal(RwFreeList * freelist,
                                        void *pData);

/* RWPUBLIC */
/* Garbage collection/enumeration */
extern RwInt32      RwFreeListPurge(RwFreeList * freelist);
extern RwFreeList  *RwFreeListForAllUsed(RwFreeList * freelist,
                                         RwFreeListCallBack
                                         fpCallBack, void *pData);
extern RwInt32      RwFreeListPurgeAllFreeLists(void);

/* RWPUBLICEND */

/* Opening/Closing */
extern void         _rwMemoryClose(void);
extern RwBool       _rwMemoryOpen(const RwMemoryFunctions * memFuncs);
extern void         _rwFreeListEnable(RwBool enabled);

/* RWPUBLIC */
#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#if (defined(RWDEBUG) && defined(RWNOFREELISTS) && !defined(RWKEEPFREELISTS))

#if ((defined(__MWERKS__) || defined(__GNUC__)) && defined(__R5900__))

/*
 * for more on memalign, see
 * http://www.gnu.org/manual/glibc-2.0.6/html_chapter/libc_3.html#SEC28
 */
#include <rtdbmalloc.h>

#define RwFreeListAlloc(_f, _h)     \
    memalign(((_f)->alignment),  (_f)->entrySize)

#else /* ((defined(__MWERKS__) || defined(__GNUC__)) && defined(__R5900__)) */

#define RwFreeListAlloc(_f, _h)     \
    RwMalloc(((_f)->entrySize), (_h))

#endif /* ((defined(__MWERKS__) || defined(__GNUC__)) && defined(__R5900__)) */

#define RwFreeListFree(_f, _p)  \
    RwFree((_p))

#endif /* (defined(RWDEBUG) && defined(RWNOFREELISTS) && !defined(RWKEEPFREELISTS)) */

#if (!defined(RwFreeListAlloc))
#define RwFreeListAlloc(_f, _h)     \
    RWSRCGLOBAL(memoryAlloc)((_f), (_h))
#endif /* (!defined(RwFreeListAlloc)) */

#if (!defined(RwFreeListFree))
#define RwFreeListFree(_f, _p)  \
    RWSRCGLOBAL(memoryFree)(_f, _p)
#endif /* (!defined(RwFreeListFree)) */

/* RWPUBLICEND */

#endif                          /* RWMEMORY_H */

