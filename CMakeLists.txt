cmake_minimum_required(VERSION 3.10)

project(KEngine2D)

add_library(KEngine2D STATIC
Boundaries2D.h
Boundaries2D.cpp
Collision2D.h
Collision2D.cpp
HierarchicalTransform2D.h
HierarchicalTransform2D.cpp
MechanicalTransform2D.h
MechanicalTransform2D.cpp
Physics2D.h
Physics2D.cpp
Renderer2D.h
RendererLuaBinding.h
RendererLuaBinding.cpp
StaticTransform2D.h
StaticTransform2D.cpp
Transform2D.h
Transform2D.cpp
)


target_include_directories(KEngine2D PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(KEngine2D PUBLIC KEngineCore)
