#pragma once

std::string strHero[] = {
    "-",
    "Miya",
    "Balmond",
    "Saber",
    "Alice",
    "Nana",
    "Tigreal",
    "Alucard",
    "Karina",
    "Akai",
    "Franco",
    "Bane",
    "Bruno",
    "Clint",
    "Rafaela",
    "Eudora",
    "Zilong",
    "Fanny",
    "Layla",
    "Minotaur",
    "Lolita",
    "Hayabusa",
    "Freya",
    "Gord",
    "Natalia",
    "Kagura",
    "Chou",
    "Sun",
    "Alpha",
    "Ruby",
    "Yi Sun-shin",
    "Moskov",
    "Johnson",
    "Cyclops",
    "Estes",
    "Hilda",
    "Aurora",
    "Lapu-Lapu",
    "Vexana",
    "Roger",
    "Karrie",
    "Gatotkaca",
    "Harley",
    "Irithel",
    "Grock",
    "Argus",
    "Odette",
    "Lancelot",
    "Diggie",
    "Hylos",
    "Zhask",
    "Helcurt",
    "Pharsa",
    "Lesley",
    "Jawhead",
    "Angela",
    "Gusion",
    "Valir",
    "Martis",
    "Uranus",
    "Hanabi",
    "Chang'e",
    "Kaja",
    "Selena",
    "Aldous",
    "Claude",
    "Vale",
    "Leomord",
    "Lunox",
    "Hanzo",
    "Belerick",
    "Kimmy",
    "Thamuz",
    "Harith",
    "Minsitthar",
    "Kadita",
    "Faramis",
    "Badang",
    "Khufra",
    "Granger",
    "Guinevere",
    "Esmeralda",
    "Terizla",
    "X.Borg",
    "Ling",
    "Dyrroth",
    "Lylia",
    "Baxia",
    "Masha",
    "Wanwan",
    "Silvanna",
    "Cecilion",
    "Carmilla",
    "Atlas",
    "Popol & Kupa",
    "Yu Zhong",
    "Luo Yi",
    "Benedetta",
    "Khaleed",
    "Barats",
    "Brody",
    "Yve",
    "Mathilda",
    "Paquito",
    "Gloo",
    "Beatrix",
    "Phoveus",
    "Natan",
    "Aulus",
    "Aamon",
    "Valentina",
    "Edith",
    "Floryn",
    "Yin",
    "Melissa",
    "Xavier",
    "Julian",
    "Fredrinn",
    "Joy",
    "Novaria",
    "Arlot",
    "Ixia",
    "Nolan"
};

std::string HeroToString(int heroid) {
	if (strHero[heroid] != "") {
		return strHero[heroid];
	} else {
		return "Hero ID: " + to_string(heroid); 
	}
}

int HeroNameToId(const std::string &heroName) {
    for (int i = 1; i < (int)(sizeof(strHero) / sizeof(strHero[0])); ++i) {
        if (strHero[i] == heroName) {
            return i;
        }
    }
    return -1;
}

std::string SpellToString(int summonSkillId) {
    std::string strSpell;
    switch(summonSkillId) {
	case 0:
		strSpell += "-";
        break;
    case 20150:
        strSpell += "Execute";
        break;
	case 20020:
        strSpell += "Retribution";
        break;
	case 20030:
        strSpell += "Inspire";
        break;
	case 20040:
        strSpell += "Sprint";
        break;
	case 20050:
        strSpell += "Revitalize";
        break;
	case 20060:
        strSpell += "Aegis";
        break;
	case 20070:
        strSpell += "Petrify";
        break;
	case 20080:
        strSpell += "Purify";
        break;
	case 20140:
        strSpell += "Flameshot";
        break;
	case 20100:
        strSpell += "Flicker";
        break;
	case 20160:
        strSpell += "Arrival";
        break;
	case 20190:
        strSpell += "Vengeance";
        break;
	default:
	strSpell += to_string(summonSkillId);
	}
	return strSpell;
}

std::string MonsterToString(int m_id) {
    std::string strMonster;
    switch(m_id) {
	case 2002:
        strMonster += "Lord";
        break;
	case 2003:
        strMonster += "Turtle";
        break;
	case 2004:
        strMonster += "Fiend";
        break;
    case 2005:
        strMonster += "Serpent";
        break;
	case 2006:
        strMonster += "Scaled Lizard";
        break;
	case 2008:
        strMonster += "Crammer";
        break;
	case 2009:
        strMonster += "Rockursa";
        break;
	case 2011:
		strMonster += "Crab";
		break;
    case 2012:
        strMonster += "Serpent Kids";
        break;
    case 2013:
        strMonster += "Crab";
        break;
    case 2056:
        strMonster += "Lithowanderer";
        break;
	case 2059:
		strMonster += "Crammer";
		break;
	case 2072:
		strMonster += "Lithowanderer";
		break;
	case 2110:
		strMonster += "Turtle";
    default:
        strMonster += /*to_string(m_id)*/"";
    }
    return strMonster;
}

int ListSummonSkillId[] = {
    20150,
    20020,
    20030,
    20040,
    20050,
    20060,
    20070,
    20080,
    20140,
    20100,
    20160,
    20190
};

int ListMonsterId[] = {
        2002,
        2003,
        2004,
        2005,
        2006,
        2008,
        2009,
        2011,
        2012,
        2013,
        2056,
        2059,
        2072,
};

bool SpellIdExist(int iValue) {
	return std::find(std::begin(ListSummonSkillId), std::end(ListSummonSkillId), iValue) != std::end(ListSummonSkillId);
}

bool iRobotBukan;
