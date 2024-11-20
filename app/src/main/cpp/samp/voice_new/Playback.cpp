#include "../vendor/bass/bass.h"
#include <opus.h>


#include "../game/common.h"
#include "../game/game.h"

#include "Playback.h"

#include "PluginConfig.h"
#include "Header.h"
#include "main.h"

extern CGame *pGame;

bool Playback::Init() noexcept
{
    if(Playback::initStatus) 
        return false;

    LogVoice("[sv:dbg:playback:init] : module initializing...");

    if(!PluginConfig::IsPlaybackLoaded())
    {
        PluginConfig::SetPlaybackLoaded(true);
        Playback::ResetConfigs();
    }

    LogVoice("[sv:dbg:playback:init] : module initialized");

    Playback::initStatus = true;

    if(!Playback::BassInitHookFunc())
    {
        LogVoice("[sv:err:bassinithook] : failed to initializing bass");
        return false;
    }

    return true;
}

void Playback::Free() noexcept
{
    if(!Playback::initStatus) 
        return;

    LogVoice("[sv:dbg:playback:free] : module releasing...");

    Playback::loadStatus = false;

    LogVoice("[sv:dbg:playback:free] : module released");

    Playback::initStatus = false;
}

void Playback::Tick() noexcept
{
    if(!Playback::loadStatus) return;
    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));

    /*BASS_Set3DPosition(
        reinterpret_cast<const BASS_3DVECTOR*>(&TheCamera.GetMatrix().m_pos), nullptr,
        reinterpret_cast<const BASS_3DVECTOR*>(&TheCamera.GetMatrix().m_forward),
        reinterpret_cast<const BASS_3DVECTOR*>(&TheCamera.GetMatrix().m_up)
    );*/

    BASS_Apply3D();
}

bool Playback::GetSoundEnable() noexcept
{
    return PluginConfig::GetSoundEnable();
}

int Playback::GetSoundVolume() noexcept
{
    return PluginConfig::GetSoundVolume();
}

bool Playback::GetSoundBalancer() noexcept
{
    return PluginConfig::GetSoundBalancer();
}

bool Playback::GetSoundFilter() noexcept
{
    return PluginConfig::GetSoundFilter();
}

void Playback::SetSoundEnable(const bool soundEnable) noexcept
{
    if(!Playback::loadStatus) return;

    PluginConfig::SetSoundEnable(soundEnable);

    if(!PluginConfig::GetSoundEnable()) BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 0);
    else BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 100 * PluginConfig::GetSoundVolume());
}

void Playback::SetSoundVolume(const int soundVolume) noexcept
{
    if(!Playback::loadStatus) return;

    int iSoundVolume = soundVolume;
    if(soundVolume < 0) iSoundVolume = 0;
    else if(soundVolume > 100) iSoundVolume = 100;

    PluginConfig::SetSoundVolume(iSoundVolume);
    //PluginConfig::SetSoundVolume(std::clamp(soundVolume, 0, 100));

    if(PluginConfig::GetSoundEnable())
    {
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 100 * PluginConfig::GetSoundVolume());
    }
}

void Playback::SetSoundBalancer(const bool soundBalancer) noexcept
{
    static HFX balancerFxHandle { NULL };

    if (!Playback::loadStatus) return;

    PluginConfig::SetSoundBalancer(soundBalancer);

    if (PluginConfig::GetSoundBalancer() && balancerFxHandle == NULL)
    {
        balancerFxHandle = BASS_ChannelSetFX(Playback::deviceOutputChannel, BASS_FX_BFX_COMPRESSOR2, 0);

        if(balancerFxHandle == NULL)
        {
            LogVoice("[sv:err:playback:setsoundbalancer] : failed to set balancer effect (code:%d)", BASS_ErrorGetCode());
            return PluginConfig::SetSoundBalancer(false);
        }

        BASS_BFX_COMPRESSOR2 balancerParameters {};

        balancerParameters.lChannel = BASS_BFX_CHANALL;
        balancerParameters.fGain = 10.f;
        balancerParameters.fAttack = 0.01f;
        balancerParameters.fRelease = 0.01f;
        balancerParameters.fThreshold = -40.f;
        balancerParameters.fRatio = 12.f;

        if(BASS_FXSetParameters(balancerFxHandle, &balancerParameters) == 0)
        {
            LogVoice("[sv:err:playback:setsoundbalancer] : failed to set parameters (code:%d)", BASS_ErrorGetCode());
            BASS_ChannelRemoveFX(Playback::deviceOutputChannel, balancerFxHandle);
            balancerFxHandle = NULL; 
            PluginConfig::SetSoundBalancer(false);
        }
    }
    else if(!PluginConfig::GetSoundBalancer() && balancerFxHandle != NULL)
    {
        BASS_ChannelRemoveFX(Playback::deviceOutputChannel, balancerFxHandle);
        balancerFxHandle = NULL;
    }
}

void Playback::SetSoundFilter(const bool soundFilter) noexcept
{
    static HFX filterFxHandle { NULL };

    if(!Playback::loadStatus) return;

    PluginConfig::SetSoundFilter(soundFilter);

    if(PluginConfig::GetSoundFilter() && filterFxHandle == NULL)
    {
        filterFxHandle = BASS_ChannelSetFX(Playback::deviceOutputChannel, BASS_FX_BFX_BQF, 0);

        if(filterFxHandle == NULL)
        {
            LogVoice("[sv:err:playback:setsoundfilter] : failed to set filter effect (code:%d)", BASS_ErrorGetCode());
            return PluginConfig::SetSoundFilter(false);
        }

        BASS_BFX_BQF parameqParameters {};

        parameqParameters.lChannel = BASS_BFX_CHANALL;
        parameqParameters.lFilter = BASS_BFX_BQF_LOWPASS;
        parameqParameters.fCenter = 3400.f;
        parameqParameters.fBandwidth = 0;
        parameqParameters.fQ = 0.707f;

        if(BASS_FXSetParameters(filterFxHandle, &parameqParameters) == 0)
        {
            LogVoice("[sv:err:playback:setsoundfilter] : failed to set parameters (code:%d)", BASS_ErrorGetCode());
            BASS_ChannelRemoveFX(Playback::deviceOutputChannel, filterFxHandle);
            filterFxHandle = NULL; 
            PluginConfig::SetSoundFilter(false);
        }
    }
    else if (!PluginConfig::GetSoundFilter() && filterFxHandle != NULL)
    {
        BASS_ChannelRemoveFX(Playback::deviceOutputChannel, filterFxHandle);
        filterFxHandle = NULL;
    }
}

void Playback::SyncConfigs() noexcept
{
    Playback::SetSoundEnable(PluginConfig::GetSoundEnable());
    Playback::SetSoundVolume(PluginConfig::GetSoundVolume());
    Playback::SetSoundBalancer(PluginConfig::GetSoundBalancer());
    Playback::SetSoundFilter(PluginConfig::GetSoundFilter());
}

void Playback::ResetConfigs() noexcept
{
    PluginConfig::SetSoundEnable(PluginConfig::kDefValSoundEnable);
    PluginConfig::SetSoundVolume(PluginConfig::kDefValSoundVolume);
    PluginConfig::SetSoundBalancer(PluginConfig::kDefValSoundBalancer);
    PluginConfig::SetSoundFilter(PluginConfig::kDefValSoundFilter);
}

bool Playback::BassInitHookFunc() noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    LogVoice("[sv:dbg:playback:bassinithook] : module loading...");

    BASS_SetConfig(BASS_CONFIG_UNICODE, 1);
    BASS_SetConfig(BASS_CONFIG_BUFFER, SV::kChannelBufferSizeInMs);
    BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, SV::kAudioUpdatePeriod);
    BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, SV::kAudioUpdateThreads);
    BASS_SetConfig(BASS_CONFIG_3DALGORITHM, BASS_3DALG_LIGHT);
    
    /*LogVoice("[sv:dbg:playback:bassinithook] : hooked function BASS_Init(device:%d, "
        "freq:%u, flags:0x%x, win:0x%x, dsguid:0x%x)...", device, freq, flags, win, dsguid);*/

    LogVoice("[sv:dbg:playback:bassinithook] : calling function BASS_Init(device:-1, "
        "freq:%u, flags:0x%x)...", SV::kFrequency, BASS_DEVICE_MONO | BASS_DEVICE_3D);

    if(BASS_Init(-1, SV::kFrequency, BASS_DEVICE_MONO | BASS_DEVICE_3D, nullptr, nullptr) == 0)
    {
        LogVoice("[sv:err:playback:bassinithook] : failed to init bass library (code:%d)", BASS_ErrorGetCode());
        return false;
    }

    /*if(HIWORD(BASS_FX_GetVersion()) != BASSVERSION)
    {
        LogVoice("[sv:err:playback:init] : failed to check version bassfx library (code:%d)", BASS_ErrorGetCode());
        return false;
    }*/

    Playback::deviceOutputChannel = BASS_StreamCreate(0, 0, NULL, STREAMPROC_DEVICE, nullptr);

    if(Playback::deviceOutputChannel == NULL)
    {
        LogVoice("[sv:err:playback:init] : failed to create device output channel (code:%d)", BASS_ErrorGetCode());
        return false;
    }

    BASS_Set3DFactors(1.f, 1.f, 0.f);
    BASS_Set3DPosition(&kZeroVector, &kZeroVector, &kZeroVector, &kZeroVector);
    BASS_Apply3D();

    LogVoice("[sv:dbg:playback:bassinithook] : module loaded");

    Playback::loadStatus = true;
    Playback::SyncConfigs();

    return true;
}

bool Playback::initStatus { false };
bool Playback::loadStatus { false };

HSTREAM Playback::deviceOutputChannel { NULL };