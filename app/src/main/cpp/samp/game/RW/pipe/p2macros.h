/****************************************************************************
 *                                                                          *
 *  Module  :   p2macros.h                                                  *
 *                                                                          *
 *  Purpose :   useful macros for nodes (mainly cluster access)             *
 *                                                                          *
 ****************************************************************************/

#ifndef P2MACROS_H
#define P2MACROS_H


/****************************************************************************
 Global Defines
 */

/* TODO: have embedded ASSERTs in these macros like:
 *       (cluster->numUsed < cluster->numAlloced) */

/* TODO: try and efficiently support clusters with both
 * power-of-two strides (can use a shifting macro to index)
 * and non-2^N strides - uses multiplication. Check a
 * cluster when you first get it to see which MACRO to use */

/* RWPUBLIC */

#define RxClusterDecCursorByStride(_cluster, _stride)                           \
    ((_cluster)->currentData =                                                  \
      (void *)(((RwUInt8 *)(_cluster)->currentData) -                           \
         (_stride)))

#define RxClusterDecCursor(_cluster) \
    RxClusterDecCursorByStride(_cluster, (_cluster)->stride)

#define RxClusterIncCursorByStride(_cluster, _stride)                           \
    ((_cluster)->currentData =                                                  \
     (void *)(((RwUInt8 *)(_cluster)->currentData) +                            \
              (_stride)))

#define RxClusterIncCursor(_cluster) \
    RxClusterIncCursorByStride(_cluster, (_cluster)->stride)

#define RxClusterResetCursor(_cluster) \
    ((_cluster)->currentData = (_cluster)->data)

#define RxClusterGetCursorData(_cluster, _type) \
    ((_type *)(_cluster)->currentData)

#define RxClusterGetIndexedData(_cluster, _type, _index)                        \
    ((_type *)(((RwUInt8 *)(_cluster)->data) + (_cluster)->stride*(_index)))

#define RxClusterGetFreeIndex(_cluster) ( (_cluster)->numUsed++ )

#define RxPipelineClusterAssertAttributeSet(_cluster, _attributeSet)    \
    RWASSERT( (_cluster)->clusterRef->attributeSet != NULL &&           \
              rwstrcmp((_cluster)->clusterRef->attributeSet,              \
                     (_attributeSet)) == 0 )

#define RxPipelineNodeParamGetData(_param) \
    ( (_param)->dataParam )

#define RxPipelineNodeParamGetHeap(_param) \
    ( (_param)->heap )


/* RWPUBLICEND */

#endif /* P2MACROS_H */
