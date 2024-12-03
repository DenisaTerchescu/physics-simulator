#The CMake integration credits go to: https://github.com/meemknight/raylibCmakeSetup
#Version 1.0.0

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)


if(MSVC) 
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_compile_options(/arch:AVX2) 
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) 

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Raylib as static libraries" FORCE)

# Set the OpenGL version
set(OPENGL_VERSION "4.3" CACHE STRING "Specify OpenGL version for raylib" FORCE)
option(SUPPORT_GLFW "Use GLFW as window/context manager" ON)
option(SUPPORT_GLAD "Use glad" OFF)

project(physicsSimulator)


add_subdirectory(thirdparty/raylib-5.0)
add_subdirectory(thirdparty/imgui-docking)
add_subdirectory(thirdparty/rlImGui)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/glad)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")


add_executable("${CMAKE_PROJECT_NAME}")

set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 17)



if(PRODUCTION_BUILD)
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/") 

	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 

else()
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 

endif()

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )


if(MSVC) 

	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)

	
endif()

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE 
	raylib_static imgui rlimgui glm glad glfw
)
