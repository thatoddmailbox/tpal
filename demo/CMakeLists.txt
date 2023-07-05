cmake_minimum_required(VERSION 3.9)
project(tpal-demo)

list(APPEND SOURCES "main.c")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ../include)

# depend on tpal as a cmake subdirectory
# this way, tpal gets rebuilt along with the demo, which is convenient for testing
add_subdirectory(../ tpal/)
target_link_libraries(${PROJECT_NAME} tpal)
