#include <gtest/gtest.h>
#include "json_features.hh"
#include <json/json.hh>
#include "io.hh"
#include "traits.hh"

#if HAS_JSON_CTORS

namespace json::testing {
	class Ctor : public ::testing::Test {
	public:
#if HAS_JSON_AS_DIRECT && HAS_JSON_GET_TYPE
		template <typename Tested>
		void simple_test(value_type VT, value const& val, Tested const& expected) {
			EXPECT_EQ(VT, val.get_type());
			if constexpr (std::is_same_v<Tested, value>) {
				EXPECT_TRUE(Eq(expected, val));
			} else {
				EXPECT_EQ(expected, val);
			}
		}

		template <typename Tested>
		void simple_test(value_type VT, value&& orig, Tested const& expected) {
			value val{ std::move(orig) };
			EXPECT_EQ(VT, val.get_type());
			if constexpr (std::is_same_v<Tested, value>) {
				EXPECT_TRUE(Eq(expected, val));
			}
			else {
				EXPECT_EQ(expected, val);
			}
		}

		template <typename Tested>
		void simple_test(value_type VT, Tested const& expected) {
			simple_test(VT, expected, expected);
		}
#endif // HAS_JSON_AS_DIRECT && HAS_JSON_GET_TYPE
	};

	TEST_F(Ctor, Null_NoCompare) {
		value a{};
		value b{ nullptr };
	}

	TEST_F(Ctor, Number_NoCompare) {
		value a{ 0ll };
		value b{ -1 };
		value c{ 1 };
		value d{ std::numeric_limits<long long>::max() };
		value e{ std::numeric_limits<long long>::min() };
	}

	TEST_F(Ctor, String_NoCompare) {
		using namespace std::literals;

		value a{ "" };
		value b{ std::string{} };
		value c{ "fee fi fo" };
		value d{ "fee fi fo"s };
	}

	TEST_F(Ctor, Array_NoCompare) {
		using namespace std::literals;

		value a{ array{} };
		value b{ { array{}, array{}, array{} } };
		value c{ { array{}, 5, {}, nullptr, "fee"s, "fi"s, "fo" } };
		value d{ { array{}, 5, {}, nullptr, "fee"s, "fi"s, "fo" } };
	}

	TEST_F(Ctor, Copy_NoCompare) {
		using namespace std::literals;

		value null{};
		value number{ 42 };
		value string{ "fee fi fo"s };
		value arr{ { null, number, string } };

		value a{ null };
		value b{ number };
		value c{ string };
		value d{ arr };
	}

	TEST_F(Ctor, Move_NoCompare) {
		using namespace std::literals;

		value null{};
		value number{ 42 };
		value string{ "fee fi fo"s };
		value arr{ { null, number, string } };

		value a{ std::move(null) };
		value b{ std::move(number) };
		value c{ std::move(string) };
		value d{ std::move(arr) };
	}

	TEST_F(Ctor, CopyAssign_NoCompare) {
		using namespace std::literals;

		value val{};
		value null{};
		value number{ 42 };
		value string{ "fee fi fo"s };
		value arr{ { null, number, string } };

		val = null;
		val = number;
		val = string;
		val = arr;
	}

	TEST_F(Ctor, MoveAssign_NoCompare) {
		using namespace std::literals;

		value val{};
		value null{};
		value number{ 42 };
		value string{ "fee fi fo"s };
		value arr{ { null, number, string } };

		val = std::move(null);
		val = std::move(number);
		val = std::move(string);
		val = std::move(arr);
	}

#if HAS_JSON_AS_DIRECT && HAS_JSON_GET_TYPE
	TEST_F(Ctor, Null) {
		simple_test(JSON_NULL, {}, nullptr);
		simple_test(JSON_NULL, nullptr);
	}

	TEST_F(Ctor, Number) {
		simple_test(JSON_NUMBER, 0);
		simple_test(JSON_NUMBER, -1);
		simple_test(JSON_NUMBER, 1);
		simple_test(JSON_NUMBER, std::numeric_limits<long long>::max());
		simple_test(JSON_NUMBER, std::numeric_limits<long long>::min());
	}

	TEST_F(Ctor, String) {
		using namespace std::literals;

		simple_test(JSON_STRING, "");
		simple_test(JSON_STRING, std::string{});
		simple_test(JSON_STRING, "fee fi fo");
		simple_test(JSON_STRING, "fee fi fo"s);
	}

	TEST_F(Ctor, Array) {
		using namespace std::literals;

		simple_test(JSON_ARRAY, array{});
		simple_test(JSON_ARRAY, array{ array{}, array{}, array{} });
		simple_test(JSON_ARRAY, array{ array{}, 5, {}, nullptr, "fee"s, "fi"s, "fo" });
		simple_test(JSON_ARRAY, { { array{}, 5, {}, nullptr, "fee"s, "fi"s, "fo" } }, array{ { array{}, 5, {}, nullptr, "fee"s, "fi"s, "fo" } });
	}

	TEST_F(Ctor, Copy) {
		using namespace std::literals;

		value null{};
		value number{ 42 };
		value string{ "fee fi fo"s };
		value arr{ { null, number, string } };
		value const items = array{ {}, 42, "fee fi fo"s };

		simple_test(JSON_NULL, null, nullptr);
		simple_test(JSON_NUMBER, number, 42);
		simple_test(JSON_STRING, string, "fee fi fo"s);
		simple_test(JSON_ARRAY, arr, items);

		EXPECT_EQ(42, number.as_number());
		EXPECT_EQ("fee fi fo", string.as_string());
		EXPECT_EQ(3, arr.as_array().size());
	}

	TEST_F(Ctor, Move) {
		using namespace std::literals;

		value null{};
		value number{ 42 };
		value string{ "fee fi fo"s };
		value arr{ { null, number, string } };
		value const items = array{ {}, 42, "fee fi fo"s };

		// original
		simple_test(JSON_NULL, null);
		simple_test(JSON_NUMBER, number);
		simple_test(JSON_STRING, string);
		simple_test(JSON_ARRAY, arr);

		// moving away
		simple_test(JSON_NULL, std::move(null), nullptr);
		simple_test(JSON_NUMBER, std::move(number), 42);
		simple_test(JSON_STRING, std::move(string), "fee fi fo"s);
		simple_test(JSON_ARRAY, std::move(arr), items);

		// moved values
		EXPECT_EQ(42, number.as_number());
		EXPECT_EQ("", string.as_string());
		EXPECT_TRUE(arr.as_array().empty());
	}

	TEST_F(Ctor, CopyAssign) {
		using namespace std::literals;

		value val{};
		value null{};
		value number{ 42 };
		value string{ "fee fi fo"s };
		value arr{ { null, number, string } };
		auto const items = array{ {}, 42, "fee fi fo"s };

		// original
		simple_test(JSON_NULL, val);
		simple_test(JSON_NULL, null);
		simple_test(JSON_NUMBER, number);
		simple_test(JSON_STRING, string);
		simple_test(JSON_ARRAY, arr);

		val = null;
		EXPECT_EQ(JSON_NULL, val.get_type());
		EXPECT_EQ(JSON_NULL, null.get_type());
		EXPECT_EQ(nullptr, val);

		val = number;
		EXPECT_EQ(JSON_NUMBER, val.get_type());
		EXPECT_EQ(JSON_NUMBER, number.get_type());
		EXPECT_EQ(42, val);

		val = string;
		EXPECT_EQ(JSON_STRING, val.get_type());
		EXPECT_EQ(JSON_STRING, string.get_type());
		EXPECT_EQ("fee fi fo"s, val);

		val = arr;
		EXPECT_EQ(JSON_ARRAY, val.get_type());
		EXPECT_EQ(JSON_ARRAY, arr.get_type());
		EXPECT_EQ(items, val);

		EXPECT_EQ(nullptr, null);
		EXPECT_EQ(42, number);
		EXPECT_EQ("fee fi fo"s, string);
		EXPECT_EQ(items, arr);
	}

	TEST_F(Ctor, MoveAssign) {
		using namespace std::literals;

		value val{};
		value null{};
		value number{ 42 };
		value string{ "fee fi fo"s };
		value arr{ { null, number, string } };
		auto const items = array{ {}, 42, "fee fi fo"s };

		simple_test(JSON_NULL, val, nullptr);
		simple_test(JSON_NULL, null);
		simple_test(JSON_NUMBER, number);
		simple_test(JSON_STRING, string);
		simple_test(JSON_ARRAY, arr);

		val = std::move(null);
		EXPECT_EQ(JSON_NULL, val.get_type());
		EXPECT_EQ(JSON_NULL, null.get_type());
		EXPECT_EQ(nullptr, val);

		val = std::move(number);
		EXPECT_EQ(JSON_NUMBER, val.get_type());
		EXPECT_EQ(JSON_NUMBER, number.get_type());
		EXPECT_EQ(42, val);

		val = std::move(string);
		EXPECT_EQ(JSON_STRING, val.get_type());
		EXPECT_EQ(JSON_STRING, string.get_type());
		EXPECT_EQ("fee fi fo"s, val);

		val = std::move(arr);
		EXPECT_EQ(JSON_ARRAY, val.get_type());
		EXPECT_EQ(JSON_ARRAY, arr.get_type());
		EXPECT_EQ(items, val);

		EXPECT_EQ(nullptr, null);
		EXPECT_EQ(42, number);
		EXPECT_EQ(""s, string);
		EXPECT_EQ(array{}, arr);
	}
#endif // HAS_JSON_AS_DIRECT && HAS_JSON_GET_TYPE
}
#endif // HAS_JSON_CTORS
