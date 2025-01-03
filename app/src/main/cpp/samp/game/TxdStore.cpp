//
// Created by x1y2z on 01.07.2023.
//

#include "TxdStore.h"
#include "../vendor/armhook/patch.h"

int32 CTxdStore::GetNumRefs(int32 index){
    return CHook::CallFunction<int32>(g_libGTASA + (VER_x32 ? 0x5D3E34 + 1 : 0x6F8DF4), index);
}

void CTxdStore::RemoveTxd(int32 index) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x5D40B8 + 1 : 0x6F9130), index);
}

void CTxdStore::InjectHooks() {
  //  CHook::Redirect(g_libGTASA, 0x0055BF14, &CTxdStore::GetNumRefs);
}

int32 CTxdStore::FindTxdSlot(const char *name) {
    return CHook::CallFunction<int32>(g_libGTASA + (VER_x32 ? 0x005D3EB0 + 1 : 0x6F8EA0), name);
}

int32 CTxdStore::FindTxdSlot(uint32 hash) {
    assert("NO x64 call");
}

int32 CTxdStore::AddTxdSlot(const char *name, const char *dbName, bool keepCPU) {
    return CHook::CallFunction<int32>(g_libGTASA + (VER_x32 ? 0x005D3B84 + 1 : 0x6F8A68), name, dbName, keepCPU);
}

void CTxdStore::Initialise() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005D3A90 + 1 : 0x6F8928));
}

void CTxdStore::PushCurrentTxd() {

    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005D41D4 + 1 : 0x6F92A8));
}

void CTxdStore::PopCurrentTxd() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005D4214 + 1 : 0x6F92D4));
}

void CTxdStore::SetCurrentTxd(int32 index) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x5D4144 + 1 : 0x6F91EC), index, nullptr);
}
