//
// Created by x1y2z on 31.07.2023.
//

#include "AtomicModelInfo.h"
#include "../vendor/armhook/patch.h"

void SetAtomicModelInfoFlags(CAtomicModelInfo* modelInfo, uint32 dwFlags) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0046AD34 + 1 : 0x556530), modelInfo, dwFlags);
}