include(FetchContent)
project("PopiPopi")

set(CMAKE_CXX_STANDARD 23) #set C++ standard to C++23 (the best standard :p)

#Since i need to work on this on both my desktop and laptop, im using cmake to manage dependencies
#In particular imgui is difficult to setup with cmake, so im using cmmw's cmake file to handle glfw, glad, glm and imgui. 
#All other libraries should be added manually to the project
FetchContent_Declare(
        imgui-glfw-glad-glm
        GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
)
FetchContent_MakeAvailable(imgui-glfw-glad-glm)
#file(GLOB POPI_SOURCE "popi/src/*.cpp")
set(POPI_SOURCE "popi/src")
set(POPI_INCLUDE "popi/include")
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
)
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        "popi/include"
)

target_link_libraries(
        ${PROJECT_NAME}
        imgui
        glm
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/${RESOURCES} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${RESOURCES})
