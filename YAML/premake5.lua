project "yaml-cpp"
    kind "StaticLib"
    language "C++"
    location "%{wks.location}/YAML"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    disablewarnings { "4996", "26812" }
    
        files
        {
            "%{wks.location}/YAML/src/**.h",
            "%{wks.location}/YAML/src/**.cpp",
            
            "%{wks.location}/YAML/include/**.h"
        }
    
        includedirs
        {
            "%{wks.location}/YAML/include"
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