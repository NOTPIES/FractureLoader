#pragma once

#ifdef max
#undef max
#endif

#include <limits>
#include "kiero.h"
#include "Minhook.h"

#include <iostream>
#include <string>
#include <vector>
#include <d3d11.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <cstdint>

#include "loader_manager.h"
#include "mod_loader.h"
#include "logger.h"

#include "mem.h"
#include "functions.h"

#include "hooks.h"
#include "gui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"