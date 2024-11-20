/*                                                                         *
 * Module  : batkreg.h                                                     *
 *                                                                         *
 * Purpose : plugin regsitry                                               *
 *                                                                         *
 **************************************************************************/

#ifndef RWTKREG_H
#define RWTKREG_H

/****************************************************************************
 Includes
 */

#include "stream.h"

/****************************************************************************
 Defines
 */

/* RWPUBLIC */

/****************************************************************************
 Global Types
 */
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkWriteCallBack represents the function
 * registered by \ref RwCameraRegisterPluginStream, etc. as the function that
 * writes extension data to a binary stream.
 *
 * \param  stream   Pointer to the binary stream
 *
 * \param  binaryLength   A RwInt32 value equal to the binary
 * size (in bytes) of the extension data that will be written to the binary
 * stream.
 *
 * \param  object   Pointer to the object containing the extension
 * data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the stream
 */
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkReadCallBack represents the function
 * registered by \ref RwCameraRegisterPluginStream, etc. as the function that
 * reads extension data from a binary stream.
 *
 * \param  stream   Pointer to the binary stream
 *
 * \param  binaryLength   A RwInt32 value equal to the binary
 * size (in bytes) of the extension data that will be read from a binary
 * stream.
 *
 * \param  object   Pointer to the object containing the extension
 * data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the stream
 */
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkGetSizeCallBack represents the callback
 * registered by \ref RwCameraRegisterPluginStream, etc. as the function that
 * determines the binary size of the extension data.
 *
 * \param  object   Pointer to the object containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return A RwInt32 value equal to the size in bytes of the plugin extension data.
 */
typedef RwInt32(*RwPluginDataChunkGetSizeCallBack)(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkAlwaysCallBack represents the callback
 * registered by \ref RwCameraSetStreamAlwaysCallBack, etc. as the
 * function that is called after the reading of plugin stream data is
 * finished (useful to set up plugin data for plugins that found no
 * data in the stream, but that cannot set up the data during the
 * \ref RwPluginObjectConstructor callback).
 *
 * \param  object   Pointer to the object containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Returns TRUE if successful, FALSE otherwise.
 */
typedef RwBool(*RwPluginDataChunkAlwaysCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkRightsCallBack represents the callback
 * registered by RwCameraSetStreamRightsCallBack, etc. as the
 * function that is called after the reading of plugin stream data is
 * finished, and the object finalised, if and only if the object's rights
 * id was equal to that of the plugin registering the call.
 * For convience the extension data is passed to the callback.
 *
 * \param  object   Pointer to the object containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \param  extraData     An RwUInt32 writen with the plugin id.
 *
 * \return Returns TRUE if successful, FALSE otherwise.
 */
typedef RwBool(*RwPluginDataChunkRightsCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject, RwUInt32 extraData);

/**
 * \ingroup rwplugin
 * \ref RwPluginObjectConstructor represents the callback
 * registered by \ref RwEngineRegisterPlugin, \ref RwCameraRegisterPlugin, etc.
 * as the function that initializes either the global extension data (in the
 * case of \ref RwEngineRegisterPlugin) or the object extension data (in all
 * other cases). Registered by \ref RwCameraSetStreamAlwaysCallBack, etc.
 *
 * \param  object   Pointer to the object (global or otherwise)
 * that contains the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the
 * byte offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the object
 */
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginObjectCopy represents the callback registered by
 * \ref RwCameraRegisterPlugin, etc. as the function that copies the object
 * extension data when an object is duplicated.
 *
 * \param  dstObject   Pointer to the destination object that will
 * receive the extension data.
 *
 * \param  srcObject   Pointer to the source object containing
 * extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte offset
 * of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the object
 */
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginObjectDestructor represents the callback registered
 * by \ref RwEngineRegisterPlugin, \ref RwCameraRegisterPlugin, etc. as the
 * function that destroys either the global extension data (in the case of
 * \ref RwEngineRegisterPlugin) or the object extension data (in all other
 * cases).
 *
 * \param  object   Pointer to the object (global or otherwise)
 * containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the object.
 */
#endif /* RWADOXYGENEXTERNAL */
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

typedef void *(*RwPluginErrorStrCallBack)(void *);

typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;

#if (!defined(DOXYGEN))
struct RwPluginRegistry
{
    RwInt32          sizeOfStruct;
    RwInt32          origSizeOfStruct;
    RwInt32          maxSizeOfStruct;
    RwInt32          staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};
static_assert(sizeof(RwPluginRegistry) == (VER_x32 ? 0x18 : 0x20));

struct RwPluginRegEntry
{
    RwInt32         offset;
    RwInt32         size;
    RwUInt32        pluginID;
    RwPluginDataChunkReadCallBack readCB;
    RwPluginDataChunkWriteCallBack writeCB;
    RwPluginDataChunkGetSizeCallBack getSizeCB;
    RwPluginDataChunkAlwaysCallBack alwaysCB;
    RwPluginDataChunkRightsCallBack rightsCB;
    RwPluginObjectConstructor constructCB;
    RwPluginObjectDestructor destructCB;
    RwPluginObjectCopy copyCB;
    RwPluginErrorStrCallBack errStrCB;
    RwPluginRegEntry *nextRegEntry;
    RwPluginRegEntry *prevRegEntry;
    RwPluginRegistry *parentRegistry;
};
static_assert(sizeof(RwPluginRegEntry) == (VER_x32 ? 0x3C : 0x70));
#endif /* (!defined(DOXYGEN)) */



/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern          "C"
{
#endif         /* __cplusplus */

/* RWPUBLICEND */

/* Opening/Closing */
extern RwBool
_rwPluginRegistryOpen(void);
extern RwBool
_rwPluginRegistryClose(void);

/* RWPUBLIC */

/* Registering toolkits and allocating memory */
extern void
RwPluginRegistrySetFreeListCreateParams( RwInt32 blockSize, RwInt32 numBlocksToPrealloc );

extern RwBool
_rwPluginRegistrySetStaticPluginsSize(RwPluginRegistry * reg,
                                      RwInt32 size);
extern RwInt32
_rwPluginRegistryAddPlugin(RwPluginRegistry * reg,
                           RwInt32 size,
                           RwUInt32 pluginID,
                           RwPluginObjectConstructor constructCB,
                           RwPluginObjectDestructor destructCB,
                           RwPluginObjectCopy copyCB);
extern RwInt32
_rwPluginRegistryGetPluginOffset(const RwPluginRegistry *reg,
                                RwUInt32 pluginID);

/* Initializing/De-initializing instances */
extern const
RwPluginRegistry *_rwPluginRegistryInitObject(const RwPluginRegistry * reg,
                                             void *object);
extern const
RwPluginRegistry *_rwPluginRegistryDeInitObject(const RwPluginRegistry * reg,
                                               void *object);
extern const
RwPluginRegistry *_rwPluginRegistryCopyObject(const RwPluginRegistry * reg,
                                             void *dstObject,
                                             const void *srcObject);

#ifdef RWDEBUG
extern RwBool
_rwPluginRegistryValidateObject(const RwPluginRegistry * reg,
                               const void *object);
extern void
_rwPluginRegistryPlaceValidationCodes(const RwPluginRegistry * reg,
                                void *object);

#endif         /* RWDEBUG */


#ifdef    __cplusplus
}
#endif         /* __cplusplus */

/* Compatibility macros */

#define rwPluginRegistryOpen() \
       _rwPluginRegistryOpen()
#define rwPluginRegistryClose() \
       _rwPluginRegistryClose()
#define rwPluginRegistrySetStaticPluginsSize(reg, size) \
       _rwPluginRegistrySetStaticPluginsSize(reg, size)
#define rwPluginRegistryAddPlugin(reg, size, pluginID, constructCB, destructCB, copyCB) \
       _rwPluginRegistryAddPlugin(reg, size, pluginID, constructCB, destructCB, copyCB)
#define rwPluginRegistryGetPluginOffset(reg, pluginID) \
       _rwPluginRegistryGetPluginOffset(reg, pluginID)
#define rwPluginRegistryInitObject(reg, object) \
       _rwPluginRegistryInitObject(reg, object)
#define rwPluginRegistryDeInitObject(reg, object) \
       _rwPluginRegistryDeInitObject(reg, object)
#define rwPluginRegistryCopyObject(reg, dstObject, srcObject) \
       _rwPluginRegistryCopyObject(reg, dstObject, srcObject)
#define rwPluginRegistryValidateObject(reg, object) \
       _rwPluginRegistryValidateObject(reg, object)

/* RWPUBLICEND */

#endif         /* RWTKREG_H */

