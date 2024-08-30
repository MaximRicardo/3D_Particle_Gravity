cmake_minimum_required(VERSION 3.10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(gravity_sim VERSION 1.0)

add_compile_options(-Wall -Wextra -Wpedantic -O3)

# For debugging
# add_compile_options(-Wall -Wextra -Wpedantic -ggdb3 -fsanitize=address)
# add_link_options(-fsanitize=address)

# For debugging with valgrind
# Don't forget to enable g++ instead of clang++ aswell, since valgrind breaks
# with clang++
# add_compile_options(-Wall -Wextra -Wpedantic -g -fno-omit-frame-pointer)

file(GLOB_RECURSE Sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(gravity_sim "${Sources}")

# target_include_directories(gravity_sim PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(gravity_sim -lraylib -lGL -lm -lpthread -ldl -lrt -lX11)
