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

// Platform specific
#ifdef PAPI_PLATFORM_WINDOWS
#include "Windows.h"
#endif
