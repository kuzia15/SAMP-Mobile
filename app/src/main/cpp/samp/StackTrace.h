//
// Created by plaka on 20.03.2023.
//

#ifndef LIVERUSSIA_CSTACKTRACE_H
#define LIVERUSSIA_CSTACKTRACE_H

#include <stdio.h>
#include <dlfcn.h>
#include <dlfcn.h>
#include <unwind.h>
#include "../main.h"

#if VER_x32
#define PRINT_CRASH_STATES(context) \
	FLog("register states:"); \
	FLog("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X", (context)->uc_mcontext.arm_r0, (context)->uc_mcontext.arm_r1, (context)->uc_mcontext.arm_r2, (context)->uc_mcontext.arm_r3); \
	FLog("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x", (context)->uc_mcontext.arm_r4, (context)->uc_mcontext.arm_r5, (context)->uc_mcontext.arm_r6, (context)->uc_mcontext.arm_r7); \
	FLog("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x", (context)->uc_mcontext.arm_r8, (context)->uc_mcontext.arm_r9, (context)->uc_mcontext.arm_r10, (context)->uc_mcontext.arm_fp); \
	FLog("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x", (context)->uc_mcontext.arm_ip, (context)->uc_mcontext.arm_sp, (context)->uc_mcontext.arm_lr, (context)->uc_mcontext.arm_pc); \
    FLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA); \
    FLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA); \
    FLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - g_libSAMP); \
    FLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - g_libSAMP);
#else
#define PRINT_CRASH_STATES(context) \
    FLog("1: libGTASA.so + 0x%llx", context->uc_mcontext.pc - g_libGTASA); \
    FLog("2: libGTASA.so + 0x%llx", context->uc_mcontext.regs[30] - g_libGTASA); \
    FLog("1: libsamp.so + 0x%llx", context->uc_mcontext.pc - g_libSAMP); \
    FLog("2: libsamp.so + 0x%llx", context->uc_mcontext.regs[30] - g_libSAMP);
#endif

void FLog(const char* fmt, ...);

class CStackTrace
{
public:
    static void printBacktrace()
    {
        FLog("------------ START BACKTRACE ------------");
        FLog(" ");
        PrintStackTrace();
    }

private:
    static _Unwind_Reason_Code TraceFunction(_Unwind_Context* context, void* arg) {
        uintptr_t pc = _Unwind_GetIP(context);

        Dl_info info;
        if (dladdr(reinterpret_cast<void*>(pc), &info) && info.dli_sname != nullptr) {
            FLog("[adr: %p samp: %p gta: %p] %s\n",
                     reinterpret_cast<void*>(pc),
                     reinterpret_cast<void*>(pc - g_libSAMP),
                     reinterpret_cast<void*>(pc - g_libGTASA),
                     info.dli_sname);
        } else {
            FLog("[adr: %p samp: %p gta: %p] name not found\n",
                     reinterpret_cast<void*>(pc),
                     reinterpret_cast<void*>(pc - g_libSAMP),
                     reinterpret_cast<void*>(pc - g_libGTASA));
        }

        return _URC_NO_REASON;
    }

    static void PrintStackTrace() {
        _Unwind_Backtrace(TraceFunction, nullptr);
    }

};

#endif //LIVERUSSIA_CSTACKTRACE_H
