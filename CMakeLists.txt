cmake_minimum_required(VERSION 3.20)

include(FetchContent)
project("PopiPopi")

set(CMAKE_CXX_STANDARD 23) #set C++ standard to C++23 (the best standard :p)

#Since i need to work on this on both my desktop and laptop, im using cmake to manage dependencies
#In particular imgui is difficult to setup with cmake, so im using a forked version cmmw's cmake file to handle glfw, glad, glm and imgui. 

# Explicitly set ImGui to use the docking branch
set(IGG_IMGUI_TAG "docking" CACHE STRING "ImGui branch/tag to use" FORCE)

FetchContent_Declare(
        imgui-glfw-glad-glm
        GIT_REPOSITORY https://github.com/AlexZades/imguidocking-glfw-glad-glm.git
)
FetchContent_MakeAvailable(imgui-glfw-glad-glm)

#file(GLOB POPI_SOURCE "popi/src/*.cpp")
set(POPI_SOURCE "popi/engine/src")
set(POPI_INCLUDE "popi/engine/include")
set(RESOURCES "resources")

#add_executable(app ${POPI_SOURCE}/importer.cpp ${POPI_SOURCE}/graphics.cpp ${POPI_SOURCE}/main.cpp ${POPI_SOURCE}/UI.cpp)
add_executable(${PROJECT_NAME}
        ${POPI_SOURCE}/importer.cpp
        ${POPI_SOURCE}/graphics.cpp
        ${POPI_SOURCE}/main.cpp
        ${POPI_SOURCE}/UI.cpp
        ${POPI_INCLUDE}/importer.h
        ${POPI_INCLUDE}/graphics.h
        ${POPI_INCLUDE}/main.h
        ${POPI_INCLUDE}/UI.h
        ${POPI_INCLUDE}/settings.h
        ${POPI_INCLUDE}/project.h
 "popi/engine/src/project.cpp" "popi/engine/include/utils.h" "popi/engine/src/utils.cpp" "popi/engine/include/components.h"  "popi/engine/include/entity.h" "popi/engine/include/scene.h" "popi/engine/include/terrain.h" "popi/engine/src/terrain.cpp" "popi/engine/include/input.h" "popi/engine/include/camera.h" "popi/engine/src/camera.cpp" "popi/engine/src/entity.cpp")
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        "popi/engine/include"
        "lib/fastNoiseLite"
        "lib/stb"
)

target_link_libraries(
        ${PROJECT_NAME}
        imgui
        glm
        
)
#update version number
file(READ "${CMAKE_SOURCE_DIR}/${RESOURCES}/ver" CURRENT_VERSION)
string(REGEX MATCH "([0-9]+$)" BUILD_NUMBER "${CURRENT_VERSION}")
math(EXPR NEW_BUILD_NUMBER "${BUILD_NUMBER} + 1")
string(REGEX REPLACE "([0-9]+$)" "${NEW_BUILD_NUMBER}" NEW_VERSION "${CURRENT_VERSION}")
file(WRITE "${CMAKE_SOURCE_DIR}/${RESOURCES}/ver" "${NEW_VERSION}")


#Copy resources to the build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/${RESOURCES} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${RESOURCES})



