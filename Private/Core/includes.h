#pragma once

#ifdef max
#undef max
#endif

#include <limits>
#include "../../kiero/kiero.h"
#include "MinHook.h"

#include <iostream>
#include <string>
#include <vector>
#include <d3d11.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <cstdint>

#include "../Loader/loader_manager.h"
#include "../Loader/mod_loader.h"
#include "../Logging/logger.h"

#include "../Memory/mem.h"
#include "../Memory/functions.h"

#include "../Hooks/hooks.h"
#include "../GUI/gui.h"

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "../../imgui/imgui_impl_dx11.h"
