//
// Created by x1y2z on 11.01.2023.
//

#include "Streaming.h"
#include "../vendor/armhook/patch.h"
#include "StreamingInfo.h"
#include "game/Models/ModelInfo.h"
#include "game/Animation/AnimManager.h"
#include "TxdStore.h"
#include "game.h"
#include "net/netgame.h"
#include "game/Collision/ColStore.h"
#include "IplStore.h"
#include "Camera.h"
#include "Renderer.h"
#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>

extern UI *pUI;

bool CStreaming::TryLoadModel(int modelId) {
    FLog("TryLoadModel %d", modelId);
    if(!CStreaming::GetInfo(modelId).IsLoaded()) {
        FLog("TryLoadModel 1");
        CStreaming::RequestModel(modelId, STREAMING_GAME_REQUIRED | STREAMING_KEEP_IN_MEMORY);
        FLog("TryLoadModel 11");
        CStreaming::LoadAllRequestedModels(false);
        FLog("TryLoadModel 2");
        uint32 count = 0;
        while (!CStreaming::GetInfo(modelId).IsLoaded()) {
            count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            if (count > 30) {
                pUI->chat()->addDebugMessage("{ff0000} Error loading model %d", modelId);
                return false;
            }
        }
        FLog("TryLoadModel 3");
    }
    FLog("TryLoadModel 4");
    return true;
}

// Finishes loading all channels. (So both channels will be `IDLE` after it returns)
// Blocking. (Calls `CdStreamSync`)
void CStreaming::FlushChannels()
{
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x2D4878 + 1 : 0x396D1C));
}

void CStreaming::ClearFlagForAll(uint32 streamingFlag) {
    for (auto i = 0; i < RESOURCE_ID_TOTAL; i++) {
        GetInfo(i).ClearFlags(streamingFlag);
    }
}

bool CStreaming::AreAnimsUsedByRequestedModels(int32 animModelId) {
    for (auto info = ms_pStartRequestedList->GetNext(); info != ms_pEndRequestedList; info = info->GetNext()) {
        const auto modelId = GetModelFromInfo(info);
        if (IsModelDFF(modelId) && CModelInfo::GetModelInfo(modelId)->GetAnimFileIndex() == animModelId)
            return true;
    }

    for (auto & channel : ms_channel) {
        for (const auto& modelId : channel.modelIds) {
            if (modelId != MODEL_INVALID && IsModelDFF(modelId) &&
                CModelInfo::GetModelInfo(modelId)->GetAnimFileIndex() == animModelId
                    ) {
                return true;
            }
        }
    }

    return false;
}

void CStreaming::RemoveTxdModel(int32 modelId) {
    RemoveModel(TXDToModelId(modelId));
}

void CStreaming::RemoveModelIfNoRefs(int32 modelId) {
    CStreamingInfo& streamingInfo = GetInfo(modelId);
    if(streamingInfo.IsLoaded() && !CModelInfo::GetModelInfo(modelId)->m_nRefCount) {
        RemoveModel(modelId);

        streamingInfo.ClearAllFlags();
    }
}

void CStreaming::SetModelIsDeletable(int32 modelId) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x2D6788 + 1 : 0x399090), modelId);
}

void CStreaming::RemoveModel(int32 modelId) {
    if(modelId == MODEL_MALE01)
        return;

    CStreamingInfo& streamingInfo = GetInfo(modelId);
    if (streamingInfo.m_nLoadState == LOADSTATE_NOT_LOADED)
        return;

    if (streamingInfo.IsLoaded()) {
        switch (GetModelType(modelId)) {
            case eModelType::DFF: {
                CBaseModelInfo* modelInfo = CModelInfo::GetModelInfo(modelId);
                modelInfo->DeleteRwObject();
                break;
            }
            case eModelType::TXD: {
                CTxdStore::RemoveTxd(ModelIdToTXD(modelId));
                break;
            }
            case eModelType::COL: {
                CColStore::RemoveCol(ModelIdToCOL(modelId));
                break;
            }
            case eModelType::IPL: {
                CIplStore::RemoveIpl(ModelIdToIPL(modelId));
                break;
            }
            case eModelType::DAT: {
              //  ThePaths.UnLoadPathFindData(ModelIdToDAT(modelId));
                break;
            }
            case eModelType::IFP: {
                CAnimManager::RemoveAnimBlock(ModelIdToIFP(modelId));
                break;
            }
            case eModelType::SCM: {
             //   CTheScripts::StreamedScripts.RemoveStreamedScriptFromMemory(ModelIdToSCM(modelId));
                break;
            }
        }
        ms_memoryUsed -= STREAMING_SECTOR_SIZE * streamingInfo.GetCdSize();
    }

    if (streamingInfo.InList()) {
        if (streamingInfo.IsRequested()) {
            ms_numModelsRequested--;
            if (streamingInfo.IsPriorityRequest()) {
                streamingInfo.ClearFlags(STREAMING_PRIORITY_REQUEST);
                ms_numPriorityRequests--;
            }
        }
        streamingInfo.RemoveFromList();
    } else if (streamingInfo.IsBeingRead()) {
        for (auto& ch : ms_channel) {
            for (auto& mId : ch.modelIds) {
                if (mId == modelId) {
                    mId = MODEL_INVALID;
                }
            }
        }
    }

    if (streamingInfo.IsLoadingFinishing()) {
        switch (GetModelType(modelId)) {
            case eModelType::DFF:
                RpClumpGtaCancelStream();
                break;
            case eModelType::TXD:
                CTxdStore::RemoveTxd(ModelIdToTXD(modelId));
                break;
            case eModelType::COL:
                CColStore::RemoveCol(ModelIdToCOL(modelId));
                break;
            case eModelType::IPL:
                CIplStore::RemoveIpl(ModelIdToIPL(modelId));
                break;
            case eModelType::IFP:
                CAnimManager::RemoveAnimBlock(ModelIdToIFP(modelId));
                break;
            case eModelType::SCM:
             //   CTheScripts::StreamedScripts.RemoveStreamedScriptFromMemory(ModelIdToSCM(modelId));
                break;
        }
    }

    streamingInfo.m_nLoadState = LOADSTATE_NOT_LOADED;
}

void CStreaming::RemoveBigBuildings() {
    /*for (auto i = GetBuildingPool()->GetSize() - 1; i >= 0; i--) {
        CBuilding* building = GetBuildingPool()->GetAt(i);
        if (building && building->m_bIsBIGBuilding && !building->m_bImBeingRendered) {
            building->DeleteRwObject();
            if (!CModelInfo::GetModelInfo(building->m_nModelIndex)->m_nRefCount)
                RemoveModel(building->m_nModelIndex);
        }
    }*/
}

void CStreaming::RemoveBuildingsNotInArea(eAreaCodes areaCode) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x002D5328 + 1 : 0x3979AC), areaCode);

}

void CStreaming::InjectHooks() {

    CHook::Write(g_libGTASA + (VER_x32 ? 0x677D04 : 0x84DA38), &ms_pEndRequestedList);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6766C0 : 0x84ADF0), &ms_pStartRequestedList);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678458 : 0x84E8D8), &ms_pEndLoadedList);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x677238 : 0x84C4B8), &ms_startLoadedList);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x676464 : 0x84A938), &ms_bEnableRequestListPurge);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x6767DC : 0x84B028), &ms_disableStreaming);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x67832C : 0x84E680), &ms_channel);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x676280 : 0x84A580), &ms_numModelsRequested);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678E40 : 0x84FCB0), &ms_numPriorityRequests);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x677D50 : 0x84DAD0), &ms_bLoadingBigModel);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x676CE0 : 0x84BA20), &ms_channelError);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x676214 : 0x84A4A8), &ms_pStreamingBuffer);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x67962C : 0x850C78), &ms_streamingBufferSize);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679EB4 : 0x851D80), &CStreaming::ms_memoryUsed);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006791EC : 0x850408), &CStreaming::ms_memoryAvailable);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006795A4 : 0x850B68), &CStreaming::desiredNumVehiclesLoaded);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676AB8 : 0x84B5D0), &CStreaming::ms_files);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677564 : 0x84CB10), &CStreaming::ms_rwObjectInstances);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677DD0 : 0x84DBD0), &CStreaming::ms_aInfoForModel);

    CHook::Redirect("_ZN10CStreaming13InitImageListEv", &CStreaming::InitImageList);
    CHook::Redirect("_ZN10CStreaming12MakeSpaceForEi", &CStreaming::MakeSpaceFor);
    //CHook::Redirect("_ZN10CStreaming22LoadAllRequestedModelsEb", &CStreaming::LoadAllRequestedModels);
}

int CStreaming::AddImageToList(char const* pFileName, bool bNotPlayerImg) {
    // find a free slot
    std::int32_t fileIndex = 0;
    for (; fileIndex < TOTAL_IMG_ARCHIVES; fileIndex++) {
        if (!ms_files[fileIndex].m_szName[0])
            break;
    }

    if (fileIndex >= TOTAL_IMG_ARCHIVES) {
        assert("AddImageToList");
    }

    // free slot found, load the IMG file
    strcpy(ms_files[fileIndex].m_szName, pFileName);
    ms_files[fileIndex].m_StreamHandle = CdStreamOpen(pFileName);
    ms_files[fileIndex].m_bNotPlayerImg = bNotPlayerImg;
    return fileIndex;
}

void CStreaming::InitImageList() {
    for (auto & ms_file : ms_files) {
        ms_file.m_szName[0] = 0;
        ms_file.m_StreamHandle = 0;
    }

#if VER_SAMP
    CStreaming::AddImageToList("TEXDB\\GTA3.IMG", true);
    CStreaming::AddImageToList("TEXDB\\GTA_INT.IMG", true);
//    CStreaming::AddImageToList("TEXDB\\SAMP.IMG", true);
//    CStreaming::AddImageToList("TEXDB\\SAMPCOL.IMG", true);
#else
    CStreaming::AddImageToList("TEXDB\\GTA3.IMG", true);
    CStreaming::AddImageToList("TEXDB\\GTA_INT.IMG", true);
    CStreaming::AddImageToList("TEXDB\\SAMP.IMG", true);
    CStreaming::AddImageToList("TEXDB\\SAMPCOL.IMG", true);
#endif
}

// Request a given model to be loaded.
// Can be called on an already requested model to add `PRIORITY_REQUEST` flag
void CStreaming::RequestModel(int32 modelId, int32 streamingFlags) {
    CStreamingInfo& info = GetInfo(modelId);

    switch (info.m_nLoadState) {
        case eStreamingLoadState::LOADSTATE_NOT_LOADED:
            break;

        case eStreamingLoadState::LOADSTATE_REQUESTED: {
            // Model already requested, just set priority request flag if not set already
            if ((streamingFlags & STREAMING_PRIORITY_REQUEST) && !info.IsPriorityRequest())
            {
                ++ms_numPriorityRequests;
                info.SetFlags(STREAMING_PRIORITY_REQUEST);
            }
            break;
        }

        default: {
            streamingFlags &= ~STREAMING_PRIORITY_REQUEST; // Remove flag otherwise
            break;
        }
    }
    info.SetFlags(streamingFlags);

    // BUG: Possibly? If the model was requested once with `PRIORITY_REQUEST` set
    //      and later is requested without it `ms_numPriorityRequests` won't be decreased.

    switch (info.m_nLoadState) {
        case eStreamingLoadState::LOADSTATE_LOADED: {
            if (info.InList()) {
                info.RemoveFromList();
                if (IsModelDFF(modelId)) {
                    switch (CModelInfo::GetModelInfo(modelId)->GetModelType()) {
                        case MODEL_INFO_PED:
                        case MODEL_INFO_VEHICLE: {
                            return;
                        }
                    }
                }

                if (!info.IsMissionOrGameRequired())
                    info.AddToList(ms_startLoadedList);
            }
            break;
        }
        case eStreamingLoadState::LOADSTATE_READING:
        case eStreamingLoadState::LOADSTATE_REQUESTED:
        case eStreamingLoadState::LOADSTATE_FINISHING:
            break;

        case eStreamingLoadState::LOADSTATE_NOT_LOADED: {
            switch (GetModelType(modelId)) {
                case eModelType::TXD: { // Request parent (if any) TXD
                    info.m_nLoadState = LOADSTATE_LOADED;
                    return;
                }
                case eModelType::DFF: { // Request TXD and (if any) IFP
                    CBaseModelInfo* modelInfo = CModelInfo::GetModelInfo(modelId);
                  //  RequestTxdModel(modelInfo->m_nTxdIndex, streamingFlags);

                    const int32 animFileIndex = modelInfo->GetAnimFileIndex();
                    if (animFileIndex != -1)
                        RequestModel(IFPToModelId(animFileIndex), STREAMING_KEEP_IN_MEMORY);
                    break;
                }
            }
            info.AddToList(ms_pStartRequestedList);

            ++ms_numModelsRequested;
            if (streamingFlags & STREAMING_PRIORITY_REQUEST)
                ++ms_numPriorityRequests;

            info.SetFlags(streamingFlags);
            info.m_nLoadState = LOADSTATE_REQUESTED;
            break;
        }
    }
}

void CStreaming::AddLodsToRequestList(const CVector* point, int32 streamingFlags) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x2D0C00 + 1 : 0x392B6C), point, streamingFlags);
}

void CStreaming::AddModelsToRequestList(const CVector* point, int32 streamingFlags) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x2D0900 + 1 : 0x392874), point, streamingFlags);
}

#include "Textures/TextureDatabaseRuntime.h"
extern CNetGame *pNetGame;
void CStreaming::Update() {

    if (CTimer::GetIsPaused())
        return;

    if(!CStreaming::GetInfo(MODEL_MALE01).IsLoaded()) {
        RequestModel(MODEL_MALE01, STREAMING_KEEP_IN_MEMORY);
        CStreaming::LoadAllRequestedModels(false);
    }
    CModelInfo::GetModelInfo(MODEL_MALE01)->m_nRefCount = 999;

    if(CTimer::m_snTimeInMillisecondsNonClipped % 100 == 0)
        RemoveLeastUsedModel(STREAMING_KEEP_IN_MEMORY);


    static double previousTime{};
    const double currentTimeInSeconds = CTimer::m_snTimeInMillisecondsNonClipped / 1000.0;
    const double deltaTime = currentTimeInSeconds - previousTime;
    previousTime = currentTimeInSeconds;
    const double clampedDeltaTime = std::min(0.1, deltaTime);
    TextureDatabaseRuntime::UpdateStreaming(clampedDeltaTime, true);

    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));

    const auto& camPos = TheCamera.GetPosition();
    const float fCamDistanceToGroundZ = camPos.z - TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_BOTTOM);
    if (!ms_disableStreaming && !CRenderer::m_loadingPriority) {
        if (fCamDistanceToGroundZ >= 50.0f) {
            if (CGame::CanSeeOutSideFromCurrArea()) {
                AddLodsToRequestList(&camPos, 0);
            }
        }
        else if (CRenderer::ms_bRenderOutsideTunnels) {
            AddModelsToRequestList(&camPos, 0);
        }
    }

//    if (CTimer::GetFrameCounter() % 128 == 106) {
//        m_bBoatsNeeded = false;
//        if (camPos.z < 500.0f) {
//            m_bBoatsNeeded = ThePaths.IsWaterNodeNearby(camPos, 80.0f);
//        }
//    }
    if(!pNetGame || !pNetGame->GetPlayerPool()->GetLocalPlayer())
        return;

    auto pLocalPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_pPed;
    const CVector& playerPos = pLocalPed->GetPosition();
//    if (!ms_disableStreaming
//        && !CCutsceneMgr::IsCutsceneProcessing()
//        && CGame::CanSeeOutSideFromCurrArea()
//        && CReplay::Mode != MODE_PLAYBACK
//        && fCamDistanceToGroundZ < 50.0f
//            ) {
//        StreamVehiclesAndPeds_Always(playerPos);
//        if (!IsVeryBusy()) {
//            StreamVehiclesAndPeds();
//            StreamZoneModels(playerPos);
//        }
//    }
    LoadRequestedModels();

    if (pLocalPed->IsInVehicle()) {
        CVehicleGTA* remoteVehicle = pLocalPed->pVehicle;

        CColStore::AddCollisionNeededAtPosn(&playerPos);
        CIplStore::AddIplsNeededAtPosn(&playerPos);

        const auto& removeVehiclePos = remoteVehicle->GetPosition();
        CColStore::LoadCollision(removeVehiclePos, false);
        CColStore::EnsureCollisionIsInMemory(&removeVehiclePos);
        CIplStore::LoadIpls(removeVehiclePos, false);
        CIplStore::EnsureIplsAreInMemory(&removeVehiclePos);
    }
    else {
        CColStore::LoadCollision(playerPos, false);
        CColStore::EnsureCollisionIsInMemory(&playerPos);
        CIplStore::LoadIpls(playerPos, false);
        CIplStore::EnsureIplsAreInMemory(&playerPos);
    }

    if (ms_bEnableRequestListPurge) {
        PurgeRequestList();
    }
}

// Call `RemoveModel` on all models in the request list except
// those ones which have either `KEEP_IN_MEMORY` or `PRIORITY_REQUEST` flag(s) set.
void CStreaming::PurgeRequestList() {
    auto info = ms_pEndRequestedList->GetPrev();
    while (info != ms_pStartRequestedList) {
        auto prev = info->GetPrev();
        if (!info->IsRequiredToBeKept() && !info->IsPriorityRequest()) {
            RemoveModel(GetModelFromInfo(info));
        }
        info = prev;
    }
}

void CStreaming::LoadRequestedModels() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x2D2104 + 1 : 0x394008));
}

// There are only 2 streaming channels within CStreaming::ms_channel. In this function,
// if your current channelIndex is zero then "1 - channelIndex" will give you the other
// streaming channel within CStreaming::ms_channel which is 1 (second streaming channel).
void CStreaming::LoadAllRequestedModels(bool bOnlyPriorityRequests) {
   static bool m_bLoadingAllRequestedModels{};

    if (m_bLoadingAllRequestedModels) {
        return;
    }
    m_bLoadingAllRequestedModels = true;

    FlushChannels();

    auto numModelsToLoad = std::max(10, 2 * ms_numModelsRequested);
    int32 chIdx = 0;
    while (true) {
        const tStreamingChannel& ch1 = ms_channel[0];
        const tStreamingChannel& ch2 = ms_channel[1];

        if (IsRequestListEmpty()
            && ch1.IsIdle()
            && ch2.IsIdle()
            || numModelsToLoad <= 0
                ) {
            break;
        }

        if (ms_bLoadingBigModel) {
            chIdx = 0;
        }

        auto& currCh = ms_channel[chIdx];

        if (!currCh.IsIdle()) {
            // Finish loading whatever it was loading
            CdStreamSync(chIdx);
            currCh.iLoadingLevel = 100;
        }
        if (currCh.IsReading()) {
            ProcessLoadingChannel(chIdx);
            if (currCh.IsStarted())
                ProcessLoadingChannel(chIdx); // Finish loading big model
        }
        if (bOnlyPriorityRequests && ms_numPriorityRequests == 0)
            break;

        if (!ms_bLoadingBigModel) {
            const auto other = 1 - chIdx;
            if (ms_channel[other].IsIdle()) {
                RequestModelStream(other);
            }

            if (currCh.IsIdle() && !ms_bLoadingBigModel) {
                RequestModelStream(chIdx);
            }
        }

        if (ch1.IsIdle() && ch2.IsIdle())
            break;

        chIdx = 1 - chIdx; // Switch to other channel
        --numModelsToLoad;
    }
    FlushChannels();
    m_bLoadingAllRequestedModels = false;
}

int32 CStreaming::GetNextFileOnCd(uint32 streamLastPosn, bool bNotPriority) {
  //  ZoneScoped;

    uint32 nextRequestModelPos    = UINT32_MAX;
    uint32 firstRequestModelCdPos = UINT32_MAX;
    int32  firstRequestModelId    = MODEL_INVALID;
    int32  nextRequestModelId     = MODEL_INVALID;
    for (auto info = ms_pStartRequestedList->GetNext(); info != ms_pEndRequestedList; info = info->GetNext()) {
        const auto modelId = GetModelFromInfo(info);
        if (bNotPriority && ms_numPriorityRequests != 0 && !info->IsPriorityRequest())
            continue;

        // Additional conditions for some model types (DFF, TXD, IFP)
        switch (GetModelType(modelId)) {
            case eModelType::DFF: {
                CBaseModelInfo* modelInfo = CModelInfo::GetModelInfo(modelId);

//                // Make sure TXD will be loaded for this model
//                const auto txdModel = TXDToModelId(modelInfo->m_nTxdIndex);
//                if (!GetInfo(txdModel).IsLoadedOrBeingRead()) {
//                    RequestModel(txdModel, GetInfo(modelId).GetFlags()); // Request TXD for this DFF
//                    continue;
//                }

                // Check if it has an anim (IFP), if so, make sure it gets loaded
                if(modelInfo) {
                    const int32 animFileIndex = modelInfo->GetAnimFileIndex();
                    if (animFileIndex != -1) {
                        const int32 animModelId = IFPToModelId(animFileIndex);
                        if (!GetInfo(animModelId).IsLoadedOrBeingRead()) {
                            RequestModel(animModelId, STREAMING_KEEP_IN_MEMORY);
                            continue;
                        }
                    }
                }
                break;
            }
//            case eModelType::TXD: {
//                // Make sure parent is/will be loaded
//                TxdDef* texDictionary = CTxdStore::ms_pTxdPool->GetAt(ModelIdToTXD(modelId));
//                const int16 parentIndex = texDictionary->m_wParentIndex;
//                if (parentIndex != -1) {
//                    const int32 parentModelIdx = TXDToModelId(parentIndex);
//                    if (!GetInfo(parentModelIdx).IsLoadedOrBeingRead()) {
//                        RequestModel(parentModelIdx, STREAMING_KEEP_IN_MEMORY);
//                        continue;
//                    }
//                }
//                break;
//            }
//            case eModelType::IFP: {
////                if (CCutsceneMgr::IsCutsceneProcessing() || !GetInfo(MODEL_MALE01).IsLoaded()) {
////                    // Skip in this case
////                    continue;
////                }
//                break;
//            }
        }

        const uint32 modelCdPos = GetInfo(modelId).GetCdPosn();
        if (modelCdPos < firstRequestModelCdPos) {
            firstRequestModelCdPos = modelCdPos;
            firstRequestModelId = modelId;
        }

        if (modelCdPos < nextRequestModelPos && modelCdPos >= streamLastPosn) {
            nextRequestModelPos = modelCdPos;
            nextRequestModelId = modelId;
        }
    }

    const int32 nextModelId = nextRequestModelId == MODEL_INVALID ? firstRequestModelId : nextRequestModelId;
    if (nextModelId != MODEL_INVALID || ms_numPriorityRequests == 0)
        return nextModelId;

    ms_numPriorityRequests = 0;
    return MODEL_INVALID;
}

// Starts reading at most 16 models at a time.
// Removes all unused (if not IsRequiredToBeKept()) IFP/TXDs models as well.
void CStreaming::RequestModelStream(int32 chIdx) {
    int32 modelId = GetNextFileOnCd(CdStreamGetLastPosn(), true);
    if (modelId == MODEL_INVALID)
        return;

    tStreamingChannel& ch = ms_channel[chIdx];
    size_t posn = 0;
    size_t nThisModelSizeInSectors = 0;
    CStreamingInfo* streamingInfo = &GetInfo(modelId);

    // Find first model that has to be loaded
    while (!streamingInfo->IsRequiredToBeKept()) {
        // In case of TXD/IFP's check if they're used at all, if not remove them.
        if (IsModelIFP(modelId)) {
            if (AreAnimsUsedByRequestedModels(ModelIdToIFP(modelId)))
                break;
        } else /*model is neither TXD or IFP*/ {
            break; // No checks needed
        }

        // TXD/IFP unused, so remove it, and go on to the next file

        RemoveModel(modelId);

        streamingInfo->GetCdPosnAndSize(posn, nThisModelSizeInSectors);    // Grab pos and size of this model
        modelId = GetNextFileOnCd(posn + nThisModelSizeInSectors, true);   // Find where the next file is after it
        if (modelId == MODEL_INVALID)
            return; // No more models...
        streamingInfo = &GetInfo(modelId); // Grab next file's info
    }

    // Grab cd pos and size for this model
    streamingInfo->GetCdPosnAndSize(posn, nThisModelSizeInSectors);

    // Check if it's big 0x40CCD5
    if (nThisModelSizeInSectors > ms_streamingBufferSize) {
        // A model is considered "big" if it doesn't fit into a single channel's buffer
        // In which case it has to be loaded entirely by channel 0.
        if (chIdx == 1 || !ms_channel[1].IsIdle())
            return;
        ms_bLoadingBigModel = true;
    }

    // Find all (but at most 16) consecutive models starting at `posn` and load them in one go
    uint32 nSectorsToRead = 0; // The # of sectors to be loaded beginning at `posn`

    bool isPreviousLargeishBigOrVeh = false;
    bool isPreviousModelPed = false;

    // 0x40CD10
    uint32 i = 0;
    for (; i < std::size(ch.modelIds); i++) {
        if (modelId == MODEL_INVALID) {
            break;
        }
        streamingInfo = &GetInfo(modelId);

        if (!streamingInfo->IsRequested())
            break; // Model not requested, so no need to load it.

        if (streamingInfo->GetCdSize())
            nThisModelSizeInSectors = streamingInfo->GetCdSize();

        const bool isThisModelLargeish = nThisModelSizeInSectors > 200;

        if (ms_numPriorityRequests && !streamingInfo->IsPriorityRequest())
            break; // There are priority requests, but this isn't one of them

        CBaseModelInfo* mi = CModelInfo::GetModelInfo(modelId);
        if (IsModelDFF(modelId)) {
            if (isPreviousModelPed && mi->GetModelType() == MODEL_INFO_PED)
                break; // Don't load two peds after each other

            if (isPreviousLargeishBigOrVeh && mi->GetModelType() == MODEL_INFO_VEHICLE)
                break; // Don't load two vehicles / big model + vehicle after each other

            // Check if TXD and/or IFP is loaded for this model.
            // If not we can't load the model yet.

            // Check TXD
//            if (!GetInfo(TXDToModelId(mi->m_nTxdIndex)).IsLoadedOrBeingRead())
//                break;

            // Check IFP (if any)
            const int32 animFileIndex = mi->GetAnimFileIndex();
            if (animFileIndex != -1) {
                if (!GetInfo(IFPToModelId(animFileIndex)).IsLoadedOrBeingRead())
                    break;
            }
        } else {
            if (isPreviousLargeishBigOrVeh && isThisModelLargeish)
                break; // Do not load a big model/car and a big model after each other
        }

        // At this point we've made sure the model can be loaded
        // so let's add it to the channel.

        // Set offset where the model's data begins at
        ch.modelStreamingBufferOffsets[i] = nSectorsToRead;

        // Set the corresponding modelId
        ch.modelIds[i] = modelId;

        // `i == 0` is a special case:
        // If the 0th model doesn't fit into the buffer it's a `big` one
        // so `ms_bLoadingBigModel` is set already (before the `for` loop).
        // But we still need to continue to set the appropriate states for the
        // model, thus we can't just `break` (which would also cause the loop below setting modelId slots `-1`'s to override the modelId)
        if (i > 0) {
            // Check if this model + all the previous fits into one channel's buffer
            if (nSectorsToRead + nThisModelSizeInSectors > ms_streamingBufferSize) {
                // No, so stop at the previous model, and ignore this one
                break;
            }
        }
        nSectorsToRead += nThisModelSizeInSectors;

        if (IsModelDFF(modelId)) {
            switch (mi->GetModelType()) {
                case ModelInfoType::MODEL_INFO_PED:
                    isPreviousModelPed = true;
                    break;
                case ModelInfoType::MODEL_INFO_VEHICLE:
                    isPreviousLargeishBigOrVeh = true; // I guess all vehicles are considered big?
                    break;
            }
        } else {
            if (isThisModelLargeish)
                isPreviousLargeishBigOrVeh = true;
        }

        // Modify the state of models
        {
            streamingInfo->m_nLoadState = LOADSTATE_READING; // Set as being read
            streamingInfo->RemoveFromList(); // Remove from it's current list (That is the requested list)
            ms_numModelsRequested--;
            if (streamingInfo->IsPriorityRequest()) {
                streamingInfo->ClearFlags(STREAMING_PRIORITY_REQUEST); // Remove priority request flag, as its not a request anymore.
                ms_numPriorityRequests--;
            }
        }

        modelId = streamingInfo->m_nNextIndexOnCd; // Continue onto the next one in the directory
    }

    // Set remaining modelId slots to `-1`
    for (auto j = i; j < std::size(ch.modelIds); j++) {
        ch.modelIds[j] = MODEL_INVALID;
    }

    CdStreamRead(chIdx, ms_pStreamingBuffer[chIdx], posn, nSectorsToRead); // Request models to be read
    ch.LoadStatus = eChannelState::READING;
    ch.iLoadingLevel = 0;
    ch.sectorCount = nSectorsToRead; // Set how many sectors to read
    ch.offsetAndHandle = posn;       // And from where to read
    ch.totalTries = 0;

    bool& m_bModelStreamNotLoaded = *reinterpret_cast<bool*>(g_libGTASA + (VER_x32 ? 0x792FBC : 0x972E80));
    if (m_bModelStreamNotLoaded)
        m_bModelStreamNotLoaded = false;
}

// If the channel is done reading (`CdStreamGetStatus(chIdx)` == READING_SUCCESS) then loads all the read models
// using either `ConvertBufferToObject` or `FinishLoadingLargeFile` (in case of big models)
bool CStreaming::ProcessLoadingChannel(int32 chIdx) {
    tStreamingChannel& ch = ms_channel[chIdx];

   // Log("ProcessLoadingChannel");
    const eCdStreamStatus streamStatus = CdStreamGetStatus(chIdx);
    switch (streamStatus) {
        case eCdStreamStatus::READING_SUCCESS:
            break;

        case eCdStreamStatus::READING:
        case eCdStreamStatus::WAITING_TO_READ:
            return false; // Not ready yet.

        case eCdStreamStatus::READING_FAILURE: {
            // Retry

            ch.m_nCdStreamStatus = streamStatus;
            ch.LoadStatus = eChannelState::ERR;

            if (ms_channelError != -1)
                return false;

            ms_channelError = chIdx;
            RetryLoadFile(chIdx);

            return true;
        }
    }
  //  Log("ProcessLoadingChannel1");
    const bool isStarted = ch.IsStarted();
    ch.LoadStatus = eChannelState::IDLE;
    if (isStarted) {
        // It's a large model so finish loading it
        auto bufferOffset = ch.modelStreamingBufferOffsets[0];
        auto* pFileContents = reinterpret_cast<uint8*>(&ms_pStreamingBuffer[chIdx][STREAMING_SECTOR_SIZE * bufferOffset]);
        FinishLoadingLargeFile(pFileContents, ch.modelIds[0]);
        ch.modelIds[0] = MODEL_INVALID;
    } else {
        // Load models individually
        for (uint32 i = 0u; i < std::size(ch.modelIds); i++) {
            const int32 modelId = ch.modelIds[i];
            if (modelId == MODEL_INVALID)
                continue;

            CBaseModelInfo* baseModelInfo = CModelInfo::GetModelInfo(modelId);
            CStreamingInfo& info = GetInfo(modelId);

            if (!IsModelDFF(modelId)
                || baseModelInfo->GetModelType() != MODEL_INFO_VEHICLE /* It's a DFF, check if its a vehicle */
              //  || ms_vehiclesLoaded.CountMembers() < desiredNumVehiclesLoaded /* It's a vehicle, so lets check if we can load more */
              //  || RemoveLoadedVehicle() /* no, so try to remove one, and load this in its place */
                || info.IsMissionOrGameRequired() /* failed, lets check if its absolutely mission critical */
                    ) {
                if (!IsModelIPL(modelId)) {
                    MakeSpaceFor(info.GetCdSize() * STREAMING_SECTOR_SIZE); // IPL's dont require any memory themselves
                }
                const auto bufferOffsetInSectors = ch.modelStreamingBufferOffsets[i];
                auto* fileBuffer = reinterpret_cast <uint8*> (&ms_pStreamingBuffer[chIdx][STREAMING_SECTOR_SIZE * bufferOffsetInSectors]);

                // Actually load the model into memory
                ConvertBufferToObject(fileBuffer, modelId);

                if (info.IsLoadingFinishing()) {
                    ch.LoadStatus = eChannelState::STARTED;
                    ch.modelStreamingBufferOffsets[i] = bufferOffsetInSectors;
                    ch.modelIds[i] = modelId;
                    if (i == 0)
                        continue;
                }
                ch.modelIds[i] = MODEL_INVALID;
            } else {
                // I think at this point it's guaranteed to be a vehicle (thus its a DFF),
                // with `STREAMING_MISSION_REQUIRED` `STREAMING_GAME_REQUIRED` flags unset.

                const int32 modelTxdIdx = baseModelInfo->m_nTxdIndex;
                RemoveModel(modelId);

                if (info.IsMissionOrGameRequired()) {
                    // Re-request it.
                    // I think this code is unreachable, because
                    // if any of the 2 flags (above) is set this code is never reached.
                    RequestModel(modelId, info.GetFlags());
                } else if (!CTxdStore::GetNumRefs(modelTxdIdx))
                    RemoveTxdModel(modelTxdIdx); // Unload TXD, as it has no refs
            }
        }
    }

    if (ms_bLoadingBigModel) {
        if (!ch.IsStarted()) {
            ms_bLoadingBigModel = false;
            for (auto& id : ms_channel[1].modelIds) {
                id = MODEL_INVALID;
            }
        }
    }

    return true;
}

bool CStreaming::ConvertBufferToObject(uint8* fileBuffer, int32 modelId) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x2D2FD0 + 1 : 0x395114), fileBuffer, modelId);
}

// Finishes loading a big model by loading the second half of the file
// residing at `pFileBuffer`.
void CStreaming::FinishLoadingLargeFile(uint8* pFileBuffer, int32 modelId) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x2D36B0 + 1 : 0x395948), pFileBuffer, modelId);
}

void CStreaming::RetryLoadFile(int32 chIdx) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x2D2314 + 1 : 0x394220), chIdx);
}

void CStreaming::MakeSpaceFor(size_t memoryToCleanInBytes) {
    auto lastmemused = ms_memoryUsed;
    while (ms_memoryUsed >= ms_memoryAvailable - memoryToCleanInBytes) {
        lastmemused = ms_memoryUsed;
        if (!RemoveLeastUsedModel(0) || lastmemused == ms_memoryUsed) {
          //  DeleteRwObjectsBehindCamera(ms_memoryAvailable - memoryToCleanInBytes);
            return;
        }
    }
}

//
void CStreaming::DeleteRwObjectsBehindCamera(size_t memoryToCleanInBytes) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x002D5CD8 + 1 : 0x3984F0), memoryToCleanInBytes);
}

// Function name is a little misleading, as it deletes the first entity it can.
bool CStreaming::DeleteLeastUsedEntityRwObject(bool bNotOnScreen, int32 streamingFlags) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x2D5A80 + 1 : 0x39825C), bNotOnScreen, streamingFlags);
}

// The name is misleading: It just removes the first model with no references.
bool CStreaming::RemoveLeastUsedModel(int32 streamingFlags) {

    auto streamingInfo = ms_pEndLoadedList->GetPrev();
    for (; streamingInfo != ms_startLoadedList; streamingInfo = streamingInfo->GetPrev()) {
        const auto modelId = GetModelFromInfo(streamingInfo);
        if (!streamingInfo->AreAnyFlagsSetOutOf(streamingFlags)) {
            switch (GetModelType(modelId)) {
                case eModelType::DFF: {
                    if (!CModelInfo::GetModelInfo(modelId)->m_nRefCount) {
                        RemoveModel(modelId);
                        return true;
                    }
                    break;
                }
//                case eModelType::TXD: {
//                    const auto txdId = ModelIdToTXD(modelId);
//                    if (!CTxdStore::GetNumRefs(txdId) && !AreTexturesUsedByRequestedModels(txdId)) {
//                        RemoveModel(modelId);
//                        return true;
//                    }
//                    break;
//                }
                case eModelType::IFP: {
                    const auto animBlockId = ModelIdToIFP(modelId);
                    if (!CAnimManager::GetNumRefsToAnimBlock(animBlockId) && !AreAnimsUsedByRequestedModels(animBlockId)) {
                        RemoveModel(modelId);
                        return true;
                    }
                    break;
                }
//                case eModelType::SCM: {
//                    const auto scmId = ModelIdToSCM(modelId);
//                    if (!CTheScripts::StreamedScripts.m_aScripts[scmId].m_nStatus) {
//                        RemoveModel(modelId);
//                        return true;
//                    }
//                    break;
//                }
            }
        }
    }

    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));
    if (TheCamera.GetPosition().z - TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_BOTTOM) <= 50.0f){
        return DeleteLeastUsedEntityRwObject(false, streamingFlags);
    }
    return false;
}

bool CStreaming::HasVehicleUpgradeLoaded(int32 modelId) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x002D2F74 + 1 : 0x395098), modelId);
//    if (!GetInfo(modelId).IsLoaded())
//        return false;
//
//    assert(modelId <= INT16_MAX);
//    const int16 otherUpgradeModelId = CVehicleModelInfo::ms_linkedUpgrades.FindOtherUpgrade((int16)modelId);
//    return otherUpgradeModelId == -1 || GetInfo(otherUpgradeModelId).IsLoaded();
}

CLink<CEntityGTA*>* CStreaming::AddEntity(CEntityGTA* entity) {
    switch (entity->GetType()) {
        case ENTITY_TYPE_PED:
        case ENTITY_TYPE_VEHICLE:
            return nullptr;
    }

    auto l = ms_rwObjectInstances.Insert(entity);
    if (!l) { // No more entries left
        // Okay, try deleting something now
        for (auto it = ms_rwObjectInstances.GetTailLink().prev; it != &ms_rwObjectInstances.GetHeadLink(); it = it->prev) {
            const auto e = it->data;

            if (!e->m_bImBeingRendered && !e->m_bStreamingDontDelete) {
                e->DeleteRwObject();
                break;
            }
        }

        // Try inserting again, should succeed now because we've deleted something (Ideally anyways)
        VERIFY(l = ms_rwObjectInstances.Insert(entity));
    }
    return l;
}


char *CStreaming::GetModelCDName(int32 index) {
    return CHook::CallFunction<char*>(g_libGTASA + (VER_x32 ? 0x2CF5D0 + 1 : 0x391320), index);
}
