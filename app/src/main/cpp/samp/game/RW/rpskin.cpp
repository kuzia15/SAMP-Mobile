//
// Created by x1y2z on 20.04.2023.
//

#include "RenderWare.h"
#include "../vendor/armhook/patch.h"

RpSkin* RpSkinGeometryGetSkin(RpGeometry* geometry) {
    return CHook::CallFunction<RpSkin*>(g_libGTASA + (VER_x32 ? 0x001C98FC + 1 : 0x25DEA0), geometry);
}

RpHAnimHierarchy* RpSkinAtomicGetHAnimHierarchy(const RpAtomic* atomic) {
    return CHook::CallFunction<RpHAnimHierarchy*>(g_libGTASA + (VER_x32 ? 0x001C98EC + 1 : 0x25DE8C), atomic);
}
