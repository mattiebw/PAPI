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

// GLM
#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"

// GLAD
#include <Vendor/glad/gl.h>

// Core project includes
#include <SDL3/SDL_assert.h>
#include "Core/PAPICore.h"
#include "Core/PAPILog.h"
#include "Core/Delegate/Delegate.h"
#include "Core/Utility/MathUtil.h"
#include "Core/Transform.h"

// Platform specific
#ifdef PAPI_PLATFORM_WINDOWS
#include "Windows.h"
#endif
