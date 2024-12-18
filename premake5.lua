workspace "PAPI"
	configurations { "Debug", "Release", "Dist" }
	platforms { "Win64", "Linux" }
	startproject "PAPI"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["spdlog"] = "PAPI/Vendor/spdlog/include" -- MW @todo @build @perf: Consider moving to compiled spdlog
IncludeDir["SDL"] = "PAPI/Vendor/SDL/include"
IncludeDir["imgui"] = "PAPI/Vendor/imgui/"
IncludeDir["msdfatlasgen"] = "PAPI/Vendor/msdf-atlas-gen/msdf-atlas-gen/"
IncludeDir["msdfgen"] = "PAPI/Vendor/msdf-atlas-gen/msdfgen/"
IncludeDir["glm"] = "PAPI/Vendor/glm/Include"
IncludeDir["stb"] = "PAPI/Vendor/stb"
IncludeDir["steamworks"] = "PAPI/Vendor/Steamworks/Include"
IncludeDir["fmod"] = "PAPI/Vendor/FMOD/Include"

group "Vendor"
include "PAPI/Vendor/imgui.lua"
include "PAPI/Vendor/msdf-atlas-gen.lua"
group ""

project "PAPI"
	cppdialect "C++20"
	kind "ConsoleApp"
	staticruntime "On"
	language "C++"
	location "PAPI"
	targetdir ("Build/%{prj.name}/" .. outputdir)
	objdir ("Build/%{prj.name}/Intermediates/" .. outputdir)

	usestandardpreprocessor 'On'
	--pchheader(iif(_ACTION == "vs2022", "papipch.h", "PAPI/PAPI/Include/papipch.h"))
	pchheader("papipch.h")
	pchsource "PAPI/Source/papipch.cpp"

	vpaths {
		["Include"] = {"PAPI/Include/**.h", "PAPI/Include/**.hpp"},
		["Source"] = {"PAPI/Source/**.cpp", "PAPI/Source/**.c"},
	}

	files { 
		"PAPI/Include/**.h", "PAPI/Include/**.hpp", 
		"PAPI/Source/**.cpp", "PAPI/Source/**.c",
		"PAPI/Content/**",
		"TODO.md", "README.md",
	}

	includedirs 
	{ 
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.SDL}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.msdfgen}",
		"PAPI/Vendor/msdfgen-custom/",
		"%{IncludeDir.msdfatlasgen}",
		"%{IncludeDir.steamworks}",
		"%{IncludeDir.fmod}",

		"PAPI/Include"
	}

	filter "system:windows"
		libdirs 
		{
			"PAPI/Vendor/SDL/Lib/Win64",
			"PAPI/Vendor/Steamworks/Lib/Win64",
			"PAPI/Vendor/FMOD/Lib/Win64"
		}

		links 
		{
			"steam_api64",

			-- FMOD
			"fmod_vc",
			"fmodstudio_vc",
			"fsbank_vc",
		}

	filter "system:linux"
		libdirs 
		{
			"PAPI/Vendor/SDL/Lib/Linux64",
			"PAPI/Vendor/Steamworks/Lib/Linux64",
			"PAPI/Vendor/FMOD/Lib/Linux64"
		}
		
		links 
		{
			"steam_api",

			-- FMOD
			"fmod",
			"fmodstudio",
			"fsbank",
		}

	filter {}

	-- Linking order matters with gcc! freetype needs to be at the bottom.
	-- MW @todo: Understand linking order, consider using 'linkgroups "On"' to avoid linking order issues at the cost of link time.
	-- https://premake.github.io/docs/linkgroups/
	links
	{
		"imgui",
		"SDL3",

		-- MSDF
		"msdf-atlas-gen",
		"msdfgen",
		"freetype",
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
		prebuildcommands { "call \"../Scripts/RunPreprocessor.bat\" " .. outputdir }

	filter "system:linux"
		prebuildcommands { "../Scripts/RunPreprocessor.sh " .. outputdir }
		postbuildcommands { "{COPYFILE} \"./RunPAPI.sh\" \"" .. path.getdirectory("path") .. "/../Build/%{prj.name}/" .. outputdir .. "/\"" }

	filter { "system:linux", "files:PAPI/Source/Vendor/stb.cpp" }
		optimize "On" -- MW @hack: stb doesn't compile properly with GCC without optimizations (@credit https://git.suyu.dev/suyu/suyu/pulls/63)

os.mkdir("PAPI/Source")
os.mkdir("PAPI/Include")

filter "configurations:Debug"
	defines { "PAPI_DEBUG", "PAPI_ENABLE_ASSERTS", "PAPI_GL_DEBUG" }
	symbols "On"
	runtime "Debug"

filter "configurations:Release"
	defines { "PAPI_RELEASE", "PAPI_ENABLE_ASSERTS", "PAPI_GL_DEBUG" }
	optimize "On"
	symbols "On"
	runtime "Release"

filter "configurations:Dist"
	defines { "PAPI_DIST" }
	kind "WindowedApp"
	optimize "On"
	symbols "Off"
	runtime "Release"

filter "system:windows"
	systemversion "latest"
	defines { "PAPI_PLATFORM_WINDOWS" }

	links
	{
		"version",
		"winmm",
		"Imm32",
		"Cfgmgr32",
		"Setupapi"
	}

filter "system:linux"
	defines { "PAPI_PLATFORM_LINUX" }

filter "platforms:Win64"
	system "Windows"
	architecture "x64"

filter "platforms:Linux"
	buildoptions { "-static-libstdc++" }
	system "linux"
	architecture "x64"
