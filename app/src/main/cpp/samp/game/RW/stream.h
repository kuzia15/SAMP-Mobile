/***************************************************************************
 * Module  : bastream.h                                                    *
 *                                                                         *
 * Purpose : Stream handling                                               *
 *                                                                         *
 **************************************************************************/

#pragma once

/****************************************************************************
 Includes
 */

#include "memory.h"

/* RWPUBLIC */

/****************************************************************************
 Defines
 */

#define rwSTREAMSTACKSIZE    512

//@{ 20050509 DDonSS
// if (RwFseek(fp, offset, SEEK_CUR))�� ���� �߰�
#ifndef SEEK_SET
#  define SEEK_SET        0       /* Seek from beginning of file.  */
#  define SEEK_CUR        1       /* Seek from current position.  */
#  define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif
//@} DDonSS

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwstream
 * \ref RwStreamType 
 * This type represents the different types of stream that 
 * can be used. 
 * See API section \ref rwstream 
 */
enum RwStreamType
{
    rwNASTREAM = 0,     /**<Invalid stream type */
    rwSTREAMFILE,       /**<File */
    rwSTREAMFILENAME,   /**<File name */
    rwSTREAMMEMORY,     /**<Memory*/
    rwSTREAMCUSTOM,     /**<Custom */
    rwSTREAMTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamType RwStreamType;

/**
 * \ingroup rwstream
 * \ref RwStreamAccessType 
 * This type represents the options available for 
 * accessing a stream when it is opened.
 * See API section \ref rwstream */
enum RwStreamAccessType
{
    rwNASTREAMACCESS = 0,   /**<Invalid stream access */
    rwSTREAMREAD,           /**<Read */
    rwSTREAMWRITE,          /**<Write */
    rwSTREAMAPPEND,         /**<Append */
    rwSTREAMACCESSTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamAccessType RwStreamAccessType;

/* Memory stream */
/**
 * \ingroup rwstream
 * \struct RwStreamMemory
 * This should be considered an opaque type.
 * Use the RwStream API functions to access.
 */
typedef struct RwStreamMemory RwStreamMemory;
#if (!defined(DOXYGEN))
struct RwStreamMemory
{
    RwUInt32            position; /* Current 'memory' position 0 is first byte */
    RwUInt32            nSize;  /* Space allocated currently */
    RwUInt8            *memBlock; /* Current memory block pointer */
};
static_assert(sizeof(RwStreamMemory) == (VER_x32 ? 0xC : 0x10));
#endif /* (!defined(DOXYGEN)) */


typedef union RwStreamFile RwStreamFile;
/**
 * \ingroup rwstream
 * \union RwStreamFile 
 * This type is used to represent a file pointer for
 * accessing data on disk through the stream mechanism.
 * See API section \ref rwstream. */
union RwStreamFile
{
    void    *fpFile;               /**< file pointer */
    const void    *constfpFile;    /**< const file pointer */
};


/* Custom stream function pointer types */
typedef             RwBool(*rwCustomStreamFnClose) (void *data);
typedef             RwUInt32(*rwCustomStreamFnRead) (void *data, void *buffer,
                                                     RwUInt32 length);
typedef             RwBool(*rwCustomStreamFnWrite) (void *data,
                                                    const void *buffer,

                                                    RwUInt32 length);
typedef             RwBool(*rwCustomStreamFnSkip) (void *data,

                                                   RwUInt32 offset);

/* Custom stream */
/**
 * \ingroup rwstream
 * \struct  RwStreamCustom
 * This should be considered an opaque type.
 * Use the RwStream API functions to access.
 */
typedef struct RwStreamCustom RwStreamCustom;
#if (!defined(DOXYGEN))
struct RwStreamCustom
{
    rwCustomStreamFnClose sfnclose;
    rwCustomStreamFnRead sfnread;
    rwCustomStreamFnWrite sfnwrite;
    rwCustomStreamFnSkip sfnskip;
    void               *data;
};
static_assert(sizeof(RwStreamCustom) == (VER_x32 ? 0x14 : 0x28));
#endif /* (!defined(DOXYGEN)) */

/* Stream */

typedef union RwStreamUnion RwStreamUnion;
/**
 * \ingroup rwstream
 * \union RwStreamUnion
 * The union of all supported stream types
 */
union RwStreamUnion
{
    RwStreamMemory      memory; /**< memory */
    RwStreamFile        file; /**< file */
    RwStreamCustom      custom; /**< custom */
};

/**
 * \ingroup rwstream
 * \struct RwStream 
 * Binary stream for reading or writing object data. 
 * This should be considered an opaque type.
 * Use the RwStream API functions to access.
 */
typedef struct RwStream RwStream;
#if (!defined(DOXYGEN))
struct RwStream
{
    RwStreamType        type;
    RwStreamAccessType  accessType;
    RwInt32             position;
    RwStreamUnion       Type;
    RwBool              rwOwned;
};
static_assert(sizeof(RwStream) == (VER_x32 ? 0x24 : 0x40));
#endif /* (!defined(DOXYGEN)) */


typedef struct RwMemory RwMemory;
/**
 * \ingroup rwstream
 * \struct RwMemory
 * This type represents a block of allocated memory.
 * It is used to specify an area of memory connected to a stream of type
 * rwSTREAMMEMORY.
 * \see \ref rwstream */
struct RwMemory
{
    RwUInt8     *start; /**< Starting address. */
    RwUInt32    length; /**< Length in bytes. */
};
static_assert(sizeof(RwMemory) == (VER_x32 ? 0x8 : 0x10));

/* RWPUBLICEND */

typedef struct rwStreamGlobals rwStreamGlobals;
struct rwStreamGlobals
{
    RwFreeList         *streamFreeList;
};
static_assert(sizeof(rwStreamGlobals) == (VER_x32 ? 0x4 : 0x8));

/* RWPUBLIC */

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */

extern void
RwStreamSetFreeListCreateParams( RwInt32 blockSize, RwInt32 numBlocksToPrealloc );

/* Open/Close streams */

extern RwStream *
_rwStreamInitialize(RwStream *stream,
                    RwBool rwOwned,
                    RwStreamType type,
                    RwStreamAccessType accessType,
                    const void *pData);

extern RwStream *
RwStreamOpen(RwStreamType type,
             RwStreamAccessType accessType,
             const void *pData);

extern RwBool       
RwStreamClose(RwStream * stream,
              void *pData);


/* Stream read/write */
extern RwUInt32     
RwStreamRead(RwStream * stream,
             void *buffer,
             RwUInt32 length);

extern RwStream *
RwStreamWrite(RwStream * stream,
              const void *buffer,
              RwUInt32 length);


/* Stream skipping */
extern RwStream *
RwStreamSkip(RwStream * stream,
             RwUInt32 offset);


/* RWPUBLICEND */

/* Opening and closing stream module */
extern void *
_rwStreamModuleOpen(void *instance,
                    RwInt32 offset,
                    RwInt32 size);

extern void *
_rwStreamModuleClose(void *instance,
                     RwInt32 offset,
                     RwInt32 size);


/* RWPUBLIC */
#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

/* RWPUBLICEND */
