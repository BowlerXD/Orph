bool showMenu = true;
int selectedFeatures = 1;
android_app *i_App = 0;
constexpr const char *kConfigPath = "/storage/emulated/0/Download/orph.ini";

struct PersistedMaphackAdjustments {
    ImVec2 startPos;
    int mapSize;
    int iconSize;
    int healthThin;
    float colorHealth[3];
};

void CenteredText(ImColor color, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const char *text, *text_end;
    ImFormatStringToTempBufferV(&text, &text_end, fmt, args);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text, text_end).x) * 0.5);
    ImGui::TextColoredV(color, fmt, args);
    va_end(args);
}

inline ImVec4 RGBA2ImVec4(int r, int g, int b, int a) {
    float newr = (float)r / 255.0f;
    float newg = (float)g / 255.0f;
    float newb = (float)b / 255.0f;
    float newa = (float)a / 255.0f;
    return ImVec4(newr, newg, newb, newa);
}

void HideMenu(bool& bShow) {
    if (bShow) {
        ImGui::OpenPopup("ConfirmHide");
    }

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("ConfirmHide", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Are you sure you want to hide the menu?");
        if (ImGui::Button("Yes", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0)))
        {
            showMenu = false;
            bShow = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            bShow = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void loadConfig() {
    int fd = open(kConfigPath, O_RDONLY);
    if (fd < 0) return;

    if (read(fd, &Config, sizeof(Config)) != (ssize_t)sizeof(Config)) {
        close(fd);
        return;
    }
    if (read(fd, &Aim, sizeof(Aim)) != (ssize_t)sizeof(Aim)) {
        close(fd);
        return;
    }
    if (read(fd, &ConfigState::GetCameraState().setFieldOfView, sizeof(ConfigState::GetCameraState().setFieldOfView)) != (ssize_t)sizeof(ConfigState::GetCameraState().setFieldOfView)) {
        close(fd);
        return;
    }
    if (read(fd, &ConfigState::GetCameraState().sliderValue, sizeof(ConfigState::GetCameraState().sliderValue)) != (ssize_t)sizeof(ConfigState::GetCameraState().sliderValue)) {
        close(fd);
        return;
    }

    PersistedMaphackAdjustments persisted{};

    ssize_t got = read(fd, &persisted, sizeof(persisted));
    if (got == (ssize_t) sizeof(persisted)) {
        StartPos = persisted.startPos;
        MapSize = persisted.mapSize;
        ICSize = persisted.iconSize;
        ICHealthThin = persisted.healthThin;
        ColorHealth[0] = persisted.colorHealth[0];
        ColorHealth[1] = persisted.colorHealth[1];
        ColorHealth[2] = persisted.colorHealth[2];
    }

    close(fd);
}
void saveConfig(){
    int fd = open(kConfigPath, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) return;

    write(fd, &Config , sizeof(Config));
    write(fd, &Aim, sizeof(Aim));
    write(fd, &ConfigState::GetCameraState().setFieldOfView, sizeof(ConfigState::GetCameraState().setFieldOfView));
    write(fd, &ConfigState::GetCameraState().sliderValue, sizeof(ConfigState::GetCameraState().sliderValue));

    PersistedMaphackAdjustments persisted{};

    persisted.startPos = StartPos;
    persisted.mapSize = MapSize;
    persisted.iconSize = ICSize;
    persisted.healthThin = ICHealthThin;
    persisted.colorHealth[0] = ColorHealth[0];
    persisted.colorHealth[1] = ColorHealth[1];
    persisted.colorHealth[2] = ColorHealth[2];

    write(fd, &persisted, sizeof(persisted));
    close(fd);
}

void loadCFG(){
    loadConfig();
}
void saveCFG(){
    saveConfig();
}

struct sTheme {
    bool TemaAsik;
    bool Color;
};
sTheme Theme{0};

std::string msg;

void LoginThread(const std::string &user_key, bool *success) {
    msg = Login(g_vm, user_key.c_str(), success);
}

inline ImColor main_color(230, 134, 224, 255);

inline ImColor text_color[3] = {ImColor(255, 255, 255, 255), ImColor(200, 200, 200, 255), ImColor(150, 150, 150, 255) };

inline ImColor foreground_color(1.f, 1.f, 1.f, 0.15f);

inline ImColor background_color(13, 14, 16, 200);

inline ImColor second_color(255, 255, 255, 20);

inline ImColor stroke_color(35, 35, 35, 255);

inline ImColor child_color(19, 19, 19, 255);

inline ImColor scroll_bg_col(24, 24, 24, 255);

inline ImColor winbg_color(15, 16, 18, 200);

inline ImFont* icon_font;

inline ImFont* logo_font;

inline ImFont* icon_big_font;

inline ImFont* medium_font;

inline ImFont* small_font;

inline ImFont* small_icon_font;

inline ImFont* arrow_icons;

inline float anim_speed = 8.f;

static inline ImDrawList* foremenu_drawlist;

inline ImColor GetColorWithAlpha(ImColor color, float alpha)
{
return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
}

inline ImVec2 center_text(ImVec2 min, ImVec2 max, const char* text)
{
    return min + (max - min) / 2 - ImGui::CalcTextSize(text) / 2;
}

inline int rotation_start_index;
inline void ImRotateStart()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

inline ImVec2 ImRotationCenter()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}

inline void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

void Trinage_background()
{

    ImVec2 screen_size = ImVec2(glWidth, glHeight);

    static ImVec2 partile_pos[100];
    static ImVec2 partile_target_pos[100];
    static float partile_speed[100];
    static float partile_size[100];
    static float partile_radius[100];
    static float partile_rotate[100];

    for (int i = 1; i < 50; i++)
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            partile_pos[i].x = rand() % (int)screen_size.x + 1;
            partile_pos[i].y = screen_size.y + 30;
            partile_speed[i] = 70 + rand() % 135;
            partile_radius[i] = 2 + rand() % 10;
            partile_size[i] = 15 + rand() % 40;

            partile_target_pos[i].x = rand() % (int)screen_size.x;
            partile_target_pos[i].y = -50;
        }

        partile_pos[i].y -= ImGui::GetIO().DeltaTime * partile_speed[i];
        partile_rotate[i] -= ImGui::GetIO().DeltaTime;

        if (partile_pos[i].y < 10)
        {
            partile_pos[i].x = 0;
            partile_pos[i].y = 0;
            partile_rotate[i] = 0;
        }

        ImRotateStart();
        ImGui::GetWindowDrawList()->AddRectFilled(partile_pos[i] - ImVec2(partile_size[i], partile_size[i]), partile_pos[i] + ImVec2(partile_size[i], partile_size[i]), GetColorWithAlpha(main_color, 0.3f), 5.f);
        ImRotateEnd(partile_rotate[i]);
    }
}
std::string xyzBuyKey = "https://t0pgamemurah.xyz/freeKey";

template <size_t N>
inline void SafeCopyToCharBuffer(char (&dst)[N], const std::string &src) {
    if (N == 0) return;
    strncpy(dst, src.c_str(), N - 1);
    dst[N - 1] = '\0';
}

inline bool ShouldShowGameplayTabs() {
    return selectedFeatures == 1 || selectedFeatures == 2;
}

struct RoomEnemyInfoRow {
    std::string name;
    std::string userId;
    std::string rank;
    std::string hero;
    std::string spell;
};

inline std::string RankLevelToString(int uiRankLevel) {
    if (uiRankLevel <= 0) return "-";
    if (uiRankLevel <= 11) return "Warrior";
    if (uiRankLevel < 27) return "Elite";
    if (uiRankLevel < 47) return "Master";
    if (uiRankLevel < 77) return "Grandmaster";
    if (uiRankLevel < 107) return "Epic";
    if (uiRankLevel < 137) return "Legend";
    if (uiRankLevel < 147) return "Mythic";
    if (uiRankLevel < 157) return "Mythical Honor";
    if (uiRankLevel < 177) return "Mythical Glory";
    return "Mythical Immortal";
}

inline bool RefreshEnemyRoomInfo(std::vector<RoomEnemyInfoRow> &outRows) {
    outRows.clear();

    auto *roomPlayers = GetBattlePlayerInfo();
    if (!roomPlayers || roomPlayers->getSize() <= 0) return false;

    static uintptr_t kCampOffset = 0;
    static uintptr_t kUidOffset = 0;
    static uintptr_t kZoneOffset = 0;
    static uintptr_t kNameOffset = 0;
    static uintptr_t kRankOffset = 0;
    static uintptr_t kHeroOffset = 0;
    static uintptr_t kSpellOffset = 0;
    if (!kCampOffset) kCampOffset = RoomData_iCamp();
    if (!kUidOffset) kUidOffset = RoomData_lUid();
    if (!kZoneOffset) kZoneOffset = RoomData_uiZoneId();
    if (!kNameOffset) kNameOffset = RoomData_sName();
    if (!kRankOffset) kRankOffset = RoomData_uiRankLevel();
    if (!kHeroOffset) kHeroOffset = RoomData_heroid();
    if (!kSpellOffset) kSpellOffset = RoomData_summonSkillId();
    if (!kCampOffset || !kUidOffset || !kZoneOffset || !kNameOffset || !kRankOffset || !kHeroOffset || !kSpellOffset) return false;

    void *selfUidBoxed = nullptr;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &selfUidBoxed);
    const int selfUid = (int) (uintptr_t) selfUidBoxed;

    int selfCamp = -1;
    for (int i = 0; i < roomPlayers->getSize(); i++) {
        auto roomData = roomPlayers->getItems()[i];
        if (!roomData) continue;
        const int uid = *(int *) ((uintptr_t) roomData + kUidOffset);
        if (uid != selfUid) continue;
        selfCamp = *(int *) ((uintptr_t) roomData + kCampOffset);
        break;
    }

    for (int i = 0; i < roomPlayers->getSize(); i++) {
        auto roomData = roomPlayers->getItems()[i];
        if (!roomData) continue;

        const int camp = *(int *) ((uintptr_t) roomData + kCampOffset);
        const int uid = *(int *) ((uintptr_t) roomData + kUidOffset);
        const int zoneId = *(int *) ((uintptr_t) roomData + kZoneOffset);
        const int rankLevel = *(int *) ((uintptr_t) roomData + kRankOffset);
        const int heroId = *(int *) ((uintptr_t) roomData + kHeroOffset);
        const int spellId = *(int *) ((uintptr_t) roomData + kSpellOffset);
        auto *nameStr = *(String **) ((uintptr_t) roomData + kNameOffset);

        if (selfCamp != -1 && camp == selfCamp) continue;
        if (selfUid != 0 && uid == selfUid) continue;

        RoomEnemyInfoRow row{};
        row.name = nameStr ? nameStr->toString() : "Unknown";
        row.userId = std::to_string(uid) + " (" + std::to_string(zoneId) + ")";
        row.rank = RankLevelToString(rankLevel) + " (" + std::to_string(rankLevel) + ")";
        row.hero = HeroToString(heroId);
        row.spell = SpellToString(spellId);
        outRows.emplace_back(row);
    }

    return true;
}

void DrawMenu() {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImVec2 center = main_viewport->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);

    ImGuiIO& io = ImGui::GetIO();

    static float window_scale;
    if (!window_scale) window_scale = 1.0f;
    io.FontGlobalScale = window_scale;

    static bool isLogin = false, isSave = false;
    static bool battleDataResolved = false;
    static char s[64] = {};
    if (isLogin && !isSave) {
        SharedPreferences sharedPref(GetJNIEnv(g_vm), "xyourzone_sharedpref");
        SharedPreferences_Editor editor=sharedPref.edit();
        editor.putString("key", s);
        editor.commit();
        isSave = true;
    }

    static bool isPopUpHide = false;
    if (isPopUpHide || ImGui::IsPopupOpen("ConfirmHide")) {
        HideMenu(isPopUpHide);
    }
    
    static bool bFlagAutoResize = true;
    static ImGuiWindowFlags window_flags;
    if (bFlagAutoResize) {
        window_flags = ImGuiWindowFlags_AlwaysAutoResize;
    } else {
        window_flags = ImGuiWindowFlags_None;
    }
    
    if (!isLogin) {
        battleDataResolved = false;
    } else if (!battleDataResolved) {
        loadBattleData(battleData);
        battleDataResolved = true;
    }
	
    static std::string cachedClientManager;
    static std::string cachedFullTitle;
    if (cachedFullTitle.empty() || cachedClientManager != clientManager) {
        cachedClientManager = clientManager;
        cachedFullTitle = "TMH | " + cachedClientManager + " | " + ABI;
    }
    ImGui::SetNextWindowSize(ImVec2((float) glWidth * 0.3f, (float) glHeight * 0.5f), ImGuiCond_Once); // 45% width 70% height
    if (!ImGui::Begin(cachedFullTitle.c_str(), 0, window_flags)) {
        ImGui::End();
        return;
    }
	
    using namespace ImGui;
	
	// revjump bypass !isLogin to isLogin visual hack
	if (!isLogin) {
        if (ImGui::BeginTabBar("TabLogin", ImGuiTabBarFlags_FittingPolicyScroll)) {
            if (ImGui::BeginTabItem("Login Menu")) {
                ImGui::BeginGroupPanel("Please Login! (Copy Key to Clipboard)", ImVec2(0.0f, 0.0f)); {
					
					ImGui::Spacing();
					ImGui::Spacing();
                    ImGui::PushItemWidth(-1);
                    ImGui::InputText("##key", s, sizeof s);
                    ImGui::PopItemWidth();
					
                    if (ImGui::Button("Paste Key", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0))) {
                        auto key = getClipboardText(g_vm);
                        SafeCopyToCharBuffer(s, key);
                    }

                    ImGui::SameLine();

                    static std::string err;
                    if (ImGui::Button("Load Saved Key", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                        SharedPreferences sharedPref(GetJNIEnv(g_vm), "xyourzone_sharedpref");
                        auto key = sharedPref.getString("key");
                        SafeCopyToCharBuffer(s, key);
                    }

                    if (ImGui::Button("Login", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                        isSave = false;
                        battleDataResolved = false;
                        std::thread login_thread(LoginThread, std::string(s), &isLogin);
                        login_thread.detach();
                    }
					ImGui::Spacing();
					ImGui::Spacing();
					if (ImGui::Button("Get a Key", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
        				openURL(g_vm, xyzBuyKey);
  					}
                    ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "%s", msg.c_str());
                    ImGui::Spacing();
                }
                ImGui::EndGroupPanel();

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    } else {
		if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_FittingPolicyScroll)) {
            const bool showGameplayTabs = ShouldShowGameplayTabs();
			if (showGameplayTabs){
				if (ImGui::BeginTabItem("ESP")) {
            	if (ImGui::CollapsingHeader("Player")) {
                	if (ImGui::BeginTable("ESPPlayer", 3)) {
                    	ImGui::TableNextColumn();	ImGui::Checkbox(" Player Line", &Config.ESP.Player.Line);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Player Box", &Config.ESP.Player.Box);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Player Name", &Config.ESP.Player.Name);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Player Hero", &Config.ESP.Player.Hero);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Player Health", &Config.ESP.Player.Health);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Player Distance", &Config.ESP.Player.Distance);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Hero Alert", &Config.ESP.Player.Alert);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Icon Hero", &Config.ESP.Player.HeroZ);
                        bool cooldownInfoEnabled = (Config.ESP.Player.CoolDown || Config.ESP.Player.CoolDown2);
                        ImGui::TableNextColumn();
                        if (ImGui::Checkbox(" Cooldown Info", &cooldownInfoEnabled)) {
                            Config.ESP.Player.CoolDown = cooldownInfoEnabled;
                            Config.ESP.Player.CoolDown2 = cooldownInfoEnabled;
                        }
                        ImGui::TableNextColumn();	ImGui::Spacing();
                        ImGui::EndTable();
                    }
                }
	                if (ImGui::CollapsingHeader("Monster")) {
	                	if (ImGui::BeginTable("Monster", 2)) {
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Round", &Config.ESP.Monster.Rounded);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Health", &Config.ESP.Monster.Health);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Name", &Config.ESP.Monster.Name);
                        ImGui::TableNextColumn();	ImGui::Checkbox(" Monster Alert", &Config.ESP.Monster.Alert);
						ImGui::TableNextColumn();	ImGui::Checkbox(" Monster UID", &Config.m_IDConf);
		                        ImGui::EndTable();
		                    }
		                }
                    if (ImGui::CollapsingHeader("Drone View")) {
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderFloat("##DroneHorizontalSlider", &ConfigState::GetCameraState().sliderValue, -10.0f, 50.0f, "Horizontal View %.1f");
                        ImGui::PopItemWidth();
                    }
	                ImGui::EndTabItem();
				}
				}
			if (showGameplayTabs){
				if (ImGui::BeginTabItem("Maphack")) {
	                ImGui::Checkbox("Minimap Icon", &Config.MinimapIcon);
	                if (!Config.MinimapIcon) ImGui::BeginDisabled();
	                ImGui::SameLine();
	                ImGui::Checkbox("Hide Line", &Config.HideLine);
                    ImGui::SameLine();
                    ImGui::Checkbox("Show Jungle", &Config.MinimapMonsterIcon);
                ImGui::BeginGroup();
                {
                    ImGui::BeginGroupPanel("MiniMap Adjustable", ImVec2(-1.0f, 0.0f));
                   {
                        ImGui::BeginGroupPanel("Map Position", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::SliderFloat("##MapPosition", &StartPos.x, 0.0f, (float)(screenWidth / 2));
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();

                        ImGui::BeginGroupPanel("Map Size", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::SliderInt("##MapSize", &MapSize, 0, 800);
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();

                        ImGui::BeginGroupPanel("Icon Size", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::SliderInt("##IconSize", &ICSize, 0, 100);
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();

                        ImGui::BeginGroupPanel("Health Thin", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::SliderInt("##HealthThin", &ICHealthThin, 0, 10);
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();
						
						ImGui::BeginGroupPanel("Health Color", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                        {
                            ImGui::PushItemWidth(-1);
                            ImGui::ColorEdit3("##HealthColor", ColorHealth);
                            ImGui::PopItemWidth();
                            ImGui::Spacing();
                        }
                        ImGui::EndGroupPanel();
														     
                        ImGui::Spacing();
                    }
                    ImGui::EndGroupPanel();
                    if (!Config.MinimapIcon) ImGui::EndDisabled();
                }
                ImGui::EndGroup();
                ImGui::EndTabItem();
            }
			}
				static int SelectInfo = 0;
            static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
            if (ImGui::BeginTabItem("Setting")) {
                ImGui::BeginGroupPanel("Menu Setting", ImVec2(-1.0f, 0.0f));
                {
                    ImGui::Checkbox("Auto Resize", &bFlagAutoResize);
                    ImGui::BeginGroupPanel("Window Size", ImVec2(-1.0f, 0.0f));
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderFloat("##Scale", &window_scale, 0.5f, 2.5f, "%.1f");
                        ImGui::PopItemWidth();
                        ImGui::Spacing();
                    }
                    ImGui::EndGroupPanel();
                    
                    if (ImGui::Button("Hide Menu", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                        isPopUpHide = true;
                    }

                    ImGui::BeginGroupPanel("English", ImVec2(-1.0f, 0.0f));
                    {
                        ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "To display the menu again,");
                        ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "simply touch on the lower left corner of your screen.");
                        ImGui::Spacing();
                    }
                    ImGui::EndGroupPanel();

                    ImGui::BeginGroupPanel("Indonesia", ImVec2(-1.0f, 0.0f));
                    {
                        ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "Untuk menampilkan kembali menu,");
                        ImGui::TextColored(RGBA2ImVec4(255, 255, 0, 255), "cukup sentuh di pojok kiri bawah layar Anda.");
                        ImGui::Spacing();
                    }
                    ImGui::EndGroupPanel();
                    
                    ImGui::Spacing();
                    
                    if (ImGui::Button("Save Cheat Setting", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0))) {
                        saveCFG();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Load Cheat Setting", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                        loadCFG();
                    }
					
        			ImGui::Separator();
					
					ImGui::Spacing();
                    ImGui::BeginGroupPanel("Key Info", ImVec2(-1.0f, 0.0f));
                    {
						Text("Telegram Group: "); SameLine();
                        TextColored(RGBA2ImVec4(176, 40, 40, 255), "@THEMAPHACK");
                        Text("Key Expired: "); SameLine();
                        ImGui::TextColored(RGBA2ImVec4(176, 40, 40, 255), expired.c_str());
                        Text("Key Slot: "); SameLine();
                        TextColored(RGBA2ImVec4(176, 40, 40, 255), slotZ.c_str());
                        Text("Mod Status: "); SameLine();
                        TextColored(RGBA2ImVec4(176, 40, 40, 255), "Safe (5K Server)");
						ImGui::Text("Vip :"); SameLine();
						if (inVip == "100"){
							TextColored(RGBA2ImVec4(176, 40, 40, 255), "Yes");
						} else {
							TextColored(RGBA2ImVec4(176, 40, 40, 255), "No");
						}
						
						Text("Telegram Channel: "); SameLine();
						Text("@THEMAPHACK");
                    }
                    ImGui::EndGroupPanel();
                }
                ImGui::EndGroupPanel();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Room Info")) {
                static std::vector<RoomEnemyInfoRow> cachedEnemies;
                static int lastRefreshFrame = -9999;
                static bool hasSnapshot = false;

                const int frameNow = ImGui::GetFrameCount();
                const bool shouldAutoRefresh = (frameNow - lastRefreshFrame) >= 30;
                if (!hasSnapshot || shouldAutoRefresh) {
                    hasSnapshot = RefreshEnemyRoomInfo(cachedEnemies);
                    lastRefreshFrame = frameNow;
                }

                if (ImGui::Button("Refresh Enemy Data", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                    hasSnapshot = RefreshEnemyRoomInfo(cachedEnemies);
                    lastRefreshFrame = frameNow;
                }

                ImGui::Spacing();
                ImGui::Text("Enemy Count: %d", (int) cachedEnemies.size());
                ImGui::Separator();

                if (!hasSnapshot) {
                    ImGui::TextColored(RGBA2ImVec4(255, 200, 0, 255), "No room data yet. Open this tab during matchmaking/draft.");
                } else if (cachedEnemies.empty()) {
                    ImGui::TextColored(RGBA2ImVec4(255, 200, 0, 255), "Room data available, but enemy side is not resolved yet.");
                } else if (ImGui::BeginTable("EnemyRoomInfoTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
                    ImGui::TableSetupColumn("#");
                    ImGui::TableSetupColumn("Enemy Name");
                    ImGui::TableSetupColumn("UID (Zone)");
                    ImGui::TableSetupColumn("Rank");
                    ImGui::TableSetupColumn("Hero");
                    ImGui::TableSetupColumn("Battle Spell");
                    ImGui::TableHeadersRow();

                    for (int i = 0; i < (int) cachedEnemies.size(); i++) {
                        const auto &enemy = cachedEnemies[i];
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("%d", i + 1);
                        ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(enemy.name.c_str());
                        ImGui::TableSetColumnIndex(2); ImGui::TextUnformatted(enemy.userId.c_str());
                        ImGui::TableSetColumnIndex(3); ImGui::TextUnformatted(enemy.rank.c_str());
                        ImGui::TableSetColumnIndex(4); ImGui::TextUnformatted(enemy.hero.c_str());
                        ImGui::TableSetColumnIndex(5); ImGui::TextUnformatted(enemy.spell.c_str());
                    }

                    ImGui::EndTable();
                }

                ImGui::EndTabItem();
            }
			
			ImGui::EndTabBar();
		}
		ImGui::Separator();
	}
    ImGui::End();
}
