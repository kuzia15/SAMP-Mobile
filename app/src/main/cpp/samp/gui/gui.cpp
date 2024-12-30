#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "gui.h"
#include "../playertags.h"
#include "../net/playerbubblepool.h"

// voice
#include "../voice_new/Plugin.h"
#include "../voice_new/MicroIcon.h"
#include "../voice_new/SpeakerList.h"
#include "../voice_new/Network.h"

#include "../gui/samp_widgets/voicebutton.h"
#include "game/Textures/TextureDatabaseRuntime.h"
#include "game/Streaming.h"
#include "game/Pools.h"

extern CNetGame* pNetGame;
extern CPlayerTags* pPlayerTags;
extern UI* pUI;

UI::UI(const ImVec2& display_size, const std::string& font_path)
	: Widget(), ImGuiWrapper(display_size, font_path)
{
	UISettings::Initialize(display_size);
	this->setFixedSize(display_size);
}

bool UI::initialize()
{
	if (!ImGuiWrapper::initialize()) return false;

	m_splashScreen = new SplashScreen();
	this->addChild(m_splashScreen);
	m_splashScreen->setFixedSize(size());
	m_splashScreen->setPosition(ImVec2(0.0f, 0.0f));
	m_splashScreen->setVisible(true);

	m_chat = new Chat();
	this->addChild(m_chat);
	m_chat->setFixedSize(UISettings::chatSize());
	m_chat->setPosition(UISettings::chatPos());
	m_chat->setItemSize(UISettings::chatItemSize());
	m_chat->setVisible(false);

	m_buttonPanel = new ButtonPanel();
	this->addChild(m_buttonPanel);
	m_buttonPanel->setFixedSize(UISettings::buttonPanelSize());
	m_buttonPanel->setPosition(UISettings::buttonPanelPos());
	m_buttonPanel->setVisible(false);

	m_voiceButton = new VoiceButton();
	this->addChild(m_voiceButton);
	m_voiceButton->setFixedSize(UISettings::buttonVoiceSize());
	m_voiceButton->setPosition(UISettings::buttonVoicePos());
	m_voiceButton->setVisible(false);

	m_spawn = new Spawn();
	this->addChild(m_spawn);
	m_spawn->setFixedSize(UISettings::spawnSize());
	m_spawn->setPosition(UISettings::spawnPos());
	m_spawn->setVisible(false);

	m_dialog = new Dialog();
	this->addChild(m_dialog);
	m_dialog->setVisible(false);
	m_dialog->setMinSize(UISettings::dialogMinSize());
	m_dialog->setMaxSize(UISettings::dialogMaxSize());

	m_keyboard = new Keyboard();
	this->addChild(m_keyboard);
	m_keyboard->setFixedSize(UISettings::keyboardSize());
	m_keyboard->setPosition(UISettings::keyboardPos());
	m_keyboard->setVisible(false);

	m_playerTabList = new PlayerTabList();
	this->addChild(m_playerTabList);
	m_playerTabList->setMinSize(UISettings::dialogMinSize());
	m_playerTabList->setMaxSize(UISettings::dialogMaxSize());
	m_playerTabList->setVisible(false);

    label = new Label(" ", ImColor(1.0f, 1.0f, 1.0f), true, UISettings::fontSize() / 2);
    pUI->addChild(label);

    label2 = new Label(" ", ImColor(1.0f, 1.0f, 1.0f), true, UISettings::fontSize() / 2);
    pUI->addChild(label2);

    label3 = new Label(" ", ImColor(1.0f, 1.0f, 1.0f), true, UISettings::fontSize() / 2);
    pUI->addChild(label3);

    label4 = new Label(" ", ImColor(1.0f, 1.0f, 1.0f), true, UISettings::fontSize() / 2);
    pUI->addChild(label4);

	// mem
	Label* d_label1;

	d_label1 = new Label("4.7.1", ImColor(1.0f, 1.0f, 1.0f), true, UISettings::fontSize() / 2);
	this->addChild(d_label1);
	d_label1->setPosition(ImVec2(3.0, 3.0));

    // ==== version ==== //
    //d_label = new Label("", ImColor(1.0f, 1.0f, 1.0f), true, UISettings::fontSize() / 2);
    //this->addChild(d_label);
   // d_label->setPosition(ImVec2(3.0, 55.0));

	return true;
}

void UI::render()
{
	ImGuiWrapper::render();

    renderDebug();

    ProcessPushedTextdraws();

	if (m_bNeedClearMousePos) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(-1, -1);
		m_bNeedClearMousePos = false;
	}
}

void UI::shutdown()
{
	ImGuiWrapper::shutdown();
}

void UI::drawList()
{
	if (!visible()) return;

	/*Label* label;
	label = new Label("1.0.11", ImColor(1.0f, 1.0f, 1.0f), true, UISettings::fontSize() / 2);
	label->setPosition(ImVec2(0.0, 0.0));
	this->addChild(label);*/

	if (pPlayerTags) pPlayerTags->Render(renderer());
	if (pNetGame && pNetGame->GetTextLabelPool()) pNetGame->GetTextLabelPool()->Render(renderer());
	if (pNetGame && pNetGame->GetPlayerBubblePool()) pNetGame->GetPlayerBubblePool()->Render(renderer());

	draw(renderer());
}

void UI::touchEvent(const ImVec2& pos, TouchType type)
{
	/* 
		� ������� ����������
		1 - ����������
		2 - ������
		3 - ���
	*/

	if (m_keyboard->visible() && m_keyboard->contains(pos))
	{
		m_keyboard->touchEvent(pos, type);
		return;
	}

	if (m_dialog->visible() && m_dialog->contains(pos))
	{
		m_dialog->touchEvent(pos, type);
		return;
	}

	Widget::touchEvent(pos, type);
}

enum eTouchType
{
	TOUCH_POP = 1,
	TOUCH_PUSH = 2,
	TOUCH_MOVE = 3
};

bool UI::OnTouchEvent(int type, bool multi, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();

	/*
	switch (type)
	{
	case 1://TOUCH_PUSH:
		io.MousePos = ImVec2(x, y);
		io.MouseDown[0] = true;
		MyLog2("TOUCH_PUSH");
		break;

	case 2://TOUCH_POP:
		io.MouseDown[0] = false;
		m_bNeedClearMousePos = true;
		MyLog2("TOUCH_POP");
		break;

	case 3://TOUCH_MOVE:
		io.MousePos = ImVec2(x, y);
		MyLog2("TOUCH_MOVE");
		break;
	}*/
	VoiceButton* vbutton = pUI->voicebutton();
	switch (type)
	{
	case TOUCH_PUSH:
		io.MousePos = ImVec2(x, y);
		io.MouseDown[0] = true;
		break;

	case TOUCH_POP:
		io.MouseDown[0] = false;
		m_bNeedClearMousePos = true;
		break;

	case TOUCH_MOVE:
		io.MousePos = ImVec2(x, y);
		//if (vbutton->countdown > 50) vbutton->countdown = 20;
		break;
	}

	return true;
}

extern CGame *pGame;
void UI::renderDebug()
{
    char szStr[30];
    char szStrMem[64];
    char szStrPos[64];

    ImVec2 pos = ImVec2(pUI->ScaleX(40.0f), pUI->ScaleY(1080.0f - UISettings::fontSize() * 10));

    static float fps = 60.f;
        static auto lastTick = CTimer::m_snTimeInMillisecondsNonClipped;
        if(CTimer::m_snTimeInMillisecondsNonClipped - lastTick > 500) {
            lastTick = CTimer::m_snTimeInMillisecondsNonClipped;
            fps = std::clamp(CTimer::game_FPS, 10.f, (float) 120);
        }
        snprintf(&szStr[0], sizeof(szStr), "FPS: %.0f", fps);

        label->setText(&szStr[0]);
        label->setPosition(pos);

        auto &msUsed = CStreaming::ms_memoryUsed;
        auto &msAvailable = CStreaming::ms_memoryAvailable;

        struct mallinfo memInfo = mallinfo();
        int totalAllocatedMB  = memInfo.uordblks / (1024 * 1024);

        snprintf(&szStrMem[0], sizeof(szStrMem), "MEM: %d mb (stream %d/%d) (Tex %d MB)",
                 totalAllocatedMB,
                 msUsed / (1024 * 1024),
                 msAvailable / (1024 * 1024),
                 TextureDatabaseRuntime::storedTexels / (1024 * 1024)
        );

        pos = ImVec2(pUI->ScaleX(40.0f), pUI->ScaleY(1080.0f - UISettings::fontSize() * 9));

        label2->setText(&szStrMem[0]);
        label2->setPosition(pos);

        if (pGame->FindPlayerPed()->m_pPed)
        {
            snprintf(&szStrPos[0], sizeof(szStrPos), "POS: %.2f, %.2f, %.2f", pGame->FindPlayerPed()->m_pPed->m_matrix->m_pos.x, pGame->FindPlayerPed()->m_pPed->m_matrix->m_pos.y, pGame->FindPlayerPed()->m_pPed->m_matrix->m_pos.z);
            pos = ImVec2(pUI->ScaleX(40.0f), pUI->ScaleY(1080.0f - UISettings::fontSize() * 8));
            label3->setText(&szStrPos[0]);
            label3->setPosition(pos);
        }
        //Log("pools = %d mem = %d", GetPedPoolGta()->GetNoOfUsedSpaces(), totalAllocatedMB);
        char debugPools[250];
        snprintf(&debugPools[0], sizeof(debugPools), "NSingle: %d; NDouble: %d; Peds: %d; Veh's: %d; Obj: %d; EntryInf: %d; Dummies: %d, Buildings: %d",
                 GetPtrNodeSingleLinkPool()->GetNoOfUsedSpaces(),
                 GetPtrNodeDoubleLinkPool()->GetNoOfUsedSpaces(),
                 GetPedPoolGta()->GetNoOfUsedSpaces(),
                 GetVehiclePoolGta()->GetNoOfUsedSpaces(),
                 GetObjectPoolGta()->GetNoOfUsedSpaces(),
                 GetEntryInfoNodePool()->GetNoOfUsedSpaces(),
                 GetDummyPool()->GetNoOfUsedSpaces(),
                 GetBuildingPool()->GetNoOfUsedSpaces()
                 );

        pos = ImVec2(pUI->ScaleX(40.0f), pUI->ScaleY(1080.0f - UISettings::fontSize() * 1));

        label4->setText(&debugPools[0]);
        label4->setPosition(pos);
}

void UI::PushToBufferedQueueTextDrawPressed(uint16_t textdrawId)
{
    BUFFERED_COMMAND_TEXTDRAW* pCmd = m_BufferedCommandTextdraws.WriteLock();

    pCmd->textdrawId = textdrawId;

    m_BufferedCommandTextdraws.WriteUnlock();
}

void UI::ProcessPushedTextdraws()
{
    BUFFERED_COMMAND_TEXTDRAW* pCmd = nullptr;
    while (pCmd = m_BufferedCommandTextdraws.ReadLock())
    {
        RakNet::BitStream bs;
        bs.Write(pCmd->textdrawId);
        pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
        m_BufferedCommandTextdraws.ReadUnlock();
    }
}