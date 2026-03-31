#include "ConfigName.h"

namespace ConfigState {
namespace {
EspDomainState gEspState{};
VisualDomainState gVisualState{};
AimDomainState gAimState{};
CameraDomainState gCameraState{};
bool gPerformanceMode = false;
} // namespace

EspDomainState &GetEspState() { return gEspState; }
VisualDomainState &GetVisualState() { return gVisualState; }
AimDomainState &GetAimState() { return gAimState; }
CameraDomainState &GetCameraState() { return gCameraState; }
bool &GetPerformanceMode() { return gPerformanceMode; }
} // namespace ConfigState

namespace Esp {
bool SkillsId = false;
}

namespace Configs {
int EglWidth = 0;
int EglHeight = 0;
int DisplayWidth = 0;
int DisplayHeight = 0;
bool LoginValid = false;
bool Initialized = false;
bool ClearMouse = true;
bool ShowImGui = true;
}

float CostumLagging = 15;

namespace AutoSkills {
bool SoYou = false;
bool Lucas = false;
bool KimmyTriggerOnNew = false;
bool KimmyTrigger = false;
bool GusionTriggerRecall = false;
bool GusionTrigger = false;
bool KimmyCreep = false;
bool KimmyCreepTrigger = false;
bool VisibleCheck = false;
bool KimmyBasicAttack = false;
bool KimmyBasicAttackTrigger = false;
bool DyrothBugTest = false;
bool DyrothBugTrigger = false;
bool YssBugCall = false;
bool AluBugCall = false;
bool BeatrixBugTest = false;
bool YinBrustDamage = false;
bool BeatrixBugTrigger = false;
bool YinBugTrigger = false;
bool LaylaDamage = false;
bool LaylaTrigger = false;
bool AamonDamage = false;
bool AamonTrigger = false;
bool NovariaDamage = false;
bool NovariaTrigger = false;
bool HanabiDamage = false;
bool ArgusDamage = false;
bool FannyCable = false;
bool HanabiTrigger = false;
bool GusionDamage = false;
bool GusionBugTrigger = false;
bool JoyDoubleDamage = false;
bool JoyTrigger = false;
bool JoyDoubleDamage2 = false;
bool JoyTrigger2 = false;
bool JhonsonDmg = false;
bool JhonsonTrigger = false;
bool FredrinBug = false;
bool FredrinTrigger = false;
bool LesleyBug = false;
bool LesleyTrigger = false;
bool MinotaurBug = false;
bool MinotaurTrigger = false;
bool EsmeraldaBug = false;
bool EsmeraldaBug2 = false;
bool EsmeraldaTrigger = false;
bool PaquitoTrigger = false;
bool CarmilaBug = false;
bool AldousBug = false;
bool KimmyFreeTrigger = false;
bool PharsaTriggetUlti = false;
}

sAutoRetribution AutoRetribution{0};
sColorPick ColorPick{0};
sAutoTrigger AutoTrigger{0};
sAutoSkill AutoSkill{0};
sAim Aim{0};
sConfig Config{0};
