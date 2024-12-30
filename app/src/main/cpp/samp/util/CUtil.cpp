//
// Created by roman on 11/19/2024.
//

#include <dlfcn.h>
#include "CUtil.h"
#include "game/Textures/TextureDatabaseRuntime.h"


uintptr_t CUtil::FindLib(const char* libname)
{
    void* handle = dlopen(libname, RTLD_LAZY);
    if (handle) {
        void* symbol = dlsym(handle, "JNI_OnLoad");
        if (symbol) {
            Dl_info info;
            if (dladdr(symbol, &info) != 0) {
                return reinterpret_cast<uintptr_t>(info.dli_fbase);
            }
        }
        dlclose(handle);
    }

    return 0;
}

RwTexture* CUtil::CUtil::LoadTextureFromDB(const char* dbname, const char* texture)
{
    TextureDatabaseRuntime* db_handle = TextureDatabaseRuntime::GetDatabase(dbname);
    if(!db_handle)
    {
        FLog("Error: Database not found! (%s)", dbname);
        return nullptr;
    }

    TextureDatabaseRuntime::Register(db_handle);

    auto tex = CUtil::GetTexture(texture);
    if(!tex)
    {
        FLog("Error: Texture (%s) not found in database (%s)", dbname, texture);
        return nullptr;
    }

    TextureDatabaseRuntime::Unregister(db_handle);

    return tex;
}

RwTexture* CUtil::GetTexture(const char* name)
{
    auto tex = TextureDatabaseRuntime::GetTexture(name);
    if (!tex)
    {
        //tex = CUtil::LoadTextureFromDB("gta3", "ahoodfence2");
        FLog("WARNING! No tex = %s", name);
        return 0;
    }
    ++tex->refCount;

    return tex;
}

void __fastcall CUtil::TransformPoint(RwV3d &result, const CSimpleTransform &t, const RwV3d &v)
{
    float cos_heading = cosf(t.m_fHeading);
    float sin_heading = sinf(t.m_fHeading);

    result = {
            t.m_vPosn.x + cos_heading * v.x - sin_heading * v.y,
            t.m_vPosn.y + sin_heading * v.x + cos_heading * v.y,
            v.z + t.m_vPosn.z
    };
}