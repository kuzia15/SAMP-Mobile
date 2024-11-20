#pragma once

#include "../gui/gui.h"

#pragma pack(push, 1)
typedef struct _TEXT_LABEL
{
	std::string text;
	uint32_t dwColor;
	CVector vecPos;
	float fDistance;
	uint8_t bTestLOS;
	PLAYERID playerId;
	VEHICLEID vehicleId;
} TEXT_LABEL;
#pragma pack(pop)

class C3DTextLabelPool
{
public:
	C3DTextLabelPool();
	~C3DTextLabelPool();

	bool GetSlotState(uint16_t wLabelId) {
		if (wLabelId < MAX_TEXT_LABELS && m_bSlotUsed[wLabelId]) {
			return true;
		}

		return false;
	}

	void NewLabel(uint16_t wLabelId, TEXT_LABEL* label);
	void ClearLabel(uint16_t wLabelId);

	void Render(ImGuiRenderer* renderer);

private:
	TEXT_LABEL	*m_TextLabels[MAX_TEXT_LABELS];
	bool		m_bSlotUsed[MAX_TEXT_LABELS];

    void Draw(ImGuiRenderer *renderer, TEXT_LABEL *label, CVector vecPos, const std::string &text,
              uint32_t dwColor);
};