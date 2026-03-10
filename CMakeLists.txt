cmake_minimum_required(VERSION 3.21)
project(3dviewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)

qt_add_resources(SHADER_RESOURCES shaders.qrc)

add_executable(3dviewer
    src/main.cpp
    src/MainWindow.cpp
    src/GLWidget.cpp
    src/Camera.cpp
    src/Scene.cpp
    src/Renderer.cpp
    src/Mesh.cpp
    src/Model.cpp
    ${SHADER_RESOURCES}
)

target_link_libraries(3dviewer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    OpenGL::GL
    assimp::assimp
)

# GLM - try CONFIG mode first (vcpkg), fall back to system headers
find_package(glm CONFIG QUIET)
if(glm_FOUND)
    target_link_libraries(3dviewer PRIVATE glm::glm)
else()
    find_path(GLM_INCLUDE_DIR glm/glm.hpp REQUIRED)
    target_include_directories(3dviewer PRIVATE ${GLM_INCLUDE_DIR})
endif()
