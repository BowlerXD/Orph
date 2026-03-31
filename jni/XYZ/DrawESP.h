#pragma once
#include <algorithm>
#include <cinttypes>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "ConfigName.h"
#include "GameClass.h"
#include "IconMinimap/DrawIcon.h"
#include "IconMinimap/IconList.h"
#include "Minimap.h"
#include "SDK.h"
#include "ToString.h"
#include "DrawMinimap.h"
#include "../imgui/icon.h"

extern bool bFullChecked;

bool isOutsideScreen(ImVec2 pos, ImVec2 screen) {
    if (pos.y < 0) {
        return true;
    }
    if (pos.x > screen.x) {
        return true;
    }
    if (pos.y > screen.y) {
        return true;
    }
    return pos.x < 0;
}

ImVec2 pushToScreenBorder(ImVec2 Pos, ImVec2 screen, int offset) {
    int x = (int) Pos.x;
    int y = (int) Pos.y;
    if (Pos.y < 0) {
        // top
        y = -offset;
    }
    if (Pos.x > screen.x) {
        // right
        x = (int) screen.x + offset;
    }
    if (Pos.y > screen.y) {
        // bottom
        y = (int) screen.y + offset;
    }
    if (Pos.x < 0) {
        // left
        x = -offset;
    }
    return ImVec2(x, y);
}

void DrawCircleHealth(ImVec2 position, int health, int max_health, float radius) {
    float a_max = ((3.14159265359f * 2.0f));
    ImU32 healthColor = IM_COL32(45, 180, 45, 255);
    if (health <= (max_health * 0.6)) {
        healthColor = IM_COL32(180, 180, 45, 255);
    }
    if (health < (max_health * 0.3)) {
        healthColor = IM_COL32(180, 45, 45, 255);
    }
    ImGui::GetForegroundDrawList()->PathArcTo(position, radius, (-(a_max / 4.0f)) + (a_max / max_health) * (max_health - health), a_max - (a_max / 4.0f));
    ImGui::GetForegroundDrawList()->PathStroke(healthColor, ImDrawFlags_None, 4);
}

//TEST NEW DRONE VIEW
/*
uintptr_t Offsets_ResetCameraParm;
static void *ResetCameraParm() {
    return ((void *(*)(void *))(Offsets_ResetCameraParm))(NULL);  //void ResetCameraParm
}

void (*oSetCameraParam)(void *instance, Vector3 offsetPt, Quaternion qua, Transform tarObj, bool bLerp);
void SetCameraParam(void *ins, Vector3 offsetPt, Quaternion qua, Transform tarObj, bool bLerp) {
    Vector3 baruNich = offsetPt;
    if (sliderValue > 0) {   
        if (offsetPt.x <= 0.0)
            baruNich.x = offsetPt.x + (float) sliderValue * -0.234;
        else
            baruNich.x = offsetPt.x + (float) sliderValue * 0.234;
        baruNich.y = offsetPt.y + (float) sliderValue * -0.307;
        if (offsetPt.z <= 0.0)
            baruNich.z = offsetPt.z + (float) sliderValue * -0.235;
        else
            baruNich.z = offsetPt.z + (float) sliderValue * 0.235;
    }  
oSetCameraParam(ins, baruNich, qua, tarObj, bLerp);
}*/
void DroneView() {
    if (GetFieldOfView == 0) {
        GetFieldOfView = get_fieldOfView();
        if (GetFieldOfView <= 1.0f) {
            GetFieldOfView = 60.0f;
        }
    }

    if (sliderValue < -10.0f) sliderValue = -10.0f;
    if (sliderValue > 50.0f) sliderValue = 50.0f;

    float targetFov = GetFieldOfView + SetFieldOfView + sliderValue;
    if (targetFov < 20.0f) targetFov = 20.0f;
    if (targetFov > 140.0f) targetFov = 140.0f;
    set_fieldOfView(targetFov);
}

bool (*s_Emblem2)(void* _this);
bool s_Emblem(void* _this) {
    if (Config.Visual.UnlockEmblem) {
        return true;
    }
    return s_Emblem2(_this);
}

void (*s_Spell2)(void *instance, int Spelll);
void s_Spell(void *instance, int Spelll) {
    if (instance != nullptr) {
        if (Config.Visual.UnlockSpell) {
            return;
        }
    }
     s_Spell2(instance, Spelll);
}

#define DefineHook(RET, NAME, ARGS) \
    RET (*o ## NAME) ARGS; \
    RET NAME ARGS
    
class FPSCounter {
protected:
    unsigned int m_fps;
    unsigned int m_fpscount;
    long m_fpsinterval;

public:
    FPSCounter() : m_fps(0), m_fpscount(0), m_fpsinterval(0) {
    }

    void update() {
        m_fpscount++;

        if (m_fpsinterval < time(0)) {
            m_fps = m_fpscount;

            m_fpscount = 0;
            m_fpsinterval = time(0) + 0;
        }
    }

    unsigned int get() const {
        return m_fps;
    }
} fps;

void (*oIsCampGrassLayer)(void *, double);
void IsCampGrassLayer(void *instance, double dmgValue){
	if (instance != nullptr){
		if (Config.Visual.NoGrass){
			dmgValue = 9999;
		}
	}
	oIsCampGrassLayer(instance, dmgValue);
}

void (*orig_ShowEntity__OnUpdate)(ShowEntity *showEntity);
void _ShowEntity__OnUpdate(ShowEntity *showEntity) {
    if (showEntity != nullptr){
        if (Config.Visual.MapHackIcon2) {
            static const uintptr_t kMinimapVisibleOffset = (uintptr_t)Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_bUnityMinimapVisible");
            if (kMinimapVisibleOffset) {
			    *(bool *) (showEntity + kMinimapVisibleOffset) = Config.Visual.MapHackIcon2;
            }
        }
    }
    orig_ShowEntity__OnUpdate(showEntity);
}

void (*orig_ShowPlayer_Unity_OnUpdate)(void* thisz);
void _ShowPlayer_Unity_OnUpdate(void* thisz){
	orig_ShowPlayer_Unity_OnUpdate(thisz);
}

void (*oLogicPlayer_Update)(void* thisz, int status);
void LogicPlayer_Update(void* thisz, int status){
	if (thisz != NULL){
		if(UpdateManiacStatus){
			*(int *) ((uintptr_t) thisz + LogicPlayer__QuadraKillTimes) = 1234;
		}
	}
	oLogicPlayer_Update(thisz, status);
}

void (*set_QuadraKillTimes)(void* thisz, int status);
void oset_QuadraKillTimes(void* thisz, int status){
	if (thisz != NULL){
		if (UpdateManiacStatus){
			set_QuadraKillTimes(thisz, 1234);
            return;
		}
	}
	set_QuadraKillTimes(thisz, status);
}

double (*orig_get_m_AttkSpeed)(void* thiz);
double get_m_AttkSpeed(void* thiz){
	if (thiz != NULL && UpdateManiacStatus){
		return 123456;
	}
    return orig_get_m_AttkSpeed(thiz);
}


DefineHook(void, UpdateMapHack, (void * pThis)) {
    int battleState = GetBattleState();
    if (battleState != 2 && battleState != 3) {
        oUpdateMapHack(pThis);
        return;
    }

    if (pThis != NULL) {
        void *BattleBridge_Instance = nullptr, *BattleManager_Instance = nullptr;
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "BattleData", "m_BattleBridge", &BattleBridge_Instance);
        bFullChecked = false;
        if (BattleBridge_Instance) {
            Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "BattleManager", "Instance", &BattleManager_Instance);
            if (BattleManager_Instance) {
                bFullChecked = true;
            }
            if (BattleManager_Instance) {
                static const uintptr_t kSameCampOffset = EntityBase_m_bSameCampType();
                static const uintptr_t kDeathOffset = EntityBase_m_bDeath();
                static const uintptr_t kCanSightOffset = EntityBase_canSight();
                static const uintptr_t kGuidOffset = EntityBase_m_uGuid();
                static const uintptr_t kPosOffset = ShowEntity__Position();
                static const uintptr_t kShowPlayersOffset = BattleManager_m_ShowPlayers();
                auto m_ShowPlayers = *(List<void **> **) ((uintptr_t)BattleManager_Instance + kShowPlayersOffset);
                if (m_ShowPlayers) {
                    for (int i = 0; i < m_ShowPlayers->getSize(); i++) {
                        auto Pawn = m_ShowPlayers->getItems()[i];               
                        if (!Pawn) continue;
                        auto m_bSameCampType = *(bool *) ((uintptr_t)Pawn + kSameCampOffset);
                        if (m_bSameCampType) continue;
                        auto m_bDeath = *(bool *) ((uintptr_t)Pawn + kDeathOffset);
                        if (m_bDeath) continue;
                        auto canSight = *(bool *) ((uintptr_t)Pawn + kCanSightOffset);
                        if (canSight) continue;
                        auto m_uGuid = *(int *) ((uintptr_t)Pawn + kGuidOffset);
                        auto _Position = *(Vector3 *) ((uintptr_t)Pawn + kPosOffset);
                        auto *m_HeadIcon = *(String **) ((uintptr_t) Pawn + ShowEntity_m_HeadIcon);
						if (Config.Visual.MapHackIcon2){
							auto CanSightMapHack = (void (*)(void *, Vector3)) (ShowEntity_CanSight);
                            if (CanSightMapHack) {
							    CanSightMapHack(BattleBridge_Instance, _Position);
                            }
						}
						
                        if (Config.Visual.MaphackIcon) {
                            auto SetMapEntityIconPos = (void (*)(void *, Vector3, int, bool)) (BattleBridge_SetMapEntityIconPos());
                            //auto SetMapInvisibility = (void (*)(void *, bool, bool, bool)) (BattleBridge_SetMapInvisibility);                            
                            //SetMapInvisibility(BattleBridge_Instance, bShowEntityLayer, true, m_bDeath);
                            if (SetMapEntityIconPos) {
                                SetMapEntityIconPos(BattleBridge_Instance, _Position,m_uGuid, true);
                            }
							
                        }
                        
                        if (Config.Visual.HealthBar) {
                            auto SetBloodInvisibility = (void (*)(void *, int, bool, bool, bool, int)) (BattleBridge_SetBloodInvisibility());
                            auto SynBloodAttr = (void (*)(void *, void *, bool)) (BattleBridge_SynBloodAttr());
                            auto UpdateBloodPos = (void (*)(void *, void *, bool)) (BattleBridge_UpdateBloodPos());
                            if (SetBloodInvisibility && SynBloodAttr && UpdateBloodPos) {
                                SetBloodInvisibility(BattleBridge_Instance, m_uGuid, true, m_bDeath, true, 0);
                                SynBloodAttr(BattleBridge_Instance, Pawn, true);
                                UpdateBloodPos(BattleBridge_Instance, Pawn, true);
                            }
                        }
                        
                        if (Config.Visual.HeadIcon) {
                            auto BattleBridge__ShowHeadEquip = (void (*)(void *, int, String*)) (BattleBridge_ShowHeadEquip());
                            if (BattleBridge__ShowHeadEquip) {
                                BattleBridge__ShowHeadEquip(BattleBridge_Instance, m_uGuid, m_HeadIcon);
                            }
                        }
						
						if (Config.Visual.MapHackIcon3){
                            auto ResetLayer = (void (*)(void *, bool)) (ShowEntity_ResetLayer);
                            auto EyeLayer = (void (*)(void *, bool, bool, bool)) (ShowEntityUpdateEyeLayer);
                            if (ResetLayer && EyeLayer) {
                                ResetLayer(Pawn, true);
                                EyeLayer(Pawn, true, true, true);
                            }
                        }
                    }
                }
                List<void **> *m_ShowMonster = *(List<void **> **) ((uintptr_t)BattleManager_Instance + BattleManager_m_ShowMonsters());
                if (m_ShowMonster) {
                    for (int i = 0; i < m_ShowMonster->getSize(); i++) {
                        auto Pawn = m_ShowMonster->getItems()[i];
                        if (!Pawn) continue;
                        bool isSoldier = *(bool *) ((uintptr_t)Pawn + ShowEntity_IsSoldier);
                        bool isTower = *(bool *) ((uintptr_t)Pawn + ShowEntity_IsTower);
                        bool isWildMonster = *(bool *) ((uintptr_t)Pawn + ShowEntity_IsWildMonster);
                        if (isTower) continue;
                        if (!isWildMonster && !isSoldier) continue;
                        auto m_bSameCampType = *(bool *) ((uintptr_t)Pawn + EntityBase_m_bSameCampType());
                        if (m_bSameCampType && !isSoldier) continue;
                        auto m_bDeath = *(bool *) ((uintptr_t)Pawn + EntityBase_m_bDeath());
                        if (m_bDeath) continue;
                        auto canSight = *(bool *) ((uintptr_t)Pawn + EntityBase_canSight());
                        if (canSight) continue;
                        auto m_uGuid = *(int *) ((uintptr_t)Pawn + EntityBase_m_uGuid());
                        auto _Position = *(Vector3 *) ((uintptr_t)Pawn + ShowEntity__Position());
                        if (Config.Visual.MonsterIcon || (Config.MinimapIcon && isSoldier)) {
                            auto SetMapEntityIconPos = (void (*)(void *, Vector3, int, bool)) (BattleBridge_SetMapEntityIconPos());
                            auto SetMapInvisibility = (void (*)(void *, int, bool, bool)) (BattleBridge_SetMapInvisibility());
                            if (SetMapEntityIconPos && m_uGuid > 0) {
                                if (!isSoldier && SetMapInvisibility) {
                                    SetMapInvisibility(BattleBridge_Instance, m_uGuid, true, m_bDeath);
                                }
                                SetMapEntityIconPos(BattleBridge_Instance, _Position, m_uGuid, true);
                            }
                        }
                        
                        if (Config.Visual.MonsterHealth) {
                            auto SetBloodInvisibility = (void (*)(void *, int, bool, bool, bool, int)) (BattleBridge_SetBloodInvisibility());
                            auto SynBloodAttr = (void (*)(void *, void *, bool)) (BattleBridge_SynBloodAttr());
                            auto UpdateBloodPos = (void (*)(void *, void *, bool)) (BattleBridge_UpdateBloodPos());
                            
                            if (SetBloodInvisibility && SynBloodAttr && UpdateBloodPos) {
                                SetBloodInvisibility(BattleBridge_Instance, m_uGuid, true, m_bDeath, true, 0);
                                SynBloodAttr(BattleBridge_Instance, Pawn, true);
                                UpdateBloodPos(BattleBridge_Instance, Pawn, true);
                            }
                        }
                        
                        if (Config.Visual.MonsterBody) {
                            auto ShowWildMonster__CanSight = (bool (*)(void*, bool)) (ShowEntity_CanSight);
                            if (ShowWildMonster__CanSight) {
                                ShowWildMonster__CanSight(Pawn, true);
                            }
                        }
                        
                        if (Config.Visual.ShowJungle){
                            auto ResetLayer = (void (*)(void *, bool)) (ShowEntity_ResetLayer);
                            auto EyeLayer = (void (*)(void *, bool, bool, bool)) (ShowEntityUpdateEyeLayer);
                            if (ResetLayer && EyeLayer) {
                                ResetLayer(Pawn, true);
                                EyeLayer(Pawn, true, true, true);
                            }
                        }
                        
                        if (Config.Visual.ShowMonsterTest){
                            auto InitSetEye = (void(*)(void*,bool, bool)) (ShowEntity_InitSetEye);
                            if (InitSetEye) {
                                InitSetEye(Pawn, true, true);
                            }
                        }
                    }
                }
                
            }
        }
    }
    oUpdateMapHack(pThis);
}

int32_t ToColor(float *col) {
    return ImGui::ColorConvertFloat4ToU32(*(ImVec4 *) (col));
}

bool bMonster(int iValue) {
    return std::find(std::begin(ListMonsterId), std::end(ListMonsterId), iValue) != std::end(ListMonsterId);
}

static float lineThickness = 1.0f;
static ImVec4 lineColor = ImVec4(0.9f, 0.9f, 0.98f, 1.0f); // RGBA normalized (0.0 - 1.0)
static ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // RGBA normalized (0.0 - 1.0)
static ImVec4 healthBarColor = ImVec4(0.88f, 0.0f, 0.0f, 1.0f);  // RGBA normalized (0.0 - 1.0)
static ImVec4 borderColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);      // Border color

ImVec2 getPosVec2(Vector3 _Position, int screenWidth, int screenHeight) {
    auto RootPosW2S = WorldToScreen(_Position);
    if (RootPosW2S.z > 0) {
        return ImVec2(RootPosW2S.x, screenHeight - RootPosW2S.y);
    }
    return ImVec2(screenWidth - RootPosW2S.x, RootPosW2S.y);
}

struct CooldownValues {
    int skillCount;
    int skill1;
    int skill2;
    int skill3;
    int skill4;
    int spell;
};

CooldownValues getPlayerCoolDown(int keys, uintptr_t values) {
    static const uintptr_t kLogicFighterSkillCompOffset = LogicFighter_m_SkillComp;
    static const uintptr_t kLogicSkillCompCoolDownCompOffset = LogicSkillComp_m_CoolDownComp;
    static const uintptr_t kShowEntityOwnSkillCompOffset = ShowEntity_m_OwnSkillComp;
    static const uintptr_t kShowOwnSkillCompSkillUseTypeListOffset = ShowOwnSkillComp_skillUseTypeList();
    static const uintptr_t kShowOwnSkillCompSkillListOffset = ShowOwnSkillComp_m_SkillList();
    static const uintptr_t kCoolDownCompDicCoolInfoOffset = CoolDownComp_m_DicCoolInfo;
    static const uintptr_t kShowPlayerSummonSkillIdOffset = ShowPlayer_m_iSummonSkillId();
    static const uintptr_t kShowSkillDataTranIdOffset = ShowSkillData_m_TranID();

    if (!kLogicFighterSkillCompOffset || !kLogicSkillCompCoolDownCompOffset || !kShowEntityOwnSkillCompOffset ||
        !kShowOwnSkillCompSkillUseTypeListOffset || !kShowOwnSkillCompSkillListOffset || !kCoolDownCompDicCoolInfoOffset ||
        !kShowPlayerSummonSkillIdOffset || !kShowSkillDataTranIdOffset) {
        return {0, 0, 0, 0, 0, 0};
    }

    auto logicFighter = GetPlayerLogic((uint)keys);
    if (!logicFighter) return {0, 0, 0, 0, 0, 0};

    auto m_SkillComp = *(uintptr_t *) ((uintptr_t)logicFighter + kLogicFighterSkillCompOffset);
    if (!m_SkillComp) return {0, 0, 0, 0, 0, 0};

    auto m_CoolDownComp = *(uintptr_t *) ((uintptr_t)m_SkillComp + kLogicSkillCompCoolDownCompOffset);
    if (!m_CoolDownComp) return {0, 0, 0, 0, 0, 0};

    auto m_OwnSkillComp = *(uintptr_t *) ((uintptr_t)values + kShowEntityOwnSkillCompOffset);
    if (!m_OwnSkillComp) return {0, 0, 0, 0, 0, 0};

    Dictionary<int, List<int> *> *skillUseTypeList = *(Dictionary<int, List<int> *> **) ((uintptr_t)m_OwnSkillComp + kShowOwnSkillCompSkillUseTypeListOffset);
    if (!skillUseTypeList) return {0, 0, 0, 0, 0, 0};

    List<uintptr_t> *m_SkillList = *(List<uintptr_t> **) ((uintptr_t)m_OwnSkillComp + kShowOwnSkillCompSkillListOffset);
    if (!m_SkillList) return {0, 0, 0, 0, 0, 0};

    auto m_DicCoolInfo = *(Dictionary<int, uintptr_t> **) ((uintptr_t)m_CoolDownComp + kCoolDownCompDicCoolInfoOffset);
    if (!m_DicCoolInfo) return {0, 0, 0, 0, 0, 0};

    const int m_iSummonSkillId = *(int *) ((uintptr_t)values + kShowPlayerSummonSkillIdOffset);
    const int maxSkillSlots = std::min(5, m_SkillList->getSize());

    int skillCount = 0, skill1 = 0, skill2 = 0, skill3 = 0, skill4 = 0, spell = 0;

    for (int v = 0; v < skillUseTypeList->getNumKeys(); v++) {
        auto keysskillUseType = skillUseTypeList->getKeys()[v];
        auto valuesskillUseType = skillUseTypeList->getValues()[v];
        if (!keysskillUseType) continue;
        if (keysskillUseType != 1) continue;

        skillCount = valuesskillUseType->getSize();
        for (int i = 0; i < maxSkillSlots; i++) {
            auto p_SkillList = m_SkillList->getItems()[i];
            if (!p_SkillList) continue;

            auto m_TranID = *(int *) ((uintptr_t)p_SkillList + kShowSkillDataTranIdOffset);

            for (int l = 0; l < m_DicCoolInfo->getNumKeys(); l++) {
                auto keysCool = m_DicCoolInfo->getKeys()[l];
                auto valuesCool = m_DicCoolInfo->getValues()[l];
                if (!keysCool || !valuesCool) continue;

                int coolTime = GetCoolTime((void*)valuesCool) / 1000;
                if (keysCool == m_iSummonSkillId) spell = coolTime;
                if ((int)m_TranID != keysCool) continue;
                if (i == 1) skill1 = coolTime;
                if (i == 2) skill2 = coolTime;
                if (i == 3) skill3 = coolTime;
                if (i == 4) skill4 = coolTime;
            }
        }
    }
    return {skillCount, skill1, skill2, skill3, skill4, spell};
}

static inline bool IsExcludedJungleObjective(int jungleTypeId) {
    return jungleTypeId == 2002 || jungleTypeId == 2003 || jungleTypeId == 2110; // lord/turtle
}

static inline uint ResolveJungleCampType(int selfCampType, int entityCampType) {
    if (selfCampType == 1) return 2;
    if (selfCampType == 2) return 1;
    return (uint) entityCampType;
}

static inline void DrawJungleMinimapMarker(ImDrawList *draw, ImVec2 pos, int jungleId) {
    const float outerRadius = 5.6f;
    const float innerRadius = 4.5f;
    ImU32 innerColor = IM_COL32(88, 216, 112, 245);   // default green

    if (jungleId == 2011 || jungleId == 2013) {        // crab
        innerColor = IM_COL32(232, 194, 88, 248);
    } else if (jungleId == 2004) {                     // fiend
        innerColor = IM_COL32(246, 148, 86, 246);
    } else if (jungleId == 2006 || jungleId == 2008 || jungleId == 2059 || jungleId == 2009) { // scaled lizard/crammer/golem
        innerColor = IM_COL32(98, 222, 124, 246);
    } else if (jungleId == 2056 || jungleId == 2072) { // litho
        innerColor = IM_COL32(122, 214, 255, 246);
    } else if (jungleId == 2005 || jungleId == 2012) { // serpent/serpent kids
        innerColor = IM_COL32(192, 118, 255, 246);
    }

    draw->AddCircleFilled(pos, outerRadius, IM_COL32(16, 40, 44, 235), 16); // dark ring
    draw->AddCircleFilled(pos, innerRadius, innerColor, 16);                 // colored fill
}

static inline ImU32 GetHealthBarColor(float ratio, int alpha = 175) {
    ratio = std::max(0.0f, std::min(1.0f, ratio));
    int r = 0, g = 0, b = 0;
    if (ratio < 0.5f) {
        float t = ratio / 0.5f; // red -> yellow
        r = (int)(235 + (255 - 235) * t);
        g = (int)(62 + (201 - 62) * t);
        b = (int)(62 + (74 - 62) * t);
    } else {
        float t = (ratio - 0.5f) / 0.5f; // yellow -> green
        r = (int)(255 + (74 - 255) * t);
        g = (int)(201 + (226 - 201) * t);
        b = (int)(74 + (120 - 74) * t);
    }
    return IM_COL32(r, g, b, alpha);
}

static inline ImU32 GetObjectiveAlertGradientColor(float ratio, int alpha = 255) {
    ratio = std::max(0.0f, std::min(1.0f, ratio));
    int r = 0, g = 0, b = 0;
    if (ratio < 0.5f) {
        // low -> mid : red to orange
        float t = ratio / 0.5f;
        r = (int)(232 + (255 - 232) * t);
        g = (int)(72 + (154 - 72) * t);
        b = (int)(72 + (46 - 72) * t);
    } else {
        // mid -> full : orange to light-blue
        float t = (ratio - 0.5f) / 0.5f;
        r = (int)(255 + (106 - 255) * t);
        g = (int)(154 + (214 - 154) * t);
        b = (int)(46 + (255 - 46) * t);
    }
    return IM_COL32(r, g, b, alpha);
}

struct ObjectiveAlertState {
    int lastHp = 0;
    int lastHpMax = 0;
    long long firstSeenMs = 0;
    long long lastUpdateMs = 0;
    bool seenFullHp = false;
    bool active = false;
    int belowFullStreak = 0;
};

static inline long long GetNowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

static inline ObjectiveAlertState &GetObjectiveAlertState(int id) {
    static ObjectiveAlertState lordState;
    static ObjectiveAlertState turtleState;
    return (id == 2002) ? lordState : turtleState;
}

static inline bool ShouldShowObjectiveAlert(int id, int hp, int hpMax, bool isDead) {
    if (id != 2002 && id != 2003) return false;

    ObjectiveAlertState &state = GetObjectiveAlertState(id);
    const long long nowMs = GetNowMs();
    const long long gracePeriodMs = 1500;

    if (isDead || hpMax <= 0) {
        state = ObjectiveAlertState{};
        return false;
    }

    if (!state.active) {
        state.active = true;
        state.firstSeenMs = nowMs;
        state.belowFullStreak = 0;
    }

    if (hp >= hpMax && hpMax > 0) {
        state.seenFullHp = true;
        state.belowFullStreak = 0;
    } else if (hp < hpMax) {
        state.belowFullStreak = std::min(state.belowFullStreak + 1, 10);
    }

    state.lastHp = hp;
    state.lastHpMax = hpMax;
    state.lastUpdateMs = nowMs;

    const bool graceElapsed = (nowMs - state.firstSeenMs) >= gracePeriodMs;
    const bool hpDroppedConsistently = state.belowFullStreak >= 2;
    return graceElapsed && state.seenFullHp && hpDroppedConsistently;
}

static inline void DrawObjectiveAlertCard(ImDrawList *draw, int objectiveId, int hp, int hpMax, float screenWidth, float screenHeight) {
    if (hpMax <= 0) return;
    float ratio = std::max(0.0f, std::min(1.0f, (float)hp / (float)hpMax));
    // In this build: 2002 = Lord, 2003 = Turtle (see MonsterToString mapping).
    const bool isLord = (objectiveId == 2002);
    const char *alertText = isLord ? "LORD IS UNDER ATTACK" : "TURTLE IS UNDER ATTACK";

    // Place alert above the minimap zoom icon (right side of minimap).
    float cardWidth = std::clamp(screenWidth * 0.22f, 240.0f, 420.0f);
    float cardHeight = std::clamp(screenHeight * 0.09f, 60.0f, 120.0f);
    ImVec2 cardSize(cardWidth, cardHeight);
    // In this overlay, minimap X is effectively anchored from screen-left, while minimapPosX
    // includes an internal offset. Use minimapWidth as visual right edge reference.
    const float zoomAnchorX = minimapWidth + std::clamp(minimapWidth * 0.05f, 10.0f, 20.0f);
    const float zoomAnchorY = minimapPosY + minimapHeight * 0.50f;
    // Nudge card diagonally to lower-left so it hugs minimap edge closer.
    const float cardNudgeLeft = std::clamp(cardSize.x * 0.13f, 26.0f, 48.0f);
    const float cardNudgeDown = std::clamp(cardSize.y * 0.26f, 16.0f, 30.0f);
    ImVec2 cardTopLeft(
        minimapWidth - 2.0f - cardNudgeLeft,
        zoomAnchorY - cardSize.y - std::clamp(cardSize.y * 0.14f, 10.0f, 18.0f) + cardNudgeDown
    );
    ImVec2 cardBottomRight(cardTopLeft.x + cardSize.x, cardTopLeft.y + cardSize.y);

    // Bubble-style panel: rounded main body.
    const float cardRounding = std::clamp(cardSize.y * 0.12f, 6.0f, 12.0f);
    const ImU32 bubbleFill = IM_COL32(16, 20, 28, 210);
    const ImU32 bubbleBorder = IM_COL32(0, 0, 0, 210);
    draw->AddRectFilled(cardTopLeft, cardBottomRight, bubbleFill, cardRounding);
    draw->AddRect(cardTopLeft, cardBottomRight, bubbleBorder, cardRounding, 0, 1.4f);

    // Alert image (use raw icon color; no gradient tint so base64 image is not distorted).
    Icon alertIcon = MonsterAlertTexture(objectiveId);
    if (!alertIcon.IsValid) {
        // Fallback to objective portrait icon to avoid empty square.
        alertIcon = MonsterTexture(objectiveId);
    }
    const float imagePadding = std::clamp(cardHeight * 0.08f, 4.0f, 8.0f);
    ImVec2 imageTopLeft(cardTopLeft.x + imagePadding, cardTopLeft.y + imagePadding);
    float imageSize = cardSize.y - imagePadding * 2.0f;
    ImVec2 imageBottomRight(imageTopLeft.x + imageSize, imageTopLeft.y + imageSize);
    if (alertIcon.IsValid) {
        draw->AddImageRounded((void*)(uintptr_t)alertIcon.texture, imageTopLeft, imageBottomRight, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255), 6.0f);
    } else {
        draw->AddRectFilled(imageTopLeft, imageBottomRight, IM_COL32(65, 72, 90, 220), 6.0f);
    }

    float titleFontSize = std::clamp(cardHeight * 0.25f, 12.0f, 20.0f);
    const float contentStartX = imageBottomRight.x + std::clamp(cardWidth * 0.025f, 6.0f, 12.0f);
    const float contentTopPadding = std::clamp(cardHeight * 0.12f, 6.0f, 10.0f);
    ImVec2 titlePos(contentStartX, cardTopLeft.y + contentTopPadding);
    draw->AddText(NULL, titleFontSize, titlePos, IM_COL32(255, 255, 255, 245), alertText);

    float barStartX = contentStartX;
    const float contentRightPadding = std::clamp(cardWidth * 0.035f, 8.0f, 14.0f);
    float barWidth = std::max(1.0f, cardBottomRight.x - barStartX - contentRightPadding);
    float barHeight = std::clamp(cardHeight * 0.16f, 8.0f, 14.0f);
    float barY = cardBottomRight.y - barHeight - std::clamp(cardHeight * 0.12f, 6.0f, 10.0f);
    ImVec2 barMin(barStartX, barY);
    ImVec2 barMax(barStartX + barWidth, barY + barHeight);
    draw->AddRectFilled(barMin, barMax, IM_COL32(60, 66, 78, 235), 3.0f);
    draw->AddRectFilled(barMin, ImVec2(barMin.x + (barWidth * ratio), barMax.y), GetObjectiveAlertGradientColor(ratio, 245), 3.0f);
    draw->AddRect(barMin, barMax, IM_COL32(0, 0, 0, 255), 3.0f, 0, 1.8f);

    std::string hpText = std::to_string(hp);
    float hpFontSize = std::max(14.0f, barHeight + 2.0f);
    ImVec2 hpTextSize = ImGui::CalcTextSize(hpText.c_str(), 0, hpFontSize);
    ImVec2 hpTextPos((barMin.x + barMax.x - hpTextSize.x) * 0.5f, barMin.y - hpTextSize.y - 2.0f);
    draw->AddText(NULL, hpFontSize, hpTextPos, IM_COL32(255, 255, 255, 245), hpText.c_str());
}

static inline void DrawWideHealthBarWithText(ImDrawList *draw, ImVec2 topLeft, float width, float height, int hp, int hpMax) {
    if (hpMax <= 0) return;
    float ratio = std::max(0.0f, std::min(1.0f, (float) hp / (float) hpMax));
    float fillW = width * ratio;
    const float slant = std::min(14.0f, width * 0.12f);
    float fillSlant = (fillW > 1.0f) ? ((fillW < slant) ? (fillW * 0.75f) : slant) : 0.0f;

    ImU32 bgCol = IM_COL32(18, 22, 30, 120);
    ImU32 borderCol = IM_COL32(255, 255, 255, 95);
    ImU32 fillCol = GetHealthBarColor(ratio, 170);

    ImVec2 bgPoly[4] = {
        ImVec2(topLeft.x, topLeft.y),
        ImVec2(topLeft.x + width - slant, topLeft.y),
        ImVec2(topLeft.x + width, topLeft.y + height),
        ImVec2(topLeft.x, topLeft.y + height)
    };
    draw->AddConvexPolyFilled(bgPoly, 4, bgCol);
    draw->AddPolyline(bgPoly, 4, borderCol, ImDrawFlags_Closed, 1.15f);

    if (fillW > 0.0f) {
        ImVec2 fillPoly[4] = {
            ImVec2(topLeft.x, topLeft.y),
            ImVec2(topLeft.x + fillW - fillSlant, topLeft.y),
            ImVec2(topLeft.x + fillW, topLeft.y + height),
            ImVec2(topLeft.x, topLeft.y + height)
        };
        draw->AddConvexPolyFilled(fillPoly, 4, fillCol);
    }

    std::string hpText = std::to_string(hp);
    float fontSize = std::max(12.0f, height - 2.0f);
    ImVec2 textSize = ImGui::CalcTextSize(hpText.c_str(), 0, fontSize);
    ImVec2 textPos(topLeft.x + (width - textSize.x) * 0.5f, topLeft.y + (height - textSize.y) * 0.5f);
    draw->AddText(NULL, fontSize, textPos, IM_COL32(255, 255, 255, 230), hpText.c_str());
}

void NewDrawESP(ImDrawList *draw, float screenWidth, float screenHeight) {
	/*
	if (g_Token.empty())
    	return;
		
	if (g_Auth.empty())
        return;
		
	if (g_Token != g_Auth)
        return;
	*/
    if (Config.ESP.FPS) {
        std::string sFPS = "FPS: ";
        sFPS += std::to_string(fps.get());
        auto textSize = ImGui::CalcTextSize(sFPS.c_str(), 0, ((float) screenHeight / 39.0f));
        draw->AddText(NULL, ((float) screenHeight / 39.0f), {(float)(screenWidth / 4.0f) - (textSize.x / 2), (float)(screenHeight - screenHeight) + (float)(screenHeight / 8.7f)}, IM_COL32(255, 255, 255, 255), sFPS.c_str());
        draw->AddText(NULL, ((float) screenHeight / 39.0f), {(float)(screenWidth / 4.0f) - (textSize.x / 2), (float)(screenHeight - screenHeight) + (float)(screenHeight / 8.7f)}, IM_COL32(10, 255, 202, 255), sFPS.c_str());
    }
    fps.update();
    void *battleBridgeInstance = nullptr, *battleManagerInstance = nullptr;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "BattleData", "m_BattleBridge", &battleBridgeInstance);
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "BattleManager", "Instance", &battleManagerInstance);
    if (!battleBridgeInstance || !battleManagerInstance) {
        return;
    }

    if (sliderValue != 0.0f || SetFieldOfView != 0.0f) {
        DroneView();
    }
    
    if (Config.MinimapIcon) {
        if (!Config.HideLine)
            draw->AddRect(ImVec2(StartPos.x, StartPos.y), ImVec2(StartPos.x + MapSize, StartPos.y + MapSize), IM_COL32(255, 255, 255, 255));
    }
    
    auto battleManager = (uintptr_t) battleManagerInstance;
    if (!battleManager) return;
    static const uintptr_t kLocalPlayerShowOffset = BattleManager_m_LocalPlayerShow();
    static const uintptr_t kMonsterDictOffset = BattleManager_m_dicMonsterShow();
    static const uintptr_t kPlayerDictOffset = BattleManager_m_dicPlayerShow();
    static const uintptr_t kEntityPosOffset = ShowEntity__Position();
    /*self*/
    auto m_LocalPlayerShow = *(uintptr_t *) ((uintptr_t)battleManager + kLocalPlayerShowOffset);
    if (!m_LocalPlayerShow) return;
    auto selfPos = *(Vector3 *) ((uintptr_t)m_LocalPlayerShow + kEntityPosOffset);
    auto selfCampType = *(int *) ((uintptr_t)m_LocalPlayerShow + EntityBase_m_EntityCampType());
    auto selfPosVec2 = getPosVec2(selfPos, screenWidth, screenHeight);
    /*monster*/
    auto m_dicMonsterShow = *(Dictionary<int, uintptr_t> **) ((uintptr_t)battleManager + kMonsterDictOffset);
    if (!m_dicMonsterShow) return;

    for (int i = 0; i < m_dicMonsterShow->getNumKeys(); i++) {
        auto keys = m_dicMonsterShow->getKeys()[i];
        auto values = m_dicMonsterShow->getValues()[i];
        if (!keys || !values) continue;
        auto m_ID = *(int *) ((uintptr_t)values + EntityBase_m_ID());
		auto new_mID = *(int *) ((uintptr_t)values + ShowEntity_m_id());
        auto m_bDeath = *(bool *) ((uintptr_t)values + EntityBase_m_bDeath());
        if (m_bDeath) continue;
        auto m_bSameCampType = *(bool *) ((uintptr_t)values + EntityBase_m_bSameCampType());
        auto canSight = *(bool *) ((uintptr_t)values + EntityBase_canSight());
        bool isVisible = canSight || *(bool *) ((uintptr_t)values + ShowEntity_bShowEntityLayer);
        auto m_Hp = *(int *) ((uintptr_t)values + EntityBase_m_Hp());
        auto m_HpMax = *(int *) ((uintptr_t)values + EntityBase_m_HpMax());
        //if (bShowEntityLayer && m_Hp != m_HpMax) continue;
        auto _Position = *(Vector3 *) ((uintptr_t)values + kEntityPosOffset);
        auto rootPosVec2 = getPosVec2(_Position, screenWidth, screenHeight);

        bool isSoldier = *(bool *) ((uintptr_t)values + ShowEntity_IsSoldier);
        bool isTower = *(bool *) ((uintptr_t)values + ShowEntity_IsTower);
        bool isWildMonster = *(bool *) ((uintptr_t)values + ShowEntity_IsWildMonster);
        if (isTower) continue;
        int jungleTypeId = bMonster(new_mID) ? new_mID : m_ID;
        bool isJungleMonster = isWildMonster && bMonster(jungleTypeId);

        if (Config.MinimapIcon) {
            auto m_EntityCampType = *(int *) ((uintptr_t)values + EntityBase_m_EntityCampType());
            auto minimapPosEntity = WorldToMinimap((uint) m_EntityCampType, _Position);
            uint jungleCampType = ResolveJungleCampType(selfCampType, m_EntityCampType);
            auto minimapPosJungle = WorldToMinimap(jungleCampType, _Position);

            if (Config.MinimapMonsterIcon && isJungleMonster && !IsExcludedJungleObjective(jungleTypeId) && !isVisible) {
                ImVec2 junglePos(minimapPosJungle.x, minimapPosJungle.y);
                DrawJungleMinimapMarker(draw, junglePos, jungleTypeId);
            }

            // Hero minimap icon is drawn with overlay; keep minion consistent with overlay path.
            if (isSoldier && !m_bSameCampType && !isVisible) {
                ImVec2 soldierPos(minimapPosEntity.x, minimapPosEntity.y);
                // Match native minimap dot style (solid red dot, no white border).
                draw->AddCircleFilled(soldierPos, 2.9f, IM_COL32(255, 70, 70, 245), 12);
            }
        }

        if (!isJungleMonster) continue;
        if (m_bSameCampType) continue;
		
        if (!m_bDeath && Config.ESP.Monster.Rounded) {
            draw->AddCircleFilled(rootPosVec2, 10, IM_COL32(255, 255, 255, 255));
        }
		/*
		if (Config.m_IDConf) {
            std::string mConfigi = to_string(new_mID);
            auto textSize = ImGui::CalcTextSize(mConfigi.c_str(), 0, 20);
            draw->AddText(NULL, 22, {rootPosVec2.x - (textSize.x / 2), rootPosVec2.y + 40}, IM_COL32(255, 255, 255, 255), mConfigi.c_str());
        }
        */
        if (!m_bDeath && Config.ESP.Monster.Name) {
            std::string strName = MonsterToString(m_ID);
            auto textSize = ImGui::CalcTextSize(strName.c_str(), 0, ((float) screenHeight / 39.0f));
            draw->AddText(NULL, ((float) screenHeight / 39.0f), {rootPosVec2.x - (textSize.x / 2), rootPosVec2.y + 25}, IM_COL32(255, 255, 255, 255), strName.c_str());
        }
        
        if (Config.ESP.Monster.Health && m_ID != 2002 && m_ID != 2003) {
            ImVec2 lineStart(rootPosVec2.x + 46.0f, rootPosVec2.y - 24.0f);
            const float monsterBarWidth = 194.0f;
            // High-contrast connector (shadow + bright stroke) so the line stays visible.
            draw->AddLine(rootPosVec2, lineStart, IM_COL32(10, 10, 10, 220), 2.8f);
            draw->AddLine(rootPosVec2, lineStart, IM_COL32(255, 255, 255, 235), 1.6f);
            // Horizontal connector under the bar (same width as HP bar), connected to the diagonal line.
            const ImVec2 monsterLineStart(lineStart.x, lineStart.y + 2.0f);
            const ImVec2 monsterLineEnd(lineStart.x + monsterBarWidth, lineStart.y + 2.0f);
            draw->AddLine(monsterLineStart, monsterLineEnd, IM_COL32(10, 10, 10, 220), 2.6f);
            draw->AddLine(monsterLineStart, monsterLineEnd, IM_COL32(255, 255, 255, 235), 1.4f);
            DrawWideHealthBarWithText(draw, ImVec2(lineStart.x, lineStart.y - 26.0f), monsterBarWidth, 22.0f, m_Hp, m_HpMax);
        }
			
		if (Config.ESP.Monster.Alert) {
            if (m_ID == 2002 && ShouldShowObjectiveAlert(m_ID, m_Hp, m_HpMax, m_bDeath)) {
                DrawObjectiveAlertCard(draw, 2002, m_Hp, m_HpMax, screenWidth, screenHeight);
            }

            if (m_ID == 2003 && ShouldShowObjectiveAlert(m_ID, m_Hp, m_HpMax, m_bDeath)) {
                DrawObjectiveAlertCard(draw, 2003, m_Hp, m_HpMax, screenWidth, screenHeight);
            }
        }
		
        if (Config.ESP.Monster.JungelAttack) {
            if (m_ID == 2004 && m_Hp < m_HpMax) {
                std::string strAlert = "[ALERT] Red Buff is under Attack!";
                auto textSize = ImGui::CalcTextSize(strAlert.c_str(), 0, ((float) glHeight / 31.0f));
                draw->AddText(NULL, ((float) glHeight / 31.0f), {(float)(glWidth / 2) - (textSize.x / 2), (float)(glHeight - glHeight) + (float)(glHeight / 8.7f)}, IM_COL32(255, 255, 255, 255), strAlert.c_str());
            }
            if (m_ID == 2005 && m_Hp < m_HpMax) {
                std::string strAlert = "[ALERT] Blue Buff is under Attack!";
                auto textSize = ImGui::CalcTextSize(strAlert.c_str(), 0, ((float) glHeight / 31.0f));
                draw->AddText(NULL, ((float) glHeight / 31.0f), {(float)(glWidth / 2) - (textSize.x / 2), (float)(glHeight - glHeight) + (float)(glHeight / 8.7f)}, IM_COL32(255, 255, 255, 255), strAlert.c_str());
            }
        }
    }
    /*enemy*/
	
	
	
    auto m_dicPlayerShow = *(Dictionary<int, uintptr_t> **) ((uintptr_t)battleManager + kPlayerDictOffset);
    if (!m_dicPlayerShow) return;
    for (int i = 0; i < m_dicPlayerShow->getNumKeys(); i++) {
        auto keys = m_dicPlayerShow->getKeys()[i];
        auto values = m_dicPlayerShow->getValues()[i];
        if (!keys || !values) continue;
        auto m_ID = *(int *) ((uintptr_t)values + EntityBase_m_ID());
        auto m_bDeath = *(bool *) ((uintptr_t)values + EntityBase_m_bDeath());
        if (m_bDeath) continue;
        auto m_bSameCampType = *(bool *) ((uintptr_t)values + EntityBase_m_bSameCampType());
        if (m_bSameCampType) continue;
        auto m_Hp = *(int *) ((uintptr_t)values + EntityBase_m_Hp());
        auto m_HpMax = *(int *) ((uintptr_t)values + EntityBase_m_HpMax());
        auto sPell = *(int *) ((uintptr_t)values + ShowPlayer_m_iSummonSkillId());
        auto _Position = *(Vector3 *) ((uintptr_t)values + kEntityPosOffset);
        auto rootPosVec2 = getPosVec2(_Position, screenWidth, screenHeight);
        auto spellDrawing = ImVec2(rootPosVec2.x + 55, rootPosVec2.y - 15);
        auto *m_HeroName = *(String **) ((uintptr_t)values + ShowPlayer_m_HeroName());
        int iconHeroId = m_ID;
        if (m_HeroName) {
            int heroIdFromName = HeroNameToId(m_HeroName->toString());
            if (heroIdFromName > 0) {
                iconHeroId = heroIdFromName;
            }
        }
        int Distance = (int) Vector3::Distance(selfPos, _Position);
        auto inBattle = *(bool **) ((uintptr_t)values + ShowPlayer_m_bInBattle);
        auto canSight = *(bool *) ((uintptr_t)values + EntityBase_canSight());

        auto HeadPosVec2 = Vector2(rootPosVec2.x, rootPosVec2.y - (screenHeight / 6.35));
		//auto skillIdData = *(int *) ((uintptr_t)values + SkillComponent_m_iComAtkId);
		
        if (Config.ESP.Player.Alert && get_InTransformation((void *)values)) {
            if (m_ID == 32 && m_ID == 69) {
                draw->AddText(NULL, 30.0f, {screenWidth / 2.3f, screenHeight / 12.0f}, IM_COL32(255, 255, 0, 255), "Johnson and Hanzo Transformed!");
            } else if (m_ID == 32) {
                draw->AddText(NULL, 30.0f, {screenWidth / 2.3f, screenHeight / 12.0f}, IM_COL32(255, 255, 0, 255), "Johnson Becomes A Car!");
            } else if (m_ID == 69) {
                draw->AddText(NULL, 30.0f, {screenWidth / 2.3f, screenHeight / 12.0f}, IM_COL32(255, 255, 0, 255), "Hanzo Becomes A Kuyang!");
            } else if (m_ID == 101) {
                draw->AddText(NULL, 30.0f, {screenWidth / 2.3f, screenHeight / 12.0f}, IM_COL32(255, 255, 0, 255), "Yve in A Ultimate!");
            }
        }
		
		auto skillLearn = *(Dictionary<int, uintptr_t> **) ((uintptr_t)battleManager + SkillComponent_m_OwnSkillDic);
	
        /*
        //if (Config.m_IDConf) {
            std::string mConfigi = to_string(iSkillLearn);
            auto textSize = ImGui::CalcTextSize(mConfigi.c_str(), 0, 20);
            draw->AddText(NULL, 22, {rootPosVec2.x - (textSize.x / 2), rootPosVec2.y + 40}, IM_COL32(255, 255, 255, 255), mConfigi.c_str());
        */

        if (Config.MinimapIcon && AttachIconDone) {
            auto m_EntityCampType = *(int *) ((uintptr_t)values + EntityBase_m_EntityCampType());
            auto minimapPos = WorldToMinimap(m_EntityCampType, _Position);
            DrawIconHero(ImVec2(minimapPos.x, minimapPos.y), iconHeroId, m_Hp, m_HpMax);
        }
		
		if (Config.ESP.Player.Locator2) {
			draw->AddText(NULL, ((float) screenHeight / 31.0f), {rootPosVec2.x, rootPosVec2.y - 115.0f}, IM_COL32(255, 255, 255, 255), ICON_FA_ARROW_DOWN);
            if (m_Hp < m_HpMax) {
				draw->AddText(NULL, ((float) screenHeight / 31.0f), {rootPosVec2.x, rootPosVec2.y - 115.0f}, IM_COL32(255, 3, 3, 255), ICON_FA_ARROW_DOWN);
            }
        }

        //if (!bShowEntityLayer) {
		if (Config.ESP.Player.Line) {
			ImU32 col = IM_COL32((int)(lineColor.x * 255), (int)(lineColor.y * 255), (int)(lineColor.z * 255), (int)(lineColor.w * 255));
			draw->AddLine(selfPosVec2, ImVec2(rootPosVec2.x, rootPosVec2.y), col, 2.0f);
            draw->AddCircleFilled(ImVec2(selfPosVec2.x, selfPosVec2.y), 5, IM_COL32(255, 0, 0, 255), 0);
            draw->AddCircleFilled(ImVec2(rootPosVec2.x, rootPosVec2.y), 7, IM_COL32(255, 0, 0, 255), 0);
            //draw->AddLine(selfPosVec2, rootPosVec2, IM_COL32(230, 230, 250, 255));
		}
            
        auto *m_RoleName = *(String **) ((uintptr_t)values + ShowEntity_m_RoleName);
        if (Config.ESP.Player.Name && m_RoleName) {
			ImU32 col = IM_COL32((int)(textColor.x * 255), (int)(textColor.y * 255), (int)(textColor.z * 255), (int)(textColor.w * 255));
        	std::string strName = m_RoleName->toString();
            auto textSize = ImGui::CalcTextSize(strName.c_str(), 0, 20);
            draw->AddText(NULL, 22, {rootPosVec2.x - (textSize.x / 2), rootPosVec2.y + 40}, col, strName.c_str());
		}

		if (Config.ESP.Player.Health) {
            ImVec2 lineStart(rootPosVec2.x + 36.0f, rootPosVec2.y - 25.0f);
            const float playerBarWidth = 204.0f;
            // High-contrast connector (shadow + bright stroke) for better visibility.
            draw->AddLine(rootPosVec2, lineStart, IM_COL32(10, 10, 10, 220), 2.8f);
            draw->AddLine(rootPosVec2, lineStart, IM_COL32(255, 255, 255, 235), 1.6f);
            // Horizontal connector under the bar (same width as HP bar), connected to the diagonal line.
            const ImVec2 playerLineStart(lineStart.x, lineStart.y + 2.0f);
            const ImVec2 playerLineEnd(lineStart.x + playerBarWidth, lineStart.y + 2.0f);
            draw->AddLine(playerLineStart, playerLineEnd, IM_COL32(10, 10, 10, 220), 2.6f);
            draw->AddLine(playerLineStart, playerLineEnd, IM_COL32(255, 255, 255, 235), 1.4f);
            // Requested tuning: make bar taller (top-bottom) so HP text fits better,
            // and reduce width slightly (left-right) to keep it compact.
            DrawWideHealthBarWithText(draw, ImVec2(lineStart.x, lineStart.y - 28.0f), playerBarWidth, 24.0f, m_Hp, m_HpMax);
		}
	
        if (Config.ESP.Player.Box) {
            float boxHeight = abs(HeadPosVec2.y - rootPosVec2.y);
            float boxWidth = boxHeight * 0.50f;
            ImVec2 vStart = {HeadPosVec2.x - (boxWidth / 2), HeadPosVec2.y};
            ImVec2 vEnd = {vStart.x + boxWidth, vStart.y + boxHeight};
            draw->AddRect(vStart, vEnd, IM_COL32(255, 255, 255, 255), 0, 235, 1.0f);
        }
        
        if (Config.ESP.Player.Round) {
            draw->AddCircleFilled(rootPosVec2, 10, IM_COL32(255, 255, 255, 255));
        }
		// default: rootPosVec2.y + 5
		if (Config.ESP.Player.Distance) {
        	std::string strDistance = to_string(Distance) + " M";
            auto textSize = ImGui::CalcTextSize(strDistance.c_str(), 0, ((float) screenHeight / 39.0f));
            draw->AddText(NULL, ((float) screenHeight / 39.0f), {rootPosVec2.x - (textSize.x / 2), rootPosVec2.y + 15}, IM_COL32(10, 255, 202, 255), strDistance.c_str());
        }

        if (Config.ESP.Player.CoolDown || Config.ESP.Player.CoolDown2) {
            auto coolDownData = getPlayerCoolDown(keys, values);
            const float skillCdRadius = 11.0f;
            const float spellCdRadius = 16.0f; // Spell slot intentionally larger than skill slots.
            const float skillTextSize = 22.0f;
            const float spellTextSize = 26.0f;
            const float cdSpacing = 34.0f;
            const float cdBaseX = rootPosVec2.x + 61.0f;
            const float cdBaseY = rootPosVec2.y - 2.0f; // move all cooldown UI slightly downward
            const float spellIconYOffset = 0.0f;
            const int spellIconSize = static_cast<int>(spellCdRadius * 1.8f); // make spell icon visibly larger than skill slots
            const ImU32 readyCol = IM_COL32(120, 220, 255, 255);

            auto drawCooldownSlot = [&](int index, int cooldown) {
                ImVec2 slotPos(cdBaseX + (cdSpacing * index), cdBaseY);
                if (cooldown > 0) {
                    std::string strCoolDown = to_string(cooldown);
                    auto textSize = ImGui::CalcTextSize(strCoolDown.c_str(), 0, skillTextSize);
                    draw->AddText(NULL, skillTextSize, ImVec2(slotPos.x - (textSize.x / 2), slotPos.y - (skillTextSize * 0.55f)), IM_COL32(255, 255, 255, 255), strCoolDown.c_str());
                } else {
                    draw->AddCircleFilled(slotPos, skillCdRadius, readyCol, 18);
                }
            };

            // Always render 3 skills + 1 spell for enemy cooldown info.
            drawCooldownSlot(0, coolDownData.skill1);
            drawCooldownSlot(1, coolDownData.skill2);
            drawCooldownSlot(2, coolDownData.skill3);

            ImVec2 spellPos(cdBaseX + (cdSpacing * 3), cdBaseY);
            if (coolDownData.spell > 0) {
                std::string strCoolDown = to_string(coolDownData.spell);
                auto textSize = ImGui::CalcTextSize(strCoolDown.c_str(), 0, spellTextSize);
                draw->AddText(NULL, spellTextSize, ImVec2(spellPos.x - (textSize.x / 2), spellPos.y - (spellTextSize * 0.55f)), IM_COL32(255, 255, 255, 255), strCoolDown.c_str());
            } else {
                // For spell ready state: render spell icon only (no blue circle background).
                DrawSpell(ImVec2(spellPos.x, spellPos.y + spellIconYOffset), sPell, spellIconSize);
            }
        }
        
        if (Config.ESP.Player.Alert && isOutsideScreen(ImVec2(rootPosVec2.x, rootPosVec2.y), ImVec2(screenWidth, screenHeight))) {
            ImVec2 hintDotRenderPos = pushToScreenBorder(ImVec2(rootPosVec2.x, rootPosVec2.y), ImVec2(screenWidth, screenHeight), - 50);
            ImVec2 hintTextRenderPos = pushToScreenBorder(ImVec2(rootPosVec2.x, rootPosVec2.y), ImVec2(screenWidth, screenHeight), - 50);
            //DrawCircleHealth(hintDotRenderPos, CurHP, MaxHP, 25);
            DrawLogo(ImGui::GetForegroundDrawList(), hintDotRenderPos, iconHeroId, m_Hp, m_HpMax);
            if(Config.ESP.Player.Distance){
                std::string strDistance = to_string(Distance) + " M";
                auto textSize = ImGui::CalcTextSize(strDistance.c_str(), 0, ((float) screenHeight / 45.0f));
                draw->AddText(NULL, ((float) screenHeight / 45.0f), {hintTextRenderPos.x - (textSize.x / 2), hintTextRenderPos.y + 7}, IM_COL32(255, 255, 255, 255), strDistance.c_str());
            }
            if (m_HeroName) {
                std::string strName = m_HeroName->toString();
                if(Config.ESP.Player.Status) {
                    strName += strName + "[" + CekBattle(inBattle).c_str() + "]";
                }
                auto textSize = ImGui::CalcTextSize(strName.c_str(), 0, ((float) screenHeight / 39.0f));
                draw->AddText(NULL, ((float) screenHeight / 39.0f), {hintTextRenderPos.x - (textSize.x / 2), hintTextRenderPos.y + 30}, IM_COL32(255, 255, 255, 255), strName.c_str());
            }
        }
        
        if (Config.ESP.Player.HeroZ) {
            float heroIconAlpha = canSight ? 0.25f : 1.0f;
            DrawHero(rootPosVec2, iconHeroId, m_Hp, m_HpMax, 55, 3, heroIconAlpha);
        }
        

    }
}
