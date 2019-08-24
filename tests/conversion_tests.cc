#include <gtest/gtest.h>
#include "json_features.hh"
#include <json/json.hh>
#include "io.hh"
#include "traits.hh"

namespace json::testing {
#if HAS_JSON_EXCEPTIONS && HAS_JSON_VT && HAS_JSON_VALUE
	using namespace std::literals;

	struct JsonConversion : ::testing::Test {
		template <value_type ExpectedType, value_type CurrentType, value_type ... Types>
		void cast_tests([[maybe_unused]] value const& val) {
#if HAS_JSON_IS_VT
			EXPECT_EQ(ExpectedType == CurrentType, val.is<CurrentType>());
#endif // HAS_JSON_IS_VT

#if HAS_JSON_AS_VT
			if constexpr (ExpectedType == CurrentType) {
				EXPECT_NO_THROW(val.as<CurrentType>()) << " Type is: " << ValueType{ CurrentType };
			} else {
				EXPECT_THROW(val.as<CurrentType>(), bad_cast_exception_t<CurrentType>) << " Type is: " << ValueType{ CurrentType };
			}
#endif // HAS_JSON_AS_VT

			if constexpr (sizeof...(Types) > 0)
				cast_tests<ExpectedType, Types...>(val);
		}

		template <value_type ExpectedType, typename CurrentType, typename ... FurtherTypes>
		void type_cast_tests([[maybe_unused]] value const& val) {
			[[maybe_unused]] constexpr auto ActualType = type_to_vt_v<CurrentType>;

#if HAS_JSON_IS_CLASS
			EXPECT_EQ(ExpectedType == ActualType, val.is<CurrentType>());
#endif // HAS_JSON_IS_VT

#if HAS_JSON_AS_CLASS
			if constexpr (ExpectedType == ActualType) {
				EXPECT_NO_THROW(val.as<CurrentType>())
					<< " Type is: " << ActualTypeTag<CurrentType>{};
			} else {
				EXPECT_THROW(val.as<CurrentType>(), bad_cast_exception_t<ActualType>)
					<< " Type is: " << ActualTypeTag<CurrentType>{};
			}
#endif // HAS_JSON_AS_CLASS

			if constexpr (sizeof...(FurtherTypes) > 0)
				type_cast_tests<ExpectedType, FurtherTypes...>(val);
		}

		template <value_type ExpectedType>
		void casting(value const& val) {
#if HAS_JSON_GET_TYPE
			EXPECT_EQ(ExpectedType, val.get_type()); // sanity check
#endif // HAS_JSON_GET_TYPE
			cast_tests<ExpectedType, JSON_NUMBER, JSON_STRING, JSON_ARRAY>(val);
			type_cast_tests<ExpectedType, long long, long, int, short, std::string
#if HAS_JSON_VT && HAS_JSON_VALUE
				, json::array
#endif
			>(val);
		}
	};

#define CASTING_TEST_MAKES_SENSE (HAS_JSON_CTORS && (HAS_JSON_GET_TYPE || HAS_JSON_IS_VT || HAS_JSON_AS_VT || HAS_JSON_IS_CLASS || HAS_JSON_AS_CLASS))
#if CASTING_TEST_MAKES_SENSE
	TEST_F(JsonConversion, FromNull) {
		casting<JSON_NULL>(nullptr);
	}

	TEST_F(JsonConversion, FromInt) {
		casting<JSON_NUMBER>(0ll);
		casting<JSON_NUMBER>(42);
	}

	TEST_F(JsonConversion, FromString) {
		casting<JSON_STRING>("fee fi fo"s);
	}

	TEST_F(JsonConversion, FromArray) {
		casting<JSON_ARRAY>(array{});
		casting<JSON_ARRAY>(array{ "fee fi fo"s, 5, {} });
	}
#endif // CASTING_TEST_MAKES_SENSE
#endif // HAS_JSON_EXCEPTIONS

#if HAS_JSON_VALUE && HAS_JSON_AS_DIRECT
	static_assert(
		std::is_same_v<std::string, decltype(std::declval<value&&>().as_string())>,
		"R-value version of as_string should be a moved-away string"
		);

	static_assert(
		std::is_same_v<std::string const&, decltype(std::declval<value&>().as_string())>,
		"L-value version of as_string should be a reference to a const string"
		);

	static_assert(
		std::is_same_v<array, decltype(std::declval<value&&>().as_array())>,
		"R-value version of as_array should be a moved-away string"
		);

	static_assert(
		std::is_same_v<array const&, decltype(std::declval<value&>().as_array())>,
		"L-value version of as_array should be a reference to a const string"
		);
#endif // HAS_JSON_VALUE && HAS_JSON_AS_DIRECT

#if HAS_JSON_VALUE && HAS_JSON_AS_VT
	static_assert(
		std::is_same_v<std::string, decltype(std::declval<value&&>().as<JSON_STRING>())>,
		"R-value version of as<JSON_STRING> should be a moved-away string"
		);

	static_assert(
		std::is_same_v<std::string const&, decltype(std::declval<value&>().as<JSON_STRING>())>,
		"L-value version of as<JSON_STRING> should be a reference to a const string"
		);

	static_assert(
		std::is_same_v<array, decltype(std::declval<value&&>().as<JSON_ARRAY>())>,
		"R-value version of as<JSON_ARRAY> should be a moved-away string"
		);

	static_assert(
		std::is_same_v<array const&, decltype(std::declval<value&>().as<JSON_ARRAY>())>,
		"L-value version of as<JSON_ARRAY> should be a reference to a const string"
		);
#endif // HAS_JSON_AS_VT

#if HAS_JSON_VALUE && HAS_JSON_AS_CLASS
	static_assert(
		std::is_same_v<std::string, decltype(std::declval<value&&>().as<std::string>())>,
		"R-value version of as<std::string> should be a moved-away string"
		);

	static_assert(
		std::is_same_v<std::string const&, decltype(std::declval<value&>().as<std::string>())>,
		"L-value version of as<std::string> should be a reference to a const string"
		);

	static_assert(
		std::is_same_v<array, decltype(std::declval<value&&>().as<array>())>,
		"R-value version of as<array> should be a moved-away string"
		);

	static_assert(
		std::is_same_v<array const&, decltype(std::declval<value&>().as<array>())>,
		"L-value version of as<array> should be a reference to a const string"
		);
#endif // HAS_JSON_AS_CLASS
}
