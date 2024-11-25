
#include "../common.h"
#include "../Pools.h"
#include "Building.h"
#include "../Models/ModelInfo.h"
#include "../../vendor/armhook/patch.h"
#include "../Enums/eEntityType.h"


int32 gBuildings;

void CBuilding::InjectHooks()
{
}

CBuilding::CBuilding() : CEntityGTA()
{
    m_nType = ENTITY_TYPE_BUILDING;
    m_bUsesCollision = true;
}

void* CBuilding::operator new(size_t size)
{
    return GetBuildingPool()->New();
}

void CBuilding::operator delete(void* data)
{
    GetBuildingPool()->Delete(static_cast<CBuilding*>(data));
}

void CBuilding::ReplaceWithNewModel(int32 newModelIndex)
{
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x00280198 + 1 : 0x33ABCC), this, newModelIndex);
}

bool IsBuildingPointerValid(CBuilding* building)
{
    if (!building)
        return false;

    return GetBuildingPool()->IsObjectValid(building);
}
