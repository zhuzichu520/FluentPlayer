# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "example/CMakeFiles/example_autogen.dir/AutogenUsed.txt"
  "example/CMakeFiles/example_autogen.dir/ParseCache.txt"
  "example/example_autogen"
  "src/CMakeFiles/fluentuiplugin_autogen.dir/AutogenUsed.txt"
  "src/CMakeFiles/fluentuiplugin_autogen.dir/ParseCache.txt"
  "src/fluentuiplugin_autogen"
  )
endif()
