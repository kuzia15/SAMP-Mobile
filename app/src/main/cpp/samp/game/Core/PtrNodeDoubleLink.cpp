/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "PtrNodeDoubleLink.h"
#include "PtrNode.h"
#include "PtrListDoubleLink.h"
#include "game/Pools.h"

void CPtrNodeDoubleLink::InjectHooks() {

}

void* CPtrNodeDoubleLink::operator new(size_t size) {
    return GetPtrNodeDoubleLinkPool()->New();
}

void CPtrNodeDoubleLink::operator delete(void* ptr, size_t sz) {
    GetPtrNodeDoubleLinkPool()->Delete(static_cast<CPtrNodeDoubleLink*>(ptr));
}

// 0x5329A0
void CPtrNodeDoubleLink::AddToList(CPtrListDoubleLink* list) {
    m_prev = nullptr;
    m_next = list->GetNode();

    if (m_next)
        m_next->m_prev = this;

    list->m_node = reinterpret_cast<CPtrNode*>(this);
}
