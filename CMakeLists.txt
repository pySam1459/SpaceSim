cmake_minimum_required(VERSION 3.22)
project(SpaceSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_executable(${PROJECT_NAME} ${SRC})

find_package(glfw3 3.4 REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glad::glad
    glm::glm)
target_include_directories(${PROJECT_NAME} PRIVATE include)
