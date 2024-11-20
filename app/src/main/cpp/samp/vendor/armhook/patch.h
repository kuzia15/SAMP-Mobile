#pragma once
#include "../main.h"
#include <string.h>
#include <dlfcn.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <ctype.h>
#include <link.h>
#include <sys/cachectl.h>

#ifdef __arm__
#define __32BIT
#define DETHUMB(_a) (((uintptr_t)_a) & ~0x1)
#define RETHUMB(_a) (((uintptr_t)_a) | 0x1)
#define THUMBMODE(_a) ((((uintptr_t)_a) & 0x1)||(((uintptr_t)_a) & 0x2)||(CHook::GetAddrBaseXDL((uintptr_t)_a) & 0x1))
extern "C" bool MSHookFunction(void* symbol, void* replace, void** result);
#elif defined __aarch64__
#define __64BIT
    #define DETHUMB(_a)
    #define RETHUMB(_a)
    #define THUMBMODE(_a) (false)
    #define cacheflush(c, n, zeroarg) __builtin___clear_cache((char*)(c), (char*)(n))
#else
    #error This lib is supposed to work on ARM only!
#endif

//#if VER_x32
#define GET_LR(dwRetAddr) \
    do { \
        uintptr_t lr = reinterpret_cast<uintptr_t>(__builtin_return_address(0)); \
        dwRetAddr = lr - g_libGTASA; \
    } while (0)
//#endif


#define SET_TO(__a1, __a2) *(void**)&(__a1) = (void*)(__a2)

#include "shadowhook.h"

class CHook {
public:

    static uintptr_t mmap_start, mmap_end, memlib_start, memlib_end;

    static void UnFuck(uintptr_t ptr, size_t len = PAGE_SIZE);

    static uintptr_t GetAddrBaseXDL(uintptr_t addr)
    {
#ifdef __XDL
        xdl_info_t info;
        void* cache = NULL;
        if(!xdl_addr((void*)addr, &info, &cache)) return 0;
        xdl_addr_clean(&cache);
        return (uintptr_t)info.dli_saddr;
#endif
        return 0;
    }



    template<typename Addr>
    static void NOP(Addr adr, size_t count)
    {
        // fully check
        auto addr = (uintptr)(adr);
#if VER_x32
        int bytesCount = 2 * count;
        uintptr_t endAddr = addr + bytesCount;
        UnFuck(addr, bytesCount);
        for (uintptr_t p = addr; p != endAddr; p += 2)
        {
            (*(char*)(p + 0)) = 0x00;
            (*(char*)(p + 1)) = 0xBF;
        }
        cacheflush(addr, endAddr, 0);

#else
        if(count > 1)
            count /= 2;

        int bytesCount = 4 * count;
        uintptr_t endAddr = addr + bytesCount;
        UnFuck(addr, bytesCount);
        for (uintptr_t p = addr; p != endAddr; p += 4)
        {
            (*(char*)(p + 0)) = 0x1F;
            (*(char*)(p + 1)) = 0x20;
            (*(char*)(p + 2)) = 0x03;
            (*(char*)(p + 3)) = 0xD5;
        }
        cacheflush(addr, endAddr, 0);
#endif
    }

    static void RET(const char* sym)
    {
        auto addr = getSym(sym);
//        #if VER_x32
//            addr -= 1;
//        #endif
        RET(addr);
    }

    static void RET(uintptr addr)
    {
        #if VER_x32
        if(THUMBMODE(addr))
        {
            WriteMemory(DETHUMB(addr), (uintptr_t)"\x70\x47", 2);
        }
        else
        {
            WriteMemory(addr, (uintptr_t)"\x1E\xFF\x2F\xE1", 4);
        }
        #else
            WriteMemory(addr, (uintptr_t)"\xC0\x03\x5F\xD6", 4);
        #endif
    }

    template <typename Src>
    static void WriteMemory(uintptr_t dest, Src src, size_t size)
    {
        UnFuck(dest, size);
        memcpy((void*)dest, (void*)src, size);

        cacheflush(dest, dest + size, 0);
    }

    template <typename Src>
    static Src Write(uintptr_t dest, Src src, size_t size = 0)
    {   
	    size = sizeof(Src);
        CHook::WriteMemory(dest, &src, size);
	    return src;
    }

    template <typename Src>
    static Src Write(const char* sym, Src src, size_t size = 0)
    {
        auto addr = (uintptr_t)dlsym(lib, sym);
        size = sizeof(Src);
        CHook::WriteMemory(addr, &src, size);
        return src;
    }

    static void InitHookStuff();
    static inline void* lib;

    static uintptr_t getSym(const char* sym)
    {

        auto res = (uintptr_t)dlsym(lib, sym);
        if(res == 0){
            FLog("Error find %s", sym);
            exit(0);
            return 0;
        }
       // Log("getsym = %x", sym - g_libGTASA);
        return res;
    }

    template <typename Ret, typename... Args>
    static inline Ret CallFunction(uintptr address, Args... args)
    {
        return (( Ret(*)(Args...))(address) )(args...);
    }

    template <typename Ret, typename... Args>
    static Ret CallFunction(const char* sym, Args... args)
    {
        auto addr = (uintptr_t)dlsym(lib, sym);

        if(addr == 0){
            FLog("Error find %s", sym);
            exit(0);
        }

        using FuncType = Ret(__cdecl *)(Args...);
        FuncType func = reinterpret_cast<FuncType>(addr);
        return func(args...);
    }

    template <typename Addr, typename Func, typename Orig>
    static void InstallPLT(Addr addr, Func hook_func, Orig* orig_func)
    {
        UnFuck(addr);

        *orig_func = reinterpret_cast<Orig>(*(uintptr_t*)addr);

        *(uintptr_t*)addr = reinterpret_cast<uintptr_t>(hook_func);
    }

    template <typename Addr, typename Func>
    static void InstallPLT(Addr addr, Func hook_func)
    {
        UnFuck(addr);
        *(uintptr_t*)addr = reinterpret_cast<uintptr_t>(hook_func);
    }

    template <typename Addr, typename Func, typename Orig>
    static void InlineHook(uintptr_t lib, Addr addr, Func func, Orig orig)
    {
        shadowhook_hook_func_addr(
                (void*)(lib + addr + 1),
                (void *)func,
                (void **)orig);
    }

    template <typename Func, typename Orig>
    static void InlineHook(const char* sym, Func func, Orig orig)
    {
        auto addr = getSym(sym);

        if(addr == 0){
            FLog("Error find %s", sym);
            exit(0);
        }

        shadowhook_hook_func_addr(
                (void*)(addr),
                (void *)func,
                (void **)orig);
    }

    template <typename Func>
    static void Redirect(const char* sym, Func func)
    {
        auto addr = getSym(sym);

#ifdef __32BIT
        uint32_t hook[2] = {0xE51FF004, reinterpret_cast<uintptr_t>(func)};
        if (THUMBMODE(addr)) {
            addr &= ~0x1;
            if (addr & 0x2) {
                NOP(RETHUMB(addr), 1);
                addr += 2;
            }
            hook[0] = 0xF000F8DF;
        }
        WriteMemory(DETHUMB(addr), reinterpret_cast<uintptr_t>(hook), sizeof(hook));
#elif defined __64BIT
        UnFuck(addr, 16);
        uint64_t hook[2] = {0xD61F022058000051, reinterpret_cast<uintptr_t>(func)};
        WriteMemory(addr, reinterpret_cast<uintptr_t>(hook), sizeof(hook));
#endif
    }
};