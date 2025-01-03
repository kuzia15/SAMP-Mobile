//
// Created by x1y2z on 19.01.2024.
//

#include "TaskSimple.h"

CTaskSimple::CTaskSimple() {
    m_pParentTask = nullptr;
}

// 0x43E300
CTask* CTaskSimple::GetSubTask() {
    return nullptr;
}

// 0x43E310
bool CTaskSimple::IsSimple() {
    return true;
}

// 0x43E320
bool CTaskSimple::SetPedPosition(CPedGTA* ped) {
    return false;
}
