#pragma once

namespace ConfigState {
struct VisualDomainState {
    bool activeVipSkill = false;
    bool setGrassLayerHook = false;
    bool eyeRangeHook = false;
    bool kimmyAttackJungle = false;
};

VisualDomainState &GetVisualState();
}
