#include "Hooks.h"

#include <dlfcn.h>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <GLES3/gl3.h>

#include "Tools/Dobby/dobby.h"
#include "xHook/xhook.h"

void Render();

namespace {
int (*orig_memcmp)(const void *lhs, const void *rhs, size_t len) = nullptr;

bool isTargetMemcmpCallsite() {
#if defined(__arm__) || defined(__aarch64__)
    void *caller = __builtin_return_address(0);
    if (caller == nullptr) {
        return false;
    }

    Dl_info info{};
    if (dladdr(caller, &info) == 0 || info.dli_fname == nullptr) {
        return false;
    }

    return strstr(info.dli_fname, "liblogic.so") != nullptr;
#else
    return false;
#endif
}

int memcmp_hook(const void *lhs, const void *rhs, size_t len) {
    if (orig_memcmp == nullptr) {
        return memcmp(lhs, rhs, len);
    }

    const int result = orig_memcmp(lhs, rhs, len);
    if (!isTargetMemcmpCallsite()) {
        return result;
    }

    return 0;
}
} // namespace

void eglSwapBuffers_handler(RegisterContext *ctx, const HookEntryInfo *info) {
    (void)ctx;
    (void)info;

    static int count = 0;
    if (count < 10) {
        count++;
    } else {
        Render();
    }
}

void *bypassMemcmp(void *arg) {
    (void)arg;
    orig_memcmp = nullptr;
    xhook_enable_sigsegv_protection(1);
    xhook_register(".*\\.so$", "memcmp", (void *)memcmp_hook, (void **)&orig_memcmp);
    xhook_refresh(0);
    return nullptr;
}
