cmake_minimum_required(VERSION 3.1)
project(p2p_network)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgmp")

set(CMAKE_CXX_STANDARD 14)
set(PROJECT_VERSION 1.0)
set(DEFAULT_BUILD_TYPE "Release")
FILE(GLOB SOURCE_FILES "src/*.cpp")

if(NOT TARGET nlohmann_json)
    add_subdirectory(lib/json)
    link_directories(lib/json)
    MESSAGE(STATUS "Connected: nlohmann_json")
endif()

include(GNUInstallDirs)
add_library(${PROJECT_NAME} ${SOURCE_FILES} include/network.h)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)
set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json)
