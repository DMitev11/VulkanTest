cmake_minimum_required(VERSION 3.15)
project(VulkanTest)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

add_subdirectory("./lib")
include ("./config.cmake")

add_subdirectory("./inc")
add_subdirectory("./src")


set(SOURCE_FILES 
    ./main.cpp
) 
add_executable(main ${SOURCE_FILES}) 

include ("./exec_config.cmake")
