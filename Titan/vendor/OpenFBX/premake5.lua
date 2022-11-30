project "OpenFBX"
	kind "StaticLib"
	language "C++"
	staticruntime "off"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**cpp",
        "src/**h",
        "src/**c",
	}
	filter "system:windows"
		systemversion "latest"
		cppdialect "C++14"
		staticruntime "off"
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
   
	filter "configurations:Release"
		runtime "Release"
		optimize "on"
	
	
	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		symbols "off"