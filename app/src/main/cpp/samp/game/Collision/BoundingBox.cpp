
#include "BoundingBox.h"
#include "../vendor/armhook/patch.h"

// 0x49C300
bool CBoundingBox::IsPointWithin(const CVector& point) const {
    return point.x >= m_vecMin.x
           && point.y >= m_vecMin.y
           && point.z >= m_vecMin.z
           && point.x <= m_vecMax.x
           && point.y <= m_vecMax.y
           && point.z <= m_vecMax.z;
}

// 0x470100
void CBoundingBox::SetMinMax(CVector min, CVector max) {
    m_vecMin = min;
    m_vecMax = max;
}

void CBoundingBox::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678A9C : 0x84F568), &numBBFailed);
}
