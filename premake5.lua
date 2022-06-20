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
    "Titan/vendor/spdlog/include",
    "Titan/vendor/GLFW/include",
    "Titan/vendor/glm",
}

externalLinks = 
{
    "GLFW",
    "glm",
}

include "Titan/vendor/GLFW"
include "Titan/vendor/glm"

project "SandBox"
    location "SandBox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    debugdir "Assets/"

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
        "Titan/vendor/vkbootstrap/",
        "Titan/vendor/Vulkan/Include/",
        externalIncludes
    }
    libdirs
    {
        "Titan/vendor/Vulkan/Lib/",
    }
    links
    {
        "Titan/vendor/Vulkan/Lib/**.lib",
        "Titan",
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

project "Titan"
    location "Titan"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    
    debugdir "Assets/"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    files
    {
        "Titan/src/**.cpp",
        "Titan/src/**.h", 
        "Titan/vendor/vkbootstrap/VkBootstrap.h",
        "Titan/vendor/vkbootstrap/VkBootstrap.cpp",
    }
    
    

    includedirs
    {
        "Titan/src",
        "Titan/vendor/Vulkan/Include/",
        "Titan/vendor/vkbootstrap/",
        externalIncludes
    }
    libdirs
    {
        "Titan/vendor/Vulkan/Lib/**.lib",
        "Titan/vendor/Vulkan/Lib/",
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