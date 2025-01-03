//
// Created by x1y2z on 25.05.2023.
//

#include "../game/common.h"

#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>

#include "RQ_Commands.h"
#include "../vendor/armhook/patch.h"

bool RQCaps[16];

//void RQ_Command_rqInit(uint8 **qData)
//{
//    const uint8 *v1; // r0
//    const uint8 *v2; // r4
//    const char *v3; // r5
//    bool v4; // r0
//    int v5; // r5
//    unsigned int v6; // r0
//    unsigned int v7; // r1
//    GLint params[3]; // [sp+4h] [bp-Ch] BYREF
//
//    glGetError();
//    glFrontFace(0x901u);
//    v1 = glGetString(0x1F00u);
//    rqVendor = j_strdup(v1);
//    v2 = glGetString(0x1F01u);
//    rqRenderer = j_strdup(v2);
//    v3 = (const char *)glGetString(0x1F03u);
//    if ( !strcmp((const char *)v2, "Adreno (TM) 320") )
//    RQCaps[0xC] = 1;
//    if ( strstr(v3, "GL_OES_depth24") )
//    RQCaps[0] = 1;
//    if ( strstr(v3, "GL_OES_packed_depth_stencil") )
//    RQCaps[1] = 1;
//    if ( strstr(v3, "GL_NV_depth_nonlinear") )
//    RQCaps[2] = 1;
//    if ( strstr(v3, "GL_EXT_texture_compression_dxt1") || strstr(v3, "GL_EXT_texture_compression_s3tc") )
//    RQCaps[3] = 1;
//    if ( strstr(v3, "GL_AMD_compressed_ATC_texture") )
//    {
//    RQCaps[0xC] = 1;
//    RQCaps[4] = 1;
//    }
//    if ( strstr(v3, "GL_IMG_texture_compression_pvrtc") )
//    {
//    v4 = 1;
//    RQCaps[5] = 1;
//    }
//    else
//    {
//    v4 = RQCaps[5];
//    }
//    if ( !((v4 || RQCaps[4] || RQCaps[3]) << 0x18) )
//    RQCaps[0xD] = 1;
//    if ( strstr(v3, "GL_OES_rgb8_rgba8") )
//    RQCaps[6] = 1;
//    if ( strstr(v3, "GL_EXT_texture_filter_anisotropic") )
//    RQCaps[7] = 1;
//    if ( strstr(v3, "GL_QCOM_alpha_test") )
//    RQCaps[0xB] = 1;
//    if ( strstr(v3, "GL_QCOM_binning_control") )
//    {
//    glHint(0x8FB0u, 0x8FB1u);
//    RQCaps[0xA] = 1;
//    }
//    if ( (unsigned int)OS_SystemChip() <= OSDC_A4 )
//    {
//    RQCaps[0xE] = 1;
//    RQCaps[8] = 1;
//    }
//    if ( RQCaps[4] && strstr((const char *)v2, (const char *)&dword_1CC4B4) )
//    RQCaps[0xE] = 1;
//    if ( RQCaps[5] && strstr((const char *)v2, (const char *)&dword_1CC4C0) )
//    RQCaps[0xE] = 1;
//    v5 = 0x80;
//    params[0] = 0x80;
//    RQCaps[9] = 0;
//    glGetIntegerv(0x8DFBu, params);
//    if ( (params[0] - 0x20) / 3 < 0x80 )
//    v5 = (params[0] - 0x20) / 3;
//    RQMaxBones = v5;
//    if ( !RQCaps[0] || (RQCaps[0xD] || RQCaps[5]) << 0x18 )
//    {
//    v6 = 0xC1200000;
//    v7 = 0xBF800000;
//    }
//    else
//    {
//    v6 = 0xBE200000;
//    v7 = 0xBC800000;
//    }
//    glPolygonOffset(*(GLfloat *)&v6, *(GLfloat *)&v7);
//    ES2InitTextures();
//}

void CRQ_Commands::rqVertexBufferSelect(uintptr_t **qData)
{
    if (!qData || !*qData)
    {
        glBindBuffer(0x8892, 0);
        return;
    }

    unsigned int buffer = *(*qData)++;

    if (!buffer)
    {
        glBindBuffer(0x8892, 0);
        return;
    }

    glBindBuffer(0x8892, buffer + 8);

    ES2VertexBuffer::curCPUBuffer = nullptr;
}

void CRQ_Commands::InjectHooks() {
   // bool (&RQCaps)[16] = *(bool(*)[16])0xA9B0C8;


    CHook::Redirect("_Z31RQ_Command_rqVertexBufferSelectRPc", &CRQ_Commands::rqVertexBufferSelect);
}
