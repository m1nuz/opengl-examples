include_directories("./include")
include_directories("./src")

add_definitions(-DUSING_SDL)

add_library(GLcoreES2 STATIC include/glcore_es2.h src/glcore_es2.c)
add_library(GLcore330 STATIC include/glcore_330.h src/glcore_330.c)
add_library(GLcore450 STATIC include/glcore_450.h src/glcore_450.c)
