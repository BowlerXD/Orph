#include "SecurityChecks.h"

#include <jni.h>
#include <android_native_app_glue.h>

#include <csignal>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

extern JavaVM *g_vm;
extern android_app *i_App;
extern jobject getJNIContext(JNIEnv *env);

namespace {
int yaserClose() {
    if (i_App == nullptr || i_App->activity == nullptr || i_App->activity->vm == nullptr || i_App->activity->clazz == nullptr) {
        return -1;
    }

    JavaVM *java_vm = i_App->activity->vm;
    JNIEnv *java_env = nullptr;
    bool attached_by_this_function = false;
    jclass native_activity_clazz = nullptr;
    jmethodID method_id = nullptr;

    jint jni_return = java_vm->GetEnv((void **)&java_env, JNI_VERSION_1_6);
    if (jni_return == JNI_ERR)
        return -1;
    if (jni_return == JNI_EDETACHED) {
        jni_return = java_vm->AttachCurrentThread(&java_env, nullptr);
        if (jni_return != JNI_OK || java_env == nullptr)
            return -2;
        attached_by_this_function = true;
    } else if (jni_return != JNI_OK || java_env == nullptr) {
        return -2;
    }

    native_activity_clazz = java_env->GetObjectClass(i_App->activity->clazz);
    if (native_activity_clazz == nullptr) {
        jni_return = -3;
        goto cleanup;
    }

    method_id = java_env->GetMethodID(native_activity_clazz, "AndroidThunkJava_RestartGame", "()V");
    if (method_id == nullptr) {
        jni_return = -4;
        goto cleanup;
    }

    java_env->CallVoidMethod(i_App->activity->clazz, method_id);
    if (java_env->ExceptionCheck()) {
#ifndef NDEBUG
        java_env->ExceptionDescribe();
#endif
        java_env->ExceptionClear();
        jni_return = -5;
        goto cleanup;
    }

    jni_return = 0;

cleanup:
    if (native_activity_clazz != nullptr) {
        java_env->DeleteLocalRef(native_activity_clazz);
    }
    if (attached_by_this_function) {
        jint detach_result = java_vm->DetachCurrentThread();
        if (detach_result != JNI_OK && jni_return == 0) {
            return -6;
        }
    }
    return jni_return;
}

bool isYaserFolderHere(const std::string &folderPath) {
    return access(folderPath.c_str(), F_OK) == 0;
}
} // namespace

void detectDump() {
    std::ifstream mapsFile("/proc/self/maps");
    std::string line;

    while (std::getline(mapsFile, line)) {
        if (line.find("liblua") != std::string::npos || line.find(".lua") != std::string::npos) {
            kill(getpid(), SIGKILL);
        }
    }
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
        if (isYaserFolderHere(folderPath)) {
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

    ctx = env->FindClass("android/content/Context");
    if (!ctx)
        goto cleanup;

    context = getJNIContext(env);
    if (!context)
        goto cleanup;

    service = env->GetMethodID(ctx, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    if (!service)
        goto cleanup;

    str = env->NewStringUTF("connectivity");
    if (!str)
        goto cleanup;

    conn_service = env->CallObjectMethod(context, service, str);
    if (!conn_service)
        goto cleanup;

    connectivity = env->FindClass("android/net/ConnectivityManager");
    if (!connectivity)
        goto cleanup;

    capabils = env->FindClass("android/net/NetworkCapabilities");
    if (!capabils)
        goto cleanup;

    has1 = env->GetMethodID(capabils, "hasCapability", "(I)Z");
    has = env->GetMethodID(capabils, "hasTransport", "(I)Z");
    getCapabil = env->GetMethodID(connectivity, "getNetworkCapabilities", "(Landroid/net/Network;)Landroid/net/NetworkCapabilities;");
    getActive = env->GetMethodID(connectivity, "getActiveNetwork", "()Landroid/net/Network;");
    if (!has1 || !has || !getCapabil || !getActive)
        goto cleanup;

    activenetwork = env->CallObjectMethod(conn_service, getActive);
    if (!activenetwork)
        goto cleanup;

    activecapabilities = env->CallObjectMethod(conn_service, getCapabil, activenetwork);
    if (!activecapabilities)
        goto cleanup;

    vpn_enabled = env->CallBooleanMethod(activecapabilities, has, 4) ||
                  env->CallBooleanMethod(activecapabilities, has1, 4);

cleanup:
    if (activecapabilities)
        env->DeleteLocalRef(activecapabilities);
    if (activenetwork)
        env->DeleteLocalRef(activenetwork);
    if (capabils)
        env->DeleteLocalRef(capabils);
    if (connectivity)
        env->DeleteLocalRef(connectivity);
    if (conn_service)
        env->DeleteLocalRef(conn_service);
    if (str)
        env->DeleteLocalRef(str);
    if (context)
        env->DeleteLocalRef(context);
    if (ctx)
        env->DeleteLocalRef(ctx);

    if (attached_by_this_function) {
        g_vm->DetachCurrentThread();
    }

    return vpn_enabled;
}
