cmake_minimum_required(VERSION 2.8)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories("./lib/GL/include")
include_directories("./lib/GLcore/include")
include_directories("./lib/glm")

add_subdirectory(./lib/GLcore)

set(project_name "opengl4.5-example")
set(libs -lSDL2 GLcore450)
set(c_flags "-std=c11 -pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes")
set(cpp_flags "-std=c++11 -pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${c_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${cpp_flags}")

add_executable(example-450-01 WIN32 src/main.c src/example1.cpp)
target_compile_definitions(example-450-01 PRIVATE -DAPP_TITLE="Example 1: Triangles")
target_link_libraries(example-450-01 ${libs})

add_executable(example-450-02 WIN32 src/targa.c src/main.c src/example2.cpp)
target_compile_definitions(example-450-02 PRIVATE -DAPP_TITLE="Example 2: Rotating textured cube")
target_link_libraries(example-450-02 ${libs})

add_executable(example-450-03 WIN32 src/targa.c src/main.c src/example3.cpp)
target_compile_definitions(example-450-03 PRIVATE -DAPP_TITLE="Example 3: Instanced cubes")
target_link_libraries(example-450-03 ${libs})
