project "PAPIPreprocessor"
    kind "ConsoleApp"
    language "C#"
    dotnetframework "4.8"
    csversion "12"

    location "PAPIPreprocessor"
    
	targetdir ("Build/%{prj.name}/" .. outputdir)
	objdir ("Build/%{prj.name}/Intermediates/" .. outputdir)

    files 
    {
        "PAPIPreprocessor/Source/**.cs",
    }
    
	filter "configurations:Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        optimize "On"
        symbols "Default"

    filter "configurations:Dist"
        optimize "Full"
        symbols "Off"
