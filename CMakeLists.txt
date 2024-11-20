
cmake_minimum_required(VERSION 3.19)

# Project configuration
project(main VERSION 0.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Collect source files
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

# Print source files (optional for debugging purposes)
message("Source files:")
foreach(file ${SRCS})
    message(STATUS ${file})
endforeach()

add_compile_options(
    -O0
    -mavx2
    -pipe
)

# Define the main executable
add_executable(${PROJECT_NAME} ${SRCS})

# Include directories for the main project
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add warning and sanitizer flags specifically for the main project
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wno-nullability-completeness
    #    -Wall
    -Wwrite-strings
    -Wstrict-prototypes
    #-Wmissing-declarations
    -fno-common
    -Wtype-limits
    -Wformat-nonliteral
    -fno-omit-frame-pointer
    #-fsanitize=address
)

# Add linker options for the main project
target_link_options(${PROJECT_NAME} PRIVATE 
    #-fsanitize=address
)

# Add debugging symbols in Debug builds
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
endif()

# Add SDL3 as a dependency
add_subdirectory(third_party/SDL)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)
