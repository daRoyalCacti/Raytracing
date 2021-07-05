cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project("Raytracer")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

set(Header_files aabb.hpp aarect.hpp box.hpp bvh.hpp camera.hpp color.hpp helpful.hpp constant_medium.hpp Halton.hpp hittable.hpp hittable_list.hpp material.hpp moving_sphere.hpp ONB.hpp pdf.hpp perlin.hpp probability.hpp ray.hpp render.hpp scenes.hpp sphere.hpp texture.hpp triangle.hpp triangle_mesh.hpp vec2.hpp vec3.hpp)
add_executable(Generate main.cpp ${Header_files})

find_package(OpenMP REQUIRED)
find_package(PNG REQUIRED)
find_package(ASSIMP REQUIRED)


target_link_libraries(Generate PUBLIC PNG::PNG)
target_link_libraries(Generate PUBLIC assimp)
if(OpenMP_CXX_FOUND)
	target_link_libraries(Generate PUBLIC OpenMP::OpenMP_CXX)
endif()

#making static build
#https://stackoverflow.com/questions/7101862/want-to-make-standalone-program-with-cmake
#set(BUILD_SHARED_LIBS OFF)
#set(BUILD_STATIC_LIBS ON)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
#if (MSVC)

#else()
	add_compile_options("$<$<CONFIG:Debug>:-Wall -Wextra -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wmissing-include-dirs -Wunknown-pragmas -Wduplicated-cond -Wfloat-equal -Wshadow -Wunsafe-loop-optimizations -Wpacked -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wmissing-noreturn -Wsuggest-attribute=malloc -Wsuggest-attribute=format -Wmissing-format-attribute -Wsuggest-attribute=cold>")
	add_compile_options("$<$<CONFIG:Release>:-Ofast -march=native")
#endif()
