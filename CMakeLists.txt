cmake_minimum_required(VERSION 3.9)
project(tpal)

file(GLOB TPAL_PUBLIC_HEADERS include/*.h)

list(APPEND TPAL_SRC src/init.c)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_definitions(-DTPAL_PLATFORM_WINDOWS)

	list(APPEND TPAL_SRC src/dialog_windows.c)

	list(APPEND TPAL_SRC src/user_windows.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_definitions(-DTPAL_PLATFORM_LINUX)

	list(APPEND TPAL_SRC src/dialog_linux_dummy.c)
	list(APPEND TPAL_SRC src/dialog_linux_gtk3.c)
	list(APPEND TPAL_SRC src/dialog_linux.c)

	list(APPEND TPAL_SRC src/user_linux.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	add_definitions(-DTPAL_PLATFORM_MACOS)

	list(APPEND TPAL_SRC src/dialog_macos.c)

	list(APPEND TPAL_SRC src/user_macos.m)
endif()

add_library(tpal STATIC ${TPAL_SRC})

set_target_properties(tpal PROPERTIES PUBLIC_HEADER "${TPAL_PUBLIC_HEADERS}")
target_include_directories(tpal PRIVATE include)
target_include_directories(tpal PRIVATE include_private)

target_link_libraries(tpal ${CMAKE_DL_LIBS})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	# depend on gtk, but only for the include files
	find_package(PkgConfig)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
	target_include_directories(tpal PRIVATE ${GTK3_INCLUDE_DIRS})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	target_link_libraries(tpal "-framework AppKit")
endif()