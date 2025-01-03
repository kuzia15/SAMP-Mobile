//
// Created by x1y2z on 14.04.2023.
//

#include "Timer.h"
#include "../vendor/armhook/patch.h"
#include "../java/jniutil.h"
#include "../gui/gui.h"
#include "../net/netgame.h"

extern CJavaWrapper *pJavaWrapper;
extern UI *pUI;
extern CNetGame* pNetGame;

uint8_t* gTimerRunning = nullptr;

float CTimer::game_FPS = 0;

bool        CTimer::m_CodePause = false;
bool        CTimer::m_UserPause = false;
float       CTimer::ms_fTimeScale;
uint32_t    CTimer::m_FrameCounter = 0;
uint32_t    CTimer::m_snTimeInMilliseconds = 0;
bool        CTimer::bSkipProcessThisFrame = false;
float       CTimer::ms_fTimeStep = 0;
uint32_t    CTimer::m_snPPPPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snPPPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snPPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snPreviousTimeInMilliseconds;
uint32_t    CTimer::m_snTimeInMillisecondsNonClipped;
uint32_t    CTimer::m_snPreviousTimeInMillisecondsNonClipped;

void CTimer::InjectHooks()
{
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006794BC : 0x8509A0), &CTimer::m_CodePause);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067892C : 0x84F280), &CTimer::m_FrameCounter);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677674 : 0x84CD28), &CTimer::game_FPS);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006776A8 : 0x84CD90), &CTimer::m_UserPause);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067689C : 0x84B1A8), &CTimer::ms_fTimeScale);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676FF0 : 0x84C030), &CTimer::m_snTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679248 : 0x8504B8), &CTimer::bSkipProcessThisFrame);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067770C : 0x84CE50), &CTimer::ms_fTimeStep);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x0067949C : 0x850960), &CTimer::m_snPPPPreviousTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677DF0 : 0x84DC10), &CTimer::m_snPPPreviousTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679D3C : 0x851A90), &CTimer::m_snPPreviousTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006779A8 : 0x84D388), &CTimer::m_snPreviousTimeInMilliseconds);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00679DBC : 0x851B90), &CTimer::m_snTimeInMillisecondsNonClipped);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006775F8 : 0x84CC30), &CTimer::m_snPreviousTimeInMillisecondsNonClipped);

    gTimerRunning = &*(uint8_t*)(g_libGTASA + (VER_x32 ? 0x0096B524 : 0xBDC5AC));

    CHook::Redirect("_ZN6CTimer14StartUserPauseEv", &CTimer::StartUserPause);
    CHook::Redirect("_ZN6CTimer12EndUserPauseEv", &CTimer::EndUserPause);
    CHook::Redirect("_ZN6CTimer4StopEv", &CTimer::Stop);
    CHook::Redirect("_ZN6CTimer21GetIsSlowMotionActiveEv", &CTimer::GetIsSlowMotionActive);
}


uint64_t GetMillisecondTime() {

}

// 0x5617E0
void CTimer::Initialise()
{

}

// 0x5618C0
void CTimer::Shutdown() {

}

// 0x5619D0
void CTimer::Suspend()
{
    CHook::CallFunction<void>("_ZN6CTimer7SuspendEv");
}

// 0x561A00
void CTimer::Resume()
{
    CHook::CallFunction<void>("_ZN6CTimer6ResumeEv");
}

// 0x561AA0
void CTimer::Stop()
{
    *gTimerRunning = 0;
    CTimer::m_snPPPPreviousTimeInMilliseconds = CTimer::m_snTimeInMilliseconds;
    CTimer::m_snPPPreviousTimeInMilliseconds = CTimer::m_snTimeInMilliseconds;
    CTimer::m_snPPreviousTimeInMilliseconds = CTimer::m_snTimeInMilliseconds;
    CTimer::m_snPreviousTimeInMilliseconds = CTimer::m_snTimeInMilliseconds;

    CTimer::m_snPreviousTimeInMillisecondsNonClipped = CTimer::m_snTimeInMillisecondsNonClipped;
}

// 0x561AF0
void CTimer::StartUserPause()
{
    if (pUI) pUI->setVisible(false);

    if(pJavaWrapper && pNetGame)
    {
        pJavaWrapper->SetPauseState(true);
    }
    m_UserPause = true;
}

// 0x561B00
void CTimer::EndUserPause()
{
    // process resume event
    if (pUI) pUI->setVisible(true);
    if(pJavaWrapper && pNetGame)
    {
        pJavaWrapper->SetPauseState(false);
    }
    m_UserPause = false;
}

// 0x561A40
uint32_t CTimer::GetCyclesPerMillisecond()
{
    return CHook::CallFunction<uint32_t>(g_libGTASA + (VER_x32 ? 0x0042100C + 1 : 0x504858));
}

// cycles per ms * 20
// 0x561A50
uint32_t CTimer::GetCyclesPerFrame()
{

}

uint64_t CTimer::GetCurrentTimeInCycles()
{
    return CHook::CallFunction<uint64_t>(g_libGTASA + (VER_x32 ? 0x00421040 + 1 : 0x504888));
}

// 0x561AD0
bool CTimer::GetIsSlowMotionActive()
{
    return CTimer::ms_fTimeScale < 1.0;
}

// 0x5618D0
void CTimer::UpdateVariables(float timeElapsed)
{

}

// 0x561B10
void CTimer::Update()
{

}

uint32_t CTimer::GetCurrentUnixTimeMoscow() {
    auto now = std::chrono::system_clock::now();

    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* ptm = std::gmtime(&now_c);
    ptm->tm_hour += 3;
    std::time_t moscow_time = std::mktime(ptm);

    auto duration = std::chrono::system_clock::from_time_t(moscow_time).time_since_epoch();
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}
