project "yaml-cpp"
    kind "StaticLib"
    language "C++"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    disablewarnings { "4996", "26812" }
    
        files
        {
            "src/**.h",
            "src/**.cpp",
            
            "include/**.h"
        }
    
        includedirs
        {
            "include/"
        }
    
        filter "system:windows"
            systemversion "latest"
            cppdialect "C++17"
            staticruntime "off"
    
        filter "configurations:Debug"
            defines {"DEBUG"}
            runtime "Debug"
            symbols "on"
            
        filter "configurations:Release"
            defines "RELEASE"
            runtime "Release"
            optimize "on"