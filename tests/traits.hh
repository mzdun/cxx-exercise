#pragma once

namespace json::testing {
#if HAS_JSON_EXCEPTIONS
	template <value_type TYPE> struct bad_cast_exception;
	template <value_type TYPE> using bad_cast_exception_t = typename bad_cast_exception<TYPE>::type;
	template <> struct bad_cast_exception<JSON_NUMBER> { using type = not_a_number; };
	template <> struct bad_cast_exception<JSON_STRING> { using type = not_a_string; };
	template <> struct bad_cast_exception<JSON_ARRAY> { using type = not_an_array; };
#endif

	template <typename Int> struct is_int : std::false_type {};
	template <typename Int> constexpr bool is_int_v = is_int<Int>::value;

	template <> struct is_int<long long> : std::true_type {};
	template <> struct is_int<long> : std::true_type {};
	template <> struct is_int<int> : std::true_type {};
	template <> struct is_int<short> : std::true_type {};

	template <value_type TYPE>
	using vtid = std::integral_constant<value_type, TYPE>;
	template <typename Arg, typename = void> struct type_to_vt;
	template <typename Arg> constexpr value_type type_to_vt_v = type_to_vt<Arg>::value;
	template <> struct type_to_vt<std::nullptr_t> : vtid<JSON_NULL> {};
	template <> struct type_to_vt<std::string> : vtid<JSON_STRING> {};
	template <> struct type_to_vt<std::vector<value>> : vtid<JSON_ARRAY> {};
	template <typename Int> struct type_to_vt<Int, std::enable_if_t<is_int_v<Int>>> : vtid<JSON_NUMBER> {};
}
