cmake_minimum_required(VERSION 3.10)

# προσθέτο τα cpp files
project(Fractal_Generator VERSION 3.5)
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE SRC_IMGUI_FILES ${CMAKE_SOURCE_DIR}/src/imgui/*.cpp)
# φτιάνω το executable
add_executable(Fractal_Generator main.cpp ${SRC_FILES} ${SRC_IMGUI_FILES} glad.c)

# προσθέτω ότι directories χρειάζομαι
target_include_directories(
    Fractal_Generator
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include/imgui
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glad
    ${CMAKE_SOURCE_DIR}/include/KHR
)

link_directories(${PROJECT_FOLDER}/src/glfw)
target_link_libraries(Fractal_Generator PRIVATE glfw)

# για να φτίαξω το πρόγραμα γράφω τα παρακάτω:
# cd build
# cmake ..
# make
# ./Fractal_Generator