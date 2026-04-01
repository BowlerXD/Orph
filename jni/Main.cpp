#include <jni.h>

#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

#include <fstream>
#include <string>
#include <vector>

#include "Tools/Il2Cpp.h"
#include "Tools/Tools.h"
#include "Hooks.h"
#include "Includes/Logger.h"

extern uintptr_t m_IL2CPP;
extern JavaVM *g_vm;
extern void UpdateMapHack(void *pThis);
extern void (*oUpdateMapHack)(void *pThis);
JNIEnv *GetJNIEnv(JavaVM *vm);
std::string getPackageName(JNIEnv *env);
bool CopyFile(const char *in, const char *out);
bool bFullChecked = false;

namespace {
#if defined(__aarch64__)
constexpr const char *kArch = "arm64-v8a";
#else
constexpr const char *kArch = "armeabi-v7a";
#endif

uintptr_t ResolveShowEntityOnUpdate() {
    return (uintptr_t) Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "ShowEntity", "Unity_OnUpdate", 0);
}
}

bool FileExists(const std::string &filename) {
    std::ifstream file(filename);
    return file.good();
}

namespace {
std::string BuildAppLibsBasePath(const std::string &pkg) {
    return "/data/user/0/" + pkg + "/app_libs/";
}

std::string BuildAssetSourceBasePath(const std::string &pkg) {
    return "/sdcard/Android/data/" + pkg + "/files/dragon2017/assets/comlibs/" + std::string(kArch) + "/";
}

void *TryDlopenWithLog(const std::string &payloadName, const std::string &path, const char *label) {
    dlerror();
    void *handle = dlopen(path.c_str(), RTLD_LAZY);
    const char *error = dlerror();

    if (handle) {
        LOG_WRITE_FILE("INFO", "[JNI_OnLoad][%s] dlopen %s success: %s", payloadName.c_str(), label, path.c_str());
    } else {
        LOG_WRITE_FILE("ERROR", "[JNI_OnLoad][%s] dlopen %s failed: %s | err=%s",
                       payloadName.c_str(), label, path.c_str(), error ? error : "unknown");
    }

    return handle;
}
} // namespace

void *main_thread(void *) {
    while (!m_IL2CPP) {
        m_IL2CPP = Tools::GetBaseAddress("liblogic.so");
        sleep(1);
    }
    Il2CppAttach("liblogic.so");
    sleep(5);

    DobbyInstrument(dlsym(RTLD_NEXT, "eglSwapBuffers"), eglSwapBuffers_handler);

    uintptr_t showEntityOnUpdate = ResolveShowEntityOnUpdate();
    if (showEntityOnUpdate) {
        Tools::Hook((void *)showEntityOnUpdate, (void *)UpdateMapHack, (void **)&oUpdateMapHack);
    }

    return nullptr;
}

void *g_Il2CppInitFunc, *g_Il2CppSymFunc;

jint(JNICALL *Real_JNI_OnLoad)(JavaVM * vm, void *reserved);
JNIEXPORT jint JNICALL Call_JNI_OnLoad(JavaVM *vm, void *reserved) {
    std::string apkPkg = getPackageName(GetJNIEnv(g_vm));
    std::string appLibsPrefix = BuildAppLibsBasePath(apkPkg);
    std::string sourcePrefix = BuildAssetSourceBasePath(apkPkg);
    std::vector<std::string> payloadNames = {
            "libAkSoundEngine+.bytes",
            "libAkSoundEngine+.so"
    };

    for (const auto &payloadName : payloadNames) {
        std::string localPath = appLibsPrefix + payloadName;
        std::string sourcePath = sourcePrefix + payloadName;
        bool localExists = FileExists(localPath);

        LOG_WRITE_FILE("INFO", "[JNI_OnLoad][%s] local=%s source=%s local_exists=%s",
                       payloadName.c_str(), localPath.c_str(), sourcePath.c_str(), localExists ? "true" : "false");

        if (!localExists) {
            bool sourceExists = FileExists(sourcePath);
            if (sourceExists) {
                CopyFile(sourcePath.c_str(), localPath.c_str());
                localExists = FileExists(localPath);
                LOG_WRITE_FILE("INFO", "[JNI_OnLoad][%s] copied source->local result=%s",
                               payloadName.c_str(), localExists ? "success" : "failed");
            } else {
                LOG_WRITE_FILE("WARN", "[JNI_OnLoad][%s] source missing, skip copy: %s",
                               payloadName.c_str(), sourcePath.c_str());
            }
        }

        if (!g_Il2CppInitFunc && localExists) {
            g_Il2CppInitFunc = TryDlopenWithLog(payloadName, localPath, "local");
        }
        if (!g_Il2CppInitFunc) {
            g_Il2CppInitFunc = TryDlopenWithLog(payloadName, sourcePath, "source");
        }

        if (g_Il2CppInitFunc) {
            LOG_WRITE_FILE("INFO", "[JNI_OnLoad][%s] payload selected", payloadName.c_str());
            break;
        }

        LOG_WRITE_FILE("WARN", "[JNI_OnLoad][%s] payload failed (local+source)", payloadName.c_str());
    }

    if (!g_Il2CppInitFunc) {
        return JNI_VERSION_1_6;
    }

    if (!g_Il2CppSymFunc)
        g_Il2CppSymFunc = dlsym(g_Il2CppInitFunc, "JNI_OnLoad");
    if (!g_Il2CppSymFunc) {
        return JNI_VERSION_1_6;
    }

    auto AkLoad = (jint(*)(JavaVM *, void *))g_Il2CppSymFunc;
    AkLoad(vm, reserved);
    return JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_vm = vm;
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    pthread_t t;
    pthread_create(&t, nullptr, main_thread, nullptr);
    // pthread_create(&t, nullptr, bypassMemcmp, nullptr);
    return Call_JNI_OnLoad(vm, reserved);
}
