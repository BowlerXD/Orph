#pragma once

#include <cstdint>

inline bool ShouldLogResolveFail(uint32_t &counter) {
    ++counter;
    return counter == 1 || (counter % 300) == 0;
}

#define RESOLVE_METHOD_CACHED(image, namespaze, klass, method, ...) \
    ([]() -> uintptr_t { \
        static uintptr_t cached = 0; \
        static uint32_t failCounter = 0; \
        if (!cached) { \
            cached = (uintptr_t) Il2CppGetMethodOffset(image, namespaze, klass, method, ##__VA_ARGS__); \
            if (!cached && ShouldLogResolveFail(failCounter)) { \
                /* intentionally throttled */ \
            } \
        } \
        return cached; \
    }())

#define RESOLVE_FIELD_CACHED(image, namespaze, klass, field) \
    ([]() -> uintptr_t { \
        static uintptr_t cached = 0; \
        static uint32_t failCounter = 0; \
        if (!cached) { \
            cached = (uintptr_t) Il2CppGetFieldOffset(image, namespaze, klass, field); \
            if (!cached && ShouldLogResolveFail(failCounter)) { \
                /* intentionally throttled */ \
            } \
        } \
        return cached; \
    }())

#define RESOLVE_STATIC_FIELD_CACHED(image, namespaze, klass, field) \
    ([]() -> uintptr_t { \
        static uintptr_t cached = 0; \
        static uint32_t failCounter = 0; \
        if (!cached) { \
            cached = (uintptr_t) Il2CppGetStaticFieldOffset(image, namespaze, klass, field); \
            if (!cached && ShouldLogResolveFail(failCounter)) { \
                /* intentionally throttled */ \
            } \
        } \
        return cached; \
    }())
