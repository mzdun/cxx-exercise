add_subdirectory(gtest)

# Switching the flags below will change the list of active
# tests in this project. This is prepared, so that your tests
# can grow with your solution.
#
# Of course, you can add any other test you deem fit.

# Have you written the exception classes?
set(HAS_JSON_EXCEPTIONS   NO)

# Do you provide the json::value_type enumeration?
set(HAS_JSON_VT           NO)

# Do you provide the json::value class, no matter which methods?
# (this flag will also try to use json::array in tests)
set(HAS_JSON_VALUE        NO)

# Do you provide the "rule-of-5" constructors and the conversion constructors?
set(HAS_JSON_CTORS        NO)

# Does your class support val.is<JSON_NUMBER>(), etc.?
set(HAS_JSON_IS_VT        NO)

# Does your class support val.is<long>(), etc.?
set(HAS_JSON_IS_CLASS     NO)

# Does your class support val.as_number(), etc.?
set(HAS_JSON_AS_DIRECT    NO)

# Does your class support val.as<JSON_NUMBER>(), etc.?
set(HAS_JSON_AS_VT        NO)

# Does your class support val.as<short>(), etc.?
set(HAS_JSON_AS_CLASS     NO)

# Do you provide a function named json::parse()?
set(HAS_JSON_PARSER       NO)

# Can your parser produce a nullptr from "null"?
set(HAS_JSON_PARSER_NULL  NO)

# Can your parser produce a number from "42"?
set(HAS_JSON_PARSER_NUM   NO)

# Can your parser produce a string from "\"str\""?
set(HAS_JSON_PARSER_STR   NO)

# Can your parser produce an array from "[]"?
set(HAS_JSON_PARSER_ARRAY NO)

# Can your parser recognize an empty document?
set(HAS_JSON_PARSER_EMPTY NO)


message(STATUS "-----------------------+-------")
message(STATUS " Library features      | Value")
message(STATUS "-----------------------+-------")
message(STATUS " has-exceptions        | ${HAS_JSON_EXCEPTIONS}")
message(STATUS " value_type            | ${HAS_JSON_VT}")
message(STATUS " value                 | ${HAS_JSON_VALUE}")
message(STATUS " value ctors           | ${HAS_JSON_CTORS}")
message(STATUS " val.is<JSON_NUMBER>() | ${HAS_JSON_IS_VT}")
message(STATUS " val.is<long long>()   | ${HAS_JSON_IS_CLASS}")
message(STATUS " val.as_number()       | ${HAS_JSON_AS_DIRECT}")
message(STATUS " val.as<JSON_NUMBER>() | ${HAS_JSON_AS_VT}")
message(STATUS " val.as<int>()         | ${HAS_JSON_AS_CLASS}")
message(STATUS " json::parse()         | ${HAS_JSON_PARSER}")
message(STATUS " json::parse() [NULL]  | ${HAS_JSON_PARSER_NULL}")
message(STATUS " json::parse() [NUM]   | ${HAS_JSON_PARSER_NUM}")
message(STATUS " json::parse() [STR]   | ${HAS_JSON_PARSER_STR}")
message(STATUS " json::parse() [ARRAY] | ${HAS_JSON_PARSER_ARRAY}")
message(STATUS " json::parse() [EMPTY] | ${HAS_JSON_PARSER_EMPTY}")
message(STATUS "-----------------------+-------")

set(TEST_SOURCES
  ctor_tests.cc
  conversion_tests.cc
  parse_tests.cc
  )

add_executable(tests gtest/googlemock/src/gmock_main.cc ${TEST_SOURCES})
target_link_libraries(tests json gmock)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(tests PRIVATE ${ADDITIONAL_WALL_FLAGS})
set_target_properties(tests PROPERTIES CXX_STANDARD 17)

configure_file(json_features.in.hh "${CMAKE_CURRENT_BINARY_DIR}/json_features.hh" @ONLY)
