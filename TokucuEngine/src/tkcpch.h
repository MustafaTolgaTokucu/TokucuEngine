#pragma once


#include <iostream>
#include <memory>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Tokucu/Log.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#ifdef TKC_PLATFORM_WINDOWS
	#include <Windows.h>
#endif // TKC_PLATFORM_WINDOWS
