//
// Created by plaka on 10.05.2023.
//

#include "PedIntelligence.h"
#include "../vendor/armhook/patch.h"

void CPedIntelligence::Process() {

}

void CPedIntelligence::FlushIntelligence() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x004C1508 + 1 : 0x5BE0E0), this);
//    m_TaskMgr.Flush();
//    m_eventHandler.m_physicalResponseTask = nullptr;
//    m_eventHandler.m_eventResponseTask = nullptr;
//    m_eventHandler.m_attackTask = nullptr;
//    m_eventHandler.m_sayTask = nullptr;
//    m_eventHandler.m_partialAnimTask = nullptr;
//    m_eventHandler.m_history.ClearAllEvents();
//    m_eventGroup.Flush(false);
//    m_vehicleScanner.Clear();
//    m_pedScanner.Clear();
//    m_eventScanner.m_attractorScanner.Clear();
}

//
//CPedIntelligence::CPedIntelligence(CPedGTA* ped) :
//        m_pPed{ ped },
//        m_TaskMgr{ CTaskManager(ped) },
//        m_eventHandler{ CEventHandler(ped) }
//       // m_eventGroup{ CEventGroup(ped) }
//{
////    m_nDecisionMakerType                  = DM_EVENT_UNDEFINED;
////    m_nDecisionMakerTypeInGroup           = -1;
////    m_fHearingRange                       = 15.0f;
////    m_fSeeingRange                        = 15.0f;
////    m_nDmNumPedsToScan                    = 3;
////    m_fDmRadius                           = 15.0f;
////    field_CC                              = 30.0f;
////    field_D0                              = -1;
////    m_nEventId                            = 0;
////    m_nEventPriority                      = 0;
////    field_188                             = 0;
////    m_collisionScanner.m_bAlreadyHitByCar = false;
////    m_AnotherStaticCounter                = 0;
////    m_StaticCounter                       = 0;
////    if (IsPedTypeGang(ped->m_nPedType)) {
////        m_fSeeingRange = 40.f;
////        m_fHearingRange = 40.f;
////    }
////    std::ranges::fill(m_apInterestingEntities, nullptr);
//}

CTaskSimpleJetPack* CPedIntelligence::GetTaskJetPack() {
//    if (m_pPed->IsPlayer()) {
//        return CTask::DynCast<CTaskSimpleJetPack>(m_TaskMgr.GetSimplestActiveTask());
//    }
    return nullptr;
}

void CPedIntelligence::FlushImmediately(bool bSetPrimaryDefaultTask) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x4C0AB4 + 1 : 0x5BD2D0), this, bSetPrimaryDefaultTask);

//    CTask* primaryTask = m_TaskMgr.GetTaskPrimary(TASK_PRIMARY_PRIMARY);
//    CTaskComplex* taskBeInGroup = nullptr;
//    if (primaryTask && primaryTask->GetTaskType() == TASK_COMPLEX_BE_IN_GROUP) {
//        taskBeInGroup = (CTaskComplex*)primaryTask->Clone();
//    }
//
//    CTaskSimpleHoldEntity* taskSimpleHoldEntity = nullptr;
//    CTask* secondaryTask = m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
//    if (secondaryTask && secondaryTask->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY) {
//        taskSimpleHoldEntity = (CTaskSimpleHoldEntity*)secondaryTask;
//    }
//
//    int32 objectType = -1;
//    bool bIsEntityVisible = false;
//    CObjectGta* objectToHold = nullptr;
//    CTaskSimpleHoldEntity* taskSimpleHoldEntityCloned = nullptr;
//    if (taskSimpleHoldEntity && taskSimpleHoldEntity->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY) {
//        objectToHold = (CObjectGta*)taskSimpleHoldEntity->m_pEntityToHold;
//        if (objectToHold) {
//            if (objectToHold->IsObject()) {
//                objectType = objectToHold->m_nObjectType;
//                bIsEntityVisible = objectToHold->m_bIsVisible;
//            }
//            taskSimpleHoldEntityCloned = (CTaskSimpleHoldEntity*)taskSimpleHoldEntity->Clone();
//        }
//    }
//
//    CTaskComplexFacial* taskComplexFacialCloned = nullptr;
//    CTask* taskComplexFacial = m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX);
//    if (taskComplexFacial && taskComplexFacial->GetTaskType() == TASK_COMPLEX_FACIAL) {
//        taskComplexFacialCloned = dynamic_cast<CTaskComplexFacial*>(taskComplexFacial->Clone());
//    }
//
////    m_eventGroup.Flush(true);
//    m_eventHandler.FlushImmediately();
//    m_TaskMgr.FlushImmediately();
// //   CPedScriptedTaskRecord::Process();
//
//    delete taskBeInGroup;
//
//
//    if (taskSimpleHoldEntityCloned) {
//        if (objectType != -1) {
//            objectToHold->m_nObjectType = objectType;
//            objectToHold->m_bIsVisible = bIsEntityVisible;
//        }
//        m_TaskMgr.SetTaskSecondary((CTaskComplex*)taskSimpleHoldEntityCloned, TASK_SECONDARY_PARTIAL_ANIM);
//        taskSimpleHoldEntityCloned->ProcessPed(m_pPed);
//    }
//
//    if (taskComplexFacialCloned) {
//        m_TaskMgr.SetTaskSecondary(taskComplexFacialCloned, TASK_SECONDARY_FACIAL_COMPLEX);
//    }
//
//    if (bSetPrimaryDefaultTask) {
//        auto taskSimplePlayerOnFoot = new CTaskSimplePlayerOnFoot();
//        m_TaskMgr.SetTask(taskSimplePlayerOnFoot, TASK_PRIMARY_DEFAULT, false);
//        return;
//    }
}
//
//void CPedIntelligence::ProcessAfterPreRender() {
//    //g_LoadMonitor.StartTimer(0);
//
//    CTask* secondaryTask = m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
//    if (secondaryTask && secondaryTask->IsSimple())
//    {
//        if (static_cast<CTaskSimple*>(secondaryTask)->SetPedPosition(m_pPed))
//        {
//            CTask* secondaryAttackTask = m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK);
//            if (secondaryAttackTask && secondaryAttackTask->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY)
//            {
//                static_cast<CTaskSimpleUseGun*>(secondaryAttackTask)->SetPedPosition(m_pPed);
//            }
//        }
//    }
//
//    CTask* secondaryAttackTask = m_TaskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK);
//    if (secondaryAttackTask && secondaryAttackTask->GetTaskType() == TASK_SIMPLE_USE_GUN)
//    {
//        static_cast<CTaskSimpleUseGun*>(secondaryAttackTask)->SetPedPosition(m_pPed);
//    }
//
////    CWeapon* activeWeapon = &m_pPed->GetActiveWeapon();
////    if (activeWeapon->m_nType == WEAPON_MOLOTOV && activeWeapon->m_pFxSystem)
////    {
////        RpHAnimHierarchy* animHierarchy = GetAnimHierarchyFromSkinClump(m_pPed->m_pRwClump);
////        int32 animIDIndex = RpHAnimIDGetIndex(animHierarchy, 24); // 24 = BONE_R_HAND?
////        RwMatrix* matrixArray = RpHAnimHierarchyGetMatrixArray(animHierarchy);
////
////        RwV3d pointIn = { 0.05f, 0.05f,  0.14f };
////        RwV3d pointOut;
////        RwV3dTransformPoint(&pointOut, &pointIn, &matrixArray[animIDIndex]);
////
////        RwMatrix matrix;
////        memcpy(&matrix, m_pPed->GetModellingMatrix(), sizeof(matrix));
////        matrix.pos = pointOut;
////        RwMatrixUpdate(&matrix);
////        activeWeapon->m_pFxSystem->SetMatrix(&matrix);
////    }
////
////    if (m_pPed->bInVehicle)
////    {
////        CVehicleGta* vehicle = m_pPed->pVehicle;
////        if (vehicle && vehicle->IsBike()) {
////            vehicle->AsBike()->FixHandsToBars(m_pPed);
////        }
////    }
//
//  //  g_LoadMonitor.EndTimer(0);
//}
//
//void CPedIntelligence::ProcessAfterProcCol() {
//   // g_LoadMonitor.StartTimer(0);
//
//    auto* activeSimplestTask = m_TaskMgr.GetSimplestActiveTask();
//    if (activeSimplestTask && activeSimplestTask->IsSimple()) {
//        bool bPositionSet = static_cast<CTaskSimple*>(activeSimplestTask)->SetPedPosition(m_pPed);
//        if (!bPositionSet) {
//            auto* simplestTask = m_TaskMgr.GetSimplestTask(TASK_PRIMARY_DEFAULT);
//            if (simplestTask && simplestTask->IsSimple()) {
//                bPositionSet = simplestTask->AsSimple()->SetPedPosition(m_pPed);
//            }
//        }
//
//        if (bPositionSet) {
//            m_pPed->UpdateRW();
//            m_pPed->UpdateRwFrame();
//        }
//    }
//
//    m_pPed->bCalledPreRender = 0;
//
// //   g_LoadMonitor.EndTimer(0);
//}

// -------- hooks

void CPedIntelligence__Process(CPedIntelligence* thiz) {
    thiz->Process();
}

void CPedIntelligence::InjectHooks() {

}

void CPedIntelligence::ProcessAfterPreRender() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x4C11BC + 1 : 0x5BDC18), this);
}
