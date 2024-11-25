#include "../main.h"
#include "game.h"

#include "sprite2d.h"
#include "../vendor/armhook/patch.h"
#include "Scene.h"

void CSprite2d::Draw(float x, float y, float width, float height, CRGBA* color)
{
	CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005C8F20 + 1 : 0x6ED440), this, x, y, width, height, color);
}

void CSprite2d::Draw(const CRect& posn, const CRGBA& color)
{
	Draw(posn, color, color, color, color);
}

void CSprite2d::Draw(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4)
{
	SetVertices(posn, color1, color2, color3, color4);
	SetRenderState();
	RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
}

// different kinds of vertices' defining
void CSprite2d::SetVertices(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4)
{
	float offset = 1.0f / 1024.0f;
	SetVertices(posn, color1, color2, color3, color4,
				offset,       offset,
				1.f + offset,       offset,
				offset, 1.f + offset,
				1.f + offset, 1.f + offset);
}

void CSprite2d::SetVertices(int32 numVerts, const CVector2D* posn, const CVector2D* texCoors, const CRGBA& color)
{
	for (int32 i = 0; i < numVerts; ++i) {
		RwIm2DVertexSetScreenX(&maVertices[i], posn[i].x);
		RwIm2DVertexSetScreenY(&maVertices[i], posn[i].y);
		RwIm2DVertexSetScreenZ(&maVertices[i], NearScreenZ + 0.0001f);
		RwIm2DVertexSetRecipCameraZ(&maVertices[i], RecipNearClip);
		RwIm2DVertexSetU(&maVertices[i], texCoors[i].x, RecipNearClip);
		RwIm2DVertexSetV(&maVertices[i], texCoors[i].y, RecipNearClip);
		RwIm2DVertexSetIntRGBA(&maVertices[i], color.r, color.g, color.b, color.a);
	}
}

void CSprite2d::SetVertices(int32 numVerts, const CVector2D* posn, const CRGBA& color)
{
	for (int32 i = 0; i < numVerts; ++i) {
		RwIm2DVertexSetScreenX(&maVertices[i], posn[i].x);
		RwIm2DVertexSetScreenY(&maVertices[i], posn[i].y);
		RwIm2DVertexSetScreenZ(&maVertices[i], NearScreenZ);
		RwIm2DVertexSetRecipCameraZ(&maVertices[i], RecipNearClip);
		RwIm2DVertexSetU(&maVertices[i], 1.f, RecipNearClip);
		RwIm2DVertexSetV(&maVertices[i], 1.f, RecipNearClip);
		RwIm2DVertexSetIntRGBA(&maVertices[i], color.r, color.g, color.b, color.a);
	}
}

void CSprite2d::SetVertices(const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4,
							float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4)
{
	SetVertices(maVertices, posn, color1, color2, color3, color4, u1, v1, u2, v2, u3, v3, u4, v4);
}

void CSprite2d::SetVertices(RwIm2DVertex* vertices, const CRect& posn, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4,
							float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4)
{
	RwIm2DVertexSetScreenX(&vertices[0], posn.left);
	RwIm2DVertexSetScreenY(&vertices[0], posn.bottom);
	RwIm2DVertexSetScreenZ(&vertices[0], NearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vertices[0], RecipNearClip);
	RwIm2DVertexSetU(&vertices[0], u1, RecipNearClip);
	RwIm2DVertexSetV(&vertices[0], v1, RecipNearClip);
	RwIm2DVertexSetIntRGBA(&vertices[0], color3.r, color3.g, color3.b, color3.a);

	RwIm2DVertexSetScreenX(&vertices[1], posn.right);
	RwIm2DVertexSetScreenY(&vertices[1], posn.bottom);
	RwIm2DVertexSetScreenZ(&vertices[1], NearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vertices[1], RecipNearClip);
	RwIm2DVertexSetU(&vertices[1], u2, RecipNearClip);
	RwIm2DVertexSetV(&vertices[1], v2, RecipNearClip);
	RwIm2DVertexSetIntRGBA(&vertices[1], color4.r, color4.g, color4.b, color4.a);

	RwIm2DVertexSetScreenX(&vertices[2], posn.right);
	RwIm2DVertexSetScreenY(&vertices[2], posn.top);
	RwIm2DVertexSetScreenZ(&vertices[2], NearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vertices[2], RecipNearClip);
	RwIm2DVertexSetU(&vertices[2], u4, RecipNearClip);
	RwIm2DVertexSetV(&vertices[2], v4, RecipNearClip);
	RwIm2DVertexSetIntRGBA(&vertices[2], color2.r, color2.g, color2.b, color2.a);

	RwIm2DVertexSetScreenX(&vertices[3], posn.left);
	RwIm2DVertexSetScreenY(&vertices[3], posn.top);
	RwIm2DVertexSetScreenZ(&vertices[3], NearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vertices[3], RecipNearClip);
	RwIm2DVertexSetU(&vertices[3], u3, RecipNearClip);
	RwIm2DVertexSetV(&vertices[3], v3, RecipNearClip);
	RwIm2DVertexSetIntRGBA(&vertices[3], color1.r, color1.g, color1.b, color1.a);
}


// Sets sprite texture as current for device rendering
// 0x727B30
void CSprite2d::SetRenderState()
{
	if (m_pTexture)
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(m_pTexture)));
	else
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
}

CSprite2d::~CSprite2d()
{
    if(m_pTexture) {
        RwTextureDestroy(m_pTexture);
        m_pTexture = 0;
    }
}

RwTexture* CSprite2d::LoadPngTex(const char* name) {
	auto sprite = new CSprite2d();
	sprite->SetTexture(name);

	return sprite->m_pTexture;
}

// set texture by name from current txd
// 0x727270
void CSprite2d::SetTexture(const char* name)
{
	CHook::CallFunction<void>("_ZN9CSprite2d10SetTextureEPc", this, name);
}

void CSprite2d::InjectHooks() {
	CHook::Write(g_libGTASA + (VER_x32 ? 0x006766AC : 0x84ADC8), &CSprite2d::RecipNearClip);
	CHook::Write(g_libGTASA + (VER_x32 ? 0x00675F8C : 0x849FA0), &CSprite2d::NearScreenZ);
}

void CSprite2d::SetRecipNearClip() {

}