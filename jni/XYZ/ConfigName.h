#pragma once

#include "ConfigState.h"
#include "Unity/Struct/Vector3.hpp"
#include "../imgui/imgui.h"

#define targetLib "liblogic.so"

enum EThemeTarget {
    None = 0,
    Transparent = 1,
    Blacki = 2,
    Blue = 3,
    Pink = 4
};
static float density = -1;

inline constexpr int ButtonPositionDis = 1500;
inline constexpr int ButtonPositionDis2 = 110;

namespace Esp{
	extern bool SkillsId;
}

namespace Configs {
	extern int EglWidth;
	extern int EglHeight;
    extern int DisplayWidth;
    extern int DisplayHeight;
    extern bool LoginValid;
    extern bool Initialized;
	extern bool ClearMouse;
	extern bool ShowImGui;
};

extern float CostumLagging;

namespace AutoSkills{
	extern bool SoYou;
	extern bool Lucas;
	extern bool KimmyTriggerOnNew;
	extern bool KimmyTrigger;
	extern bool GusionTriggerRecall;
	extern bool GusionTrigger;
	extern bool KimmyCreep;
	extern bool KimmyCreepTrigger;
	extern bool VisibleCheck;
	extern bool KimmyBasicAttack;
	extern bool KimmyBasicAttackTrigger;
	extern bool DyrothBugTest;
	extern bool DyrothBugTrigger;
	extern bool YssBugCall;
	extern bool AluBugCall;
	extern bool BeatrixBugTest;
	extern bool YinBrustDamage;
	extern bool BeatrixBugTrigger;
	extern bool YinBugTrigger;
	extern bool LaylaDamage;
	extern bool LaylaTrigger;
	extern bool AamonDamage;
	extern bool AamonTrigger;
	extern bool NovariaDamage;
	extern bool NovariaTrigger;
	extern bool HanabiDamage;
	extern bool ArgusDamage;
	extern bool FannyCable;
	extern bool HanabiTrigger;
	extern bool GusionDamage;
	extern bool GusionBugTrigger;
	extern bool JoyDoubleDamage;
	extern bool JoyTrigger;
	extern bool JoyDoubleDamage2;
	extern bool JoyTrigger2;
	extern bool JhonsonDmg;
	extern bool JhonsonTrigger;
	extern bool FredrinBug;
	extern bool FredrinTrigger;
	extern bool LesleyBug;
	extern bool LesleyTrigger;
	extern bool MinotaurBug;
	extern bool MinotaurTrigger;
	extern bool EsmeraldaBug;
	extern bool EsmeraldaBug2;
	extern bool EsmeraldaTrigger;
	extern bool PaquitoTrigger;
	extern bool CarmilaBug;
	extern bool AldousBug;
	extern bool KimmyFreeTrigger;
	extern bool PharsaTriggetUlti;
};

struct sAutoRetribution {
    bool RedandBlue;
    bool TurtleandLord;
    bool Crab;
    bool litho;
	float Custom = 7.0f;
};
extern sAutoRetribution AutoRetribution;

struct sColorPick {
    float *Line;
    float *Box;
    float *Name;
    float *Health;
    float *Distance;
    float *Second;
};
extern sColorPick ColorPick;

struct sAutoTrigger {
    bool Retribution;
};
extern sAutoTrigger AutoTrigger;

struct sAutoSkill {
    bool Lesley;
	bool Lucas;
};
extern sAutoSkill AutoSkill;

struct sAim {
    bool SelfPos;
    bool TargetPriority;
    bool AutoTakeSword;
    bool AutoTakeSword2;
    bool ActivePredict;
    struct sPredict {
        bool Skill1;
        bool Skill2;
        bool Skill3;
        bool Skill4;
        bool Basic;
    };
    sPredict Predict {0};
    struct sAutoSkills {
        bool Joy2;
        bool Joy;
        bool ExecuteV2;
        bool LingLung;
        bool Yve;
        bool AutoTakeSword;
        // SKILL 1 //
        bool Skill1Basic;
        bool Skill1Skill2;
        bool Skill1Skill3;
        bool Skill1Skill4;
        // SKILL 2 //
        bool Skill2Basic;
        bool Skill2Skill1;
        bool Skill2Skill3;
        bool Skill2Skill4;
        // SKILL 3 //
        bool Skill3Basic;
        bool Skill3Skill1;
        bool Skill3Skill2;
        bool Skill3Skill4;
        // SKILL 4 //
        bool Skill4Basic;
        bool Skill4Skill1;
        bool Skill4Skill2;
        bool Skill4Skill3;
        // SKILL COMBO //
        bool Yz;
        bool Lylia;
        bool LyliaV2;
        bool WanWan;
        bool KimmyV1;
        bool KimmyV2;
		bool KimmyNewAim;
        bool KimmyV3;
        bool XavierUlt;
        bool Gusion2;
		bool Karina;
		bool PharsaUlti;
		bool Lucas;
		bool SoYou;
    };
    sAutoSkills AutoSkills{0};
    struct sHelper {
        bool iCalculateDamage;
        
        struct sComboSkill {
            bool Balmond;
        };
        sComboSkill ComboSkill{0};
        
        struct sSkills {
            bool Basic;
            bool Spell;
            bool Skill1;
            bool Skill1Testing;
            bool Skill1Test;
            bool Skill2;
            bool Skill3;
            bool Skill4;
            bool BeatrixUlti;
            bool BeatrixSnipper;
            bool Selena;
            bool Kimmy2;
            bool Flameshot;
            bool HookFranco;;
            bool Gusion;
            bool Gusion2;
            bool Gusion3;
            bool KimmyAttack;
            bool FrancoUlt;
            bool Pharsa;
            bool Xavier;
            bool Paquito;
            bool Paquito2;
            bool Eudora;
            bool Yve;
            bool Martis;
            bool Chou;
            bool Badang;
            bool Argus;
            bool Change;
            bool Xavier2;
            bool Execute;
            bool Fanny;
            bool Tigreal;
            bool KimmyV2;
            bool Irithel;
            bool AutoFlameshot;
            bool AutoFlameshotV2;
        };
        sSkills Skills{0};

        struct sPriority {
            int Target = 0;
            bool AutoTakeSword;
        };
        sPriority Priority{0};
        
        struct sAutoSpell {
            bool Executed;
            bool FlameShoot;
            bool Vengeance;
            bool Flicker;
            bool Aegis;
        };
        sAutoSpell AutoSpell{0};
        
        struct sAutoRetribution {
            bool RedandBlue;
            bool Buff1;
            bool Buff2;
            bool TurtleLord;
            bool Turtle;
            bool Lord;
            bool Crab;
            bool BuffV3;
            bool litho;
            bool hasEnemyButton;
            int hasEnemyDistOnly;
            int iCalculateDamage;
            bool Test;
        };
        sAutoRetribution AutoRetribution{0};
        
        
        
    };
    sHelper Helper{0};
    
    Vector3 Dir = Vector3::zero();
    Vector3 Pos = Vector3::One();
    Vector3 NearestEnemyPos = Vector3::zero();
            int NearestEnemyGuid = 0;
            float NearestEnemyDist = 9999;
            bool RetriDistButton;
};
extern sAim Aim;

//'''''MAPHACK INIT'''''//
struct sConfig {
   
    bool m_IDConf;
    bool MinimapIcon;
	bool MinimapIconEnable;
    bool HideLine;
    int MapSize = 341; //width & height
    ImVec2 StartPos = {(float)105, (float)0};
    bool ActiveSkill;
    bool DnsString = true;
    bool Bypass;
    float getFieldOfView;
    float setFieldOfView; 
    struct sInitImGui {
        bool g_Initialized;
        bool clearMousePos = true;
        bool bInitDone;
        uintptr_t thiz;
    };
    sInitImGui InitImGui{0};
    
    struct iThemes{
        bool Enable;
        bool AimBot;
        bool MagicBullet;;
        EThemeTarget Trigger;
        int Target = 0;
    };
    iThemes initThemes{0};
    
    struct sColorsESP {
        float *Line;
        float *Box;
        float *Name;
        float *Health;
        float *Distance;
    };
    sColorsESP ColorsESP{0};
    
    struct sESP {
        bool FPS;
        struct sPlayer {
            bool Line;
            bool RedDot;
            bool Box;
            bool Health;
            bool Name;
            bool Distance;
            bool Hero;
            bool CoolDown;
            bool CoolDown2;
            bool Circle;
            bool Alert;
            bool Damage;
            bool Defense;
            bool Magic;
            bool Nigga;
            bool Fps30;
            bool Fps60;
            bool Fps90;
            bool Fps100;
            bool Fps120;
            bool Visible;
            bool Antilag;
            bool CircleHealth;
            bool ShowPlayer;
            bool Health2;
            bool ShowMonster;
            bool ShowMonsterV2;
            bool ShowPlayerV2;
            bool SecondEsp;
            bool Round;
            bool HeroZ;
            bool Status;
			bool Locator2;
        };
        sPlayer Player{0};

        struct sMonster {
            bool Box;
            bool Health;
            bool Name;
            bool Alert;
            bool Locator;
            bool JungelAttack;
            bool ShowMonster;
            bool Minion;
            bool Rounded;
			bool Locator2;
        };
        sMonster Monster{0};
    };
    sESP ESP{0};

    struct sVisual {
		bool UnlockHero;
        bool UnlockSkin;
        bool UnlockEmblem;
        bool UnlockSkinV2;
        bool UnlockStarlight;
        bool MaphackIcon;
        bool MapHackIcon2;
		bool MapHackIcon3;
        bool HealthBar;
        bool ShowMonster;
        bool IconMap;
        bool FrameRate;
        bool UnlockSpell;
        bool Icon;
        bool RemoveGrass;
        bool ShowMonsterMap;
        bool HeadIcon;
        bool MonsterIcon;
        bool MonsterHealth;
        bool GrassEye;
        bool MonsterHeadIcon;
        bool MonsterBody;
        bool ShowJungle;
        bool ShowMonsterTest;
		bool NoGrass;
    };
    sVisual Visual{0};

    bool MinimapMonsterIcon;
};
extern sConfig Config;

enum ETargetPriority {
    Dist = 0,
    LowHP = 1,
    LowHPPercent = 2,
};

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 55.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
