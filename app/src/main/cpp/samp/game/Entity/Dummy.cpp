//
// Created by x1y2z on 19.04.2023.
//
#include "../common.h"
#include "game/Pools.h"
#include "Dummy.h"

void* CDummy::operator new(size_t size) {
    return GetDummyPool()->New();
}

// 0x5326F0
void CDummy::operator delete(void* obj) {
    GetDummyPool()->Delete(static_cast<CDummy*>(obj));
}

// 0x532540
//CDummy::CDummy() : CEntity() {
//    m_nType = ENTITY_TYPE_DUMMY;
//}

bool IsDummyPointerValid(CDummy* dummy) {
    return GetDummyPool()->IsObjectValid(dummy);
}