#include <libgen.h>
#include <fcntl.h>
#include <inttypes.h>
#include <jni.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string>
#include <random>
#include <cstdio>

#include "Tools.h"

#if defined(__arm__)
#define process_vm_readv_syscall 376
#define process_vm_writev_syscall 377
#elif defined(__aarch64__)
#define process_vm_readv_syscall 270
#define process_vm_writev_syscall 271
#elif defined(__i386__)
#define process_vm_readv_syscall 347
#define process_vm_writev_syscall 348
#else
#define process_vm_readv_syscall 310
#define process_vm_writev_syscall 311
#endif

pid_t target_pid = -1;
#define INRANGE(x, a, b)        (x >= a && x <= b)
#define getBits(x)              (INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte(x)              (getBits(x[0]) << 4 | getBits(x[1]))

namespace {
    struct ProcMapRegion {
        uintptr_t start;
        uintptr_t end;
        bool readable;
        bool writable;
    };

    bool ParseProcMapLine(const char *line, ProcMapRegion *region) {
        if (line == nullptr || region == nullptr) {
            return false;
        }

        uintptr_t start = 0;
        uintptr_t end = 0;
        char perms[5] = {};
        if (sscanf(line, "%" SCNxPTR "-%" SCNxPTR " %4s", &start, &end, perms) != 3) {
            return false;
        }

        region->start = start;
        region->end = end;
        region->readable = perms[0] == 'r';
        region->writable = perms[1] == 'w';
        return true;
    }

    bool IsAddressRangeMapped(void *addr, size_t length, bool requireRead, bool requireWrite) {
        if (addr == nullptr || length == 0) {
            return false;
        }

        const uintptr_t startAddr = reinterpret_cast<uintptr_t>(addr);
        const uintptr_t endAddr = startAddr + length;
        if (endAddr < startAddr) {
            return false;
        }

        FILE *maps = fopen("/proc/self/maps", "r");
        if (maps == nullptr) {
            return false;
        }

        char line[512];
        bool mapped = false;
        while (fgets(line, sizeof(line), maps) != nullptr) {
            ProcMapRegion region{};
            if (!ParseProcMapLine(line, &region)) {
                continue;
            }

            if (startAddr >= region.start && endAddr <= region.end) {
                if (requireRead && !region.readable) {
                    break;
                }
                if (requireWrite && !region.writable) {
                    break;
                }
                mapped = true;
                break;
            }
        }

        fclose(maps);
        return mapped;
    }

    bool UnprotectPageForAddress(void *address) {
        const uintptr_t rawAddress = reinterpret_cast<uintptr_t>(address);
        const unsigned long pageSize = static_cast<unsigned long>(sysconf(_SC_PAGESIZE));
        const uintptr_t pageStart = rawAddress & ~(static_cast<uintptr_t>(pageSize) - 1U);
        return mprotect(reinterpret_cast<void *>(pageStart), pageSize, PROT_EXEC | PROT_READ | PROT_WRITE) == 0;
    }
}

ssize_t process_v(pid_t __pid, const struct iovec *__local_iov, unsigned long __local_iov_count, const struct iovec *__remote_iov, unsigned long __remote_iov_count, unsigned long __flags, bool iswrite) {
    return syscall((iswrite ? process_vm_writev_syscall : process_vm_readv_syscall), __pid, __local_iov, __local_iov_count, __remote_iov, __remote_iov_count, __flags);
}

bool pvm(void *address, void *buffer, size_t size, bool write = false) {
    struct iovec local[1];
    struct iovec remote[1];

    local[0].iov_base = buffer;
    local[0].iov_len = size;
    remote[0].iov_base = address;
    remote[0].iov_len = size;

    if (target_pid == -1) {
        target_pid = getpid();
    }

    ssize_t bytes = process_v(target_pid, local, 1, remote, 1, 0, write);
    return bytes == size;
}

void Tools::Hook(void *target, void *replace, void **backup) {
    if (UnprotectPageForAddress(target)) {
		DobbyHook(target, replace, backup);
    }
}

bool Tools::Read(void *addr, void *buffer, size_t length) {
	return memcpy(buffer, addr, length) != 0;
}

bool Tools::Write(void *addr, void *buffer, size_t length) {
	return memcpy(addr, buffer, length) != 0;
}

bool Tools::ReadAddr(void *addr, void *buffer, size_t length) {
	return UnprotectPageForAddress(addr) && memcpy(buffer, addr, length) != 0;
}

bool Tools::WriteAddr(void *addr, void *buffer, size_t length) {
    return UnprotectPageForAddress(addr) && memcpy(addr, buffer, length) != 0;
}

bool Tools::PVM_ReadAddr(void *addr, void *buffer, size_t length) {
    return pvm(addr, buffer, length, false);
}

bool Tools::PVM_WriteAddr(void *addr, void *buffer, size_t length) {
    return pvm(addr, buffer, length, true);
}

bool Tools::IsPtrValid(void *addr) {
    return IsReadablePtr(addr);
}

bool Tools::IsReadablePtr(void *addr, size_t length) {
    return IsAddressRangeMapped(addr, length, true, false);
}

bool Tools::IsWritablePtr(void *addr, size_t length) {
    return IsAddressRangeMapped(addr, length, true, true);
}

uintptr_t Tools::GetBaseAddress(const char *name) {
    uintptr_t base = 0;
    char line[512];

    FILE *f = fopen("/proc/self/maps", "r");

    if (!f) {
        return 0;
    }
    while (fgets(line, sizeof line, f)) {
        uintptr_t tmpBase;
        char tmpName[256];
        if (sscanf(line, "%" PRIXPTR "-%*" PRIXPTR "%*s %*s %*s %*s %s", &tmpBase, tmpName) > 0) {
            if (!strcmp(basename(tmpName), name)) {
                base = tmpBase;
                break;
            }
        }
    }

    fclose(f);
    return base;
}

uintptr_t Tools::GetRealOffsets(const char *libraryName, uintptr_t relativeAddr) {
	uintptr_t libBase = Tools::GetBaseAddress(libraryName);
	if (libBase == 0)
		return 0;
	return (reinterpret_cast<uintptr_t>(libBase + relativeAddr));
}


uintptr_t Tools::GetEndAddress(const char *name) {
    uintptr_t end = 0;
    char line[512];

    FILE *f = fopen("/proc/self/maps", "r");

    if (!f) {
        return 0;
    }

    bool found = false;
    while (fgets(line, sizeof line, f)) {
        uintptr_t tmpEnd;
        char tmpName[256];
        if (sscanf(line, "%*" PRIXPTR "-%" PRIXPTR "%*s %*s %*s %*s %s", &tmpEnd, tmpName) > 0) {
            if (!strcmp(basename(tmpName), name)) {
                if (!found) {
                    found = true;
                }
            } else {
                if (found) {
                    end = tmpEnd;
                    break;
                }
            }
        }
    }

    fclose(f);
    return end;
}


uintptr_t Tools::FindPattern(const char *lib, const char *pattern) {
    auto start = GetBaseAddress(lib);
    if (!start)
        return 0;
    auto end = GetEndAddress(lib);
    if (!end)
        return 0;
    auto curPat = reinterpret_cast<const unsigned char *>(pattern);
    uintptr_t firstMatch = 0;
    for (uintptr_t pCur = start; pCur < end; ++pCur) {
        if (*(uint8_t *) curPat == (uint8_t) '\?' || *(uint8_t *) pCur == getByte(curPat)) {
            if (!firstMatch) {
                firstMatch = pCur;
            }
            curPat += (*(uint16_t *) curPat == (uint16_t) '\?\?' || *(uint8_t *) curPat != (uint8_t) '\?') ? 2 : 1;
            if (!*curPat) {
                return firstMatch;
            }
            curPat++;
            if (!*curPat) {
                return firstMatch;
            }
        } else if (firstMatch) {
            pCur = firstMatch;
            curPat = reinterpret_cast<const unsigned char *>(pattern);
            firstMatch = 0;
        }
    }
    return 0;
}

std::string Tools::RandomString(const int len) {
    static const char alphanumerics[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<size_t> distribution(0, sizeof(alphanumerics) - 2);
    std::string tmp;
    tmp.reserve(len);
    for (int i = 0; i < len; ++i) {
        tmp += alphanumerics[distribution(generator)];
    }
    return tmp;
}

std::string Tools::GetPackageName(JNIEnv *env, jobject context) {
    jclass contextClass = env->FindClass("android/content/Context");
    jmethodID getPackageNameId = env->GetMethodID(contextClass, "getPackageName", "()Ljava/lang/String;");
    auto str = (jstring) env->CallObjectMethod(context, getPackageNameId);
    const char *packageNameChars = env->GetStringUTFChars(str, nullptr);
    std::string packageName(packageNameChars == nullptr ? "" : packageNameChars);
    if (packageNameChars != nullptr) {
        env->ReleaseStringUTFChars(str, packageNameChars);
    }
    env->DeleteLocalRef(str);
    env->DeleteLocalRef(contextClass);
    return packageName;
}
