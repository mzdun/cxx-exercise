#add other files to the ${LIBRARY_SOURCES} variable:
set(LIBRARY_SOURCES
  src/value.cc
  include/json/value.hh
  include/json/json.hh
  )

add_library(json ${LIBRARY_SOURCES})
target_include_directories(json PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(json PRIVATE ${ADDITIONAL_WALL_FLAGS})
set_target_properties(json PROPERTIES CXX_STANDARD 11) # Modify as needed
