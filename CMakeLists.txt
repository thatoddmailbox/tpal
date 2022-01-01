cmake_minimum_required(VERSION 3.9)
project(tpal)

file(GLOB TPAL_PUBLIC_HEADERS include/*.h)
# TODO: make source list smarter
file(GLOB TPAL_SRC src/*.c)

add_library(tpal STATIC ${TPAL_SRC})

set_target_properties(tpal PROPERTIES PUBLIC_HEADER "${TPAL_PUBLIC_HEADERS}")
target_include_directories(tpal PRIVATE include_private)