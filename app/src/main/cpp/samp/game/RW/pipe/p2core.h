#pragma once

#include "types.h"
#include "p2macros.h"
#include "p2heap.h"
#include "memory.h"

/* RWPUBLIC */

//extern RwInt32 _rxPipelineMaxNodes;
//extern RwInt32 _rxHeapInitialSize;
/* LEGACY-SUPPORT MACRO */
#define _rwRxHeapInitialSize _rxHeapInitialSize

/* Beneficial padding of PowerPipe types is still being worked out... */
#define PADCLUSTERSx

/* RWPUBLICEND */

#define rwID_RXPIPELINEPLUGIN ((('_'<<24)|('P'<<16)|('L'<<8)|('2'<<0))U)

#define RXNODEPRIVATESPACEMAGIC ( (RwUInt32) (0x4D534D53U) )

/* RWPUBLIC */

/*************************************************************
 * Global Defines
 */

#define RWBOOLTOGGLE(bVar) ((bVar == FALSE)?(bVar = TRUE):(bVar = FALSE))

/* Both these currently limited due to the use of RwUInt32 bit-fields */
#define RXNODEMAXCLUSTERSOFINTEREST 32
#define RXNODEMAXOUTPUTS            32

/* Useful (for memory alloc) to limit this during pipeline construction */
#define RXPIPELINEDEFAULTMAXNODES   64

/*
 * Cluster flags
 */

/* RWPUBLICEND
 *
 * TODO: Why are cluster flags #defined and not an enum??
 *
 * NULL -> cluster not currently valid
 * RWPUBLIC */
#define rxCLFLAGS_NULL          ((RwUInt16) 0x0000U)
/* RWPUBLICEND
 * If this bit not set, assume all other fields are invalid
 * RWPUBLIC */
#define rxCLFLAGS_CLUSTERVALID  ((RwUInt16) 0x0001U)
/* RWPUBLICEND
 *
 * rxCLFLAGS_EXTERNAL
 * ==================
 *
 * rxCLFLAGS_EXTERNAL data usually lives outside the pipeline execution heap;
 * it may have been allocated with RwMalloc(), as a ResEntry, with a DMA
 * buffer allocator, may live in ROM, who knows...
 *
 * The crucial point is that powerpipe is not empowered to free/resize/modify
 * this block - a cluster passively references the block; the onus is on the
 * block's owner to clean-up as necessary after pipeline execution
 *
 * Attempts to lockwrite() external clusters will cause the cluster data
 * to be copied into the pipeline execution heap, and this (now
 * non-rxCLFLAGS_EXTERNAL) copy will be propagated with the cluster.
 *
 * rxCLFLAGS_EXTERNAL is sometimes used for memory which is in the pipe
 * execution heap so as to convey that a cluster merely references (vs. owns)
 * the memory, and that the memory shouldn't die with that cluster
 *
 * RWPUBLIC */
#define rxCLFLAGS_EXTERNAL ((RwUInt16) 0x0002U)
/* RWPUBLICEND
 *
 * rxCLFLAGS_EXTERNALMODIFIABLE
 * ============================
 *
 * rxCLFLAGS_EXTERNAL + 0x0004: you can't free or resize the data (it's
 * external to the pipeline execution heap so we don't know how), but
 * modification of the data in-place is permitted.
 *
 * lockwrite() will return a pointer to the original data and not cause
 * a copy; it is not possible to free/resize this block.
 *
 * RWPUBLIC */
#define rxCLFLAGS_EXTERNALMODIFIABLE ((RwUInt16) 0x0004U | 0x0002U)
/* RWPUBLICEND
 *
 * rxCLFLAGS_MODIFIED
 * ==================
 *
 * Set every time the cluster is lockwritten.
 * Useful for seeing whether a cluster that was set up
 * by pointing it at external data (e.g. during instancing
 * for EXTERNALMODIFIABLE data) has been modified.
 *
 * RWPUBLIC */
#define rxCLFLAGS_MODIFIED ((RwUInt16) 0x0008U)

/*
 * Packet flags
 */

/* RWPUBLICEND
 * No packet flags as of yet (there will be though)
 * RWPUBLIC */
#define rxPKFLAGS_NULL        ((RwUInt16) 0x0000U)

/* RWPUBLICEND
 * NOTE: you can't (ANSI C) do forward typedefs of enums,
 * so the following must be defined up here
 * RWPUBLIC */
/*
 * used in input specification
 */

/**
 * \ingroup rwcoregeneric
 * \ref RxClusterValidityReq
 * Flags specifying the state requirements for
 * a \ref RxCluster on entry to a node */
enum RxClusterValidityReq
{
    rxCLREQ_DONTWANT = 0, /**<The cluster is required but any data within it is
                           * not wanted and will be overwritten */
    rxCLREQ_REQUIRED = 1, /**<The cluster is required and it must contain
                           * valid data */
    rxCLREQ_OPTIONAL = 2, /**<The cluster will be used if it is present and
                           * contains valid data, otherwise the node will
                           * make do without it. */
    rxCLUSTERVALIDITYREQFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

/* RWPUBLICEND
 * used in specifying the state of a cluster and its data
 * on output from one of a node's outputs
 * RWPUBLIC */

/**
 * \ingroup rwcoregeneric
 * \ref RxClusterValid
 * Flags specifying the state requirements for
 * a \ref RxCluster on exit from a node */
enum RxClusterValid
{
    rxCLVALID_NOCHANGE = 0, /**<The cluster and its data will not change in
                             * validity on passing through this node */
    rxCLVALID_VALID = 1,    /**<The cluster and its data will be valid on
                             * exit from this node */
    rxCLVALID_INVALID = 2,  /**<The cluster's data will be invalid on
                             * exit from this node */
    rxCLUSTERVALIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};

/****************************************************************************
 Global Types
 */

typedef struct RxClusterDefinition RxClusterDefinition;

typedef struct rxReq rxReq;
/* RWPUBLICEND */
/* data hiding; end users need know nothing about rxReq - it enjoys
 * a fleeting existence during dependency chasing and should not be exposed
 */
typedef struct RwReqEntry RwReqEntry;
typedef struct RwScopeTrace RwScopeTrace;
/* RWPUBLIC */

typedef enum RxClusterValidityReq RxClusterValidityReq;
typedef enum RxClusterValid RxClusterValid;
typedef struct RxOutputSpec RxOutputSpec;
typedef struct RxClusterRef RxClusterRef;
typedef struct RxIoSpec RxIoSpec;

typedef struct RxNodeMethods RxNodeMethods;
typedef struct RxNodeDefinition RxNodeDefinition;

typedef struct RxCluster RxCluster;
typedef struct RxPipelineCluster RxPipelineCluster;
typedef struct RxPacket RxPacket;
typedef struct RxPipelineNode RxPipelineNode;
typedef struct RxPipelineNodeTopSortData RxPipelineNodeTopSortData;
typedef struct RxPipelineNode RxPipelineNodeInstance;
typedef struct RxPipelineNodeParam RxPipelineNodeParam;
typedef struct RxExecutionContext RxExecutionContext;
typedef struct RxPipelineRequiresCluster RxPipelineRequiresCluster;
typedef struct RxPipeline RxPipeline;

/***************************************************************************
 *
 * C L U S T E R   D E F I N I T I O N
 *
 ***************************************************************************/

/**
 * \ingroup rwcoregeneric
 * \struct RxClusterDefinition
 * Structure describing a cluster */
struct RxClusterDefinition
{
    /* RWPUBLICEND
     * nothing more need currently be specified, because a
     * cluster is intimately associated with a C struct!
     * RWPUBLIC     */
    RwChar             *name;                  /**< Name */
    /* RWPUBLICEND
     * as a rule, clusters are created by nodes that "know" the semantics of
     * the cluster, so don't need to be informed of the cluster's stride
     * (nor need a given cluster always be created with the same stride);
     * occasionally, however, esp. when construction-time (vs. pipe
     * execution-time) node customisation APIs are used to make nodes
     * handle additional clusters, they are left creating clusters of
     * which they have no semantic knowledge - hence defaultStride
     * RWPUBLIC     */
    RwUInt32            defaultStride;         /**< Default stride */
    /* RWPUBLICEND
     * [optional] attribute support fields follow; these are used by nodes to
     * convey context-specific flags with a cluster
     * RWPUBLIC     */
    RwUInt32            defaultAttributes;     /**< Default attributes */
    /* RWPUBLICEND
     * this string identifies the "name space" for attribute values
     * RWPUBLIC     */
    const RwChar         *attributeSet;           /**< Attribute set */
};
static_assert(sizeof(RxClusterDefinition) == (VER_x32 ? 0x10 : 0x18));
/* RWPUBLICEND */

/*
 * none of the following is in any way relevant to the end user; in fact
 * these structures are only used within p2dep.c and exist only
 * fleetingly - the reason they need to be in p2core.h is coz each
 * RxPipelineNode contains an rxReq pointer
 */

/*
 * rxReq: an rxReq is created for each RxPipelineNode during
 * dependency chasing; it maintains a collection of RwReqEntrys, which
 * describe the requirements of that node
 */
struct rxReq
{
    RwUInt32             numused;
    RwUInt32             numalloced;
    RwUInt32             unavailslots;
        /* RwReqEntry collection wot we maintain */
    RwReqEntry           *reqentries;
        /* back pointer (to "owning" RxPipelineNode) */
    RxPipelineNode       *node;
};
static_assert(sizeof(rxReq) == (VER_x32 ? 0x14 : 0x20));

/*
 * RwReqEntry: describes the requirements of a node with respect to a
 * particular cluster; a collection of RwReqEntrys is maintained by each rxReq
 */
struct RwReqEntry
{
        /* RxClusterDefinition wot we describe requirements on */
    RxClusterDefinition *clusterDef;
    RxClusterValidityReq required;
    RwUInt32             inputs;
        /* which RwScopeTrace have we been assigned to? RwReqEntrys
         * maintain this value so overlapping RwScopeTraces (which
         * need to be merged) can be identified */
    RwScopeTrace        *st;
        /* link pointer for list maintained by RwScopeTrace:
         * RwScopeTrace.head -> next -> next -> ... */
    RwReqEntry          *next;
        /* back pointer (to "owning" rxReq) */
    rxReq               *req;
    RwUInt32             outbf;
    RwUInt32             assignedslot;
        /* remember that requirements get propagated up the pipeline;
         * to provide informative errors, we want to know where the
         * requirement originated */
    RxPipelineNode      *originatingNode;
};
static_assert(sizeof(RwReqEntry) == (VER_x32 ? 0x24 : 0x38));

/* RwScopeTrace:
 *
 * Scope traces are conceptually and logically fairly complex: THE
 * OBJECTIVE IS TO IDENTIFY A SUBGRAPH [within the pipeline graph] OF
 * NODES BETWEEN WHICH A CLUSTER FLOWS; this provides us useful
 * information on the lifetime of a cluster, which can then be used to
 * determine an optimal allocation of slots within a packet's cluster
 * array
 *
 * each scope trace maintains a linked list of RwReqEntrys (on the same
 * cluster, obviously, referenced by different nodes)
 *
 * the "next" pointers provide a means of iterating across root-level
 * scope traces (a scope trace ceases to be root-level if it is subsumed
 * by another scope trace during merging)
 *
 * scope tracing for a given cluster "cl" proceeds effectively as
 * follows:
 *
 * (o) create a new root-level scope trace for cluster cl
 *
 * (o) starting at node x [where cluster cl originates], add to the scope
 * trace the RwReqEntry wherein node x references cluster cl
 *
 * (o) traverse the pipeline graph commencing at node x down through
 * those outputs of node x *through which cluster cl flows*, and the
 * outputs' outputs, etc.
 *
 * (o) while traversing, add all RwReqEntrys referencing cluster cl to
 *     the scope trace under construction
 *
 * (o) we've now traced the scope of cluster cl
 *
 * (o) there is a complication, however: pipeline graphs, as well as
 *     branching, can recombine
 *
 * (o) recombination means that following cluster cl down from node y
 *     (perhaps a sibling of node x, which also creates cluster cl), we may
 *     ultimately reach a node where the ReqEntry for cluster cl has already
 *     been assigned to the scope trace which tracked cluster cl down from
 *     node x (i.e. the scope traces overlap)
 *
 * (o) in that case, the two scope traces (which we had in effect
 *     previously assumed corresponded to independent existences of cluster
 *     cl) are merged into one
 */

struct RwScopeTrace
{
        /* linked list of RwReqEntrys starts here! */
    RwReqEntry          *head;
        /* if scope traces are merged... */
    RwScopeTrace        *continuation;
        /* for master list of root-level scope traces */
    RwScopeTrace        *next;
        /* for merging: used to find root if
         * we are a child (= previously merged) trace */
    RwScopeTrace        *parent;
};
static_assert(sizeof(RwScopeTrace) == (VER_x32 ? 0x10 : 0x20));
/* RWPUBLIC */

/***************************************************************************
 *
 * N O D E   D E F I N I T I O N   S T R U C T S
 *
 ***************************************************************************/

/**
 * \ingroup rwcoregeneric
 * \struct RxOutputSpec
 * Structure describing an output specification of a node */
struct RxOutputSpec
{
     RwChar             *name;                  /**< Name */
    /* RWPUBLICEND
     * During dependency-chasing, the following fields are used to
     * work out which clusters each node creates, which they invalidate
     * and which they leave unchanged.
     *
     * The node should not usually destroy clusters because it assumes
     * they are no longer relevant - the pipeline execution code makes
     * this determination; however, there are occassions when the actions
     * of a node cause the contents of a cluster to become invalid and in
     * this case it should be destroyed to prevent subsequent nodes
     * receiving invalid data.
     * RWPUBLIC     */
     RxClusterValid     *outputClusters;        /**< States of clusters of interest on output */
    /* RWPUBLICEND
     * If the packet contains clusters which don't appear in the
     * ClustersOfInterest array; what happens to these?
     * RWPUBLIC     */
     RxClusterValid      allOtherClusters;      /**< States of clusters not of interest on output */
};
static_assert(sizeof(RxOutputSpec) == (VER_x32 ? 0xC : 0x18));
/* RWPUBLICEND
 * [note inserted in a random location]
 *
 * what sequence of operations have to be performed on
 * a packet as it is moved from one node to the next?
 *
 * (i)  Packet.inputToClusterSlot <- PipelineNode.inputToClusterSlot
 * (ii) Use the current slotsContinue bit-field for all cluster slots
 *      to determine whether a cluster is needed by subsequent nodes
 *      or whether it should be destroyed. Then update this bit-field:
 *      Packet.slotsContinue <- PipelineNode.slotsContinue
 * RWPUBLIC */

/* RWPUBLICEND
 * See RxClusterRef below.
 * RWPUBLIC */

/**
 * \ingroup rwcoregeneric
 * \ref RxClusterForcePresent
 *  Flags specifying whether an \ref RxCluster
 * should be forced to be present in an \ref RxPipelineNode */
enum RxClusterForcePresent
{
    rxCLALLOWABSENT = FALSE, /**<The cluster is allowed to be absent if no prior
                              *  nodes create it and no subsequent nodes require it */
    rxCLFORCEPRESENT = TRUE, /**<The cluster must be present even if no prior
                              *  nodes create it and no subsequent nodes require it */

    rxCLUSTERFORCEPRESENTFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxClusterForcePresent RxClusterForcePresent;

/**
 * \ingroup rwcoregeneric
 * \struct RxClusterRef
 * Structure describing a cluster reference */
struct RxClusterRef
{
    RxClusterDefinition  *clusterDef;           /**< Cluster definition */
    /* RWPUBLICEND
     * Force the cluster to be present for the current node,
     * whether it's created by prior nodes or required by
     * later nodes or not
     * RWPUBLIC     */
    RxClusterForcePresent forcePresent;         /**< Specifies whether the cluster should be forced present */
    RwUInt32              reserved;             /**< Omit or initialize to zero */
};
static_assert(sizeof(RxClusterRef) == (VER_x32 ? 0xC : 0x10));

#define rxCLRESERVED       ((RwUInt32)0)

/**
 * \ingroup rwcoregeneric
 * \struct RxIoSpec
 * Structure describing an input/output specification of a node */
struct RxIoSpec
{
    RwUInt32              numClustersOfInterest;/**< Number of Clusters of interest */
    RxClusterRef         *clustersOfInterest;   /**< Clusters of interest array */
    /* RWPUBLICEND
     * runs parallel to the ClustersOfInterest array; it specifies
     * what is required of each cluster on input to the node.
     * RWPUBLIC     */
    RxClusterValidityReq *inputRequirements;    /**< Requirements of the clusters of interest on input to the node */
    RwUInt32              numOutputs;           /**< Number of outputs from the node */
    /* RWPUBLICEND
     * What's true of the clusters of interest (and others)
     * on output from this node.
     * RWPUBLIC     */
    RxOutputSpec         *outputs;              /**< Output specification array */
};
static_assert(sizeof(RxIoSpec) == (VER_x32 ? 0x14 : 0x28));

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeBodyFn is the callback to be
 * called during pipeline execution -- and, typically, process
 * \ref RxPacket's -- for the owning pipeline node.
 *
 * \param  self   A pointer to the pipeline node being executed
 * \param  params   A pointer to a parameter structure
 *
 * \return TRUE on success, FALSE otherwise.
 *
 * \see RxNodeMethods
 */
typedef RwBool (*RxNodeBodyFn) (RxPipelineNode * self,
                                const RxPipelineNodeParam *params);

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeInitFn
 * is the callback to be called,
 * for the owning node definition, the first time an \ref RxPipeline
 * referencing that node definition is unlocked.
 *
 * \param  self   A pointer to the node definition
 *
 * \return TRUE on success, FALSE otherwise.
 *
 * \see RxNodeMethods
 */
typedef RwBool (*RxNodeInitFn) (RxNodeDefinition * self);

/**
 * \ingroup rwcoregeneric
 * \ref RxNodeTermFn
 * is the callback to be called,
 * for the owning node definition, the last time an \ref RxPipeline
 * referencing that node definition is destroyed or locked.
 *
 * \param  self   A pointer to the node definition
 *
 * \return None
 *
 * \see RxNodeMethods
 */
typedef void   (*RxNodeTermFn) (RxNodeDefinition * self);

/**
 * \ingroup rwcoregeneric
 * \ref RxPipelineNodeInitFn
 * is the callback to be called, for the owning pipeline node, whenever a
 * \ref RxPipeline containing that that pipeline node is unlocked.
 *
 * \param  self   A pointer to the pipeline node
 *
 * \return TRUE on success, FALSE otherwise.
 *
 * \see RxNodeMethods
 */
typedef RwBool (*RxPipelineNodeInitFn) (RxPipelineNode * self);

/**
 * \ingroup rwcoregeneric
 * \ref RxPipelineNodeTermFn
 * is the callback to be called, for the owning pipeline node, whenever a
 * \ref RxPipeline containing that that pipeline node is locked or
 * destroyed.
 *
 * \param  self   A pointer to the pipeline node
 *
 * \return None
 *
 * \see RxNodeMethods
 */
typedef void   (*RxPipelineNodeTermFn) (RxPipelineNode * self);

/**
 * \ingroup rwcoregeneric
 * \ref RxPipelineNodeConfigFn
 * is the callback to be called, for the owning pipeline node, whenever a
 * \ref RxPipeline containing that that pipeline node is unlocked,
 * *after* all \ref RxPipelineNodeInitFn's have been called for the
 * pipeline in question. This func is to be used as described in
 * RxPipelineNodeSendConfigMsg.
 *
 * \param  self       A pointer to the pipeline node
 * \param  pipeline   A pointer to the containing pipeline
 *
 * \return TRUE on success, FALSE otherwise.
 *
 * \see RxNodeMethods
 */
typedef RwBool (*RxPipelineNodeConfigFn) (RxPipelineNode * self,
                                          RxPipeline * pipeline);

/**
 * \ingroup rwcoregeneric
 * \ref RxConfigMsgHandlerFn
 * is the callback to be called, for the owning pipeline node, whenever
 * a message is sent to it by the \ref RxPipelineNodeConfigFn of another
 * pipeline node in the same pipeline. See \ref RxPipelineNodeSendConfigMsg.
 * 
 * \param  self   A pointer to the pipeline node
 * \param  msg   Message ID
 * \param  intparam   Meaning is message-specific
 * \param  ptrparam   Meaning is message-specific
 *
 * \return A RwInt32 value, 0: unserviced; -ve: error; +ve: informative success
 * 
 * \see RxNodeMethods
 */
typedef RwUInt32 (*RxConfigMsgHandlerFn) (RxPipelineNode * self,
                                          RwUInt32 msg,
                                          RwUInt32 intparam,
                                          void *ptrparam);

/**
 * \ingroup rwcoregeneric
 * \struct RxNodeMethods
 * A structure describing a set
 * of node methods
 *
 * \see RxNodeBodyFn
 * \see RxNodeInitFn
 * \see RxNodeTermFn
 * \see RxPipelineNodeInitFn
 * \see RxPipelineNodeTermFn
 * \see RxPipelineNodeConfigFn
 * \see RxConfigMsgHandlerFn
 */
struct RxNodeMethods
{
    RxNodeBodyFn         nodeBody;              /**< Node body function */
    RxNodeInitFn         nodeInit;              /**< Node initialization function */
    RxNodeTermFn         nodeTerm;              /**< Node termination function */
    RxPipelineNodeInitFn pipelineNodeInit;      /**< Pipeline node initialization function */
    RxPipelineNodeTermFn pipelineNodeTerm;      /**< Pipeline node termination function */
    /* RWPUBLICEND
     * called after init - opportunity to msg other
     * nodes, which have now been initialised
     * RWPUBLIC     */
    RxPipelineNodeConfigFn pipelineNodeConfig;  /**< Pipleline node configuation function */
    RxConfigMsgHandlerFn configMsgHandler;      /**< Configuaraton message handler function */
};
static_assert(sizeof(RxNodeMethods) == (VER_x32 ? 0x1C : 0x38));

/* RWPUBLICEND
 * See RxNodeDefinition below
 * RWPUBLIC     */
/**
 * \ingroup rwcoregeneric
 * \ref RxNodeDefEditable
 * Flags specifying whether an \ref RxNodeDefinition
 * is editable or not (\ref RxPipelineNodeCloneDefinition,
 * \ref RxPipelineNodeReplaceCluster and \ref RxPipelineNodeRequestCluster
 * create editable copies of node definitions as the originals may be
 * static definitions). */
enum RxNodeDefEditable
{
    /**<This node definition should not be freed */
    rxNODEDEFCONST = FALSE,
    /**< This node definition is a temporary,
     * modified copy of another and can be freed */
    rxNODEDEFEDITABLE = TRUE,
    rxNODEDEFEDITABLEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RxNodeDefEditable RxNodeDefEditable;

/**
 * \ingroup rwcoregeneric
 * \struct RxNodeDefinition
 * A structure describing a node definition */
struct RxNodeDefinition
{
    RwChar             *name;                   /**< Name */
    RxNodeMethods       nodeMethods;            /**< Node methods */
    RxIoSpec            io;                     /**< Input/output specification */
    RwUInt32            pipelineNodePrivateDataSize; /**< Size in bytes of node's private data  */
    /* RWPUBLICEND
     * Standard and user-defined node definitions are non-editable
     * (they are probably globals), but functions such as
     * RxPipelineNodeRequestCluster() and RxPipelineNodeReplaceCluster()
     * create temporary, modified node definitions for RxPipelineNodes
     * and these will need to be freed.
     * RWPUBLIC     */
    RxNodeDefEditable   editable;               /**< Flags whether a node is editable */
    /* RWPUBLICEND
     * How many PipelineNodes (in pipes) have been made from this node
     * ##**##**##NB##**##**##
     * InputPipesCnt will only be modified in RxPipelineLock/Unlock()
     * and only in RxPipelineDestroy IF the pipeline being destroyed
     * is locked. These are references to UNLOCKED pipeline nodes
     * RWPUBLIC     */
    RwInt32             InputPipesCnt;          /**< Count of the unlocked pipelines containing this node */
};
static_assert(sizeof(RxNodeDefinition) == (VER_x32 ? 0x40 : 0x78));
/***************************************************************************
 *
 * P I P E L I N E - E X E C U T I O N - T I M E   S T R U C T S
 *
 ***************************************************************************/

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineCluster
 * A structure describing a pipeline cluster;
 * that is, an \ref RxClusterDefinition in the context of a specific \ref RxPipeline
 * (in the same way as an \ref RxPipelineNode is an \ref RxNodeDefinition in the
 * context of a specific \ref RxPipeline). The \ref RxCluster is the structure
 * representing this and the \ref RxClusterDefinition within \ref RxPacket's at
 * pipeline-execution-time */
struct RxPipelineCluster
{
    RxClusterDefinition *clusterRef;            /**< Cluster refererence */
    RwUInt32             creationAttributes;    /**< Creation Attributes */
};
static_assert(sizeof(RxPipelineCluster) == (VER_x32 ? 0x8 : 0x10));

/**
 * \ingroup rwcoregeneric
 * \struct RxCluster
 * A structure describing a cluster; this is
 * the representative of an \ref RxClusterDefinition and \ref RxPipelineCluster
 * within \ref RxPacket's at pipeline-execution-time. */
struct RxCluster
{
    /* RWPUBLICEND
     * e.g rxCLFLAGS_NULL, rxCLFLAGS_CLUSTERVALID, rxCLFLAGS_EXTERNAL,
     * rxCLFLAGS_EXTERNALMODIFIABLE, rxCLFLAGS_MODIFIED
     * RWPUBLIC     */
    RwUInt16            flags;                  /**< Bitfield of flags e.g. modification permissions */
    RwUInt16            stride;                 /**< Stride in bytes of the cluster's data */
    void               *data;                   /**< The Cluster's data */
    /* RWPUBLICEND
     * For RxClusterGetCursorData(), RxClusterCursorInc(), etc.
     * RWPUBLIC     */
    void               *currentData;            /**< A 'cursor', referencing the current element in the data */
    RwUInt32            numAlloced;             /**< Allocated count */
    RwUInt32            numUsed;                /**< Used count */
    RxPipelineCluster  *clusterRef;             /**< Valid after a cluster has been locked for writing,
                                                 *   otherwise NULL. Analog of \ref RxPipelineNode and
                                                 *   its 'nodeDef' member. */
    /* RWPUBLICEND
     * meaning is context specific;
     * see ClusterRef->clusterRef->attributeSet
     * RWPUBLIC     */
    RwUInt32            attributes;             /**< Attributes */
#ifdef PADCLUSTERS
    /* RWPUBLICEND
     * 7 DWORDS (before structure_packing applied) without the pad
     * 8 sounds much nicer, but *is* it? (not verified yet)
     * RWPUBLIC     */
    RwUInt32 pad[1];                            /**< Alignment padding */
#endif
};
static_assert(sizeof(RxCluster) == (VER_x32 ? 0x1C : 0x30));

/**
 * \ingroup rwcoregeneric
 * \struct RxPacket
 * A structure describing a packet header */
struct RxPacket
{
    /* RWPUBLICEND
     * Following commented out because we can't keep track
     * of changes in Cluster sizes easily (many Packets can
     * reference the same Cluster data potentially).
     * RwUInt32                 Size;  _field Size in bytes of this packet (including the header or just the clusters?)
     * RWPUBLIC     */
    RwUInt16            flags;                  /**< Flags to guide pipeline execution */
    /* RWPUBLICEND
     * The max number of Clusters simultaneously present in the current
     * pipeline and hence the (real) length of the Clusters[] array
     * RWPUBLIC     */
    RwUInt16            numClusters;            /**< Maximum number of clusters simultanesouly present in the current pipeline */
    /* RWPUBLICEND
     * It's cheaper to retain use of the current packet when passing between
     * pipelines, instead of copying everything across into the destination
     * pipeline's embedded packet, so we will eventually use global packets
     * of a common/max cluster array length, and then we will just shuffle
     * clusters in-place real-quick-like.
     * RWPUBLIC     */
    RxPipeline         *pipeline;               /**< The pipeline in which this packet is embedded (the current pipeline) */
    /* RWPUBLICEND
     * for each input of the current node, which cluster slot corresponds?
     * RWPUBLIC     */
    RwUInt32           *inputToClusterSlot;     /**< LUT to locate clusters of interest in the packet */
    /* RWPUBLICEND
     * bit-fields: for each output (0..31) which slots persist to output node?
     * RWPUBLIC     */
    RwUInt32           *slotsContinue;          /**< Bitfields specifying clusters persisting to output node */
    RxPipelineCluster **slotClusterRefs;        /**< Internal Use */
#ifdef PADCLUSTERS
    RwUInt32 pad[3];                            /**< Alignment padding */
#endif
    RxCluster           clusters[1];            /**< Cluster array large enough for widest part of the pipeline */
    /* RWPUBLICEND
     * extra memory after this header continues this array - there are
     * enough RwClusters for the 'widest' part of the pipeline
     * RWPUBLIC     */
};
static_assert(sizeof(RxPacket) == (VER_x32 ? 0x30 : 0x58));

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineNode
 * Structure describing a pipeline Node;
 * that is an \ref RxNodeDefinition in the context 
 * of a specific \ref RxPipeline. 
 */
struct RxPipelineNode
{
    RxNodeDefinition   *nodeDef;            /**< Node definition reference */
    RwUInt32            numOutputs;         /**< Output count */
    RwUInt32           *outputs;            /**< Output array, indexing the pipeline's array of pipeline nodes */
    /* RWPUBLICEND
     * for each cluster slot, which cluster is assigned?
     * Point of interest: this field is guaranteed to be NULL when the
     * containing pipeline is locked, and guaranteed to be non-NULL
     * after a successful Unlock() call on the containing pipe. This
     * behaviour can be used to derive assert()ions to guard against
     * misuse of a node's construction time and run time APIs.
     * RWPUBLIC     */
    RxPipelineCluster **slotClusterRefs;    /**< For packets passing through this node, SlotClusterRefs[n]
                                             * identifies the cluster to be found at index n in the packet's
                                             * Clusters[] array. These assignments are determined at pipeline
                                             * Unlock() time and are good for the life of the pipeline. */
    /* RWPUBLICEND
     * bit-field: for each output node (0..31), which clusters persist?
     * The first entry is the AND of all subsequent entries,
     * used to optimize EMBEDDEDPACKETBETWEENNODESMACRO().
     * RWPUBLIC     */
    RwUInt32           *slotsContinue;      /**< Cluster persistence table for the node's outputs */
    void               *privateData;        /**< Pointer to the pipeline Node's private data */
    /* RWPUBLICEND
     * for each cluster of interest of this node, which cluster slot corresponds?
     * RWPUBLIC     */
    RwUInt32           *inputToClusterSlot; /**< LUT to locate clusters of interest in packets passing through this pipeline Node */
    /* RWPUBLICEND
     * Max number of bytes in output packets until packets are split
     * (obsolete but not implemented again yet)
     * RwUInt32          dispatchThreshold;  _field Maximum byte count until packets are split
     *
     * topSortData points at 'topSortData' when the pipe's locked (it's used in
     * topological sorting during \ref RxLockedPipeUnlock). When unlocked, it's NULL.
     * RWPUBLIC     */
    RxPipelineNodeTopSortData *topSortData; /**< Internal Use */
    void               *initializationData; /**< Used by the pipeline node initialisation
                                             * function in setting up the Private data. */
    RwUInt32        initializationDataSize; /**< Present so that if the node is cloned
                                             * we can copy the initialisation data. */
};
static_assert(sizeof(RxPipelineNode) == (VER_x32 ? 0x28 : 0x50));

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineNodeTopSortData
 * Structure describing data used during topological sorting 
 * during \ref RxLockedPipeUnlock ; 
 * RenderWare users do not need to understand this */
struct RxPipelineNodeTopSortData
{
    RwUInt32            numIns;                 /**< Input count */
    RwUInt32            numInsVisited;          /**< Count of inputs visited during dependencies
                                                 * propagation/cluster slot allocation */
    rxReq              *req;                    /**< Req used in dependencies propagation/cluster
                                                 * slot allocation */
};
static_assert(sizeof(RxPipelineNodeTopSortData) == (VER_x32 ? 0xC : 0x10));

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineNodeParam
 * Structure holding parameters
 * to pass to node body functions */
struct RxPipelineNodeParam
{
    /* RWPUBLICEND */
    /*
     * This struct is extensible so we can change effective node parameters
     * at a later date without breaking all nodes in existence. Hurrah.
     * Access macros in p2macros.h/c
     */
    /* RWPUBLIC */
    void   *dataParam;   /**< The data pointer passed in to \ref RxPipelineExecute */
    RxHeap *heap;        /**< The heap associated with the current pipeline exeuction */
};
static_assert(sizeof(RxPipelineNodeParam) == (VER_x32 ? 0x8 : 0x10));

/* RWPUBLICEND
 * TODO: These should go into pacekt flags now that
 *       foreign packets are gone...
 * RWPUBLIC */
enum rxEmbeddedPacketState
{
    rxPKST_PACKETLESS = 0, /* Packet not created */
    rxPKST_UNUSED     = 1, /* Packet created and then destroyed */
    rxPKST_INUSE      = 2, /* Packet created but not yet destroyed and fetched but not yet dispatched */
    rxPKST_PENDING    = 3, /* Packet created but not destroyed and dispatched but not yet fetched */
    rxEMBEDDEDPACKETSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum rxEmbeddedPacketState rxEmbeddedPacketState;

/**
 * \ingroup rwcoregeneric
 * \struct RxExecutionContext
 * Structure describing an execution context */
struct RxExecutionContext
{
    /* RWPUBLICEND */
    /*
     * This may change to a stack to facilitate packet copying
     * without the pain of nodeClone... though the whole external
     * data issue isn't just going to go away.
     */
#if (0)
    /* Also note "packet == currentPipeline->embeddedPacket" is true atm
     * ATM (since the death of the foreign packet stuff) but we will be
     * changing to a global execution context freelist where these own
     * packets instead of embedding packets in pipelines. This is so that
     * we can use the same packet all down the pipeline and not have to
     * change between pipes. This implies a common/max clusters array
     * size in the packets. */
    RxPacket              *packet;             /* _field the packet in use by this execution context */
    rxEmbeddedPacketState embeddedPacketState; /* _field the state of the packet in use by this execution context */
#endif /* (0) */

    /* RWPUBLIC */
    RxPipeline           *pipeline;     /**< Currently executing pipeline */
    RxPipelineNode       *currentNode;  /**< Currently executing node */
    RwInt32               exitCode;     /**< Used internally for nodes returning error codes on exit */
    RwUInt32              pad;          /**< Alignment padding */
    RxPipelineNodeParam   params;       /**< The parameters passed to node bodies */
    /*RwUInt32              pad[2];*/
};
static_assert(sizeof(RxExecutionContext) == (VER_x32 ? 0x18 : 0x28));

/**
 * \ingroup rwcoregeneric
 * \struct RxPipelineRequiresCluster
 * Structure describing a pipeline requirement of a cluster */
struct RxPipelineRequiresCluster
{
    RxClusterDefinition *clusterDef;    /**< Reference to a Cluster definition */
    RxClusterValidityReq rqdOrOpt;      /**< Cluster validity requirement (rxCLREQ_DONTWANT, rxCLREQ_REQUIRED or rxCLREQ_OPTIONAL) */
    RwUInt32             slotIndex;     /**< Index into the packet's cluster array within this pipeline */
};
static_assert(sizeof(RxPipelineRequiresCluster) == (VER_x32 ? 0xC : 0x10));

/**
 * \ingroup rwcoregeneric
 * \struct RxPipeline
 * Structure describing a pipeline */
struct RxPipeline
{
    /* RWPUBLICEND
     * Pipelines must be locked before creation/modification
     * RWPUBLIC     */
    RwBool                     locked;                /**< Flags whether the pipeline is locked for editing */
    RwUInt32                   numNodes;              /**< Node count */
    /* RWPUBLICEND
     * An array of PipelineNodes in Simon's 'One True Order'
     * RWPUBLIC     */
    RxPipelineNode            *nodes;                 /**< Topologically sorted array of pipeline nodes */
    /* RWPUBLICEND
     * Number of slots allocated for clusters, equals the maximum number
     * of clusters active simultaneously within this pipeline.
     * RWPUBLIC     */
    RwUInt32                   packetNumClusterSlots; /**< Number of slots allocated for clusters */
    rxEmbeddedPacketState      embeddedPacketState;   /**< The state of this pipeline's embedded packet */
    RxPacket                  *embeddedPacket;        /**< This pipeline's embedded packet */

    RwUInt32                   numInputRequirements;  /**< Input requirements count */
    RxPipelineRequiresCluster *inputRequirements;     /**< Input requirements array */

    /* RWPUBLICEND
     * PipelineNode.[SlotClusterIDs|SlotsContinue|Input2ClusterSlot]
     * are suballocated from this block
     * RWPUBLIC     */
    void                      *superBlock;            /**< Internally used block of memory */
    RwUInt32                   superBlockSize;        /**< Internally used block of memory */

    /* RWPUBLICEND
     * only required between Lock()/Unlock()
     * RWPUBLIC     */
    RwUInt32                   entryPoint;            /**< The index of the node which is the entry point of this pipeline */
    RwUInt32                   pluginId;              /**< If required, the Id of the plugin owning this pipeline, or 0 */
    RwUInt32                   pluginData;            /**< Rights callback extra data */
};
static_assert(sizeof(RxPipeline) == (VER_x32 ? 0x34 : 0x48));
/****************************************************************************
 * Global Prototypes
 */

/**
 * \ingroup rwcoregeneric
 * \ref RxPipelineNodeOutputCallBack 
 * is the callback function supplied 
 * to \ref RxPipelineNodeForAllConnectedOutputs.
 *
 * The callback will be passed a pointer to the \ref RxPipelineNode whose
 * outputs are being traversed and a pointer to the current output
 * \ref RxPipelineNode, as well as a pointer to an optional user-defined
 * data structure (callbackdata). If no such structure was specified, this
 * will be NULL.
 *
 * \param  node         A pointer to the pipeline node whose outputs
 *                      are being traversed
 * \param  outputnode   A pointer to the current output
 *                      pipeline node
 * \param  callbackdata A pointer to optional user-supplied data
 *
 * \return Returns a pointer to the \ref RxPipelineNode whose outputs are being
 * traversed, or NULL to terminate traversal
 */
typedef RxPipelineNode * (*RxPipelineNodeOutputCallBack) (RxPipelineNode * node,
                                                  RxPipelineNode * outputnode,
                                                  void *callbackdata);

#ifdef RWDEBUG
#define RXCHECKFORUSERTRAMPLING(_pipeline)                       \
    ( _rwPipelineCheckForTramplingOfNodePrivateSpace(_pipeline) )
#endif /* RWDEBUG */

#if (!defined(RXCHECKFORUSERTRAMPLING))
#define RXCHECKFORUSERTRAMPLING(_pipeline) /* No op */
#endif /* (!defined(RXCHECKFORUSERTRAMPLING)) */

/* RWPUBLICEND
 * Compatibiltity macros
 * RWPUBLIC     */

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */

extern void
RxPipelineSetFreeListCreateParams( RwInt32 blockSize, RwInt32 numBlocksToPrealloc );

extern RwBool
_rxPipelineOpen(void);

extern RwBool
_rxPipelineClose(void);

extern RwBool
rxPipelinePluginsAttach(void);

extern RxPipeline* RxPipelineCreate(void);

/* RWPUBLICEND
 * LEGACY-SUPPORT macro. RxPipelineDestroy now returns void really.
 * RWPUBLIC */

extern void
_rxPipelineDestroy(RxPipeline * Pipeline);

#define _RxPipelineDestroy(_ppln)  _rxPipelineDestroy(_ppln)
#define RxPipelineDestroy(_ppln) (_rxPipelineDestroy(_ppln), TRUE)

extern RxHeap      *
RxHeapGetGlobalHeap(void);

extern RxPipeline *
RxPipelineExecute(RxPipeline  * pipeline,
                  void        * data,
                  RwBool       heapReset);

extern RxPacket *
RxPacketCreate(RxPipelineNode * node);

extern RxCluster   *
RxClusterSetStride(RxCluster * cluster,
                   RwInt32 stride);

extern RxCluster   *
RxClusterSetExternalData(RxCluster * cluster,
                         void *data,
                         RwInt32 stride,
                         RwInt32 numElements);

extern RxCluster   *
RxClusterSetData(RxCluster * cluster,
                 void *data,
                 RwInt32 stride,
                 RwInt32 numElements);

/* underlying PacketDestroy function */
extern void
_rxPacketDestroy(RxPacket * Packet);

/* more convenient parameterization */
#define RxPacketDestroy(pk, self) \
    ( _rxPacketDestroy(pk) )

#if (defined(RWDEBUG))
extern RxPacket *RxPacketFetch(RxPipelineNode * Node);
extern void      RxPacketDispatch(RxPacket * packet,
                                  RwUInt32 output,
                                  RxPipelineNode * self);
extern void      RxPacketDispatchToPipeline(RxPacket * packet,
                                            RxPipeline * dest,
                                            RxPipelineNode * self);
#else /* (defined(RWDEBUG)) */
#define RxPacketFetch(_self) \
    rxPacketFetchMacro(_self)
#define RxPacketDispatch(     _packet, _output, _self) \
    rxPacketDispatchMacro(_packet, _output, _self)
#define RxPacketDispatchToPipeline(     _packet, _pipeline, _self) \
    rxPacketDispatchToPipelineMacro(_packet, _pipeline, _self)
#endif /* (defined(RWDEBUG)) */

/* RWPUBLICEND
 * Change to American spelling. Remove this LEGACY-SUPPORT macro whenever.
 * RWPUBLIC */
#define RxClusterInitialiseData(_clstr, _nmlmnts, _strd) \
    ( RxClusterInitializeData((_clstr), (_nmlmnts), (_strd)) )
extern RxCluster   *
RxClusterInitializeData(RxCluster *cluster, RwUInt32 numElements, RwUInt16 stride);

extern RxCluster   *
RxClusterResizeData(RxCluster *CurrentCluster, RwUInt32 NumElements);

extern RxCluster   *
RxClusterDestroyData(RxCluster *CurrentCluster);

#if (defined(RWDEBUG))

extern RxCluster *RxClusterLockRead(RxPacket * packet, RwUInt32 clusterIndex);

#else  /* !RWDEBUG */

#define RXCLSLOT(PKT, CLIND)             \
    ((PKT)->inputToClusterSlot[(CLIND)])

#define RxClusterLockRead(PKT, CLIND)                               \
    ( (((RwInt32)RXCLSLOT(PKT, CLIND)) == -1) ?                     \
      ((RxCluster *)NULL) :                                         \
      (RxClusterResetCursor(&PKT->clusters[RXCLSLOT(PKT, CLIND)]),  \
       &PKT->clusters[RXCLSLOT(PKT, CLIND)]) )

#endif /* !RWDEBUG */

extern RxCluster   *
RxClusterLockWrite(RxPacket * packet,
                   RwUInt32 clusterIndex,
                   RxPipelineNode * node);

extern void
RxClusterUnlock(RxCluster * cluster);

extern RwUInt32
RxPipelineNodeSendConfigMsg(RxPipelineNode * dest,
                            RwUInt32 msg,
                            RwUInt32 intparam,
                            void *ptrparam);

extern RxPipelineNode *
RxPipelineNodeForAllConnectedOutputs(RxPipelineNode * node,
                                     RxPipeline * pipeline,
                                     RxPipelineNodeOutputCallBack callbackfn,
                                     void *callbackdata);

/* Cluster attributes api [pipeline construction time] */

extern RxPipelineCluster *
RxPipelineNodeGetPipelineCluster(RxPipelineNode *node,
                                   RwUInt32 clustersOfInterestIndex);

extern RwUInt32
RxPipelineClusterGetCreationAttributes(RxPipelineCluster *cluster);

extern RxPipelineCluster *
RxPipelineClusterSetCreationAttributes(RxPipelineCluster *cluster,
                                         RwUInt32 creationAttributes);

/* Cluster attributes api [pipeline execution time] */

extern RwUInt32
RxClusterGetAttributes(RxCluster *cluster);

extern RxCluster *
RxClusterSetAttributes(RxCluster *cluster, RwUInt32 attributes);

/* RWPUBLICEND */

extern void
_rwPipelineCheckForTramplingOfNodePrivateSpace(RxPipeline *pipeline);

extern RxPipelineNode *
PipelineNodeDestroy(RxPipelineNode *node, RxPipeline *pipeline);

/* From p2core.c, used in p2define.c - has RxPipeline been opened yet? */
extern RwBool RxPipelineInstanced;

/* RWPUBLIC */

extern void
_rxEmbeddedPacketBetweenPipelines(RxPipeline * fromPipeline,
                                 RxPipeline * toPipeline);

extern RxPipelineNode *
_rxEmbeddedPacketBetweenNodes(RxPipeline     *pipeline,
                             RxPipelineNode *nodeFrom,
                             RwUInt32        whichOutput);

extern RxExecutionContext _rxExecCtxGlobal;

/* Summary of dispatch rules:
 * o nodes that never fetch are safe to dispatch NULL, whether
 *   nodes above pass them a packet or not
 * o if you destroy the packet you can dispatch(NULL,,)
 * o if you fetch/create and dispatch(NULL), it doesn't really
 *   matter - the packet'll get passed on anyway */

/* TODO: there's currently no way to prematurely terminate the pipeline
 *      without doing so as an error condition. You should create an
 *      enum for the exit code, either RXNODEEXITCONTINUE, RXNODEEXITTERMINATE
 *      or RXNODEEXTTERMINATEERROR and then test for RXNODEEXITCONTINUE in
 *      the below macros rather than FALSE. */

/* TODO: _packet redundant here... create a new macro and legacy wrapper */
#define rxPacketDispatchMacro(_packet, _output, _self)                      \
MACRO_START                                                                 \
{                                                                           \
    RxPipeline *curPipeline = _rxExecCtxGlobal.pipeline;                    \
                                                                            \
    /* _packet is now an obsolete parameter */                              \
                                                                            \
    if ( FALSE != _rxExecCtxGlobal.exitCode )                               \
    {                                                                       \
        RxPipelineNode *nextNode =                                          \
            _rxEmbeddedPacketBetweenNodes(curPipeline,                      \
                                         _self,                             \
                                         (_output));                        \
        if ( nextNode != NULL )                                             \
        {                                                                   \
            RwUInt32 exitCode =                                             \
                nextNode->nodeDef->nodeMethods.nodeBody(                    \
                    nextNode, &(_rxExecCtxGlobal.params));                  \
            /* Don't overwrite 'error' with 'success' */                    \
            if (FALSE == exitCode) _rxExecCtxGlobal.exitCode = exitCode;    \
        }                                                                   \
    }                                                                       \
    if ( curPipeline->embeddedPacketState > rxPKST_UNUSED                   \
         /* !UNUSED and !PACKETLESS */ )                                    \
    {                                                                       \
        curPipeline->embeddedPacketState = rxPKST_INUSE;                    \
        _rxPacketDestroy(curPipeline->embeddedPacket);                      \
    }                                                                       \
}                                                                           \
MACRO_STOP

/* TODO: _self redundant here... create a new macro and legacy wrapper */
#define rxPacketDispatchToPipelineMacro(_packet, _pipeline, _self)          \
MACRO_START                                                                 \
{                                                                           \
    RxPipeline *toPipeline = (_pipeline);                                   \
                                                                            \
    /* _packet is now an obsolete parameter */                              \
                                                                            \
    if ( FALSE != _rxExecCtxGlobal.exitCode )                               \
    {                                                                       \
        RwUInt32 exitCode;                                                  \
        RxPipeline *fromPipeline = _rxExecCtxGlobal.pipeline; /* save */    \
        _rxEmbeddedPacketBetweenPipelines(fromPipeline,                     \
                                         toPipeline);                       \
        _rxExecCtxGlobal.pipeline = toPipeline; /* modify */                \
        exitCode =                                                          \
            toPipeline->nodes[0].nodeDef->nodeMethods.nodeBody(             \
                &toPipeline->nodes[0], &(_rxExecCtxGlobal.params));         \
        if ( FALSE == exitCode ) _rxExecCtxGlobal.exitCode = exitCode;      \
        _rxExecCtxGlobal.pipeline = fromPipeline; /* restore */             \
    }                                                                       \
    if ( toPipeline->embeddedPacketState > rxPKST_UNUSED                    \
         /* !UNUSED and !PACKETLESS */ )                                    \
    {                                                                       \
        toPipeline->embeddedPacketState = rxPKST_INUSE;                     \
        _rxPacketDestroy(toPipeline->embeddedPacket);                       \
    }                                                                       \
}                                                                           \
MACRO_STOP

#define rxPacketFetchMacro(_node)                                           \
    ( ((_rxExecCtxGlobal.pipeline)->embeddedPacketState == rxPKST_PENDING) ?\
      ((_rxExecCtxGlobal.pipeline)->embeddedPacketState = rxPKST_INUSE,     \
       (_rxExecCtxGlobal.pipeline)->embeddedPacket) :                       \
      (NULL) )

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

