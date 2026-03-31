#include <jni.h>

#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

#include <fstream>
#include <string>
#include <vector>

#include "Tools/Il2Cpp.h"
#include "Tools/Tools.h"
#include "Config/setup.h"
#include "Config/JNIStuff.h"
#include "XYZ/DrawMinimap.h"
#include "Hooks.h"

bool FileExists(const std::string &filename) {
    std::ifstream file(filename);
    return file.good();
}

void *main_thread(void *) {
    while (!m_IL2CPP) {
        m_IL2CPP = Tools::GetBaseAddress("liblogic.so");
        sleep(1);
    }
    Il2CppAttach("liblogic.so");
    sleep(5);

    DobbyInstrument(dlsym(RTLD_NEXT, "eglSwapBuffers"), eglSwapBuffers_handler);

    uintptr_t showEntityOnUpdate = ShowEntity_OnUpdate;
    if (showEntityOnUpdate) {
        Tools::Hook((void *)showEntityOnUpdate, (void *)UpdateMapHack, (void **)&oUpdateMapHack);
    }

    return nullptr;
}

void *g_Il2CppInitFunc, *g_Il2CppSymFunc;

jint(JNICALL *Real_JNI_OnLoad)(JavaVM * vm, void *reserved);
JNIEXPORT jint JNICALL Call_JNI_OnLoad(JavaVM *vm, void *reserved) {
    std::string apkPkg = getPackageName(GetJNIEnv(g_vm));
    std::vector<std::string> payloadNames = {
            "libAkSoundEngine+.bytes",
            "libAkSoundEngine+.so"
    };

    for (const auto &payloadName : payloadNames) {
        std::string localPath = std::string("/data/user/0/") + apkPkg.c_str() + std::string("/app_libs/") + payloadName;
        if (!g_Il2CppInitFunc && FileExists(localPath)) {
            g_Il2CppInitFunc = dlopen(localPath.c_str(), RTLD_LAZY);
        }

        if (!g_Il2CppInitFunc) {
            std::string fromPath = std::string("/sdcard/Android/data/") + apkPkg.c_str() + std::string("/files/dragon2017/assets/comlibs/") + std::string(ARCH) + std::string("/") + payloadName;
            std::string toPath = std::string("/data/user/0/") + apkPkg.c_str() + std::string("/app_libs/") + payloadName;
            CopyFile(fromPath.c_str(), toPath.c_str());
            if (!g_Il2CppInitFunc)
                g_Il2CppInitFunc = dlopen(toPath.c_str(), RTLD_LAZY);
            if (!g_Il2CppInitFunc)
                g_Il2CppInitFunc = dlopen(fromPath.c_str(), RTLD_LAZY);
        }

        if (g_Il2CppInitFunc) {
            break;
        }
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
