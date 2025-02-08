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
	render_core.c
	mem.c
	gmath.c
)

add_executable(ConsoleGraphicsLib ${SOURCES})
find_package(X11 REQUIRED)
find_library(TERMCAP_LIB termcap REQUIRED)
find_package(Curses REQUIRED)

# ncurses aren't needed for now
target_link_libraries(ConsoleGraphicsLib X11::X11 ${TERMCAP_LIB} ncursesw m)

set(CFLAGS -DDEBUG -Wall -O2)
target_compile_options(ConsoleGraphicsLib PRIVATE ${CFLAGS})


project(hhpc C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../hhpc/")
add_executable(hhpc hhpc/hhpc.c)

target_link_libraries(hhpc X11::X11)
target_compile_options(hhpc PRIVATE -Wall)