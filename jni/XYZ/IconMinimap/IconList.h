#include "ICON.h"
#include "SPELL.h"
#include "RANK.h"
#include "Flagsicon.h"
#include "MonsterIcon.h"
#include "AlertIcon.h"

struct Icon {
    GLuint texture = 0;
    int width = 0;
    int height = 0;
    bool IsValid = false;
};

std::vector<Icon> CountryIcon;
std::vector<Icon> MonsterIcon;
std::vector<Icon> MonsterAlert;
std::vector<Icon> HeroIcon;
std::vector<Icon> SpellIcon;
std::vector<Icon> RankIcon;

Icon ICTexture(int Id) {
    if (HeroIcon.empty()) {
        return {};
    }

    auto fallbackHero = [&]() -> Icon {
        int defaultIndex = (HeroIcon.size() > 18) ? 18 : (int)HeroIcon.size() - 1;
        if (defaultIndex >= 0 && HeroIcon[defaultIndex].IsValid) {
            return HeroIcon[defaultIndex];
        }
        for (const auto &icon : HeroIcon) {
            if (icon.IsValid) return icon;
        }
        return {};
    };

    int resolvedId = Id;
    switch(Id) {
	case 9995:
		resolvedId = 18;
        break;
	case 9996:
		resolvedId = 16;
        break;
	case 9997:
		resolvedId = 6;
        break;
	case 9998:
		resolvedId = 16;
        break;
	case 9999:
		resolvedId = 18;
        break;
	default:
        resolvedId = Id;
	}

    if (resolvedId < 0 || resolvedId >= (int)HeroIcon.size()) {
        return fallbackHero();
    }

    if (!HeroIcon[resolvedId].IsValid) {
        return fallbackHero();
    }

	return HeroIcon[resolvedId];
}

Icon MonsterTexture(int Id) {
    if (MonsterIcon.empty()) {
        return {};
    }

    auto fallbackMonster = [&]() -> Icon {
        for (const auto &icon : MonsterIcon) {
            if (icon.IsValid) return icon;
        }
        return {};
    };

    int resolvedId = Id;
    switch(Id) {
	case 2002:
		resolvedId = 1;
        break;
	case 2003:
		resolvedId = 2;
        break;
	case 2004:
		resolvedId = 3;
        break;
	case 2005:
		resolvedId = 4;
        break;
	case 2006:
		resolvedId = 5;
        break;
	case 2008:
		resolvedId = 6;
        break;
	case 2009:
		resolvedId = 7;
        break;    
	case 2011:
		resolvedId = 8;
        break;
	case 2012:
		resolvedId = 9;
        break;    
	case 2013:
		resolvedId = 10;
        break;
	case 2056:
		resolvedId = 11;
        break;
	case 2059:
		resolvedId = 12;
        break;
	default:
	    resolvedId = Id;
	}
    if (resolvedId < 0 || resolvedId >= (int)MonsterIcon.size()) {
        return fallbackMonster();
    }
	return MonsterIcon[resolvedId].IsValid ? MonsterIcon[resolvedId] : fallbackMonster();
}

Icon MonsterAlertTexture(int Id) {
    if (MonsterAlert.empty()) {
        return {};
    }

    auto fallbackAlert = [&]() -> Icon {
        for (const auto &icon : MonsterAlert) {
            if (icon.IsValid) return icon;
        }
        return {};
    };

    int resolvedId = 1;
    switch (Id) {
    case 2002: // turtle
        resolvedId = 1;
        break;
    case 2003: // lord
        // Some builds only ship one alert icon in AlertIcon.h.
        resolvedId = (MonsterAlert.size() > 2) ? 2 : 1;
        break;
    default:
        resolvedId = Id;
        break;
    }

    if (resolvedId < 0 || resolvedId >= (int)MonsterAlert.size()) {
        return fallbackAlert();
    }

    return MonsterAlert[resolvedId].IsValid ? MonsterAlert[resolvedId] : fallbackAlert();
}

Icon SpellTexture(int Id) {
    switch(Id) {
	case 20060:
		return SpellIcon[1];
        break;
	case 20160:
		return SpellIcon[2];
        break;
	case 20150:
		return SpellIcon[3];
        break;
	case 20140:
		return SpellIcon[4];
        break;
	case 20100:
		return SpellIcon[5];
        break;
	case 20030:
		return SpellIcon[6];
        break;
	case 20070:
		return SpellIcon[7];
        break;    
	case 20080:
		return SpellIcon[8];
        break;
	case 20020:
		return SpellIcon[9];
        break;    
	case 20050:
		return SpellIcon[10];
        break;
	case 20040:
		return SpellIcon[11];
        break;
	case 20190:
		return SpellIcon[12];
        break;
	default:
	return SpellIcon[Id];
	}
	return SpellIcon[Id];
}

Icon RankTexture(const int uiRankLevel) {
    if (uiRankLevel <= 136) {
        if (uiRankLevel <= 11) {
            return RankIcon[1];
        }
        if (uiRankLevel < 27) {
            return RankIcon[2];
        }
        if (uiRankLevel < 47) {
            return RankIcon[3];
        }
        if (uiRankLevel < 77) {
            return RankIcon[4];
        }
        if (uiRankLevel < 107) {
            return RankIcon[5];
        }
        return RankIcon[6];
    } else {
        int Star = uiRankLevel - 136;
        if (Star <= 24) {
            return RankIcon[7];
        }
        if (Star <= 49) {
            return RankIcon[8];
        }
        if (Star <= 99) {
            return RankIcon[9];
        }
        return RankIcon[10];
    }
      return RankIcon[0]; 
}
