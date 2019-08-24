#include <gtest/gtest.h>
#include "json_features.hh"
#include <json/json.hh>
#include "io.hh"

#if HAS_JSON_CTORS && HAS_JSON_PARSER

namespace json::testing {
	using namespace std::literals;

	struct parsing_result {
		std::string_view text;
		value expected;
		friend std::ostream& operator<<(std::ostream& log, parsing_result const& param) {
			try {
				auto val = parse(param.text.data(), param.text.size());
				return log << "R\"(" << param.text << ")\" -> " << Value{ param.expected } << " // " << Value{ val };
			} catch (...) {}
			return log << "R\"(" << param.text << ")\" -> " << Value{ param.expected } << " // <parse error>";
		}
	};

	struct json_text {
		std::string_view text;
		constexpr json_text(std::string_view text) : text(std::move(text)) {}
		friend std::ostream& operator<<(std::ostream& log, json_text const& param) {
			try {
				auto val = parse(param.text.data(), param.text.size());
				return log << "R\"(" << param.text << ")\" // " << Value{ val };
			}
			catch (...) {}
			return log << "R\"(" << param.text << ")\"";
		}
	};

#if HAS_JSON_EXCEPTIONS
	class Good : public ::testing::TestWithParam<parsing_result> {};

	class Bad : public ::testing::TestWithParam<json_text> {};

	TEST_P(Good, Parse) {
		auto const& param = GetParam();
		EXPECT_NO_THROW(parse(param.text.data(), param.text.size()));
		try {
			auto actual = parse(param.text.data(), param.text.size());
			EXPECT_TRUE(Eq(param.expected, actual));
		} catch (parser_error const&) {
			// ... filter failures already reported by EXPECT_NO_THROW
		}
	}

#if HAS_JSON_PARSER_EMPTY
	TEST_F(Bad, Empty) {
		EXPECT_THROW(parse(""), empty_document);
	}

	TEST_F(Bad, WhitespacesOnly) {
		EXPECT_THROW(parse(R"(  


               


       

)"), empty_document);
	}
#endif

	TEST_P(Bad, Parse) {
		auto const& param = GetParam();
		EXPECT_THROW(parse(param.text.data(), param.text.size()), parser_error);
	}

#define HAS_JSON_PARSER_ANY (\
	HAS_JSON_PARSER_NULL || \
	HAS_JSON_PARSER_STR || \
	HAS_JSON_PARSER_NUM || \
	HAS_JSON_PARSER_ARRAY)

#define HAS_JSON_EQUALITY_COMP (HAS_JSON_AS_DIRECT && HAS_JSON_GET_TYPE)

#if HAS_JSON_EQUALITY_COMP && HAS_JSON_PARSER_ANY
	static parsing_result good_tests[] = {
#if HAS_JSON_PARSER_NULL
		{ "null"sv, {} },
#endif // HAS_JSON_PARSER_NIL
#if HAS_JSON_PARSER_STR
		{ R"("")"sv, ""s },
		{ R"("abcd")"sv, "abcd"s },
		{ R"("efgh")"sv, "efgh"s },
		{ R"("e\f\r\n\tgh")"sv, R"(e\f\r\n\tgh)"s },
#endif // HAS_JSON_PARSER_STR
#if HAS_JSON_PARSER_NUM
		{ "0"sv, 0ll },
		{ "-0"sv, 0ll },
		{ "12345"sv, 12345 },
		{ "-543210987654321"sv, -543210987654321 },
#endif // HAS_JSON_PARSER_NUM
#if HAS_JSON_PARSER_ARRAY
#if HAS_JSON_PARSER_NULL && HAS_JSON_PARSER_STR
		{ R"([null, "", "x"])"sv, array{ {}, ""s, "x" } },
#endif // HAS_JSON_PARSER_NIL && HAS_JSON_PARSER_STR
		{ R"([[],[],[]])"sv, array{ array{}, array{}, array{} } },
#if HAS_JSON_PARSER_NUM
		{ R"([0,1,2,3,4,5])"sv, array{ 0ll, 1, 2, 3, 4, 5 } },
#endif // HAS_JSON_PARSER_NUM
		{ R"([])"sv, array{} },
#endif // HAS_JSON_PARSER_ARRAY
	};

	INSTANTIATE_TEST_SUITE_P(Data, Good, ::testing::ValuesIn(good_tests));
#endif

	static constexpr json_text full_json[] = {
		"true"sv, "false"sv,
		R"({ "params": [0, 1, 2, 3] })"sv,
		R"("escape codes: \r \t \ubaad\uf00d\"\"")"sv,
		R"(["before\"after"])"sv,
		"123.456"sv,
		"123e3"sv,
		"123E-2"sv
	};

	INSTANTIATE_TEST_SUITE_P(FullJSON, Bad, ::testing::ValuesIn(full_json));

	static constexpr json_text bad_tests[] = {
		"true, false"sv,
		R"(["unfinished")"sv,
		R"([1234, null, )"sv,
		R"([1234 / null])"sv,
		R"("unfinished)"sv,
		"\"new\nline\""sv,
		"\"carriage\rreturn\""sv,
		"nul"sv,
		"+1234"sv,
		"123a456"sv,
		"-a"sv,
		R"(-"string")"sv,
		"-[]"sv,
		"+a"sv,
		R"(+"string")"sv,
		"+[]"sv
	};

	INSTANTIATE_TEST_SUITE_P(Data, Bad, ::testing::ValuesIn(bad_tests));
#endif // HAS_JSON_EXCEPTIONS
}

#endif // HAS_JSON_CTORS