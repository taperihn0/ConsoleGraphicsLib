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


project(hhpc C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../hhpc/")
add_executable(hhpc hhpc/hhpc.c)

target_link_libraries(hhpc X11::X11)
target_compile_options(hhpc PRIVATE -Wall)