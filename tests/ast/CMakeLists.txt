# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved


# Add a test program.
macro(add_beaker_test target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} beaker)
endmacro()


# Unit tests
add_beaker_test(test-ast-assert-1 assert-1.cpp)
add_beaker_test(test-ast-assert-2 assert-2.cpp) # NOTE: Expected to abort.
add_beaker_test(test-ast-var-1 var-1.cpp)
add_beaker_test(test-ast-var-2 var-2.cpp)
add_beaker_test(test-ast-var-3 var-3.cpp)
add_beaker_test(test-ast-assign-1 assign-1.cpp)
add_beaker_test(test-ast-fn-1 fn-1.cpp)
add_beaker_test(test-ast-fn-2 fn-2.cpp)
add_beaker_test(test-ast-fn-3 fn-3.cpp)


add_beaker_test(test-ast-logic-1 logic-1.cpp)
# add_beaker_test(test-ast-numeric-1 numeric-1.cpp)