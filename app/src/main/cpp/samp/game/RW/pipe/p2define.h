#pragma once

#include "p2core.h"

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeOutput 
 * typedef for a reference to an output of a pipeline node */
typedef RwUInt32       *RxNodeOutput;

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeInput 
 *  typedef for a reference to the input of a pipeline node */
typedef RxPipelineNode *RxNodeInput;

/**
 * \ingroup rwcoregeneric
 * \ref RxLockedPipe
 * typedef for a reference to a locked pipeline 
 */
typedef RxPipeline      RxLockedPipe;

/* RWPUBLICEND */

/****************************************************************************
 Global Prototypes
 */

/* RWPUBLIC */

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* RWPUBLICEND */

/* Used in _PropagateDependenciesAndKillDeadPaths() -> _ReqCreate() */

extern RwUInt32
PipelineCalcNumUniqueClusters(RxPipeline *pipe);

/* Used in p2dep.c */

extern void *
StalacTiteAlloc(RwUInt32 size);

extern void *
StalacMiteAlloc(RwUInt32 size);

/* RWPUBLIC */

/* PIPELINENODE API */

extern RxNodeOutput
RxPipelineNodeFindOutputByName(RxPipelineNode *node,
                               const RwChar *outputname);

extern RxNodeOutput
RxPipelineNodeFindOutputByIndex(RxPipelineNode *node,
                                RwUInt32 outputindex);

extern RxNodeInput
RxPipelineNodeFindInput(RxPipelineNode *node);

extern RxNodeDefinition *
RxPipelineNodeCloneDefinition(RxPipelineNode *node,
                              RxClusterDefinition *cluster2add);

extern RxPipeline *
RxPipelineNodeRequestCluster(RxPipeline *pipeline,
                             RxPipelineNode *node,
                             RxClusterDefinition *clusterDef);

extern RxPipeline *
RxPipelineNodeReplaceCluster(RxPipeline *pipeline,
                             RxPipelineNode *node,
                             RxClusterDefinition *oldClusterDef,
                             RxClusterDefinition *newClusterDef);

extern void *
RxPipelineNodeGetInitData(RxPipelineNode *node);

extern void *
RxPipelineNodeCreateInitData(RxPipelineNode *node,
                             RwUInt32 size);

/* PIPELINE MANIPULATION API */

extern RxPipeline *
RxPipelineClone(RxPipeline *pipeline);

extern RxPipelineNode *
RxPipelineFindNodeByName(RxPipeline *pipeline,
                         const RwChar *name,
                         RxPipelineNode *start,
                         RwInt32 *nodeIndex);

extern RxPipelineNode *
RxPipelineFindNodeByIndex(RxPipeline *pipeline,
                          RwUInt32 nodeindex);

extern RxLockedPipe *
RxPipelineLock(RxPipeline *pipeline);

extern RxPipeline *
RxLockedPipeUnlock(RxLockedPipe *pipeline);


extern RxLockedPipe *
RxLockedPipeAddFragment(RxLockedPipe *pipeline,
                        RwUInt32 *firstIndex,
                        RxNodeDefinition *nodeDef0, RwUInt32 nodeUnk = 0U);


extern RxPipeline *
RxLockedPipeReplaceNode(RxLockedPipe *pipeline,
                        RxPipelineNode *node,
                        RxNodeDefinition *nodeDef);

extern RxPipeline *
RxLockedPipeDeleteNode(RxLockedPipe *pipeline,
                       RxPipelineNode *node);


extern RxPipeline *
RxLockedPipeSetEntryPoint(RxLockedPipe *pipeline,
                          RxNodeInput in);


extern RxPipelineNode *
RxLockedPipeGetEntryPoint(RxLockedPipe *pipeline);


extern RxPipeline *
RxLockedPipeAddPath(RxLockedPipe *pipeline,
                    RxNodeOutput out,
                    RxNodeInput in);

extern RxPipeline *
RxLockedPipeDeletePath(RxLockedPipe *pipeline,
                       RxNodeOutput out,
                       RxNodeInput in);


extern RxPipeline *
RxPipelineInsertDebugNode(RxPipeline *pipeline,
                          RxPipelineNode *before,
                          RxPipelineNode *after,
                          RxNodeDefinition *debugNode);

#ifdef    __cplusplus
}
#endif /* __cplusplus */