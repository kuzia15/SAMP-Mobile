#pragma once

#include "EntryInfoNode.h"

class CEntryInfoList {
public:
    CEntryInfoNode* m_pNode;

public:
    static void InjectHooks();

    void Flush(); //0x536E10
    void DeleteNode(CEntryInfoNode* pNode); //Most likely inlined in final exe
};
VALIDATE_SIZE(CEntryInfoList, (VER_x32 ? 0x4 : 0x8));
