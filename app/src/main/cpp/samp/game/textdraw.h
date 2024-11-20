#pragma once

#include "Rect.h"

enum eTextDrawStyle : unsigned char {
	TEXTDRAW_FONT_GOTHIC,
	TEXTDRAW_FONT_SUBTITLES,
	TEXTDRAW_FONT_MENU,
	TEXTDRAW_FONT_PRICEDOWN,
	TEXTDRAW_TXD_SPRITE,
	TEXTDRAW_MODEL_PREVIEW
};

typedef struct _TEXT_DRAW_DATA
{
	float fLetterWidth;
	float fLetterHeight;
	uint32_t dwLetterColor;
	uint8_t byteUnk12;
	uint8_t	byteCentered;
	uint8_t byteBox;
	float fLineWidth;
	float fLineHeight;
	uint32_t dwBoxColor;
	uint8_t byteProportional;
	uint32_t dwBackgroundColor;
	uint8_t byteShadow;
	uint8_t byteOutline;
	uint8_t byteAlignLeft;
	uint8_t byteAlignRight;
	uint32_t dwStyle;
	float fX;
	float fY;
	char cGXT[8];
	uint32_t dwParam1;
	uint32_t dwParam2;
	int iTextureSlot;
	uint8_t byteSelectable;
	uint16_t wModelID;
	CVector vecRot;
	float fZoom;
	uint16_t wColor1;
	uint16_t wColor2;
	bool bHasRectArea;
	bool bHasKeyCode;
} TEXT_DRAW_DATA;

typedef struct _TEXT_DRAW_TRANSMIT
{
	union
	{
		uint8_t byteFlags;
		struct {
			uint8_t byteBox : 1;
			uint8_t byteLeft : 1;
			uint8_t byteRight : 1;
			uint8_t byteCenter : 1;
			uint8_t byteProportional : 1;
			uint8_t bytePadding : 3;
		};
	};
	float fLetterWidth;
	float fLetterHeight;
	uint32_t dwLetterColor;
	float fLineWidth;
	float fLineHeight;
	uint32_t dwBoxColor;
	uint8_t byteShadow;
	uint8_t byteOutline;
	uint32_t dwBackgroundColor;
	uint8_t byteStyle;
	uint8_t byteSelectable;
	float fX;
	float fY;
	uint16_t wModelID;
	CVector vecRot;
	float fZoom;
	uint16_t wColor1;
	uint16_t wColor2;
} TEXT_DRAW_TRANSMIT;

class CTextDraw
{
public:
	CTextDraw(TEXT_DRAW_TRANSMIT *pTextDrawTransmit, const char* szText);
	~CTextDraw();

	void Draw();
	void DrawImage();
	void DrawDefault();
	void DrawTextured();
	void DrawTexture7();
	void DrawTexture6();

	void SetText(const char* szText);
	void LoadTexture();

	void SnapshotProcess();

private:
	char m_szText[801];
	uint16_t m_szString[801];

	RwTexture* m_police;
	RwTexture* m_taxi;
	RwTexture* m_doctor;
	RwTexture* m_human;
	RwTexture* m_food;
	RwTexture* m_water;
	RwTexture* m_redmoney;
	RwTexture* m_greenmoney;
	RwTexture* m_redmoney2;
	RwTexture* m_greenmoney2;
	RwTexture* m_mec;
	RwTexture* m_inv;

private:
	//Label* m_label;

public:
	TEXT_DRAW_DATA m_TextDrawData;
	CRect m_rectArea;
	bool m_bHovered;
	uint32_t m_dwHoverColor;
};