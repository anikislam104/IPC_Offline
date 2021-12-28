cmake_minimum_required(VERSION 3.21)
project(IPC_Offline C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS -pthread)
add_executable(IPC_Offline main.c)
add_executable(Practice ThreadCreate.c)
add_executable(ArgPassing ArgPassing.c)
add_executable(MultArgPassing MultArgPassing.c)
add_executable(ArgPassingError ArgPassingError.c)

