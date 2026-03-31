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

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui/icon.h"
#include "imgui_additional.h"
#include "imgui_themes.h"
#include <android_native_app_glue.h>
#include "backends/imgui_impl_android.h"
#include "backends/imgui_impl_opengl3.h"

#include "Includes/Logger.h"
#include "Includes/Utils.h"
#include "Includes/Macros.h"

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

#include "consolas.hpp"
#include "Render.h"
