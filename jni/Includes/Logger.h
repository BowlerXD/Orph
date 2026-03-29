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

#ifndef LOG_FILE_PATH
#define LOG_FILE_PATH "/sdcard/Download/themaphack-debug.log"
#endif

static inline void LOG_WRITE_FILE(const char *level, const char *fmt, ...) {
    if (!fmt) return;
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
}

#define LOGD(...) LOG_WRITE_FILE("D", __VA_ARGS__)
#define LOGE(...) LOG_WRITE_FILE("E", __VA_ARGS__)
#define LOGI(...) LOG_WRITE_FILE("I", __VA_ARGS__)
#define LOGW(...) LOG_WRITE_FILE("W", __VA_ARGS__)

#endif /* Logger_h */
