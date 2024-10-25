#pragma once

#define SPDLOG_EOL ""

// Utility functions and types
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

// Data types
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// Core project includes
#include <SDL3/SDL_assert.h>
#include "Core/PAPICore.h"
#include "Core/PAPILog.h"

// Platform specific
#ifdef PAPI_PLATFORM_WINDOWS
#include "Windows.h"
#endif
