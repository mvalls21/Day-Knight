cmake_minimum_required(VERSION 3.8)

project(day-knight)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        Projecte/Game.cpp
        Projecte/Scene.cpp

        Projecte/Player.cpp
        Projecte/Skeleton.cpp
        Projecte/Vampire.cpp

        Projecte/Shader.cpp
        Projecte/ShaderProgram.cpp
        Projecte/Sprite.cpp
        Projecte/StaticSprite.cpp
        Projecte/Texture.cpp
        Projecte/TileMap.cpp

        Projecte/main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC Projecte/)

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Projecte/shaders/
        ${CMAKE_BINARY_DIR}/shaders/)

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Projecte/levels/
        ${CMAKE_BINARY_DIR}/levels/)

add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Projecte/images/
        ${CMAKE_BINARY_DIR}/images/)

add_subdirectory(vendor/soil/)
target_include_directories(${PROJECT_NAME} PUBLIC vendor/soil/src/)
target_link_libraries(${PROJECT_NAME} PUBLIC SOIL)

add_subdirectory(vendor/glm/)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

add_subdirectory(vendor/glew/)
target_link_libraries(${PROJECT_NAME} PUBLIC libglew_shared)

add_subdirectory(vendor/freeglut/)
target_link_libraries(${PROJECT_NAME} PUBLIC glut)
