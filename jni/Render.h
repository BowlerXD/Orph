#if defined(__aarch64__) //Compile for arm64 lib only
#include <Substrate/And64InlineHook/And64InlineHook.hpp>

#else //Compile for armv7 lib only. Do not worry about greyed out highlighting code, it still works
#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

#endif

void newHook(void *offset, void* ptr, void **orig){
#if defined(__aarch64__)
    A64HookFunction(offset, ptr, orig);
#else
    MSHookFunction(offset, ptr, orig);
#endif
}

namespace XYZ {
    void Hook(void *target, void *replace, void **backup);
}

void XYZ::Hook(void *target, void *replace, void **backup) {
    unsigned long page_size = sysconf(_SC_PAGESIZE);
    unsigned long size = page_size * sizeof(uintptr_t);
    void *p = (void *) ((uintptr_t) target - ((uintptr_t) target % page_size) - page_size);
    if (mprotect(p, (size_t) size, PROT_EXEC | PROT_READ | PROT_WRITE) == 0) {
		newHook(target, replace, backup);
    }
}


class Window
{
private:
    int ViewPort[4];

public:
    bool getViewPort() {
        glGetIntegerv(GL_VIEWPORT, this->ViewPort);
        if (this->ViewPort[2] && this->ViewPort[3]) return true;
        return false;
    }

    float getX() {
        return (float)this->ViewPort[0];
    }

    float getY() {
        return (float)this->ViewPort[1];
    }

    float getW() {
        return (float)this->ViewPort[2];
    }

    float getH() {
        return (float)this->ViewPort[3];
    }

    float getWidth() {
        return (this->getW() + (this->getX() * 2.0f));
    }

    float getHeight() {
        return (this->getH() + (this->getY() * 2.0f));
    }

    ImVec2 getSize() {
        return ImVec2(this->getWidth(), this->getHeight());
    }

    ImVec2 getCenter() {
        return ImVec2(this->getWidth() / 2.0f, this->getHeight() / 2.0f);
    }

    ImVec2 getScale() {
        return ImVec2(this->getWidth() * 0.00052083333f, this->getHeight() * 0.00092592592f);
    }

    bool isRotation() {
        if (this->getWidth() < this->getHeight()) return true;
        return false;
    }
};

class Display
{
public:
    static Window * m_pWindow()
    {
        static Window * instance = NULL;
        if (instance == NULL) instance = new Window;
        return instance;
    }
};
float screenDensity = 0.0f;

static inline bool IsSafeMatchRunning(void *battleBridgeInstance, void *battleManagerInstance) {
    static int stableActiveFrames = 0;
    if (!battleBridgeInstance || !battleManagerInstance) {
        stableActiveFrames = 0;
        return false;
    }

    // Start trigger (most reliable in runtime):
    // local player show object exists.
    auto localPlayerShow = *(uintptr_t *) ((uintptr_t) battleManagerInstance + BattleManager_m_LocalPlayerShow());
    bool hasLocalPlayer = (localPlayerShow != 0);
    if (!hasLocalPlayer) {
        stableActiveFrames = 0;
        return false;
    }

    int battleState = GetBattleState();

    // End trigger:
    // eBState_WaitEnd = 7, eBState_End = 8
    bool logicEnded = (battleState == 7 || battleState == 8);
    if (logicEnded) {
        stableActiveFrames = 0;
        return false;
    }

    // Require a few consecutive frames to avoid transient false-positive.
    if (stableActiveFrames < 6) stableActiveFrames++;
    return stableActiveFrames >= 3;
}

void Render()
{
    if (!Display::m_pWindow()->getViewPort()) return;
    glWidth = Display::m_pWindow()->getWidth();
    glHeight = Display::m_pWindow()->getHeight();

    if (!g_Initialized) {
        ImGui::CreateContext();
		
		ImGuiStyle& style = ImGui::GetStyle();
    	style.WindowPadding = ImVec2(17, 17);
    	style.WindowRounding = 7.0f;
    	style.WindowBorderSize = 0.0f;
    	style.WindowTitleAlign = ImVec2(0.5, 0.5);
    	style.FrameRounding = 4.0f;
    	style.ScrollbarRounding = 5.0f;
    	style.ChildRounding = 11.0f;
    	style.ScrollbarSize = 8.0f;
    	style.GrabMinSize = 9.0f;
    	style.GrabRounding = 3.0f;
    	style.ButtonTextAlign = ImVec2(0.5, 0.5);
        style.FramePadding = ImVec2(13, 7);
        style.ScaleAllSizes(1.5f);
        style.ScrollbarSize /= 1;
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.03f, 0.16f, 0.24f, 0.78f };
        style.Colors[ImGuiCol_Border] = ImVec4{ 0.00f, 0.78f, 1.00f, 0.48f };
        style.Colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };
        style.Colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
        style.Colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };
        style.Colors[ImGuiCol_WindowBg] = ImVec4{ 0.02f, 0.09f, 0.16f, 0.76f };
        style.Colors[ImGuiCol_Header] = ImColor(0, 128, 196, 155);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.00f, 0.70f, 1.00f, 0.88f };
        style.Colors[ImGuiCol_HeaderActive] = ImVec4{ 0.00f, 0.56f, 0.86f, 0.92f };
        style.Colors[ImGuiCol_Button] = ImColor(0, 114, 178, 150);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.00f, 0.74f, 1.00f, 0.88f };
        style.Colors[ImGuiCol_ButtonActive] = ImVec4{ 0.00f, 0.60f, 0.92f, 0.95f };
        style.Colors[ImGuiCol_CheckMark] = ImVec4{ 0.32f, 0.90f, 1.00f, 1.0f };
        style.Colors[ImGuiCol_PopupBg] = ImVec4{ 0.03f, 0.10f, 0.17f, 0.84f };
        style.Colors[ImGuiCol_SliderGrab] = ImVec4{ 0.00f, 0.72f, 1.00f, 0.68f };
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.32f, 0.90f, 1.00f, 0.88f };
        style.Colors[ImGuiCol_FrameBg] = ImColor(12, 45, 66, 142);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.00f, 0.52f, 0.80f, 0.72f };
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.00f, 0.63f, 0.94f, 0.78f };
        style.Colors[ImGuiCol_Tab] = ImVec4{ 0.02f, 0.17f, 0.26f, 0.75f };
        style.Colors[ImGuiCol_TabHovered] = ImVec4{ 0.00f, 0.70f, 1.00f, 0.82f };
        style.Colors[ImGuiCol_TabActive] = ImVec4{ 0.00f, 0.56f, 0.86f, 0.86f };
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.02f, 0.13f, 0.22f, 0.72f };
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.00f, 0.45f, 0.72f, 0.75f };
        style.Colors[ImGuiCol_TitleBg] = ImVec4{ 0.00f, 0.30f, 0.47f, 0.80f };
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.00f, 0.42f, 0.64f, 0.88f };
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.00f, 0.18f, 0.30f, 0.66f };
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.02f, 0.11f, 0.18f, 0.66f };
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.00f, 0.44f, 0.70f, 0.82f };
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.00f, 0.60f, 0.92f, 0.90f };
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.18f, 0.84f, 1.00f, 0.95f };
        style.Colors[ImGuiCol_Separator] = ImVec4{ 0.00f, 0.72f, 1.00f, 0.64f };
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.25f, 0.90f, 1.00f, 0.90f };
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.38f, 0.95f, 1.00f, 1.0f };
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.00f, 0.62f, 0.96f, 0.45f };
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.22f, 0.88f, 1.00f, 0.86f };
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.38f, 0.95f, 1.00f, 0.96f };
		
        ImGuiIO* io = &ImGui::GetIO();
        io->IniFilename = nullptr;
        
        io->DisplaySize = ImVec2((float)glWidth, (float)glHeight);
        screenDensity = Screen::get_dpi();
        style.ScaleAllSizes(std::max(1.0f, screenDensity / 400.0f));
        ImGui_ImplOpenGL3_Init("#version 300 es");

        ImFontConfig font_cfg;
        io->Fonts->AddFontFromMemoryTTF(consolas, sizeof(consolas), 19.5f, &font_cfg, io->Fonts->GetGlyphRangesChineseFull());
        setRes(glWidth, glHeight);
        
        g_Initialized = true;
    }

    ImGuiIO* io = &ImGui::GetIO();
    
    ImGui_ImplOpenGL3_NewFrame();
    ImplAndroid_NewFrame();
    ImGui::NewFrame();
	
    //IsVPNEnabled();
	
    if (screenWidth < glWidth && screenHeight < glHeight) setRes(glWidth, glHeight);
    
    // set the unity screen resolution to fix touch issue
    if (screenWidth != Screen::get_width() || screenHeight != Screen::get_height()) {
        Screen::SetResolution(screenWidth, screenHeight, true);
    }
    
    ImGui_GetTouch(io, screenHeight);
    
    auto hideShowMin = ImVec2(0, screenHeight - 80);
    auto hideShowMax = ImVec2(80, screenHeight);
    auto mousePos = io->MousePos;
    
    if (mousePos.x >= hideShowMin.x && mousePos.x <= hideShowMax.x && mousePos.y >= hideShowMin.y && mousePos.y <= hideShowMax.y) {
        showMenu = true;
    }

    //ShowWindow
	if (showMenu) DrawMenu();

    static bool loggedEspEntry = false;
    void *battleBridgeInstance = nullptr, *battleManagerInstance = nullptr;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "BattleData", "m_BattleBridge", &battleBridgeInstance);
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "BattleManager", "Instance", &battleManagerInstance);
    bFullChecked = IsSafeMatchRunning(battleBridgeInstance, battleManagerInstance);

    if (bFullChecked) {
        if (!loggedEspEntry) {
            loggedEspEntry = true;
        }
        NewDrawESP(ImGui::GetBackgroundDrawList(), screenWidth, screenHeight);
    } else {
        loggedEspEntry = false;
    }
	
	if (selectedFeatures == 2){
		//Tools::Hook((void *) ShowSelfPlayer_TryUseSkill2, (reinterpret_cast<void *>(TryUseSkill2)), (void **) &orig_TryUseSkill2);
    	//Tools::Hook((void *) ShowSelfPlayer_TryUseSkill, (reinterpret_cast<void *>(TryUseSkill)), (void **) &oTryUseSkill);
		//Tools::Hook((void *) ShowSelfPlayer_OnUpdate, (void *) iShowSelfPlayer_OnUpdate, (void **) &oShowSelfPlayer_OnUpdate);
		//Tools::Hook((void *) ShowUnitAIComp_TryUseSkill, (void *) New_TryUseSkill, (void **) &New_OrigTryUseSkill);
	}
	
    //YaserAntiCrack1();
	
    if (!AttachIconDone) {
        AttachIcon();
        AttachIconDone = true;
    }
    
    if (!AttachSpellDone) {
        AttachSpell();
        AttachSpellDone = true;
    }

    if (!AttachRankDone) {
        AttachRank();
        AttachRankDone = true;
    }
	
	if (!AttachMonsterDone) {
        AttachMonster();   
        AttachMonsterDone = true;
    }
     
    if (!AttachMonsterAlertDone) {
        AttachMonsterAlert();   
        AttachMonsterAlertDone = true;
    }
    
    if (!AttachCountryDone) {
        AttachCountry();   
        AttachCountryDone = true;
    }

    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    if (clearMousePos) {
        io->MousePos = ImVec2(-1, -1);
        clearMousePos = false;
    }
}
