//
// Created by x1y2z on 28.04.2023.
//

#include <cstdlib>
#include "MemoryMgr.h"
#include "common.h"
#include "../vendor/armhook/patch.h"

void CMemoryMgr::Init() {
    CHook::CallFunction<void>("_ZN10CMemoryMgr4InitEv");
}

void CMemoryMgr::Free(void* memory) {
    return free(memory);
}

void* CMemoryMgr::MoveMemory(void* memory) {
    return CHook::CallFunction<void*>(g_libGTASA + (VER_x32 ? 0x005D3032 + 1 : 0x6F7C90), memory);
}

void* CMemoryMgr::Malloc(uint32 size, uint32 nHint) {
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
    _LOCK_MEMORYHEAP();
    void* memory = nullptr;
    int32 iAttempts = 0;
    while (true) {
        memory = InternalMalloc(size);
        if (memory) {
            break;
        }
        if (_gfnMemCompact) {
            _UNLOCK_MEMORYHEAP();
            // sub_2644FC(_nSize, ++i);
            _gfnMemCompact(size, ++iAttempts);
            _LOCK_MEMORYHEAP();
        } else {
            _FATAL("Failed to allocate %d bytes!", size);
        }
    }
    RegisterMalloc(memory);
    CMemoryHeap::SetDebugInfo(memory, nHint);
    _UNLOCK_MEMORYHEAP();
    return memory;
#else
    return ::malloc(size);
#endif
}

void* CMemoryMgr::Malloc(uint32 size) {
    void* memory = Malloc(size, 0);
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    GET_HEAP_DESC(memory)->m_Flags.NoDebugHint = true;
#endif
    return memory;
}

void CMemoryMgr::FreeAlign(void* memory) {
   // return plugin::Call<0x72F4F0, void*>(memory);
    Free(*((void**)memory - 1));
}

uint8* CMemoryMgr::MallocAlign(uint32 size, uint32 align, uint32 nHint) {
    auto* memory = Malloc(size + align, nHint);

    auto* result = (void*)(uintptr ((uint8*)memory + align) & ~(align - 1));
    *((void**)result - 1) = memory;
    return static_cast<uint8*>(result);
}

void* CMemoryMgr::MallocAlign(uint32 size, uint32 align) {
  //  return plugin::CallAndReturn<void*, 0x72F4C0, uint32, uint32>(size, align);

    void* memory = MallocAlign(size, align, 0);
#if defined MEMORY_MGR_USE_HEAP_FLAGS
    GET_HEAP_DESC(memory)->m_Flags.NoDebugHint = true;
#endif
    return memory;
}