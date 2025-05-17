cmake_minimum_required(VERSION 3.20)
set(C_STANDARD 11)
project(ConsoleGraphicsLib C)

set(
	SOURCES 
	main.c
	input.c
	cursor.c
	terminal.c
	buffer.c
	render.c
	render_utils.c
	mem.c
	coremath.c
	dev.c
	thread.c
	ctx.c
	charmap.c
	light.c
	color.c
)

add_executable(ConsoleGraphicsLib ${SOURCES})

find_package(X11 REQUIRED)
find_library(LIBTERMCAP termcap REQUIRED)
find_package(Curses REQUIRED)
find_package(Freetype REQUIRED)

target_link_libraries(ConsoleGraphicsLib X11::X11 ${LIBTERMCAP} ncursesw m pthread ${FREETYPE_LIBRARIES})
target_include_directories(ConsoleGraphicsLib PRIVATE ${FREETYPE_INCLUDE_DIRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG -Ofast -Wall -Wextra -Wpedantic -Werror -Wall -msse4.2 -fopenmp -flto")
