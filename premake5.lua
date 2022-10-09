workspace "Titan"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

externalIncludes = 
{
    "Titan/vendor/spdlog/include/",
    "Titan/vendor/GLFW/include/",
    "Titan/vendor/glm/",
}

externalLinks = 
{
    "GLFW",
    "d3d12.lib",
    "dxgi.lib",
    "d3dcompiler.lib",
    "dxguid.lib",
}

include "Titan/vendor/GLFW/"
include "Titan/vendor/glm/"



project "SandBox"
    location "SandBox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    debugdir "AssetData/"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    files
	{
		"SandBox/src/**.cpp",
        "SandBox/src/**.h"
	}

    includedirs
    {
        "SandBox/src/",
        "Titan/src",
        "Titanium/src",
        "Titan/vendor/tinygltf/",
        "Titan/vendor/Snowflake/",
        "Titan/vendor/ImGui/",
        "Titan/vendor/freetype/Include/",
        "Titan/vendor/stb_image/",
        "Titan/vendor/",
        externalIncludes
    }
    libdirs
    {
    }
    links
    {
        "Titan",
        "Titanium",
        externalLinks
    }

    defines {"_CONSOLE"}

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
        links
        {
            "Titan/vendor/freetype/lib/freetyped.lib",
        }

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        links
        {
            "Titan/vendor/freetype/lib/freetype.lib",
        }

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
        links
        {
            "Titan/vendor/freetype/lib/freetype.lib",
        }
    
    filter "system:windows"
		symbols "On"		
		systemversion "latest"

		flags 
        { 
			"MultiProcessorCompile"
		}









        
project "Titan"
    location "Titan"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    
    debugdir "AssetData/"
    
    pchheader "TNpch.h"
	pchsource "Titan/src/TNpch.cpp"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    files
    {
        "Titan/src/**.cpp",
        "Titan/src/**.h", 
        "Titan/vendor/tinygltf/**.h",
        "Titan/vendor/tinygltf/**.hpp",
        "Titan/vendor/Snowflake/**.hpp",
        "Titan/vendor/Snowflake/**.h",
        "Titan/vendor/ImGui/**.h",
        "Titan/vendor/ImGui/**.cpp",
        "Titan/vendor/freetype/Include/**.h",
        "Titan/vendor/stb_image/**.h",
        "Titan/vendor/dx12helpers/**.h",
    }
    
    includedirs
    {
        "Titan/src",
        "Titan/vendor/tinygltf/",
        "Titan/vendor/Snowflake/",
        "Titan/vendor/ImGui/",
        "Titan/vendor/freetype/Include/",
        "Titan/vendor/stb_image/",
        "Titan/vendor/",
        externalIncludes
    }
    libdirs
    {
    }
    links
    {
        externalLinks
    }
    
    defines {"_CONSOLE"}
    
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

    filter "system:windows"
        symbols "On"		
        systemversion "latest"
    
        flags 
        { 
            "MultiProcessorCompile"
        }








 project "Titanium"
    location "Titanium"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
        
    debugdir "Assets/"
        
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    files
    {
        "Titanium/src/**.cpp",
        "Titanium/src/**.h",  
    }
        
    includedirs
    {
        "Titan/src",
        "Titanium/src",
        "Titan/vendor/tinygltf/",
        "Titan/vendor/Snowflake/",
        "Titan/vendor/ImGui/",
        "Titan/vendor/freetype/Include/",
        "Titan/vendor/stb_image/",
        externalIncludes
    }
    libdirs
    {
    }
    links
    {
        "Titan",
        externalLinks
    }
        
    defines {"_CONSOLE"}
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        links
        {
            "Titan/vendor/freetype/lib/freetyped.lib",
        }
        
    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        links
        {
            "Titan/vendor/freetype/lib/freetype.lib",
        }
        
    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "off"
        links
        {
            "Titan/vendor/freetype/lib/freetype.lib",
        }
            
    filter "system:windows"
        symbols "On"		
        systemversion "latest"
    
        flags 
        { 
            "MultiProcessorCompile"
        }