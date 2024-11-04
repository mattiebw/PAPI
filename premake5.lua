workspace "PAPI"
	configurations { "Debug", "Release", "Dist" }
	platforms { "Win64", "Linux" }
	startproject "PAPI"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["spdlog"] = "PAPI/Vendor/spdlog/include" -- MW @todo @build @perf: Consider moving to compiled spdlog
IncludeDir["SDL"] = "PAPI/Vendor/SDL/include"
IncludeDir["imgui"] = "PAPI/Vendor/imgui/"
IncludeDir["glm"] = "PAPI/Vendor/glm/Include"
IncludeDir["stb"] = "PAPI/Vendor/stb"

group "Vendor"
include "PAPI/Vendor/imgui.lua"
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
	}

	includedirs 
	{ 
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.SDL}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",

		"PAPI/Include"
	}

	filter "system:windows"
		libdirs 
		{
			"PAPI/Vendor/SDL/lib/Win64"
		}

	filter "system:linux"
		libdirs 
		{
			"PAPI/Vendor/SDL/lib/Linux64"
		}

	filter {}

	links
	{
		"imgui",
		"SDL3"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	postbuildcommands { "{COPYDIR} " .. path.getdirectory(".") .. "\"./PAPI/Content/\" \"" .. path.getdirectory("path") .. "/../Build/%{prj.name}/" .. outputdir .. "/Content/\""}

	filter "system:windows"
		postbuildcommands { "{COPYFILE} " .. path.getdirectory(".") .. "\"./PAPI/Vendor/SDL/lib/Win64/SDL3.dll\" \"" .. path.getdirectory("path") .. "/../Build/%{prj.name}/" .. outputdir .. "/\"" }

	filter "system:linux"
		postbuildcommands { "{COPYFILE} " .. path.getdirectory(".") .. "\"./PAPI/Vendor/SDL/lib/Linux64/libSDL3.so\" \"" .. path.getdirectory("path") .. "/../Build/%{prj.name}/" .. outputdir .. "/\"" }
		postbuildcommands { "{COPYFILE} " .. path.getdirectory(".") .. "\"./PAPI/Vendor/SDL/lib/Linux64/libSDL3.so.0\" \"" .. path.getdirectory("path") .. "/../Build/%{prj.name}/" .. outputdir .. "/\"" }
		postbuildcommands { "{COPYFILE} " .. path.getdirectory(".") .. "\"./PAPI/Vendor/SDL/lib/Linux64/libSDL3.so.0.1.5\" \"" .. path.getdirectory("path") .. "/../Build/%{prj.name}/" .. outputdir .. "/\"" }
		postbuildcommands { "{COPYFILE} \"./RunPAPI.sh\" \"" .. path.getdirectory("path") .. "/../Build/%{prj.name}/" .. outputdir .. "/\"" }

	filter { "system:linux", "action:gmake2" }
		buildoptions { "-j8" } -- 8 is probably a decent middle-ground when we don't know how many cores the machine we're compiling on has.

	filter "files:PAPI/Source/Vendor/stb.cpp"
		optimize "On" -- MW @hack: stb doesn't compile properly with GCC without optimizations (@credit https://git.suyu.dev/suyu/suyu/pulls/63)

os.mkdir("PAPI/Source")
os.mkdir("PAPI/Include")

filter "configurations:Debug"
	defines { "PAPI_DEBUG", "PAPI_ENABLE_ASSERTS" }
	symbols "On"
	runtime "Debug"

filter "configurations:Release"
	defines { "PAPI_RELEASE", "PAPI_ENABLE_ASSERTS" }
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

filter "platforms:Win64"
	system "Windows"
	architecture "x64"

filter "platforms:Linux"
	system "linux"
	architecture "x64"