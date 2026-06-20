cmake_minimum_required(VERSION 3.20)
project(cx C)

add_library(scanner scanner.c)
add_library(parser parser.c)
add_executable(main main.c)
target_link_libraries(main PRIVATE scanner)