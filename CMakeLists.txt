# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)
project (Eruption LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LVK_WITH_GLFW ON CACHE BOOL "" FORCE)
set(LVK_WITH_SAMPLES OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/lightweightvk)
add_subdirectory(third_party/assimp)
add_subdirectory(third_party/glm)

add_executable(Eruption
	source/main.cpp
	source/Utils.cpp
	source/Utils.h
)

target_link_libraries(Eruption PRIVATE LVKLibrary glfw assimp glm::glm)

set(LVK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/lightweightvk")
set(LVK_DEPS "${LVK_ROOT}/third-party/deps/src")

target_include_directories(Eruption PRIVATE
	${LVK_ROOT}
	${LVK_DEPS}
	${LVK_DEPS}/ldrutils
	${LVK_DEPS}/ldrutils/include
	${LVK_DEPS}/minilog
)

target_compile_definitions(Eruption PRIVATE LVK_WITH_GLFW=1)
target_compile_definitions(Eruption PRIVATE
	SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
	MODEL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

if(MSVC)
	target_compile_options(Eruption PRIVATE /permissive-)
endif()