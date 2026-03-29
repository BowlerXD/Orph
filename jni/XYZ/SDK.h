#pragma once 

std::map<std::string, uintptr_t> Fields;
std::map<std::string, uintptr_t> Methods;

inline bool ShouldLogOffsetFailure(uint32_t &counter) {
    ++counter;
    return counter == 1 || (counter % 300) == 0;
}

inline uintptr_t CachedFieldOffset(const char *image, const char *namespaze, const char *klass, const char *field, uint32_t &failCounter) {
    uintptr_t &cached = Fields[field];
    if (!cached) {
        cached = (uintptr_t) Il2CppGetFieldOffset(image, namespaze, klass, field);
        if (!cached && ShouldLogOffsetFailure(failCounter)) {
            LOGW("[OffsetCache] Failed field offset %s::%s::%s.%s (attempt=%u)", image, namespaze, klass, field, failCounter);
        }
    }
    return cached;
}

inline uintptr_t CachedMethodOffset(const char *image, const char *namespaze, const char *klass, const char *method, uint32_t &failCounter, int argsCount = -1) {
    std::string key = std::string(method) + "#" + std::to_string(argsCount);
    uintptr_t &cached = Methods[key];
    if (!cached) {
        cached = (argsCount >= 0)
                 ? (uintptr_t) Il2CppGetMethodOffset(image, namespaze, klass, method, argsCount)
                 : (uintptr_t) Il2CppGetMethodOffset(image, namespaze, klass, method);
        if (!cached && ShouldLogOffsetFailure(failCounter)) {
            LOGW("[OffsetCache] Failed method offset %s::%s::%s.%s (args=%d attempt=%u)", image, namespaze, klass, method, argsCount, failCounter);
        }
    }
    return cached;
}

class ShowEntityBase {
    public:
    
    bool m_bDeath() {
        static uint32_t failCounter = 0;
        uintptr_t offset = CachedFieldOffset("Assembly-CSharp.dll", "Battle", "EntityBase", "<m_bDeath>k__BackingField", failCounter);
        if (!offset) return false;
        return *(bool *) ((uintptr_t) this + offset);
    }

    bool m_bSameCampType() {
        static uint32_t failCounter = 0;
        uintptr_t offset = CachedFieldOffset("Assembly-CSharp.dll", "Battle", "EntityBase", "m_bSameCampType", failCounter);
        if (!offset) return false;
        return *(bool *) ((uintptr_t) this + offset);
    }
    
    int m_Hp() {
        static uint32_t failCounter = 0;
        uintptr_t offset = CachedFieldOffset("Assembly-CSharp.dll", "Battle", "EntityBase", "m_Hp", failCounter);
        if (!offset) return 0;
        return *(int *) ((uintptr_t) this + offset);
    }
    
    int m_HpMax() {
        static uint32_t failCounter = 0;
        uintptr_t offset = CachedFieldOffset("Assembly-CSharp.dll", "Battle", "EntityBase", "m_HpMax", failCounter);
        if (!offset) return 0;
        return *(int *) ((uintptr_t) this + offset);
    }
};

class ShowEntity : public ShowEntityBase {
    public:
    
    String *m_RoleName() {
        return *(String **) ((uintptr_t) this + Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_RoleName"));
    }
};

Vector3 AimEnemy,My;

static void *GetPlayerSelf(void *thiz){
    static uint32_t failCounter = 0;
    uintptr_t method = CachedMethodOffset("Assembly-CSharp.dll", "", "LogicBattleManager", "GetPlayerRealSelf", failCounter);
    if (!method) return nullptr;
    auto *(*LogicBattleManagerZ_GetPlayerRealSelf)(void *nul) = (void *(*)(void *))(method);
    return LogicBattleManagerZ_GetPlayerRealSelf(thiz);
}

static Vector3 Player_position(void *thiz) {
    static uint32_t failCounter = 0;
    uintptr_t method = CachedMethodOffset("Assembly-CSharp.dll", "Battle", "LogicFighter", "get_Position", failCounter);
    if (!method) return Vector3::zero();
    auto (*LogicFighter_get_Position)(void *players) = (Vector3 (*)(void *))(method);
    return LogicFighter_get_Position(thiz);
}

static int GetCampType(void *thiz,void *entity,bool benemy){
    static uint32_t failCounter = 0;
    uintptr_t method = CachedMethodOffset("Assembly-CSharp.dll", "", "LogicBattleManager", "GetCampType", failCounter, 2);
    if (!method) return 0;
    auto (*LogicBattleManager_GetCampType)(void *,void*,bool) = (uintptr_t (*)(void *,void*,bool))(method);
    return LogicBattleManager_GetCampType(thiz,entity,benemy);
}

static int Player_GetTotalHP(void *thiz){
    auto (*LogicFighter_GetTotalHp)(void*_thiz)= (uintptr_t(*)(void*))(LogicFighter__GetTotalHp);
    return LogicFighter_GetTotalHp(thiz);
}
