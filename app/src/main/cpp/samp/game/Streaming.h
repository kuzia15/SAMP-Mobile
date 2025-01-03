//
// Created by x1y2z on 11.01.2023.
//

#pragma once

#include <cassert>
#include <unistd.h>
#include <thread>
#include "constants.h"
#include "common.h"
#include "CdStreamInfo.h"
#include "game/Core/LinkList.h"
#include "StreamingInfo.h"
#include "game/Enums/eAreaCodes.h"
#include "main.h"
#include "Timer.h"

struct CEntityGTA;
enum class eChannelState
{
    // Doing nothing
    IDLE = 0,

    // Currently reading model(s)
    READING = 1,

    // A big model (also called a large file) is loaded in steps:
    // First, the variable `ms_bLoadingBigModel` is set to `true` in `RequestModelStream`.
    // When the first half has finished loading, and `ProcessLoadingChannel` is called
    // `ConvertBufferToObject` will be called by it, which will set the model's load state to
    // `LOADSTATE_FINISHING`. When the latter function returns the former checks if
    // the model's loadstate is `FINISHING`, if it is the channel's state is set to
    // `STARTED` to indicate a large model's loading has started and is yet to be finished.
    // Loading a large model is finished when `ProcessLoadingChannel`.
    // (In which case it's state still should be `STARTED`)
    STARTED = 2,

    // Also called ERROR, but that's a `windgi.h` macro
    ERR = 3,
};

enum eResourceFirstID : int32_t {
    // First ID of the resource
    RESOURCE_ID_DFF                = 0,                                     // default: 0
    RESOURCE_ID_TXD                = RESOURCE_ID_DFF + TOTAL_DFF_MODEL_IDS, // default: 20000
    RESOURCE_ID_COL                = RESOURCE_ID_TXD + TOTAL_TXD_MODEL_IDS, // default: 25000
    RESOURCE_ID_IPL                = RESOURCE_ID_COL + TOTAL_COL_MODEL_IDS, // default: 25255
    RESOURCE_ID_DAT                = RESOURCE_ID_IPL + TOTAL_IPL_MODEL_IDS, // default: 25511
    RESOURCE_ID_IFP                = RESOURCE_ID_DAT + TOTAL_DAT_MODEL_IDS, // default: 25575
    RESOURCE_ID_RRR                = RESOURCE_ID_IFP + TOTAL_IFP_MODEL_IDS, // default: 25755   (vehicle recordings)
    RESOURCE_ID_SCM                = RESOURCE_ID_RRR + TOTAL_RRR_MODEL_IDS, // default: 26230   (streamed scripts)

    // Used for CStreaming lists, just search for xrefs (VS: shift f12)
    RESOURCE_ID_LOADED_LIST_START  = RESOURCE_ID_SCM + TOTAL_SCM_MODEL_IDS, // default: 26312
    RESOURCE_ID_LOADED_LIST_END    = RESOURCE_ID_LOADED_LIST_START + 1,     // default: 26313

    RESOURCE_ID_REQUEST_LIST_START = RESOURCE_ID_LOADED_LIST_END + 1,       // default: 26314
    RESOURCE_ID_REQUEST_LIST_END   = RESOURCE_ID_REQUEST_LIST_START + 1,    // default: 26315
    RESOURCE_ID_TOTAL                                               // default: 26316
};

enum class eModelType {
    DFF,
    TXD,
    COL,
    IPL,
    DAT,
    IFP,
    RRR,
    SCM,

    INTERNAL_1,
    INTERNAL_2,
    INTERNAL_3,
    INTERNAL_4
};

// Helper functions to deal with modelID's


inline bool IsModelDFF(int32 model) { return RESOURCE_ID_DFF <= model && model < RESOURCE_ID_TXD; }
inline bool IsModelTXD(int32 model) { return RESOURCE_ID_TXD <= model && model < RESOURCE_ID_COL; }
inline bool IsModelCOL(int32 model) { return RESOURCE_ID_COL <= model && model < RESOURCE_ID_IPL; }
inline bool IsModelIPL(int32 model) { return RESOURCE_ID_IPL <= model && model < RESOURCE_ID_DAT; }
inline bool IsModelDAT(int32 model) { return RESOURCE_ID_DAT <= model && model < RESOURCE_ID_IFP; }
inline bool IsModelIFP(int32 model) { return RESOURCE_ID_IFP <= model && model < RESOURCE_ID_RRR; }
inline bool IsModelRRR(int32 model) { return RESOURCE_ID_RRR <= model && model < RESOURCE_ID_SCM; }
inline bool IsModelSCM(int32 model) { return RESOURCE_ID_SCM <= model && model < RESOURCE_ID_LOADED_LIST_START; }

inline eModelType GetModelType(int32 model) {
    if (IsModelDFF(model))
        return eModelType::DFF;

    else if (IsModelTXD(model))
        return eModelType::TXD;

    else if (IsModelCOL(model))
        return eModelType::COL;

    else if (IsModelIPL(model))
        return eModelType::IPL;

    else if (IsModelDAT(model))
        return eModelType::DAT;

    else if (IsModelIFP(model))
        return eModelType::IFP;

    else if (IsModelRRR(model))
        return eModelType::RRR;

    else if (IsModelSCM(model))
        return eModelType::SCM;

    else {
        assert(0); // NOTSA
        return (eModelType)-1;
    }
}

// Turn relative IDs into absolute ones.
inline int32 DFFToModelId(int32 relativeId) { return RESOURCE_ID_DFF + relativeId; }
inline int32 TXDToModelId(int32 relativeId) { return RESOURCE_ID_TXD + relativeId; }
inline int32 COLToModelId(int32 relativeId) { return RESOURCE_ID_COL + relativeId; }
inline int32 IPLToModelId(int32 relativeId) { return RESOURCE_ID_IPL + relativeId; }
inline int32 DATToModelId(size_t relativeId) { return (size_t)RESOURCE_ID_DAT + relativeId; }
inline int32 IFPToModelId(int32 relativeId) { return RESOURCE_ID_IFP + relativeId; }
inline int32 RRRToModelId(int32 relativeId) { return RESOURCE_ID_RRR + relativeId; }
inline int32 SCMToModelId(int32 relativeId) { return RESOURCE_ID_SCM + relativeId; }

// Turn absolute IDs into relative ones
inline int32 ModelIdToDFF(int32 absId) { return absId - RESOURCE_ID_DFF; }
inline int32 ModelIdToTXD(int32 absId) { return absId - RESOURCE_ID_TXD; }
inline int32 ModelIdToCOL(int32 absId) { return absId - RESOURCE_ID_COL; }
inline int32 ModelIdToIPL(int32 absId) { return absId - RESOURCE_ID_IPL; }
inline int32 ModelIdToDAT(int32 absId) { return absId - RESOURCE_ID_DAT; }
inline int32 ModelIdToIFP(int32 absId) { return absId - RESOURCE_ID_IFP; }
inline int32 ModelIdToRRR(int32 absId) { return absId - RESOURCE_ID_RRR; }
inline int32 ModelIdToSCM(int32 absId) { return absId - RESOURCE_ID_SCM; }

struct tStreamingFileDesc {
    tStreamingFileDesc() = default;

    tStreamingFileDesc(const char* name, bool bNotPlayerImg) :
            m_bNotPlayerImg(bNotPlayerImg),
            m_StreamHandle(CdStreamOpen(name))
    {
        strncpy(m_szName, name, std::size(m_szName));
    }

    [[nodiscard]] bool IsInUse() const noexcept { return m_szName[0]; }

    char  m_szName[40]{}; // If this string is empty (eg.: first elem in array is NULL) the entry isnt in use
    bool  m_bNotPlayerImg{};
    uint8 pad[3];
    int32 m_StreamHandle{-1};
};
VALIDATE_SIZE(tStreamingFileDesc, 0x30);

struct tStreamingChannel {
    int32               modelIds[16];
    int32               modelStreamingBufferOffsets[16];
    eChannelState       LoadStatus;
    int32               iLoadingLevel; // the value gets modified, but it's not used
    int32               offsetAndHandle;
    int32               sectorCount;
    int32               totalTries;
    eCdStreamStatus     m_nCdStreamStatus;

    [[nodiscard]] bool IsIdle() const noexcept    { return LoadStatus == eChannelState::IDLE; }
    [[nodiscard]] bool IsReading() const noexcept { return LoadStatus == eChannelState::READING; }
    [[nodiscard]] bool IsStarted() const noexcept { return LoadStatus == eChannelState::STARTED; }
};

VALIDATE_SIZE(tStreamingChannel, 0x98);

class CStreaming {
public:
    static inline size_t ms_memoryUsed;
    static inline size_t ms_memoryAvailable;
    static inline uint32 desiredNumVehiclesLoaded;
    static inline int32     ms_numModelsRequested;
    static inline uint32    ms_numPriorityRequests;
    static inline bool ms_bLoadVehiclesInLoadScene;
    static inline tStreamingFileDesc ms_files[TOTAL_IMG_ARCHIVES];
    // There are only two channels within CStreaming::ms_channel
    static inline tStreamingChannel ms_channel[2];
    static inline CLinkList<CEntityGTA*> ms_rwObjectInstances;
    static inline CStreamingInfo ms_aInfoForModel[RESOURCE_ID_TOTAL];
    static inline bool ms_bEnableRequestListPurge{};
    static inline bool ms_disableStreaming{};
    static inline bool ms_bLoadingBigModel{};
    static inline int32 ms_channelError{};
    static inline uint8* ms_pStreamingBuffer[2];
    static inline int32 ms_streamingBufferSize;
   // static inline bool m_bModelStreamNotLoaded;

    static inline CStreamingInfo* ms_pEndRequestedList;
    static inline CStreamingInfo* ms_pStartRequestedList;
    static inline CStreamingInfo* ms_pEndLoadedList;
    static inline CStreamingInfo* ms_startLoadedList;

public:
    static void InjectHooks();

    static CLink<CEntityGTA*>* AddEntity(CEntityGTA* entity);
    static int AddImageToList(const char *name, bool b);
    static void AddLodsToRequestList(const CVector* point, int32 flags);
    static void AddModelsToRequestList(const CVector* point, int32 flags);
    static bool AreAnimsUsedByRequestedModels(int32 animModelId);
    static void ClearFlagForAll(uint32 streamingFlag);
    static void DeleteAllRwObjects();

    static void InitImageList();

    static char* GetModelCDName(int32 index);
    static bool TryLoadModel(int modelId);
    static void RemoveModel(int32 modelId);
    static void RemoveTxdModel(int32 modelId);
    static void RequestModel(int32 modelId, int32 flags = STREAMING_GAME_REQUIRED);
    static void SetModelIsDeletable(int32 index);

    static void LoadAllRequestedModels(bool bPriorityRequestsOnly);
    static void RemoveBuildingsNotInArea(eAreaCodes areaCode);
    static void RemoveBigBuildings();
    static void MakeSpaceFor(size_t memoryToCleanInBytes);
    static bool RemoveLeastUsedModel(int32 streamingFlags);
    static void DeleteRwObjectsBehindCamera(size_t memoryToCleanInBytes);
    static bool HasVehicleUpgradeLoaded(int32 modelId);
    static void FinishLoadingLargeFile(uint8* pFileBuffer, int32 modelId);
    static bool ConvertBufferToObject(uint8* fileBuffer, int32 modelId);

    static void Update();

    static void LoadRequestedModels();
    static void PurgeRequestList();

    static void FlushChannels();

    // Inlined or NOTSA
    static bool IsModelLoaded(int32 model) { return ms_aInfoForModel[model].m_nLoadState == eStreamingLoadState::LOADSTATE_LOADED; }
    static CStreamingInfo& GetInfo(int32 modelId) { assert(modelId >= 0); return ms_aInfoForModel[modelId]; }
    static bool IsRequestListEmpty() { return ms_pEndRequestedList->GetPrev() == ms_pStartRequestedList; }
    static ptrdiff_t GetModelFromInfo(const CStreamingInfo* info) { return info - CStreaming::ms_aInfoForModel; }
    static void RemoveModelIfNoRefs(int32 modelId);

    static bool ProcessLoadingChannel(int32 chIdx);

    static void RequestModelStream(int32 chIdx);

    static void RetryLoadFile(int32 chIdx);

    static int32 GetNextFileOnCd(uint32 streamLastPosn, bool bNotPriority);

    static bool DeleteLeastUsedEntityRwObject(bool bNotOnScreen, int32 streamingFlags);
};
