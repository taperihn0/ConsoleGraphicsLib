cmake_minimum_required(VERSION 3.20)

project(ConsoleGraphicsLib C)

set(
	SOURCES 
	main.c
)

add_executable(ConsoleGraphicsLib ${SOURCES})

set(CFLAGS -DDEBUG -Wall)
target_compile_options(ConsoleGraphicsLib PRIVATE ${CFLAGS})