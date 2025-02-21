cmake_minimum_required(VERSION 3.20)

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
)

add_executable(ConsoleGraphicsLib ${SOURCES})

find_package(X11 REQUIRED)
find_library(LIBTERMCAP termcap REQUIRED)
find_package(Curses REQUIRED)

target_link_libraries(ConsoleGraphicsLib X11::X11 ${LIBTERMCAP} ncursesw m)

set(CFLAGS -Wall -Ofast)
target_compile_options(ConsoleGraphicsLib PRIVATE ${CFLAGS})