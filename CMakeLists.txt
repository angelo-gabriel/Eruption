# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)
project (Vulkano LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LVK_WITH_GLFW ON CACHE BOOL "" FORCE)
set(LVK_WITH_SAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/lightweightvk)

add_executable(Vulkano
	source/main.cpp
	source/Utils.cpp
	source/Utils.h
)

target_link_libraries(Vulkano PRIVATE LVKLibrary glfw)

set(LVK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/lightweightvk")
set(LVK_DEPS "${LVK_ROOT}/third-party/deps/src")

target_include_directories(Vulkano PRIVATE
	${LVK_ROOT}
	${LVK_DEPS}
	${LVK_DEPS}/ldrutils
	${LVK_DEPS}/ldrutils/include
	${LVK_DEPS}/minilog
)

target_compile_definitions(Vulkano PRIVATE LVK_WITH_GLFW=1)
target_compile_definitions(Vulkano PRIVATE
	SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources")

if(MSVC)
	target_compile_options(Vulkano PRIVATE /permissive-)
endif()