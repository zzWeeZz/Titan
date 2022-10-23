project "Optick"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    staticruntime "off"
	warnings "Off"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	links
	{
		"d3d12.lib",
		"dxgi.lib",
		"d3dcompiler.lib",
		"dxguid.lib",
	}
	linkoptions 
	{
		"/ignore:4006",
		"/ignore:4099"
	}
	defines
	{
		"_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
        optimize "off"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        symbols "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "off"
