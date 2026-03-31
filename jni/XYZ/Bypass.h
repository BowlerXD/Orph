void (*oAntiCheatReporter_StartBattle)(void *);
void iAntiCheatReporter_StartBattle(void *thiz){
    if (oAntiCheatReporter_StartBattle) {
        oAntiCheatReporter_StartBattle(thiz);
    }
}

void (*oAntiCheatReporter_OnReleaseUseSkill)(...);
void iAntiCheatReporter_OnReleaseUseSkill(void *thiz, int skillId){
    if (oAntiCheatReporter_OnReleaseUseSkill) {
        oAntiCheatReporter_OnReleaseUseSkill(thiz, skillId);
    }
}

void (*oAntiCheatReporter_OnTryUseSkill)(void *, int, Vector3);
void *iAntiCheatReporter_OnTryUseSkill(void *thiz, int skillId, Vector3 dir){
    if (oAntiCheatReporter_OnTryUseSkill) {
        oAntiCheatReporter_OnTryUseSkill(thiz, skillId, dir);
    }
    return 0LL;
}

void (*oAntiCheatReporter_OnTryUseSkill2)(void *, int, int, Vector3);
void iAntiCheatReporter_OnTryUseSkill2(void *thiz, int initSkill, int skillId, Vector3 dir){
    if (oAntiCheatReporter_OnTryUseSkill2) {
        oAntiCheatReporter_OnTryUseSkill2(thiz, initSkill, skillId, dir);
    }
}

void (*oTestCase_gp_start_anti_cheat)(void *);
void iTestCase_gp_start_anti_cheat(void *thiz){
    if (oTestCase_gp_start_anti_cheat) {
        oTestCase_gp_start_anti_cheat(thiz);
    }
}

float (*oDeviceUtil_GetRefreshRate)(void *);
float iDeviceUtil_GetRefreshRate(void* thiz){
    if (thiz != nullptr){
        return 120;
    }
    return oDeviceUtil_GetRefreshRate(thiz);
}

bool (*oDeviceUtil_GetIsRoot)(void *);
bool iDeviceUtil_GetIsRoot(void *thiz){
    return false;
}

void (*oDeviceUtil_OpenURL)(void *thiz, string);
void iDeviceUtil_OpenURL(void *thiz, string url){
	if (!oDeviceUtil_OpenURL || thiz == NULL) return;
	if (ConfigState::GetEspState().openLinkUrl) {
    	oDeviceUtil_OpenURL(thiz, "https://xyourzone.my.id/datZ/");
        return;
    }
    oDeviceUtil_OpenURL(thiz, url);
}

void (*oAntiCheatReporter_OnRequestSkillMsg)(void *, int, Vector3);
void iAntiCheatReporter_OnRequestSkillMsg(void *thiz, int skillId, Vector3 dir){
    if (oAntiCheatReporter_OnRequestSkillMsg) {
        oAntiCheatReporter_OnRequestSkillMsg(thiz, skillId, dir);
    }
}

bool (*oAntiCheatReporter_HasSkillInfo)(void *);
bool iAntiCheatReporter_HasSkillInfo(void *thiz){
    if (oAntiCheatReporter_HasSkillInfo) {
        return oAntiCheatReporter_HasSkillInfo(thiz);
    }
    return false;
}

bool (*oAPKSignature_IsSignatureSame)(void *);
bool iAPKSignature_IsSignatureSame(void *thiz, void* out){
    return true;
}

void (*oBattleBridge_OnSignReport)(void *thiz, uint uGuid, Vector3 pos, bool bFloor, int orderType, uint uTargetGuid, float fTime, uint uFriendGuid, bool floorEffect);
void *iBattleBridge_OnSignReport(void *thiz, uint uGuid, Vector3 pos, bool bFloor, int orderType, uint uTargetGuid, float fTime, uint uFriendGuid, bool floorEffect){
    if (oBattleBridge_OnSignReport) {
        oBattleBridge_OnSignReport(thiz, uGuid, pos, bFloor, orderType, uTargetGuid, fTime, uFriendGuid, floorEffect);
    }
    return 0;
}

void (*oBattleBridge_SafeSendGameReportData)(void *thiz, string*);
void *iBattleBridge_SafeSendGameReportData(void* thiz, string* uy){
    if (oBattleBridge_SafeSendGameReportData) {
        oBattleBridge_SafeSendGameReportData(thiz, uy);
    }
    return 0;
}

void (*oBattleBridge_SendGameReportData)(...);
void *iBattleBridge_SendGameReportData(void* thiz, string* uy){
    if (oBattleBridge_SendGameReportData) {
        oBattleBridge_SendGameReportData(thiz, uy);
    }
    return 0;
}

void (*oBattleBridge_SendLargeGameReportData)(...);
void *iBattleBridge_SendLargeGameReportData(void* thiz, string* uy){
    if (oBattleBridge_SendLargeGameReportData) {
        oBattleBridge_SendLargeGameReportData(thiz, uy);
    }
    return 0;
}

void (*oAntiCheatReporter_ctor)(...);
void *iAntiCheatReporter_ctor(void *thiz){
    if (oAntiCheatReporter_ctor) {
        oAntiCheatReporter_ctor(thiz);
    }
	return 0;
}

void (*oAntiCheatReporter_cctor)(...);
void *iAntiCheatReporter_cctor(void *thiz){
    if (oAntiCheatReporter_cctor) {
        oAntiCheatReporter_cctor(thiz);
    }
	return 0;
}

void (*oCheat_Start)(...);
void *iCheat_Start(void *thiz){
	return 0;
}

void (*oCheat_Apply)(...);
void *iCheat_Apply(void *thiz){
	return 0;
}

void (*oLuaHelper_SaveActivityClientInfo)(...);
void *iLuaHelper_SaveActivityClientInfo(void *thiz, uint actvityType, uint activityId, string strData){
	return 0;
}

void (*oACInterface_GetTimeOfDay)(...);
void *iACInterface_GetTimeOfDay(string so_name, string image_name, int start_addr, int data_size){
	return 0;
}

void (*oACInterface_GetSystemTime)(...);
void *iACInterface_GetSystemTime(string so_name, string image_name){
	return 0;
}

void (*oACInterface_LoadCert)(...);
void *iACInterface_LoadCert(string streamPath, string val){
	return 0;
}

void (*oACInterface_SignErrorVerify)(...);
void *iACInterface_SignErrorVerify(...){
	return 0;
}

void (*oACInterface_GetSign)(...);
void *iACInterface_GetSign(...){
	return 0;
}

void (*oACInterface_CheckHook)(void *instance);
void *iACInterface_CheckHook(void*instance){
	return 0;
}

void (*oGameReportMgr_Report_client_match_box)(...);
void *iGameReportMgr_Report_client_match_box(...){
    if (oGameReportMgr_Report_client_match_box) {
        oGameReportMgr_Report_client_match_box();
    }
	return 0LL;
}

void (*oGameReportMgr_Report_MemoryHack)(...);
void *iGameReportMgr_Report_MemoryHack(...){
    if (oGameReportMgr_Report_MemoryHack) {
        oGameReportMgr_Report_MemoryHack();
    }
	return 0;
}

void (*oSleepingFunctionHook)(...);
void *iSleepingFunctionHook(...){
	return 0;
}

bool BypassDNS = false;

bool (*oDns_cctor)(void *unk);
bool Dns_cctor(void *unk)
{
    if (BypassDNS) {
        return true;
    }
    if (!oDns_cctor) return false;
    return oDns_cctor(unk);
}

bool (*oDns_GetHostByName_internal)(void *unk, string host, string h_name, string h_aliases, string h_addr_list);
bool Dns_GetHostByName_internal(void *unk, string host, string h_name, string h_aliases, string h_addr_list) {
    if (!oDns_GetHostByName_internal) return false;
    if (BypassDNS) {
    return oDns_GetHostByName_internal(unk, "ip.ml.youngjoygame.com:30220", "ip.ml.youngjoygame.com", "ip.ml.youngjoygame.com", "ip.ml.youngjoygame.com");
    }
    return oDns_GetHostByName_internal(unk, host, h_name, h_aliases, h_addr_list);
}    

bool (*oDns_GetHostByAddr_internal)(void * unk, string host, string h_name, string h_aliases, string h_addr_list);
bool Dns_GetHostByAddr_internal(void * unk, string host, string h_name, string h_aliases, string h_addr_list) {
    if (!oDns_GetHostByAddr_internal) return false;
    if (BypassDNS) {
    return oDns_GetHostByAddr_internal(unk, "ip.ml.youngjoygame.com:30220", "ip.ml.youngjoygame.com", "ip.ml.youngjoygame.com", "ip.ml.youngjoygame.com");
    }
    return oDns_GetHostByAddr_internal(unk, host, h_name, h_aliases, h_addr_list);
}    

bool (*oDns_hostent_to_IPHostEntry)(void * unk, string h_name, string h_aliases, string h_addrlist);
bool Dns_hostent_to_IPHostEntry(void * unk, string h_name, string h_aliases, string h_addrlist) {
    if (!oDns_hostent_to_IPHostEntry) return false;
    if (BypassDNS) {
    return oDns_hostent_to_IPHostEntry(unk, "ip.ml.youngjoygame.com", "ip.ml.youngjoygame.com", "ip.ml.youngjoygame.com");
    }
    return oDns_hostent_to_IPHostEntry(unk, h_name, h_aliases, h_addrlist);
}    

bool (*oDns_GetHostByAddressFromString)(void * unk, string address);
bool Dns_GetHostByAddressFromString(void * unk, string address) {
    if (!oDns_GetHostByAddressFromString) return false;
    if (BypassDNS) {
    return oDns_GetHostByAddressFromString(unk, "127.0.0.1");
    }
    return oDns_GetHostByAddressFromString(unk, address);
}    
   
bool (*oDns_GetHostEntry)(void * unk, string hostNameOrAddress);
bool Dns_GetHostEntry(void * unk, string hostNameOrAddress) {
    if (!oDns_GetHostEntry) return false;
    if (BypassDNS) {
    return oDns_GetHostEntry(unk, "ip.ml.youngjoygame.com:30220");
    }
    return oDns_GetHostEntry(unk, hostNameOrAddress);
}    

bool (*oDns_GetHostAddresses)(void * unk, string hostNameOrAddress);
bool Dns_GetHostAddresses(void * unk, string hostNameOrAddress) {
    if (!oDns_GetHostAddresses) return false;
    if (BypassDNS) {
    return oDns_GetHostAddresses(unk, "ip.ml.youngjoygame.com:30220");
    }
    return oDns_GetHostAddresses(unk, hostNameOrAddress);
}    
        
bool (*oDns_GetHostByName)(void * unk, string hostName);
bool Dns_GetHostByName(void * unk, string hostName) {
    if (!oDns_GetHostByName) return false;
	if (unk != nullptr){
		if (BypassDNS) return oDns_GetHostByName(unk, "ip.ml.youngjoygame.com:30220");
	}
    return oDns_GetHostByName(unk, hostName);
}

void (*oDisposeServerObject)() = 0;
void DisposeServerObject(void* unk) {
    if (oDisposeServerObject) {
        oDisposeServerObject();
    }
}

// Hook for BattleEndReport::ReportShowEntityReportData
void (*old_ReportShowEntityReportData)(void *instance);
void ReportShowEntityReportData(void *instance) {
    if (instance != NULL) {
        // Modify some data in the instance if needed
        *(int *)((uint64_t)instance + 0x48) = 0;
    }
    if (!old_ReportShowEntityReportData) return;
    return old_ReportShowEntityReportData(instance);
}

// Hook for XmlTextReaderImpl::set_DisableUndeclaredEntityCheck
void (*old_set_DisableUndeclaredEntityCheck)(void *instance, bool value);
void set_DisableUndeclaredEntityCheck(void *instance, bool value) {
    // Example: Force the check to always be enabled
    value = false;
    if (!old_set_DisableUndeclaredEntityCheck) return;
    return old_set_DisableUndeclaredEntityCheck(instance, value);
}


void (*oSetAntiCheatReport)(void *instance, string r);
void iSetAntiCheatReport(void *instance, string r){
	if (oSetAntiCheatReport) {
        oSetAntiCheatReport(instance, r);
    }
}
