#pragma once

namespace ConfigState {
struct AimDomainState {
    bool buttonComboGusion = false;
    bool toggleRetri = false;
    int speedInt = 21;
    bool customTrigger = false;
    bool activeRetribution = false;
    bool activeAutoSkill = false;
};

struct CameraDomainState {
    float setFieldOfView = 0.0f;
    float getFieldOfView = 0.0f;
    float sliderValue = 0.0f;
    float rangeCombo = 10.0f;
    float rangePredict = 13.0f;
    float targetHP = 1750.0f;
    float rangeFOV = 10.0f;
    float speedAtt = 1.0f;
    float targetHPPer = 40.0f;
    float speedPredict = 1.0f;
};

AimDomainState &GetAimState();
CameraDomainState &GetCameraState();
bool &GetPerformanceMode();
}
