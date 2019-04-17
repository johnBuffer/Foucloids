cmake_minimum_required(VERSION 3.5)
project(Foucloids VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenGL)
set(SOURCES "src/main.cpp")
target_include_directories(foucloids PRIVATE "include")
add_definitions(-DSFML_STATIC)

if(WIN32)
    set(SFML_DIR "" CACHE PATH "SFML lib path")
    set(SFML_LIB_DIR "${SFML_DIR}/lib")
    set(SFML_INC_DIR "${SFML_DIR}/include")


    set(SFML_LIBS "${SFML_LIB_DIR}/sfml-graphics-s.lib"
        "${SFML_LIB_DIR}/sfml-window-s.lib"
        "${SFML_LIB_DIR}/sfml-system-s.lib"
        "${SFML_LIB_DIR}/freetype.lib"
        "${OPENGL_LIBRARIES}"
        "winmm.lib"
    )

    target_include_directories(foucloids PRIVATE "${SFML_INC_DIR}")
    target_link_libraries(foucloids ${SFML_LIBS})
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
    find_package(SFML COMPONENTS graphics window system REQUIRED)
    target_include_directories(foucloids PRIVATE "${SFML_INCLUDE_DIR}")
    target_link_libraries(foucloids ${SFML_LIBRARIES})
endif()

add_executable(foucloids ${SOURCES})



