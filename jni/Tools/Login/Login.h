#pragma once
#include "curl/curl.h"
#include "Tools.h"
#include "json.hpp"

using json = nlohmann::ordered_json;

std::string title, version;
std::string UUID;

static std::string slotZ = "";
static std::string inVip = "";
static std::string autoskillsZ = "";
static std::string expired = "";
static std::string EXP = "LOGIN FIRST";
static std::string name ="";
static std::string device = "";
static std::string status = "";
static std::string floating ="";
static std::string battleData = "true";
static std::string clientManager = "";

std::string g_Token, g_Auth;
bool bValid = false;

bool bInitDone;

namespace SignIN
{
    static bool user;
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;
    mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

std::string Login(JavaVM *jvm, const char *user_key, bool *success) {
    (void)jvm;
    (void)user_key;

    if (success) *success = true;

    inVip = "100";
    battleData = "true";
    clientManager = "NO LOGIN";
    expired = "UNLIMITED";
    slotZ = "UNLIMITED";

    g_Token = "LOCAL_BYPASS";
    g_Auth = "LOCAL_BYPASS";

    return "";
}

static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

bool download_file(std::string url,std::string path){
	curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl) {   
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
    
    FILE *file = fopen(path.c_str(), "wb");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
    CURLcode result = curl_easy_perform(curl);
    
    fclose(file);
    curl_easy_cleanup(curl);

    if (result == CURLE_OK) {
      return true;
    } else {
      
        return false;
    }
    
    return false;
  }

  return false;
  curl_global_cleanup();
}





