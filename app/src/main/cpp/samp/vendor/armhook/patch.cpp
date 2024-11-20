#include "../main.h"
#include "patch.h"

#include <sys/mman.h>

#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen

//uintptr_t CHook::mmap_start 	= 0;
//uintptr_t CHook::mmap_end		= 0;
//uintptr_t CHook::memlib_start	= 0;
//uintptr_t CHook::memlib_end	= 0;

void CHook::UnFuck(uintptr_t addr, size_t len)
{
#if VER_x32
        if(mprotect((void*)(addr & 0xFFFFF000), len, PROT_READ | PROT_WRITE | PROT_EXEC) == 0)
            return;

        mprotect((void*)(addr & 0xFFFFF000), len, PROT_READ | PROT_WRITE);
#else
        if(mprotect((void*)(addr & 0xFFFFFFFFFFFFF000), len, PROT_READ | PROT_WRITE | PROT_EXEC) == 0)
            return;

        mprotect((void*)(addr & 0xFFFFFFFFFFFFF000), len, PROT_READ | PROT_WRITE);
#endif
}

void CHook::InitHookStuff()
{
//    memlib_start = g_libGTASA + 0x180044;
//    memlib_end = memlib_start + 0x450;
//
//    mmap_start = (uintptr_t)mmap(0, PAGE_SIZE, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//    mprotect((void*)(mmap_start & 0xFFFFF000), PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE);
//    mmap_end = (mmap_start + PAGE_SIZE);

    lib = dlopen("libGTASA.so", RTLD_LAZY);
}

