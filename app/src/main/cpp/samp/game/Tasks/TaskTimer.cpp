//
// Created by x1y2z on 19.04.2023.
//

#include "TaskTimer.h"

// 0x420E30
bool CTaskTimer::IsOutOfTime() {
    if (!m_bStarted)
        return false;

    if (m_bStopped) {
        m_nStartTime = CTimer::GetTimeInMS();
        m_bStopped = false;
    }

    return CTimer::GetTimeInMS() >= (m_nStartTime + m_nInterval);
}