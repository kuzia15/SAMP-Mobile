//
// Created by admin on 17.08.2023.
//

#pragma once
#include <jni.h>
#include "game/common.h"

#define INVALID_EDITED_SLOT -1

#pragma pack(push, 4)
class CObjectEditor {
public:
    enum eEditType {
        TYPE_NONE,
        TYPE_PLAYER_ATTACH,
        TYPE_OBJECT
    };

    enum eResponse : uint32_t {
        EDIT_RESPONSE_CANCEL,
        EDIT_RESPONSE_FINAL,
        EDIT_RESPONSE_UPDATE
    };

    static inline int       iEditedId   = INVALID_EDITED_SLOT;
    static inline eEditType editType    = TYPE_NONE;
    static inline bool      bIsToggle   = false;
    static inline bool      bPlayerObj  = false; // hueta by samp
    static inline uint32_t  time = 0;

public:
    static void startEditPlayerAttach(int slot);

    static void
    SendOnEditAttach(int response, int index, int modelid, int bone, CVector offset, CVector rot, CVector scale);

    static void showGui();

    static void startEditObject(uint16_t objectId);
};
#pragma pack(pop)
