cmake_minimum_required(VERSION 4.0)
project(cx C)

add_library(scanner scanner.c)
add_executable(main main.c)
target_link_libraries(main PRIVATE scanner)