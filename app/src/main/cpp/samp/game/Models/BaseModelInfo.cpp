//
// Created by x1y2z on 27.04.2023.
//
#include "BaseModelInfo.h"
#include "game/TxdStore.h"
#include "../vendor/armhook/patch.h"

void CBaseModelInfo::SetTexDictionary(const char* txdName, const char *dbName) {
    m_nTxdIndex = CTxdStore::FindOrAddTxdSlot(txdName, dbName);
}

ModelInfoType CBaseModelInfo::GetModelType() {
    return CHook::CallFunction<ModelInfoType>(*(uintptr_t*)(vtable + (VER_x32 ? 0x14 : 0x14*2)), this);
}

int32 CBaseModelInfo::GetAnimFileIndex() {
    return CHook::CallFunction<int32>(*(uintptr_t*)(vtable + (VER_x32 ? 0x3C : 0x3C*2)), this);
}

void CBaseModelInfo::DeleteRwObject() {
    CHook::CallFunction<void>(*(uintptr_t*)(vtable + (VER_x32 ? 0x24 : 0x24*2)), this);
}
