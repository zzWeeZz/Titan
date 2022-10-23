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
VULKAN_SDK = os.getenv("VULKAN_SDK")
externalIncludes = 
{
    "Titan/vendor/spdlog/include/",
    "Titan/vendor/GLFW/include/",
    "Titan/vendor/dxil/include/",
    "Titan/vendor/glm/",
    "Titan/vendor/YAML/include/",
    "Titan/vendor/Optick/src/",
    "%{VULKAN_SDK}/Include/"
}

externalLinks = 
{
    "GLFW",
    "yaml-cpp",
    "Optick",
    "d3d12.lib",
    "dxgi.lib",
    "dxguid.lib",
    "Titan/vendor/dxil/lib/dxcompiler.lib",
}

externalDebugLinks = 
{
    "%{VULKAN_SDK}/Lib/GenericCodeGend.lib",
    "%{VULKAN_SDK}/Lib/glslangd.lib",
    "%{VULKAN_SDK}/Lib/glslang-default-resource-limitsd.lib",
    "%{VULKAN_SDK}/Lib/HLSLd.lib",
    "%{VULKAN_SDK}/Lib/MachineIndependentd.lib",
    "%{VULKAN_SDK}/Lib/OGLCompilerd.lib",
    "%{VULKAN_SDK}/Lib/OSDependentd.lib",
    "%{VULKAN_SDK}/Lib/shaderc_combinedd.lib",
    "%{VULKAN_SDK}/Lib/shaderc.lib",
    "%{VULKAN_SDK}/Lib/shaderc_sharedd.lib",
    "%{VULKAN_SDK}/Lib/shaderc_utild.lib",
    "%{VULKAN_SDK}/Lib/shadercd.lib",
    "%{VULKAN_SDK}/Lib/SPIRVd.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-cd.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-cored.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-cppd.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-c-sharedd.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-glsld.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-hlsld.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-msld.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-reflectd.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-utild.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Toolsd.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-diffd.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-linkd.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-optd.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-reduced.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-sharedd.lib",
    "%{VULKAN_SDK}/Lib/SPVRemapperd.lib",
    "%{VULKAN_SDK}/Lib/VkLayer_utils.lib",
    "%{VULKAN_SDK}/Lib/vulkan-1.lib",
}

externalReleaseLinks = 
{
    "%{VULKAN_SDK}/Lib/GenericCodeGen.lib",
    "%{VULKAN_SDK}/Lib/glslang.lib",
    "%{VULKAN_SDK}/Lib/glslang-default-resource-limits.lib",
    "%{VULKAN_SDK}/Lib/HLSL.lib",
    "%{VULKAN_SDK}/Lib/MachineIndependent.lib",
    "%{VULKAN_SDK}/Lib/OGLCompiler.lib",
    "%{VULKAN_SDK}/Lib/OSDependent.lib",
    "%{VULKAN_SDK}/Lib/shaderc_combined.lib",
    "%{VULKAN_SDK}/Lib/shaderc.lib",
    "%{VULKAN_SDK}/Lib/shaderc_shared.lib",
    "%{VULKAN_SDK}/Lib/shaderc_util.lib",
    "%{VULKAN_SDK}/Lib/shaderc.lib",
    "%{VULKAN_SDK}/Lib/SPIRV.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-c.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-core.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-cpp.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-c-shared.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-glsl.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-hlsl.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-msl.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-reflect.lib",
    "%{VULKAN_SDK}/Lib/spirv-cross-util.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-diff.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-link.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-opt.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-reduce.lib",
    "%{VULKAN_SDK}/Lib/SPIRV-Tools-shared.lib",
    "%{VULKAN_SDK}/Lib/SPVRemapper.lib",
    "%{VULKAN_SDK}/Lib/VkLayer_utils.lib",
    "%{VULKAN_SDK}/Lib/vulkan-1.lib",
}

include "Titan/vendor/GLFW/"
include "Titan/vendor/glm/"
include "Titan/vendor/YAML/"
include "Titan/vendor/Optick/"



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
    linkoptions 
	{
		"/ignore:4006",
		"/ignore:4099"
	}

    defines {"_CONSOLE"}

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
        defines
        {
            "TN_CONFIG_DEBUG"
        }
        links
        {
            "Titan/vendor/freetype/lib/freetyped.lib",
            externalDebugLinks
        }

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        defines
        {
            "TN_CONFIG_RELEASE"
        }
        links
        {
            "Titan/vendor/freetype/lib/freetype.lib",
            externalReleaseLinks
        }

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
        defines
        {
            "TN_CONFIG_DIST"
        }
        links
        {
            "Titan/vendor/freetype/lib/freetype.lib",
            externalReleaseLinks
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
    disablewarnings { "4996" }
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
        "Titan/vendor/dx12helpers/**.cpp",
        "Titan/vendor/vma/**.h",
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
    linkoptions 
	{
		"/ignore:4006",
		"/ignore:4099"
	}
    
    filter "files:Titan/vendor/**.h"
        disablewarnings { "26451", "6387", "26812", "26439", "26800", "26495", "4717", "5232", "4067" }

    filter "files:Titan/vendor/**.cpp"
        flags {"noPCH"}
        disablewarnings { "26451", "6387", "26812", "26439", "26800", "26495", "4717", "5232", "4067" }

    filter "files:Titan/vendor/**.hpp"
        disablewarnings { "26451", "6387", "26812", "26439", "26800", "26495", "4717", "5232", "4067" }

    defines {"_CONSOLE"}
    
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        defines
        {
            "TN_CONFIG_DEBUG"
        }
        links
        {
            externalDebugLinks
        }
    
    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        defines
        {
            "TN_CONFIG_RELEASE"
        }
        links
        {
            externalReleaseLinks
        }
    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "off"
        defines
        {
            "TN_CONFIG_DIST"
        }
        links
        {
            externalReleaseLinks
        }
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
        "Titan/vendor/",
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
    linkoptions 
	{
		"/ignore:4006",
		"/ignore:4099"
	}
        
    defines {"_CONSOLE"}
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        defines
        {
            "TN_CONFIG_DEBUG"
        }
        links
        {
            "Titan/vendor/freetype/lib/freetyped.lib",
        }
        
    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        defines
        {
            "TN_CONFIG_RELEASE"
        }
        links
        {
            "Titan/vendor/freetype/lib/freetype.lib",
        }
        
    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "off"
        defines
        {
            "TN_CONFIG_DIST"
        }
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