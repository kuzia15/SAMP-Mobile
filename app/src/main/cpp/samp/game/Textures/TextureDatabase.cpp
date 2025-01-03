//
// Created by x1y2z on 20.11.2023.
//

#include "TextureDatabase.h"
#include "../vendor/armhook/patch.h"

void TextureDatabase::GetPNGFilename(char *pngBuffer, const char *entryName) {
    snprintf(pngBuffer, 0x100u, "texdb/%s/src/%s.png", name, entryName);
}

bool TextureDatabase::LoadThumbs(TextureDatabaseFormat forFormat, bool setEntries) {
//    char buff[256];
//
//    switch ( forFormat )
//    {
//        case DF_UNC:
//            snprintf(buff, sizeof(buff), "texdb/%s/%s.unc.tmb", name, name);
//            break;
//        case DF_DXT:
//            snprintf(buff, sizeof(buff), "texdb/%s/%s.dxt.tmb", name, name);
//            break;
//        case DF_360:
//            snprintf(buff, sizeof(buff), "texdb/%s/%s.360.tmb", name, name);
//            break;
//        case DF_PS3:
//            snprintf(buff, sizeof(buff), "texdb/%s/%s.ps3.tmb", name, name);
//            break;
//        case DF_PVR:
//            snprintf(buff, sizeof(buff), "texdb/%s/%s.pvr.tmb", name, name);
//            break;
//        case DF_ETC:
//            snprintf(buff, sizeof(buff), "texdb/%s/%s.etc.tmb", name, name);
//            break;
//        default:
//            return LoadDataFile(buff, &thumbs[forFormat]);
//    }
//    return LoadDataFile(buff, &thumbs[forFormat]);
}

void TextureDatabase::InjectHooks() {
    //CHook::Redirect("_ZN15TextureDatabase10LoadThumbsE21TextureDatabaseFormatb", &TextureDatabase__LoadThumbs);
   // CHook::Redirect("_ZN15TextureDatabase9LoadEntryEPcb", &TextureDatabase__LoadEntry);
}

void TextureDatabase::LoadEntry(char *line, bool hashName) {

}
