//
// Created by x1y2z on 19.04.2023.
//

#include "MatrixLink.h"
#include "../vendor/armhook/patch.h"

void CMatrixLink::Insert(CMatrixLink* pWhere)
{
    pWhere->m_pNext = m_pNext;
    m_pNext->m_pPrev = pWhere;

    pWhere->m_pPrev = this;
    m_pNext = pWhere;
}

void CMatrixLink::Remove()
{
    m_pNext->m_pPrev = m_pPrev;
    m_pPrev->m_pNext = m_pNext;
}

void CMatrixLink::InjectHooks() {
//    CHook::MethodHook(g_libGTASA, 0x003AB2C0, &CMatrixLink::Remove);
//    CHook::MethodHook(g_libGTASA, 0x003AB2A8, &CMatrixLink::Insert);
}
