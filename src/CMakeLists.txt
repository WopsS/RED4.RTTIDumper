add_library(RED4.RTTIDumper SHARED)

file(GLOB_RECURSE HEADER_FILES *.hpp)
file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE RC_FILES *.rc)

source_group(_CMake REGULAR_EXPRESSION cmake_pch.*)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES} ${RC_FILES})

target_include_directories(RED4.RTTIDumper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(RED4.RTTIDumper PRIVATE ${HEADER_FILES} ${SOURCE_FILES} ${RC_FILES})

target_precompile_headers(RED4ext.RTTIDumper PRIVATE stdafx.hpp)
target_link_libraries(RED4ext.RTTIDumper PRIVATE nlohmann_json::nlohmann_json RED4ext::SDK)
