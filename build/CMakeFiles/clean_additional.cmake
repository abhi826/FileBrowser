# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appLazyFileBrowserQML_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appLazyFileBrowserQML_autogen.dir\\ParseCache.txt"
  "appLazyFileBrowserQML_autogen"
  )
endif()
