#define CMAKE_TRUE 1
#define CMAKE_YES 1
#define CMAKE_1 1
#define CMAKE_FALSE 0
#define CMAKE_NO 0
#define CMAKE_0 0
#define CMAKE_ does_not_compute{}

#define HAS_JSON_VT           CMAKE_@HAS_JSON_VT@
#define HAS_JSON_VALUE        CMAKE_@HAS_JSON_VALUE@
#define HAS_JSON_CTORS        CMAKE_@HAS_JSON_CTORS@
#define HAS_JSON_GET_TYPE     CMAKE_@HAS_JSON_GET_TYPE@
#define HAS_JSON_IS_VT        CMAKE_@HAS_JSON_IS_VT@
#define HAS_JSON_IS_CLASS     CMAKE_@HAS_JSON_IS_CLASS@
#define HAS_JSON_AS_DIRECT    CMAKE_@HAS_JSON_AS_DIRECT@
#define HAS_JSON_AS_VT        CMAKE_@HAS_JSON_AS_VT@
#define HAS_JSON_AS_CLASS     CMAKE_@HAS_JSON_AS_CLASS@
#define HAS_JSON_EXCEPTIONS   CMAKE_@HAS_JSON_EXCEPTIONS@

#define HAS_JSON_PARSER       CMAKE_@HAS_JSON_PARSER@
#define HAS_JSON_PARSER_NULL  CMAKE_@HAS_JSON_PARSER_NULL@
#define HAS_JSON_PARSER_NUM   CMAKE_@HAS_JSON_PARSER_NUM@
#define HAS_JSON_PARSER_STR   CMAKE_@HAS_JSON_PARSER_STR@
#define HAS_JSON_PARSER_ARRAY CMAKE_@HAS_JSON_PARSER_ARRAY@
#define HAS_JSON_PARSER_EMPTY CMAKE_@HAS_JSON_PARSER_EMPTY@

#if !HAS_JSON_VALUE
#undef HAS_JSON_CTORS
#undef HAS_JSON_IS_VT
#undef HAS_JSON_IS_CLASS
#undef HAS_JSON_AS_DIRECT
#undef HAS_JSON_AS_VT
#undef HAS_JSON_AS_CLASS

#define HAS_JSON_CTORS      CMAKE_NO
#define HAS_JSON_GET_TYPE   CMAKE_NO
#define HAS_JSON_IS_VT      CMAKE_NO
#define HAS_JSON_IS_CLASS   CMAKE_NO
#define HAS_JSON_AS_DIRECT  CMAKE_NO
#define HAS_JSON_AS_VT      CMAKE_NO
#define HAS_JSON_AS_CLASS   CMAKE_NO
#endif // !HAS_JSON_VALUE
