#pragma once

namespace ConfigState {
struct EspDomainState {
    bool openLinkUrl = false;
    bool activeEsp = false;
    bool updateManiacStatus = false;
};

EspDomainState &GetEspState();
}
