
#include "../main.h"
#include "multitouch.h"
#include "nv_event.h"
#include "../vendor/armhook/patch.h"
#include <string.h>

/* MultiTouch */

extern void AND_TouchEvent_hook(int type, int num, int posX, int posY);

void touch_event(int type, int num, int x, int y)
{
	// LOGI("touch_event: type %d | num %d | x %d | y %d", type, num, x, y);

	AND_TouchEvent_hook(type, num, x, y);
}

int lastNvEvent;
#include "..//nv_event.h"
int32_t(*NVEventGetNextEvent_hooked)(NVEvent* ev, int waitMSecs);
int32_t NVEventGetNextEvent_hook(NVEvent* ev, int waitMSecs)
{
    if(!ev)
        return 0;

    int32_t ret = NVEventGetNextEvent_hooked(ev, waitMSecs);

    lastNvEvent =  ev->m_type;

    NVEvent event;
    if(NVEventGetNextEvent(&event))
    {
        int type = event.m_data.m_multi.m_action & NV_MULTITOUCH_ACTION_MASK;
        int num = (event.m_data.m_multi.m_action & NV_MULTITOUCH_POINTER_MASK) >> NV_MULTITOUCH_POINTER_SHIFT;

        int x1 = event.m_data.m_multi.m_x1;
        int y1 = event.m_data.m_multi.m_y1;

        int x2 = event.m_data.m_multi.m_x2;
        int y2 = event.m_data.m_multi.m_y2;

        int x3 = event.m_data.m_multi.m_x3;
        int y3 = event.m_data.m_multi.m_y3;

        if (type == NV_MULTITOUCH_CANCEL)
        {
            type = NV_MULTITOUCH_UP;
        }

        if ((x1 || y1) || num == 0)
        {
            if (num == 0 && type != NV_MULTITOUCH_MOVE)
            {
                ((void(*)(int, int, int posX, int posY))(g_libGTASA + (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(type, 0, x1, y1); // AND_TouchEvent
            }
            else
            {
                ((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(NV_MULTITOUCH_MOVE, 0, x1, y1); // AND_TouchEvent
            }
        }

        if ((x2 || y2) || num == 1)
        {
            if (num == 1 && type != NV_MULTITOUCH_MOVE)
            {
                ((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(type, 1, x2, y2); // AND_TouchEvent
            }
            else
            {
                ((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(NV_MULTITOUCH_MOVE, 1, x2, y2); // AND_TouchEvent
            }
        }
        if ((x3 || y3) || num == 2)
        {
            if (num == 2 && type != NV_MULTITOUCH_MOVE)
            {
                ((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(type, 2, x3, y3); // AND_TouchEvent
            }
            else
            {
                ((void(*)(int, int, int posX, int posY))(g_libGTASA +  (VER_x32 ? 0x00269740  + 1 : 0x31EC0C)))(NV_MULTITOUCH_MOVE, 2, x3, y3); // AND_TouchEvent
            }
        }
    }

    return ret;
}

int test_pointsArray[1000];
int test_pointersLibArray[1000];

void MultiTouch::initialize()
{
	LOGI("Initializing multi touch..");

    // 3 touch begin
    memset(test_pointsArray, 0, 999 * sizeof(int));
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679E90 : 0x851D38), &test_pointsArray);

    memset(test_pointersLibArray, 0, 999 * sizeof(int));
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006D7178 : 0x8B5028), test_pointersLibArray);

    // 3 touch end
#if VER_x32
    CHook::WriteMemory(g_libGTASA + 0x0026B03C, (uintptr_t)"\x03\x20", 2); // OS_PointerGetNumber
    CHook::WriteMemory(g_libGTASA + 0x0026B046, (uintptr_t) "\x03\x28", 2); // OS_PointerGetType
    CHook::WriteMemory(g_libGTASA + 0x002700AC, (uintptr_t) "\x03\x28", 2); // LIB_PointerGetCoordinates
    CHook::WriteMemory(g_libGTASA + 0x00270118, (uintptr_t) "\x03\x28", 2); // LIB_PointerGetWheel
    CHook::WriteMemory(g_libGTASA + 0x00270164, (uintptr_t) "\x03\x28", 2); // LIB_PointerDoubleClicked
    CHook::WriteMemory(g_libGTASA + 0x002700F2, (uintptr_t) "\x03\x2A", 2); // LIB_PointerGetButton
#else
    CHook::WriteMemory(g_libGTASA + 0x320844, (uintptr_t)"\x60\x00\x80\x52", 4); // OS_PointerGetNumber
    CHook::WriteMemory(g_libGTASA + 0x320850, (uintptr_t) "\x1F\x0C\x00\x71", 4); // OS_PointerGetType
    CHook::WriteMemory(g_libGTASA + 0x326FEC, (uintptr_t) "\x1F\x0C\x00\x71", 4); // LIB_PointerGetCoordinates
    CHook::WriteMemory(g_libGTASA + 0x32706C, (uintptr_t) "\x1F\x0C\x00\x71", 4); // LIB_PointerGetWheel
    CHook::WriteMemory(g_libGTASA + 0x3270AC, (uintptr_t) "\x1F\x0C\x00\x71", 4); // LIB_PointerDoubleClicked
    CHook::WriteMemory(g_libGTASA + 0x327040, (uintptr_t) "\x1F\x0D\x00\x71", 4); // LIB_PointerGetButton
#endif

	// NVEventGetNextEvent
    CHook::InlineHook("_Z19NVEventGetNextEventP7NVEventi", NVEventGetNextEvent_hook, &NVEventGetNextEvent_hooked);
}