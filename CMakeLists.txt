cmake_minimum_required(VERSION 3.20)

project(ConsoleGraphicsLib C)

set(
	SOURCES 
	main.c
	input.c
	cursor.c
	terminal.c
)

add_executable(ConsoleGraphicsLib ${SOURCES})
find_package(X11 REQUIRED)

target_link_libraries(ConsoleGraphicsLib X11::X11)

set(CFLAGS -DDEBUG -Wall)
target_compile_options(ConsoleGraphicsLib PRIVATE ${CFLAGS})