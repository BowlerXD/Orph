//
//  Logger.h
//
//  Created by MJ (Ruit) on 1/1/19.
//

#ifndef ZYGISK_IMGUI_MODMENU_LOG_H
#define ZYGISK_IMGUI_MODMENU_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#if !defined(LOG_ENABLE_FILE)
#if defined(DEBUG) || !defined(NDEBUG)
#define LOG_ENABLE_FILE 1
#else
#define LOG_ENABLE_FILE 0
#endif
#endif

static inline void LOG_WRITE_FILE(const char *level, const char *fmt, ...) {
#if LOG_ENABLE_FILE && defined(LOG_FILE_PATH)
    if (!fmt || !LOG_FILE_PATH[0]) return;
    FILE *fp = fopen(LOG_FILE_PATH, "a");
    if (!fp) return;

    time_t now = time(NULL);
    struct tm local_tm;
    localtime_r(&now, &local_tm);

    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &local_tm);
    fprintf(fp, "[%s][%s] ", timebuf, level);

    va_list args;
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);

    fputc('\n', fp);
    fclose(fp);
#else
    (void) level;
    (void) fmt;
#endif
}

#define LOGD(...) ((void)0)
#define LOGE(...) ((void)0)
#define LOGI(...) ((void)0)
#define LOGW(...) ((void)0)

#endif /* Logger_h */
