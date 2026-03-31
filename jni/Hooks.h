#pragma once

#include "Tools/Dobby/dobby.h"

void eglSwapBuffers_handler(RegisterContext *ctx, const HookEntryInfo *info);
void *bypassMemcmp(void *arg);
