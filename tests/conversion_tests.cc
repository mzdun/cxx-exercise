#include <gtest/gtest.h>
#include "json_features.hh"
#include <json/json.hh>
#include "io.hh"
#include "traits.hh"

namespace json::testing {
#if HAS_JSON_EXCEPTIONS && HAS_JSON_VT && HAS_JSON_VALUE
	using namespace std::literals;

	struct ConversionBase : ::testing::Test {
#if HAS_JSON_GET_TYPE
		template <value_type ExpectedType>
		void get_type(value const& val) {
			EXPECT_EQ(ExpectedType, val.get_type()); // sanity check
		}
#endif // HAS_JSON_GET_TYPE

#if HAS_JSON_IS_VT
		template <value_type ExpectedType, value_type CurrentType, value_type ... Types>
		void is_vt_tests(value const& val) {
			EXPECT_EQ(ExpectedType == CurrentType, val.is<CurrentType>());
			if constexpr (sizeof...(Types) > 0)
				is_vt_tests<ExpectedType, Types...>(val);
		}

		template <value_type ExpectedType>
		void checking_vt(value const& val) {
			is_vt_tests<ExpectedType, JSON_NUMBER, JSON_STRING, JSON_ARRAY>(val);
		}
#endif // HAS_JSON_IS_VT

#if HAS_JSON_IS_CLASS
		template <value_type ExpectedType, typename CurrentType, typename ... FurtherTypes>
		void is_class_tests(value const& val) {
			constexpr auto ActualType = type_to_vt_v<CurrentType>;
			EXPECT_EQ(ExpectedType == ActualType, val.is<CurrentType>());

			if constexpr (sizeof...(FurtherTypes) > 0)
				is_class_tests<ExpectedType, FurtherTypes...>(val);
		}

		template <value_type ExpectedType>
		void checking_class(value const& val) {
			is_class_tests<ExpectedType, long long, long, int, short, std::string, json::array>(val);
		}
#endif // HAS_JSON_IS_VT

#if HAS_JSON_AS_VT
		template <value_type ExpectedType, value_type CurrentType, value_type ... Types>
		void as_vt_tests(value const& val) {
			if constexpr (ExpectedType == CurrentType) {
				EXPECT_NO_THROW(val.as<CurrentType>()) << " Type is: " << ValueType{ CurrentType };
			} else {
				EXPECT_THROW(val.as<CurrentType>(), bad_cast_exception_t<CurrentType>) << " Type is: " << ValueType{ CurrentType };
			}

			if constexpr (sizeof...(Types) > 0)
				as_vt_tests<ExpectedType, Types...>(val);
		}

		template <value_type ExpectedType>
		void casting_vt(value const& val) {
			as_vt_tests<ExpectedType, JSON_NUMBER, JSON_STRING, JSON_ARRAY>(val);
		}
#endif // HAS_JSON_AS_VT

#if HAS_JSON_AS_CLASS
		template <value_type ExpectedType, typename CurrentType, typename ... FurtherTypes>
		void as_class_tests(value const& val) {
			constexpr auto ActualType = type_to_vt_v<CurrentType>;

			if constexpr (ExpectedType == ActualType) {
				EXPECT_NO_THROW(val.as<CurrentType>())
					<< " Type is: " << ActualTypeTag<CurrentType>{};
			} else {
				EXPECT_THROW(val.as<CurrentType>(), bad_cast_exception_t<ActualType>)
					<< " Type is: " << ActualTypeTag<CurrentType>{};
			}

			if constexpr (sizeof...(FurtherTypes) > 0)
				as_class_tests<ExpectedType, FurtherTypes...>(val);
		}

		template <value_type ExpectedType>
		void casting_class(value const& val) {
			as_class_tests<ExpectedType, long long, long, int, short, std::string, json::array>(val);
		}
#endif // HAS_JSON_AS_CLASS
	};

#if HAS_JSON_GET_TYPE
	struct GetType_SanityCheck {
		template <value_type ExpectedType>
		static void test(ConversionBase* self, value const& val) {
			self->get_type<ExpectedType>(val);
		}
	};
#endif

#if HAS_JSON_AS_VT
	struct AsValueType {
		template <value_type ExpectedType>
		static void test(ConversionBase* self, value const& val) {
			self->casting_vt<ExpectedType>(val);
		}
	};
#endif

#if HAS_JSON_AS_CLASS
	struct AsClass {
		template <value_type ExpectedType>
		static void test(ConversionBase* self, value const& val) {
			self->casting_class<ExpectedType>(val);
		}
	};
#endif

#if HAS_JSON_IS_VT
	struct IsValueType {
		template <value_type ExpectedType>
		static void test(ConversionBase* self, value const& val) {
			self->checking_vt<ExpectedType>(val);
		}
	};
#endif

#if HAS_JSON_IS_CLASS
	struct IsClass {
		template <value_type ExpectedType>
		static void test(ConversionBase* self, value const& val) {
			self->checking_class<ExpectedType>(val);
		}
	};
#endif

	template <typename Checker>
	struct Conversion : ConversionBase {
		using checker_t = Checker;
	};

	TYPED_TEST_SUITE_P(Conversion);

	TYPED_TEST_P(Conversion, Null) {
		using CurrentTest = std::remove_reference_t<std::remove_cv_t<decltype(*this)>>;
		using checker = typename CurrentTest::checker_t;
		checker::template test<JSON_NULL>(this, nullptr);
	}

	TYPED_TEST_P(Conversion, Int) {
		using CurrentTest = std::remove_reference_t<std::remove_cv_t<decltype(*this)>>;
		using checker = typename CurrentTest::checker_t;
		checker::template test<JSON_NUMBER>(this, 0ll);
		checker::template test<JSON_NUMBER>(this, 42);
	}

	TYPED_TEST_P(Conversion, String) {
		using CurrentTest = std::remove_reference_t<std::remove_cv_t<decltype(*this)>>;
		using checker = typename CurrentTest::checker_t;
		checker::template test<JSON_STRING>(this, "fee fi fo"s);
	}

	TYPED_TEST_P(Conversion, Array) {
		using CurrentTest = std::remove_reference_t<std::remove_cv_t<decltype(*this)>>;
		using checker = typename CurrentTest::checker_t;
		checker::template test<JSON_ARRAY>(this, array{});
		checker::template test<JSON_ARRAY>(this, array{ "fee fi fo"s, 5, {} });
	}

	REGISTER_TYPED_TEST_SUITE_P(Conversion,
		Null, Int, String, Array);


	template <size_t IgnoreJustAllowComma, typename... Classes>
	using Types = ::testing::Types<Classes...>;

	using Checkers = Types<0
#if HAS_JSON_GET_TYPE
		, GetType_SanityCheck
#endif
#if HAS_JSON_IS_VT
		, IsValueType
#endif
#if HAS_JSON_IS_CLASS
		, IsClass
#endif
#if HAS_JSON_AS_VT
		, AsValueType
#endif
#if HAS_JSON_AS_CLASS
		, AsClass
#endif
	>;
	INSTANTIATE_TYPED_TEST_SUITE_P(Checkers, Conversion, Checkers);
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
