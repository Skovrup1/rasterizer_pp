cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nullability-completeness")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wwrite-strings -Wstrict-prototypes -Wmissing-declarations -fno-common -mavx2 -pipe -Wtype-limits -Wformat-nonliteral"
                                    )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")

project(main VERSION 0.0.0 LANGUAGES CXX)

file(GLOB SRCS
    src/main.cpp
    src/core.hpp
    src/display.cpp
    src/display.hpp
    src/vector.cpp
    src/vector.hpp
    src/mesh.cpp
    src/mesh.hpp
    src/triangle.cpp
    src/triangle.hpp
    src/matrix.hpp
    src/texture.hpp
)

message("src files:")
foreach(file ${SRCS})
    message(STATUS ${file})
endforeach()

add_executable(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

#
# Linking
#

add_subdirectory(third_party/SDL)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

