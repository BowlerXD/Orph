#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <thread>
#include <pthread.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <jni.h>
#include <android/log.h>
#include <elf.h>
#include <dlfcn.h>
#include <sys/system_properties.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/native_window.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dlfcn.h>

// internal module start here
#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_additional.h"
#include "imgui_themes.h"
#include <android_native_app_glue.h>
#include "backends/imgui_impl_android.h"
#include "backends/imgui_impl_opengl3.h"
#include "fonts/GoogleSans.h"
#include "imgui/icon.h"

#include "KittyMemory/MemoryPatch.h"

#include "Includes/Logger.h"
#include "Includes/Utils.h"
#include "Includes/Macros.h"

#include "Tools/xDL/xdl.h"
#include "Tools/fake_dlfcn.h"
#include "Tools/Il2Cpp.h"
#include "Tools/Tools.h"

#include "XYZ/Unity/Struct/Vector3.hpp"
#include "XYZ/Unity/Struct/Vector2.h"
#include "XYZ/Unity/Struct/Rect.h"
#include "XYZ/Unity/Struct/Quaternion.h"
#include "XYZ/ConfigName.h"
#include "Config/setup.h"
#include "Config/JNIStuff.h"
#include "Config/TouchSystem.h"
#include "XYZ/IconMinimap/DrawIcon.h"
#include "Tools/Login/Login.h"
#include "XYZ/GameClass.h"
#include "XYZ/ToString.h"
#include "XYZ/SDK.h"
#include "XYZ/DrawMinimap.h"
#include "XYZ/Minimap.h"
#include "XYZ/DrawESP.h"
#include "XYZ/Bypass.h"
#include "DrawMenu.h"

#undef stderr
#undef stdout
#undef stdin

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <thread> // Added for sleep

using std::chrono::seconds;
using std::this_thread::sleep_for;

#include "xHook/xhook.h"
#include "consolas.hpp"
#include "Render.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

bool PatchOffset(uintptr_t address, const void *buffer, size_t length) {
	unsigned long page_size = sysconf(_SC_PAGESIZE);
    unsigned long size = page_size * sizeof(uintptr_t);
    return mprotect((void *)(address - (address % page_size) - page_size), (size_t) size, PROT_EXEC | PROT_READ | PROT_WRITE) == 0 && memcpy((void *)address, (void *)buffer, length) != 0;
}

void detectDump() {
    std::ifstream mapsFile("/proc/self/maps");
    std::string line;

    while (std::getline(mapsFile, line)) {
        if (line.find("liblua") != std::string::npos || line.find(".lua") != std::string::npos) {
            kill(getpid(), SIGKILL);
        }
    }
}


int yaserClose() {
    JavaVM* java_vm = i_App->activity->vm;
    JNIEnv* java_env = NULL;
    jint jni_return = java_vm->GetEnv((void**)&java_env, JNI_VERSION_1_6);
    if (jni_return == JNI_ERR)
        return -1;
    jni_return = java_vm->AttachCurrentThread(&java_env, NULL);
    if (jni_return != JNI_OK)
        return -2;
    jclass native_activity_clazz = java_env->GetObjectClass(i_App->activity->clazz);
    if (native_activity_clazz == NULL)
        return -3;
    jmethodID method_id = java_env->GetMethodID(native_activity_clazz, "AndroidThunkJava_RestartGame","()V");
    if (method_id == NULL)
        return -4;
    java_env->CallVoidMethod(i_App->activity->clazz, method_id);
    jni_return = java_vm->DetachCurrentThread();
    if (jni_return != JNI_OK)
        return -5;
    return 0;
}
bool isyaserFolderHere(const std::string& folderPath) {
    return (access(folderPath.c_str(), F_OK) == 0);
}

void YaserAntiCrack1() {
    static const std::vector<std::string> suspiciousFolders = {
            "/storage/emulated/0/Android/data/com.guoshi.httpcanary",
            "/storage/emulated/0/Android/data/com.sniffer",
            "/storage/emulated/0/Android/data/com.guoshi.httpcanary.premium",
            "/storage/emulated/0/Android/data/com.httpcanary.pro",
            "/storage/emulated/0/Android/data/com.minhui.networkcapture",
            "/storage/emulated/0/Android/data/app.greyshirts.sslcapture",
            "/storage/emulated/0/Android/data/com.emanuelef.remote_capture",
            "/storage/emulated/0/Android/data/com.minhui.networkcapture.pro"
    };

    for (const auto &folderPath : suspiciousFolders) {
        if (isyaserFolderHere(folderPath)) {
            yaserClose();
            return;
        }
    }
}

bool IsVPNEnabled() {
    JNIEnv *env = nullptr;
    bool attached_by_this_function = false;
    bool vpn_enabled = false;

    jint env_status = g_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (env_status == JNI_EDETACHED) {
        if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK || env == nullptr) {
            return false;
        }
        attached_by_this_function = true;
    } else if (env_status != JNI_OK || env == nullptr) {
        return false;
    }

    jclass ctx = nullptr;
    jobject context = nullptr;
    jstring str = nullptr;
    jobject conn_service = nullptr;
    jclass connectivity = nullptr;
    jclass capabils = nullptr;
    jobject activenetwork = nullptr;
    jobject activecapabilities = nullptr;
    jmethodID service = nullptr;
    jmethodID has1 = nullptr;
    jmethodID has = nullptr;
    jmethodID getCapabil = nullptr;
    jmethodID getActive = nullptr;
    jboolean hasvpn1 = JNI_FALSE;
    jboolean hasvpn2 = JNI_FALSE;

    ctx = env->FindClass("android/content/Context");
    if (!ctx) goto cleanup;

    context = getJNIContext(env);
    if (!context) goto cleanup;

    service = env->GetMethodID(ctx, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    if (!service) goto cleanup;

    str = env->NewStringUTF("connectivity");
    if (!str) goto cleanup;

    conn_service = env->CallObjectMethod(context, service, str);
    if (!conn_service) goto cleanup;

    connectivity = env->FindClass("android/net/ConnectivityManager");
    if (!connectivity) goto cleanup;

    capabils = env->FindClass("android/net/NetworkCapabilities");
    if (!capabils) goto cleanup;

    has1 = env->GetMethodID(capabils, "hasCapability", "(I)Z");
    has = env->GetMethodID(capabils, "hasTransport", "(I)Z");
    getCapabil = env->GetMethodID(connectivity, "getNetworkCapabilities", "(Landroid/net/Network;)Landroid/net/NetworkCapabilities;");
    getActive = env->GetMethodID(connectivity, "getActiveNetwork", "()Landroid/net/Network;");
    if (!has1 || !has || !getCapabil || !getActive) goto cleanup;

    activenetwork = env->CallObjectMethod(conn_service, getActive);
    if (!activenetwork) goto cleanup;

    activecapabilities = env->CallObjectMethod(conn_service, getCapabil, activenetwork);
    if (!activecapabilities) goto cleanup;

    hasvpn1 = env->CallBooleanMethod(activecapabilities, has, 4);
    hasvpn2 = env->CallBooleanMethod(activecapabilities, has1, 4);
    vpn_enabled = (hasvpn1 || hasvpn2);

cleanup:
    if (activecapabilities) env->DeleteLocalRef(activecapabilities);
    if (activenetwork) env->DeleteLocalRef(activenetwork);
    if (capabils) env->DeleteLocalRef(capabils);
    if (connectivity) env->DeleteLocalRef(connectivity);
    if (conn_service) env->DeleteLocalRef(conn_service);
    if (str) env->DeleteLocalRef(str);
    if (context) env->DeleteLocalRef(context);
    if (ctx) env->DeleteLocalRef(ctx);

    if (attached_by_this_function) {
        g_vm->DetachCurrentThread();
    }

    return vpn_enabled;
}


bool FileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void eglSwapBuffers_handler(RegisterContext * ctx, const HookEntryInfo * info)
{
    static int count = 0;
    if(count < 10){
        count++;
    }else{
        Render();
    }
}

static int (*orig_memcmp)(const void *lhs, const void *rhs, size_t len) = nullptr;

static inline bool isTargetMemcmpCallsite() {
#if defined(__arm__) || defined(__aarch64__)
    void *caller = __builtin_return_address(0);
    if (caller == nullptr) {
        return false;
    }

    Dl_info info{};
    if (dladdr(caller, &info) == 0 || info.dli_fname == nullptr) {
        return false;
    }

    // Only alter behavior for the known target module/callsite family.
    return strstr(info.dli_fname, "liblogic.so") != nullptr;
#else
    return false;
#endif
}

static int memcmp_hook(const void *lhs, const void *rhs, size_t len) {
    if (orig_memcmp == nullptr) {
        return memcmp(lhs, rhs, len);
    }

    const int result = orig_memcmp(lhs, rhs, len);
    if (!isTargetMemcmpCallsite()) {
        return result;
    }

    // Scope-limited override only for the target callsite/module.
    return 0;
}

void *bypassMemcmp(void *arg) {
    (void)arg;
    orig_memcmp = nullptr;
    xhook_enable_sigsegv_protection(1);
    xhook_register(".*\\.so$", "memcmp", (void *)memcmp_hook, (void **)&orig_memcmp);
    xhook_refresh(0);
    return NULL;
}

void hideLibrary() {
    //unlink("/proc/self/maps");
    char path[64];
    sprintf(path, "/proc/%d/maps", getpid());
    unlink(path);
}

void *main_thread(void *) {
    while (!m_IL2CPP) {
        m_IL2CPP = Tools::GetBaseAddress("liblogic.so");
        sleep(1);
    }
    Il2CppAttach("liblogic.so");
    sleep(5);

	// hideLibrary(); // disabled for stability during loading
	
	DobbyInstrument(dlsym(RTLD_NEXT, "eglSwapBuffers"), eglSwapBuffers_handler);
	
	// Anti-cheat reporter hooks disabled to avoid loading-screen deadlock/stall.
    void *battleManagerInstance = nullptr;
    for (int wait = 0; wait < 120; ++wait) {
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "BattleManager", "Instance", &battleManagerInstance);
        if (battleManagerInstance) break;
        sleep(1);
    }

    if (battleManagerInstance) {
        Tools::Hook((void *) ShowEntity_OnUpdate, (void *) UpdateMapHack, (void **) &oUpdateMapHack);
    } else {
    }
	
    return 0;
}

void *g_Il2CppInitFunc, *g_Il2CppSymFunc;

jint (JNICALL *Real_JNI_OnLoad)(JavaVM *vm, void *reserved);
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
            if (!g_Il2CppInitFunc) g_Il2CppInitFunc = dlopen(toPath.c_str(), RTLD_LAZY);
            if (!g_Il2CppInitFunc) g_Il2CppInitFunc = dlopen(fromPath.c_str(), RTLD_LAZY);
        }

        if (g_Il2CppInitFunc) {
            break;
        }
    }

    if (!g_Il2CppInitFunc) {
        return JNI_VERSION_1_6;
    }

    if (!g_Il2CppSymFunc) g_Il2CppSymFunc = dlsym(g_Il2CppInitFunc, "JNI_OnLoad");
    if (!g_Il2CppSymFunc) {
        return JNI_VERSION_1_6;
    }

    auto AkLoad = (jint(*)(JavaVM *, void *)) g_Il2CppSymFunc;
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
    pthread_create(&t, NULL, main_thread, nullptr);
	//pthread_create(&t, NULL, bypassMemcmp, NULL);
	//pthread_create(&t, nullptr, libentec, nullptr);
    return Call_JNI_OnLoad(vm, reserved);
}
