#pragma once
#include "../../game/game.h"
#include "../../game/util.h"

#include "../../main.h"

#include "../../settings.h"
#include "util/CUtil.h"


extern CSettings* pSettings;

class VoiceButton : public Button
{
public:
	VoiceButton() : Button("TALK", UISettings::fontSize() / 2) {
		m_recording = false;
		/* 5:3 aspect ratio */
		//m_texture_micro_on = (RwTexture*)CUtil::LoadTextureFromDB("samp", "icon_micro_on");
		//m_texture_micro_off = (RwTexture*)CUtil::LoadTextureFromDB("samp", "icon_micro_off");
		m_texture_micro_on = (RwTexture*)CUtil::LoadTextureFromDB("samp", "voiceactive"); //default
		//if (Server == 40) m_texture_micro_on = (RwTexture*)CUtil::LoadTextureFromDB("samp", "voiceactive");
		//if (Server == 40) m_texture_micro_off = (RwTexture*)CUtil::LoadTextureFromDB("samp", "voicepassive");
		m_texture_micro_off = (RwTexture*)CUtil::LoadTextureFromDB("samp", "voicepassive"); //default
		//if (Server == 13) m_texture_micro_off = (RwTexture*)CUtil::LoadTextureFromDB("samp", "icon_micro_on");
		//if (Server == 13) m_texture_micro_on = (RwTexture*)CUtil::LoadTextureFromDB("samp", "icon_micro_off");
	}

	virtual void draw(ImGuiRenderer* renderer) override
	{
		if(!pSettings->Get().bVoiceChatEnable) return;

		if (countdown > 0 && recording() == 1) countdown--;
		if (countdown == 0 && recording() == 1) setRecording(0);
		renderer->drawImage(absolutePosition(), absolutePosition() + size(),
			recording() ? m_texture_micro_on->raster : m_texture_micro_off->raster);

		//MyLog2("%f,%f",absolutePosition(), absolutePosition() + size());
		//MyLog2("countdown %d", countdown);
		//MyLog2("recording %d", recording());
		//MyLog2("press %d", press);
		
	}

	void touchPopEvent() override
	{
		countdown = 500;
		setRecording(0);
	}

	void touchPushEvent() override
	{
			//setRecording(recording() ^ 1);
		setRecording(1);
		countdown = 500;
	}

	void setRecording(bool recording) 
	{ 
		if (recording == 1) countdown = 200;
		m_recording = recording;
		this->setCaptionColor(m_recording ? ImColor(1.0f, 0.0f, 0.0f) : ImColor(1.0f, 1.0f, 1.0f));
	}

	bool recording() const { return m_recording; }

private:
	bool m_recording;
	RwTexture* m_texture_micro_on;
	RwTexture* m_texture_micro_off;
	//int countdown = 1000;
public:
	int countdown = 1000;
};