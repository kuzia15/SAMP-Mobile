/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "Task.h"
#include "../vendor/armhook/patch.h"

void* CTask::operator new(size_t size) {
   // return GetTaskPool()->New();
}

void CTask::operator delete(void* object) {
    //GetTaskPool()->Delete(static_cast<CTask*>(object));
}

void CTask::StopTimer(const CEvent* event) {
    // NOP
}


bool CTask::IsGoToTask(CTask* task) {
    switch (task->GetTaskType()) {
        case TASK_SIMPLE_GO_TO_POINT:
        case TASK_SIMPLE_GO_TO_POINT_FINE:
            return true;
        default:
            return false;
    }
}

bool CTask::IsTaskPtr(CTask* task) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x004D69F0 + 1 : 0x5D7350), task);
}

void CTask::Serialize() {

}
