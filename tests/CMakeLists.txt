# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved


# Add a test program.
macro(add_beaker_test target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} beaker)
endmacro()

add_subdirectory(ast)
add_subdirectory(lex)
