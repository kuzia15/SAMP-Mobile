//
// Created by x1y2z on 02.05.2024.
//

#include "p2define.h"
#include "../vendor/armhook/patch.h"
#include "pipe.h"
#include "balibtyp.h"
#include "opengl/openglpipepriv.h"
#include "rwcore.h"

/**
 * \ingroup rwengine
 * The Core Library is consisted from a collection of plugins that constitute
 * it. This is a list of the pluginIDs that belong to the Core. See
 * \ref RwEngineRegisterPlugin
 */
enum RwCriterionCoreID
{
    /* Guard value that should not be used. */
    rwID_NACOREID               = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x00),

    /* The valid plugin IDs */
    /** Vector module pluginID. See \ref rwv2d and \ref rwv3d */
    rwID_VECTORMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x01),
    /** Matrix module pluginID. See \ref rwmatrix */
    rwID_MATRIXMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x02),
    /** Frame module pluginID. See \ref rwframe */
    rwID_FRAMEMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x03),
    /** Stream module pluginID. See \ref rwstream */
    rwID_STREAMMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x04),
    /** Camera module pluginID. See \ref rwcamera */
    rwID_CAMERAMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x05),
    /** Image module pluginID. See \ref rwimage */
    rwID_IMAGEMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x06),
    /** Raster module pluginID. See \ref rwraster */
    rwID_RASTERMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x07),
    /** Texture module pluginID. See \ref rwtexture */
    rwID_TEXTUREMODULE          = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x08),
    /** Pipeline module pluginID. See \ref RxPipeline */
    rwID_PIPEMODULE             = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x09),
    /** Immediate module pluginID. See \ref rwim3d */
    rwID_IMMEDIATEMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0A),
    /** Resource module pluginID. See \ref rwresources */
    rwID_RESOURCESMODULE        = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0B),
    /** Device module pluginID */
    rwID_DEVICEMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0C),
    /** Color module pluginID. See \ref rwrgba */
    rwID_COLORMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0D),
    /* Not used */
    rwID_POLYPIPEMODULE         = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0E),
    /** Error module pluginID. See \ref rwerror */
    rwID_ERRORMODULE            = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0F),
    /** Metrics module pluginID. See \ref RwMetrics */
    rwID_METRICSMODULE          = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x10),
    /** Driver module pluginID */
    rwID_DRIVERMODULE           = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x11),
    /** Chunk group module pluginID. See \ref rwchunkgroup */
    rwID_CHUNKGROUPMODULE       = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x12),
    rwCRITERIONCOREIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionCoreID RwCriterionCoreID;

typedef struct P2MemoryLimits P2MemoryLimits;
struct P2MemoryLimits
{
    RwUInt8 *StalacTiteBase;
    RwUInt8 *StalacMiteBase;
};

typedef struct tagTopSortData TopSortData;
struct tagTopSortData
{
    RxPipeline         *pipeline;
    RwUInt32            nodesArraySlot;
};

/****************************************************************************
 Local Defines
 */

/* by their NodeRefs shall ye know them */
#define ISNODELIVE(_node) ( NULL != ( (_node)->nodeDef) )

#define ALIGN(n) ( ((n) + 3U) & ~3U )

#define RWPLUGINOFFSET(_type, _base, _offset)                   \
   ((_type *)((RwUInt8 *)(_base) + (_offset)))

/* NOTE: we're very careful with parentheses here because
 *       the precendence relationships dictate that:
 * "a ? b : c = d" => "(a ? b : c) = d"
 *       because people will want to assign to/from the
 *       result of the "?:" operator. Also note that:
 * "a ? b = c : d" => "a ? (b = c) : d"
 *        because "b = c" is INSIDE the "?:" operator and
 *        so is correctly turned into a sub-experession */
/* For fixup of [non-NULL] pointers: */
#define _PTRINC(_ptr, _type, _numbytes)                      \
    (((_type *)NULL == (_ptr))?                              \
     ((_type *)NULL):                                        \
     ((_ptr) = (_type *)(((RwUInt8 *)_ptr) + (_numbytes))) )

#define CalcTopSortDataMemSize()                                            \
     /* An array of topSortData structs, one per node */                    \
    ( sizeof(RxPipelineNodeTopSortData)*RXPIPELINEGLOBAL(maxNodesPerPipe) )

/* The memory required for the three conservatively-large arrays allocated
 * in RxPipelineLock() (pipeline nodes, node output indices and topSortData): */
#define CalcNodeOutputsTopSortDataUnCompactedMemSize()                     \
     /* The pipeline nodes array */                                         \
    (sizeof(RxPipelineNode           )*RXPIPELINEGLOBAL(maxNodesPerPipe) +  \
     /* The outputs arrays for each node (max. possible size) */            \
     sizeof(RwUInt32)*RXNODEMAXOUTPUTS*RXPIPELINEGLOBAL(maxNodesPerPipe) +  \
     /* An array of topSortData structs, one per node */                    \
     CalcTopSortDataMemSize()                                              )

/* Temporary memory required by dep-chasing [_PropagateDependenciesAndKillDeadPaths,
 * _ForAllNodeReqsAddOutputClustersAndBuildContinuityBitfields, _TraceClusterScopes
 * and _AssignClusterSlots], a conservative over-estimate: */
#define CalcTempDepChaseMemSize(_pipeParam, _numClustersParam)              \
    /* One rxReq per node */                                                \
    ( _pipeParam->numNodes*sizeof(rxReq) +                                  \
    /* Per rxReq we have an array of numUniqueClusters RwReqEntrys */       \
      _pipeParam->numNodes*_numClustersParam*sizeof(RwReqEntry) +           \
    /* One scope trace per reqentry per node */                             \
      _pipeParam->numNodes*_numClustersParam*sizeof(RwScopeTrace) )         \

#ifdef RWDEBUG
/* Space for trample-detecting magic value and its alignment */
#define tramplingMagicValueSpace (2*sizeof(RwUInt32) - 1)
#else /* RWDEBUG */
#define tramplingMagicValueSpace (0)
#endif /* RWDEBUG */

#define RWSRCGLOBAL(variable) \
   (((RwGlobals *)RwEngineInstance)->variable)

/****************************************************************************
 Local Variables
 */

/* See RxLockedPipeUnlock,
 * used during unlock for memory allocation - NOT threadsafe! */
static P2MemoryLimits gMemoryLimits = { (RwUInt8 *) NULL, (RwUInt8 *) NULL };

/*****************************************************************************
 PipelineTopSort
 - called from RxLockedPipeUnlock() (& from self!)
*/
static void                /* RECURSIVE */
PipelineTopSort(TopSortData *data, RwUInt32 nodeIndex)
{
    RxPipelineNode *curNode;
    RwUInt32 i = data->nodesArraySlot;
    RwUInt32 j = nodeIndex;

    RWFUNCTION(RWSTRING("PipelineTopSort"));

    /* Move this node to its final destination in the post-sort array.
     * To do this, we swap two nodes, and their outputs arrays and
     * topSortData, in the conservatively large, uniform arrays
     * created and described in RxPipelineLock() */
    if (i != j)
    {
        RwUInt32                   tmpOutput;
        RwUInt32                  *outputsI,     *outputsJ;
        RxPipelineNodeTopSortData  tempTopSortData;
        RxPipelineNodeTopSortData *topSortDataI, *topSortDataJ;
        RxPipelineNode             tmpNode;
        RwUInt32                   k, l;

        /* Exchange outputs arrays */
        outputsI = data->pipeline->nodes[i].outputs;
        outputsJ = data->pipeline->nodes[j].outputs;
        for (k = 0;k < RXNODEMAXOUTPUTS;k++)
        {
            tmpOutput   = outputsI[k];
            outputsI[k] = outputsJ[k];
            outputsJ[k] = tmpOutput;
        }
        /* Fix up outputs array pointers */
        data->pipeline->nodes[i].outputs = outputsJ;
        data->pipeline->nodes[j].outputs = outputsI;


        /* Exchange topSortData for this node */
        topSortDataI = data->pipeline->nodes[i].topSortData;
        topSortDataJ = data->pipeline->nodes[j].topSortData;
        tempTopSortData = *topSortDataI;
        *topSortDataI    = *topSortDataJ;
        *topSortDataJ    =  tempTopSortData;
        /* Fix up topSortData pointers */
        data->pipeline->nodes[i].topSortData = topSortDataJ;
        data->pipeline->nodes[j].topSortData = topSortDataI;


        /* Exchange node data */
        tmpNode = data->pipeline->nodes[i];
        data->pipeline->nodes[i] = data->pipeline->nodes[j];
        data->pipeline->nodes[j] = tmpNode;
        /* Fix up output indices */
        for (k = 0;k < data->pipeline->numNodes;++k)
        {
            RxPipelineNode *node = &(data->pipeline->nodes[k]);
            for (l = 0;l < node->numOutputs;++l)
            {
                if (node->outputs[l] == i)
                {
                    node->outputs[l] = j;
                }
                else
                if (node->outputs[l] == j)
                {
                    node->outputs[l] = i;
                }
            }
        }
    }

    curNode = &(data->pipeline->nodes[data->nodesArraySlot]);
    ++data->nodesArraySlot;

    if (curNode->numOutputs != 0)
    {
        for (i = 0;i < curNode->numOutputs;++i)
        {
            RwUInt32 outIndex = curNode->outputs[i];

            if (outIndex != (RwUInt32)-1)
            {
                RxPipelineNode *outNode = &(data->pipeline->nodes[outIndex]);

                /* If all of this output node's inputs have been processed
                 * (including the current node, right now) then traverse
                 * down, that node can be next in the post-sort array */
                ++outNode->topSortData->numInsVisited;
                if (outNode->topSortData->numIns ==
                    outNode->topSortData->numInsVisited)
                {
                    PipelineTopSort(data, outIndex);
                }
            }
        }
    }

    RWRETURNVOID();
}

static RwUInt32
CalcNodesOutputsCompactedMemSize(RxPipeline *pipeline)
{
    RwUInt32 size, i;

    /* Calc the memory required for the pipeline nodes array and the output
     * indices of its nodes, once compacted in RxLockedPipeUnlock(): */

    RWFUNCTION(RWSTRING("CalcNodesOutputsCompactedMemSize"));

    size = pipeline->numNodes*sizeof(RxPipelineNode);
    for (i = 0;i < pipeline->numNodes;++i)
    {
        size += pipeline->nodes[0].numOutputs*sizeof(RwUInt32);
    }

    RWRETURN(size);
}

static RwUInt32
CalcUnlockPersistentMemSize(RxPipeline *pipeline, RwUInt32 numClusters)
{
    RwUInt32        blockSize = 0;
    RxPipelineNode *node;
    RwUInt32        i;

    /* Calc the amount of memory required by _ForAllNodesWriteClusterAllocations
     * for all memory which will persist after unlock (doesn't count the
     * compacted pipeline nodes and node output indices arrays): */

    RWFUNCTION(RWSTRING("CalcUnlockPersistentMemSize"));


    /* The RxPipelineClusters (n separate allocs but they end up in an */
    /* array anyway - see _MyEnumPipelineClustersCallBack())           */
    blockSize += numClusters*sizeof(RxPipelineCluster);
    /* The input requirements of the *pipeline* */
    blockSize += numClusters*sizeof(RxPipelineRequiresCluster);
    /* An array of pointers to the appropriate RxPipelineCluster */
    /* for each cluster (used by the packet)                     */
    blockSize += pipeline->numNodes*numClusters*sizeof(RxPipelineCluster *);
    /* A bit-field per cluster for which outputs it continues through   */
    /* [the first entry is the AND of all subsequent ones and is used  */
    /*  to optimise EMBEDDEDPACKETBETWEENNODESMACRO() - if a given bit */
    /*  (output) is '1' then ALL clusters persist through that output] */
    blockSize += pipeline->numNodes*(numClusters + 1)*sizeof(RwUInt32);

    /* Sum up private data size and the lengths */
    /* of the inputToClusterSlot arrays         */
    for (i = 0;i < pipeline->numNodes;++i)
    {
        node = &(pipeline->nodes[i]);
        if (node->nodeDef->pipelineNodePrivateDataSize != 0)
        {
            blockSize += tramplingMagicValueSpace +
                         node->nodeDef->pipelineNodePrivateDataSize;
        }
        blockSize += sizeof(RwUInt32)*node->nodeDef->io.numClustersOfInterest;
    }

    /* The 'embedded' packet and its cluster array */
    blockSize += sizeof(RxPacket) + sizeof(RxCluster)*(numClusters - 1);

    /* C'est fait */
    RWRETURN(blockSize);
}

static RwBool
ReallocAndFixupSuperBlock(RxPipeline *pipeline, RwUInt32 newSize)
{
    void*& RwEngineInstance = *reinterpret_cast<void**>(g_libGTASA + (VER_x32 ? 0x6BCD38 : 0x89A358));
    RwInt32& _rxPipelineGlobalsOffset = *reinterpret_cast<RwInt32*>(g_libGTASA + (VER_x32 ? 0x6BCF98 : 0x89A778));

    void    *oldBlock, *newBlock;
    RwUInt32 i;

    /* Reallocing the superblock isn't completely trivial, since it
     * contains internal cross-references :o/ */
    RWFUNCTION(RWSTRING("ReallocAndFixupSuperBlock"));

    /* NOTE:
     * People decide whether to do this realloc before calling this function.
     * Sometimes they want to grow only, sometimes they want to shrink.
     * So we just do as we're told here. */
    oldBlock = pipeline->superBlock;
    newBlock = realloc(oldBlock, newSize);
//    newBlock = RwRealloc(oldBlock, newSize,
//                         rwMEMHINTDUR_GLOBAL | rwMEMHINTFLAG_RESIZABLE | rwID_PIPEMODULE);
    if (NULL != newBlock)
    {
        const RwUInt32 numNodes = pipeline->numNodes;
        RwInt32 diff;

        diff = ((RwUInt8 *)newBlock) - ((RwUInt8 *)oldBlock);

        /* Fix up per-pipeline pointers */
        pipeline->superBlock = newBlock;
        pipeline->superBlockSize = newSize;
        pipeline->nodes = (RxPipelineNode *)pipeline->superBlock;
        _PTRINC(pipeline->embeddedPacket, RxPacket, diff);
        _PTRINC(pipeline->inputRequirements,
                RxPipelineRequiresCluster, diff);

        /* Fix up per-pipelinenode pointers */
        for (i = 0; i < numNodes; ++i)
        {
            _PTRINC(pipeline->nodes[i].outputs, RwUInt32, diff);
            _PTRINC(pipeline->nodes[i].slotClusterRefs,
                    RxPipelineCluster *, diff);
            _PTRINC(pipeline->nodes[i].slotsContinue, RwUInt32, diff);
            _PTRINC(pipeline->nodes[i].privateData, void, diff);
            _PTRINC(pipeline->nodes[i].inputToClusterSlot, RwUInt32, diff);
            _PTRINC(pipeline->nodes[i].topSortData,
                    RxPipelineNodeTopSortData, diff);
        }
    }
    else
    {
        RWERROR((E_RW_NOMEM, newSize));
        RWRETURN(FALSE);
    }

    RWRETURN(TRUE);
}

static void
PipelineTallyInputs(RxPipeline * pipeline)
{
    RxPipelineNode     *nodes;
    RwUInt32            i;

    RWFUNCTION(RWSTRING("PipelineTallyInputs"));

    nodes = &pipeline->nodes[0];
    for (i = 0; i < pipeline->numNodes; ++i)
    {
        if (ISNODELIVE(nodes))
        {
            /* clear this down prior to _TopSort() */
            nodes->topSortData->numInsVisited = 0;

            /* cleared for below */
            nodes->topSortData->numIns = 0;
        }
        ++nodes;
    }

    nodes = &pipeline->nodes[0];
    for (i = 0; i < pipeline->numNodes; ++i)
    {
        if (ISNODELIVE(nodes))
        {
            if (nodes->numOutputs != 0)
            {
                int                 j = nodes->numOutputs;
                RwUInt32           *outputs = &nodes->outputs[0];

                do
                {              /* loop over node outputs */
                    if (*(RwInt32 *) outputs != -1)
                    {
                        pipeline->nodes[*outputs].topSortData->numIns++;
                    }
                }
                while (outputs++, --j);
            }
        }
        ++nodes;
    }

    RWRETURNVOID();
}

static RxPipeline *
PipelineUnlockTopSort(RxPipeline *pipeline)
{
    TopSortData data;
    RwUInt32    i;

    RWFUNCTION(RWSTRING("PipelineUnlockTopSort"));
    RWASSERT(pipeline != NULL);

    data.pipeline = pipeline;
    data.nodesArraySlot = 0;

    /* Add up inputs for each node by enumerating outputs in all nodes */
    PipelineTallyInputs(pipeline);

    /* Does the entry-point has zero in-degrees? */
    if (pipeline->nodes[pipeline->entryPoint].topSortData->numIns != 0)
    {
        /* entry point is invalid;
         * fail (pipeline remains Lock()ed) */
        RWERROR((E_RX_INVALIDENTRYPOINT));
        RWRETURN((RxPipeline *)NULL);
    }

    /* Do all nodes form one connected graph? */
    for (i = 0; i < pipeline->numNodes; ++i)
    {
        if ((i != pipeline->entryPoint) &&
            (pipeline->nodes[i].topSortData->numIns == 0))
        {
            /* the pipeline is fragmented;
             * fail (pipeline remains Lock()ed) */
            RWERROR((E_RX_FRAGMENTEDPIPELINE));
            RWRETURN((RxPipeline *)NULL);
        }
    }

    /* Sort nodes so that dependencies always go in the same direction
     * (we put the user-defined entrypoint at the start - I don't think
     * this can ever be different to the one we'd calculate though.
     * If people want to enter at the 2nd node in a linear chain,
     * topsort will just fail, won't it ?).
     * NOTE: this updates cross-references as it sorts, so even if we
     *      fail it "preserves the integrity of the graph, boyo" */
    PipelineTopSort(&data, pipeline->entryPoint);

    /* cycle-free graph? */
    for (i = 0; i < pipeline->numNodes; ++i)
    {
        /* If a cycle is present, not all the nodes will be visited
         * along all their inputs during the PipelineTopSort */
        if (pipeline->nodes[i].topSortData->numIns !=
            pipeline->nodes[i].topSortData->numInsVisited)
        {
            /* pipeline had cycles;
             * fail (pipeline remains Lock()ed, partially sorted) */
            RWERROR((E_RX_CYCLICPIPELINE));
            RWRETURN((RxPipeline *)NULL);
        }
    }

    /* Paranoia (above error-checking should have caught this) */
    RWASSERT(pipeline->numNodes == data.nodesArraySlot);

    pipeline->entryPoint = 0;

    RWRETURN(pipeline);
}

RwUInt32 _rxChaseDependencies(RxPipeline * pipeline)
{
    return CHook::CallFunction<RwUInt32>("_Z20_rxChaseDependenciesP10RxPipeline", pipeline);
}

static RwBool
LockPipelineExpandData(RxPipeline *dstPipe, RxPipeline *srcPipe)
{
    void*& RwEngineInstance = *reinterpret_cast<void**>(g_libGTASA + (VER_x32 ? 0x6BCD38 : 0x89A358));
    RwInt32& _rxPipelineGlobalsOffset = *reinterpret_cast<RwInt32*>(g_libGTASA + (VER_x32 ? 0x6BCF98 : 0x89A778));
    RxPipelineNodeTopSortData *topSortArray;
    RwUInt32   *outputs;
    RwInt32     i;

    RWFUNCTION(RWSTRING("LockPipelineExpandData"));

    /* RxPipelineLock() expands the pipelineNodes and outputs arrays
     * to conservative sizes and creates a topSortData array too.
     * This is also used by RxPipelineClone, to copy the relevant
     * bits of an unlocked pipeline into a locked clone of it. */

    if (dstPipe != srcPipe)
    {
        /* We're cloning an unlocked pipe's nodes into a locked pipe. */
        /* First make sure the locked pipe's been set up right        */
        RWASSERT(FALSE != dstPipe->locked);
        RWASSERT(NULL  != dstPipe->superBlock);
        RWASSERT(dstPipe->superBlockSize >=
                 CalcNodeOutputsTopSortDataUnCompactedMemSize());
        RWASSERT(dstPipe->nodes == dstPipe->superBlock);
        RWASSERT(0 == dstPipe->numNodes);
        RWASSERT(FALSE == srcPipe->locked);

        /* Then copy the relevant bits of the pipeline nodes across */
        for (i = srcPipe->numNodes - 1;i >= 0;--i)
        {
            memcpy(&(dstPipe->nodes[i]),
                   &(srcPipe->nodes[i]),
                   sizeof(RxPipelineNode));

            /* There are some things that a locked   */
            /* pipe just doesn't need to know about! */
            dstPipe->nodes[i].slotClusterRefs = (RxPipelineCluster **)NULL;
            dstPipe->nodes[i].slotsContinue = (RwUInt32 *)NULL;
            dstPipe->nodes[i].privateData = NULL;
            dstPipe->nodes[i].inputToClusterSlot = (RwUInt32 *)NULL;

            /* DO copy initializationData though, m'bebbeh.
             * it's the heart and soul of cloning, man.
             * NOTE: if there are internal references within the
             *      initData, this'll screw it up... cloning sucks. */
            if (0 < dstPipe->nodes[i].initializationDataSize)
            {
                dstPipe->nodes[i].initializationData = malloc(dstPipe->nodes[i].initializationDataSize);
//                        RwMalloc(dstPipe->nodes[i].initializationDataSize,
//                                 rwMEMHINTDUR_EVENT | rwID_PIPEMODULE);
                if (NULL == dstPipe->nodes[i].initializationData)
                {
                    RWERROR((E_RW_NOMEM,
                            dstPipe->nodes[i].initializationDataSize));
                    RWRETURN(FALSE);
                }
                memcpy(dstPipe->nodes[i].initializationData,
                       srcPipe->nodes[i].initializationData,
                       dstPipe->nodes[i].initializationDataSize);
            }
        }
        /* _NodeCreate() incs numNodes, but we copied nodes directly, so: */
        dstPipe->numNodes = srcPipe->numNodes;
    }

    outputs = (RwUInt32 *)
            &(dstPipe->nodes[RXPIPELINEGLOBAL(maxNodesPerPipe)]);
    /* outputs get packed to follow on from the pipeline nodes  */
    /* array at the end of unlock, so if we work from the end   */
    /* backwards we'll avoid overwriting as we go.              */
    for (i = srcPipe->numNodes - 1;i >= 0;--i)
    {
        /* We copy more than we need to, it matters not.    */
        /* [i.e. the extra entries will never be read from] */
        dstPipe->nodes[i].outputs = &(outputs[i*RXNODEMAXOUTPUTS]);
        if (NULL != srcPipe->nodes[i].outputs)
        {
            memcpy(dstPipe->nodes[i].outputs,
                   srcPipe->nodes[i].outputs,
                   RXNODEMAXOUTPUTS*sizeof(RwUInt32));
        }
        else
        {
            RWASSERT(0 == dstPipe->nodes[i].numOutputs);
        }
    }

    /* Create topSortData for the existing nodes */
    topSortArray = (RxPipelineNodeTopSortData *)
            &(outputs[RXNODEMAXOUTPUTS*RXPIPELINEGLOBAL(maxNodesPerPipe)]);
    for (i = 0;(RwUInt32)i < srcPipe->numNodes;++i)
    {
        topSortArray[i].numIns = 0;
        topSortArray[i].numInsVisited = 0;
        topSortArray[i].req = (rxReq *)NULL;
        dstPipe->nodes[i].topSortData = &(topSortArray[i]);
    }

    RWRETURN(TRUE);
}

RwUInt32
PipelineCalcNumUniqueClusters(RxPipeline *pipeline)
{
    RxClusterDefinition *lastAddress, *newAddress;
    RwUInt32             numUniqueClusters;
    RwUInt32             i, j;

    /* To determine the max. possible amount of memory required during
     * dependency chasing, we count the number of unique clusters in the
     * pipeline - the worst case is that every node requires them all. */

    RWFUNCTION(RWSTRING("PipelineCalcNumUniqueClusters"));

    RWASSERT(NULL != pipeline);

    /* Basically, we don't want to allocate any memory  */
    /* so we count up the unique clusters by traversing */
    /* them in memory address order.                    */
    numUniqueClusters = 0;
    newAddress = (RxClusterDefinition *)NULL;
    while (1)
    {
        lastAddress = newAddress;
        newAddress  = (RxClusterDefinition *)0xFFFFFFFF;

        for (i = 0;i < pipeline->numNodes;++i)
        {
            RxNodeDefinition *nodeDef = pipeline->nodes[i].nodeDef;
            for (j = 0;j < nodeDef->io.numClustersOfInterest;++j)
            {
                RxClusterDefinition *address;
                address = nodeDef->io.clustersOfInterest[j].clusterDef;
                if ((address > lastAddress) &&
                    (address < newAddress ) )
                {
                    newAddress = address;
                }
            }
        }

        /* This will correctly deal with pipes with */
        /* no nodes and/or no clusters of interest. */
        if (newAddress == (RxClusterDefinition *)0xFFFFFFFF) break;
        ++numUniqueClusters;
    }

    RWRETURN(numUniqueClusters);
}


RxPipeline* RxLockedPipeUnlock(RxPipeline * pipeline)
{
    //   RwInt32      _rxPipelineGlobalsOffset;
    RwInt32& _rxPipelineGlobalsOffset = *reinterpret_cast<RwInt32*>(g_libGTASA + (VER_x32 ? 0x6BCF98 : 0x89A778));
    void*& RwEngineInstance = *reinterpret_cast<void**>(g_libGTASA + (VER_x32 ? 0x6BCD38 : 0x89A358));

    RWAPIFUNCTION(RWSTRING("RxLockedPipeUnlock"));

    /* MEMORY ALLOCATION DURING RXPIPELINE CONSTRUCTION:
     *
     * The RxPipeline structure is allocated in a block of its own.
     * It's actually allocated from a freelist (one block of which
     * should be able to hold all default and common plugin pipes).
     *
     * During the first RxPipelineLock(), a secondary block is
     * allocated to contain three arrays - RxPipelineNodes,
     * RwUInt32 'outputs' (a list of output indexes per node)
     * and 'topSortData', a RxPipelineNodeTopSortData per node.
     * These three arrays are allocated to be as big as they
     * will ever need to be. That's RXPIPELINEGLOBAL(maxNodesPerPipe)
     * (initialised at startup from _rxPipelineMaxNodes) for the
     * nodes and topsort arrays and RXNODEMAXOUTPUTS outputs per
     * node in the outputs array.
     *
     * These arrays are filled by calls to RxLockedPipeAddFragment.
     * RxLockedPipe[Delete|Replace]Node modify them too (keeping the
     * array fully compacted).
     *
     * At the beginning of RxLockedPipeUnlock, the pipelinenodes
     * and outputs arrays are packed down to the start of the memory
     * block. The block is then realloced to contain as much space
     * as will be required during unlock and the topsortdata array
     * is then packed to the end of this memory block. Temporary
     * data is allocated from the top of the block down (the
     * 'stalactite' stack, starts at the bottom of topsortdata) and
     * persistent data (i.e that which will persist after unlock
     * completes) will be allocated from the bottom of the block up
     * (the 'stalacmite' stack, from the top of the outputs array).
     *
     * At the end of unlock, topsortdata is discarded along with other
     * temporary data (e.g RwReqEntrys) and topsortdata pointers in
     * the pipeline nodes are set to NULL. The persistent data is then
     * 'shrinkwrapped' by reallocing down to the top of the stalacmite
     * stack.
     *
     * If RxPipelineLock is called again, the pipelinenodes and outputs
     * arrays are moved and expanded up to their conservatively large
     * sizes again and the topsortdata array is recreated. The pointers
     * from nodes to outputs and topsortdata are fixed up. Realloc is
     * used to grow the memory block if necessary (again, w/ fixup).
     *
     *
     * RxPipelineDestroy() merely has 2 memory blocks to free, the
     * pipeline itself and the auxiliary block mentioned above. It
     * must as before call node term callbacks and free any
     * initializationData that's been allocated.
     *
     * RxPipelineClone() has to create a locked pipe and copy the
     * pipelinenodes and outputs arrays into its memory block, recreate
     * the topsortdata array, fix up pointers to outputs and topsortdata,
     * copy initializationData (and fix up pointers to that) and then
     * unlock the new pipe (we don't actually expect pipelineclone to
     * work, you know. It'll probably get removed in later revisions. I'd
     * only expect it to reappear once/if we make pipe creation more
     * automated and data-driven). */


    RWASSERT(RxPipelineInstanced);
    RWASSERT(NULL != pipeline);
    RWASSERT(FALSE != pipeline->locked);

    if ((NULL  != pipeline) &&
        (FALSE != pipeline->locked))
    {
        if (pipeline->numNodes != 0)
        {
            RxPipelineNodeTopSortData *newTopSortData;
            RxPipelineNodeTopSortData *topSortData;
            RwUInt32  numUniqueClusters;
            RwUInt32  unlockStartBlockSize;
            RwUInt32  unlockEndBlockSize;
            RwUInt32  topSortBlockSize;
            RwUInt32  depChaseBlockSize;
            RwUInt32  totalOutputs = 0;
            RwUInt32 *newOutputs;
            RwUInt32 *outputs;
            RwBool    error = FALSE;
            RwUInt32  doneNodes = 0;
            RwInt32   i;


            RWASSERT(pipeline->entryPoint < pipeline->numNodes);
            RWASSERT(ISNODELIVE(&pipeline->nodes[pipeline->entryPoint]));

            if (!( (pipeline->entryPoint < pipeline->numNodes) &&
                   (ISNODELIVE(&pipeline->nodes[pipeline->entryPoint])) )  )
            {
                /* Entry-point is invalid; fail (pipeline remains Lock()ed) */
                RWERROR((E_RX_INVALIDENTRYPOINT));
                RWRETURN((RxPipeline *)NULL);
            }


            /* Sets up numUniqueClusters, which is used in
             * subsequent macros and dep-chase functions */
            numUniqueClusters = PipelineCalcNumUniqueClusters(pipeline);


            /* We need to calculate a conservative value for the amount
             * of memory we'll need during unlock() and allocate it.
             * Here's the sequence of memory usage and freeing/packing:
             *
             *  - lock, allocate a conservatively-sized block
             *  o 3 arrays [nodes, outputs, topsortdata] (mite)
             *  - edit the pipe (no further allocation)
             *  - start unlock, allocate the big block
             *  o 3 arrays [nodes, outputs, topsortdata] (mite)
             *  - do topsort
             *  - move topSortData to top of mem (tite), fixup ptrs
             *  - compact nodes+outputs arrays, fixup ptrs
             *  - set up mite ptr (should be NULL before this) to just after outputs
             *  o compacted 2 arrays [nodes, outputs] (mite) + further persistent mem (mite) +
             *    depchase temp mem (tite) + topsortdata (tite)
             *  - do depchase (including _ForAllNodesWriteClusterAllocations)
             *  - shrinkwrap to (compacted 2 arrays + further persistent mem)
             */

            /* First task is PipelineUnlockTopSort() which needs this much: */
            topSortBlockSize = CalcNodeOutputsTopSortDataUnCompactedMemSize();

            /* Second task is _rxChaseDependencies() (including
             * _ForAllNodesWriteClusterAllocations()), which requires: */
            depChaseBlockSize  = CalcTopSortDataMemSize();
            depChaseBlockSize += CalcNodesOutputsCompactedMemSize(pipeline);
            depChaseBlockSize += CalcTempDepChaseMemSize(
                    pipeline, numUniqueClusters);
            depChaseBlockSize += CalcUnlockPersistentMemSize(
                    pipeline, numUniqueClusters);


            /* Allocate the largest required block */
            unlockStartBlockSize = topSortBlockSize;
            if (depChaseBlockSize > unlockStartBlockSize)
            {
                unlockStartBlockSize = depChaseBlockSize;
            }

            /* Grow superBlock if necessary */
            if (unlockStartBlockSize > pipeline->superBlockSize)
            {
                if (FALSE == ReallocAndFixupSuperBlock(
                        pipeline, unlockStartBlockSize))
                {
                    RWRETURN((RxPipeline *)NULL);
                }
            }

            /* Just for PipelineUnlockTopSort(), we start the stalacTite array after the
             * topSortData array. This is so that we can keep the three arrays uniform and
             * full-sized until after PipelineUnlockTopSort() (makes its shuffling simpler) */
            gMemoryLimits.StalacTiteBase = ((RwUInt8 *)pipeline->superBlock) + unlockStartBlockSize;
            /* Mite ptr NULL for now, it should not be used. If it was going to be used,
             * it'd be: (RwUInt8 *)&(topSortData[RXPIPELINEGLOBAL(maxNodesPerPipe)]) */
            gMemoryLimits.StalacMiteBase = (RwUInt8 *) NULL;

            pipeline = PipelineUnlockTopSort(pipeline);
            if (NULL == pipeline)
            {
                /* The three arrays haven't moved (stuff might have been
                 * shuffled but that's okay), so if we return now we're in
                 * the locked state, as desired. */
                RWRETURN((RxPipeline *)NULL);
            }


            /* Now we compact the pipeline nodes and outputs arrays to the bottom
             * of superBlock and move the topSortData array to the top of mem,
             * fixing up pointers as we go. NOTE: PipelineUnlockTopSort() should
             * have shuffled all three arrays to remain in the same order as
             * each other. */

            outputs = (RwUInt32 *)&(pipeline->nodes[RXPIPELINEGLOBAL(maxNodesPerPipe)]);
            topSortData = (RxPipelineNodeTopSortData *)&
                    (outputs[RXNODEMAXOUTPUTS*RXPIPELINEGLOBAL(maxNodesPerPipe)]);
            topSortData += pipeline->numNodes - 1;
            newTopSortData = (RxPipelineNodeTopSortData *)
                    (((RwUInt8 *)pipeline->superBlock) + unlockStartBlockSize);
            --newTopSortData;
            RWASSERT(newTopSortData > topSortData);
            /* Move the topSortData in reverse order to avoid overwriting */
            for (i = (pipeline->numNodes - 1);i >= 0;--i)
            {
                memcpy(newTopSortData,
                       topSortData,
                       sizeof(RxPipelineNodeTopSortData));
                pipeline->nodes[i].topSortData = newTopSortData;

                --topSortData;
                --newTopSortData;
            }

            /* Pack the outputs array after the used pipeline nodes */
            newOutputs = (RwUInt32 *)&(pipeline->nodes[pipeline->numNodes]);
            for (i = 0;(RwUInt32)i < pipeline->numNodes;i++)
            {
                if (0 == pipeline->nodes[i].numOutputs)
                {
                    pipeline->nodes[i].outputs = (RwUInt32 *)NULL;
                }
                else
                {
                    memcpy(newOutputs,
                           outputs,
                           pipeline->nodes[i].numOutputs*sizeof(RwUInt32));
                    pipeline->nodes[i].outputs = newOutputs;
                }

                outputs      += RXNODEMAXOUTPUTS;
                newOutputs   += pipeline->nodes[i].numOutputs;
                totalOutputs += pipeline->nodes[i].numOutputs;
            }

            /* For dep-chasing and persistent data setup, the stalacTite
             * pointer starts at the bottom of the topSortData array and
             * moves down from there. The stalacMite pointer starts above
             * the packed outputs array. */
            gMemoryLimits.StalacMiteBase  = (RwUInt8 *)&(outputs[totalOutputs]);
            gMemoryLimits.StalacTiteBase  = (RwUInt8 *)topSortData;

            if (_rxChaseDependencies(pipeline) != 0)
            {
                /* dep. chasing failed;
                 * fail (pipeline remains Lock()ed, sorted) */
                RWRETURN((RxPipeline *)NULL);
            }

            /* Now we're done, 'shrinkwrap' our persistent allocations
             * (the stalacMite stack) and fix up internal pointers
             * where necessary */
            unlockEndBlockSize = gMemoryLimits.StalacMiteBase -
                                 (RwUInt8 *)pipeline->superBlock;
            if (FALSE == ReallocAndFixupSuperBlock(
                    pipeline, unlockEndBlockSize))
            {
                goto fail;
            }

            /* We've effectively 'free' topSortData with the above
             * realloc, so set those pipelinenode pointers to NULL: */
            for (i = 0;(RwUInt32)i < pipeline->numNodes;++i)
            {
                pipeline->nodes[i].topSortData = (RxPipelineNodeTopSortData *)NULL;
            }

            RXCHECKFORUSERTRAMPLING(pipeline);


            /* init & config method calls flow from bottom to
             * top (i.e. consumer -> producer) */
            for (i = (pipeline->numNodes - 1);i >= 0;--i)
            {
                RxNodeDefinition *nodeDef =   pipeline->nodes[i].nodeDef;
                RxPipelineNode   *node    = &(pipeline->nodes[i]);

                /* nodeinit (called first time node
                 * is used in an application) */
                if (nodeDef->InputPipesCnt++ == 0)
                {
                    if (NULL != nodeDef->nodeMethods.nodeInit)
                    {
                        if (nodeDef->nodeMethods.nodeInit(nodeDef) == FALSE)
                        {
                            /* Early-out but unroll nodeInits with nodeTerms */
                            doneNodes = (pipeline->numNodes - 1) - i;
                            error = TRUE;
                            break;
                        }
                    }
                }

                /* pipelinenodeinit */
                if (nodeDef->nodeMethods.pipelineNodeInit != NULL)
                {
                    if (nodeDef->nodeMethods.pipelineNodeInit(node) == FALSE)
                    {
                        /* Early-out but unroll nodeInits with nodeTerms */
                        --nodeDef->InputPipesCnt;
                        if (0 == nodeDef->InputPipesCnt)
                        {
                            if (NULL != nodeDef->nodeMethods.nodeTerm)
                            {
                                nodeDef->nodeMethods.nodeTerm(nodeDef);
                            }
                        }
                        doneNodes = (pipeline->numNodes - 1) - i;
                        error = TRUE;
                        break;
                    }
                }
            }

            if (FALSE == error)
            {
                for (i = (pipeline->numNodes - 1);i >= 0;--i)
                {
                    RxNodeDefinition *nodeDef =   pipeline->nodes[i].nodeDef;
                    RxPipelineNode   *node    = &(pipeline->nodes[i]);

                    /* pipelinenodeconfig */
                    if (nodeDef->nodeMethods.pipelineNodeConfig != NULL)
                    {
                        if (nodeDef->nodeMethods.pipelineNodeConfig(
                                node, pipeline) == FALSE)
                        {
                            /* Early-out but unroll nodeInits with nodeTerms */
                            doneNodes = pipeline->numNodes;
                            error = TRUE;
                            break;
                        }
                    }
                }
            }

            if (FALSE != error)
            {
                /* On error, undo completed node initializations in reverse order */
                for (i = (pipeline->numNodes - 1) - (doneNodes - 1);
                     (RwUInt32)i < pipeline->numNodes;
                     ++i)
                {
                    RxNodeDefinition *nodeDef =   pipeline->nodes[i].nodeDef;
                    RxPipelineNode   *node    = &(pipeline->nodes[i]);

                    /* pipelinenodeterm undoes pipelinenodeinit */
                    if (nodeDef->nodeMethods.pipelineNodeTerm != NULL)
                    {
                        nodeDef->nodeMethods.pipelineNodeTerm(node);
                    }

                    /* nodeterm undoes nodeinit */
                    nodeDef->InputPipesCnt--;
                    if (0 == nodeDef->InputPipesCnt)
                    {
                        if (NULL != nodeDef->nodeMethods.nodeTerm)
                        {
                            nodeDef->nodeMethods.nodeTerm(nodeDef);
                        }
                    }
                }

                /* Revert to the locked state and exit */
                goto fail;
            }

            RXCHECKFORUSERTRAMPLING(pipeline);
        }

        pipeline->locked = FALSE;

        RWRETURN(pipeline);    /* succeed */
    }

    if (pipeline == NULL)
    {
        RWERROR((E_RW_NULLP));
    }
    else
    {
        RWERROR((E_RX_UNLOCKEDPIPE));
    }

    RWRETURN((RxPipeline *)NULL); /* No pipeline or not locked; fail */

    fail:

    /* We can't merely free superBlock here, we need to return to the
     * locked pipe's state. so we use the code from RxPipelineLock()
     * which expands the nodes and outputs and creates the topsortdata */
    LockPipelineExpandData(pipeline, pipeline);

    RWRETURN((RxPipeline *)NULL);
}

RxPipeline* RxPipelineLock(RxPipeline *pipeline) {
    return CHook::CallFunction<RxPipeline*>("_Z14RxPipelineLockP10RxPipeline", pipeline);
}

RxPipelineNode* RxPipelineFindNodeByName(RxPipeline* pipeline, const RwChar *name, RxPipelineNode* start, RwInt32* nodeIndex) {
    return CHook::CallFunction<RxPipelineNode*>("_Z24RxPipelineFindNodeByNameP10RxPipelinePKcP14RxPipelineNodePi", pipeline, name, start, nodeIndex);
}

RxPipeline* RxLockedPipeAddFragment(RxPipeline *pipeline, RwUInt32 *firstIndex, RxNodeDefinition *nodeDef0, RwUInt32 nodeUnk) {
    return CHook::CallFunction<RxPipeline*>("_Z23RxLockedPipeAddFragmentP10RxPipelinePjP16RxNodeDefinitionz", pipeline, firstIndex, nodeDef0, nodeUnk);
}