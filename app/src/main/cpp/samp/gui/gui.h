#pragma once

#define DEBUG_GUI 0

#include "../vendor/encoding/encoding.h"

#include "imguiwrapper.h"
#include "uisettings.h"

#include "widget.h"
#include "widgets/layout.h"
#include "widgets/label.h"
#include "widgets/button.h"
#include "widgets/image.h"
#include "widgets/progressbar.h"
#include "widgets/scrollpanel.h"
#include "widgets/listbox.h"
#include "widgets/editbox.h"

#include "samp_widgets/keyboard.h"
#include "samp_widgets/splashscreen.h"
#include "samp_widgets/chat.h"
#include "samp_widgets/spawn.h"
#include "samp_widgets/buttonpanel.h"
#include "samp_widgets/playertablist.h"
#include "samp_widgets/voicebutton.h"

#include "samp_widgets/dialogs/dialog.h"

#include "..//vendor/raknet/SingleProducerConsumer.h"
#include "..//CDebugInfo.h"

#pragma pack(push, 1)
struct BUFFERED_COMMAND_TEXTDRAW
{
    uint16_t textdrawId;
};
#pragma pack(pop)

class Chat;
class UI : public Widget, public ImGuiWrapper
{
public:
    UI(const ImVec2& display_size, const std::string& font_path);

    bool initialize() override;
    void render() override;
    void shutdown() override;

    SplashScreen* splashscreen() const { return m_splashScreen; }
    Chat* chat() const { return m_chat; }
    Spawn* spawn() const { return m_spawn; }
    Keyboard* keyboard() const { return m_keyboard; }
    Dialog* dialog() const { return m_dialog; }
    ButtonPanel* buttonpanel() const { return m_buttonPanel; }
    PlayerTabList* playertablist() const { return m_playerTabList; }
    VoiceButton* voicebutton() const { return m_voiceButton; }

    static const ImColor fixcolor(uint32_t color) {
        return ImColor(
                (int)((color & 0xFF000000) >> 24),
                (int)((color & 0x00FF0000) >> 16),
                (int)((color & 0x0000FF00) >> 8));
    }

    float ScaleX(float x) {
        return x * ImGui::GetIO().DisplaySize.x * (1.0f / 1920.0f);
    }

    float ScaleY(float y) {
        return y * ImGui::GetIO().DisplaySize.y * (1.0f / 1080.0f);
    }

    float GetFontSize() {
        return UISettings::fontSize();
    }

    virtual void touchEvent(const ImVec2& pos, TouchType type) override;

    bool OnTouchEvent(int type, bool multi, int x, int y);

    void renderDebug();

    void ProcessPushedTextdraws();
    void PushToBufferedQueueTextDrawPressed(uint16_t textdrawId);

protected:
    void drawList() override;

private:
    Keyboard* m_keyboard;
    SplashScreen* m_splashScreen;
    Chat* m_chat;
    Spawn* m_spawn;
    Dialog* m_dialog;
    ButtonPanel* m_buttonPanel;
    PlayerTabList* m_playerTabList;
    VoiceButton* m_voiceButton;
    Label* label;
    Label* label2;
    Label* label3;
    Label* label4;

    float m_fFontSize;

    bool m_bNeedClearMousePos = false;

    DataStructures::SingleProducerConsumer<BUFFERED_COMMAND_TEXTDRAW> m_BufferedCommandTextdraws;

    CDebugInfo* m_debugInfo = nullptr;
};