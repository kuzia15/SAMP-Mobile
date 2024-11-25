#pragma once

#include "common.h"
#include "rgba.h"
#include "Core/Rect.h"

class CSprite2d
{
public:
	RwTexture* m_pTexture;

//	static inline int32 nextBufferIndex = *(int32*)0xC80458;
//	static inline int32 nextBufferVertex = *(int32*)0xC8045C;
	static inline float NearScreenZ;
	static inline float RecipNearClip;
	//static inline RwIm2DVertex (&maVertices)[8]
	static inline RwIm2DVertex maVertices[8];

public:
	static void InjectHooks();

	~CSprite2d();

	void Draw(float x, float y, float width, float height, CRGBA* color);
	void Draw(const CRect& posn, const CRGBA& color);

	static void SetRecipNearClip();
	static void InitPerFrame();
	static bool IsVertexBufferEmpty();
	static bool IsVertexBufferFull();
	static void RenderVertexBuffer();

	void SetTexture(const char *name);

	static RwTexture *LoadPngTex();

	static RwTexture *LoadPngTex(const char *name);

    void SetRenderState();

	void Draw(const CRect &posn, const CRGBA &color1, const CRGBA &color2, const CRGBA &color3, const CRGBA &color4);

    void SetVertices(const CRect &posn, const CRGBA &color1, const CRGBA &color2, const CRGBA &color3, const CRGBA &color4, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4);

	void SetVertices(int32 numVerts, const CVector2D *posn, const CVector2D *texCoors, const CRGBA &color);

	void SetVertices(int32 numVerts, const CVector2D *posn, const CRGBA &color);

	void SetVertices(RwIm2DVertex *vertices, const CRect &posn, const CRGBA &color1, const CRGBA &color2, const CRGBA &color3, const CRGBA &color4, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4);

	void SetVertices(const CRect &posn, const CRGBA &color1, const CRGBA &color2, const CRGBA &color3, const CRGBA &color4);
};