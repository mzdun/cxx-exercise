#pragma once

#if HAS_JSON_VT && HAS_JSON_VALUE
namespace json {
	struct ValueType {
		value_type val;
		inline friend std::ostream& operator<<(std::ostream& o, ValueType type) {
#define CASE(x) case x: return o << #x
			switch (type.val) {
				CASE(JSON_NULL);
				CASE(JSON_NUMBER);
				CASE(JSON_STRING);
				CASE(JSON_ARRAY);
			default:
				break;
			}

			return o << "json::value_type(" << static_cast<int>(type.val) << ')';
#undef CASE
		}
	};

	static inline void PrintTo(value_type const& val, std::ostream* o) {
		*o << ValueType{ val };
	}

	template <typename Type> struct ActualTypeTag;
#define TYPE_TAG_(type_name, type_string) template <> \
struct ActualTypeTag<type_name> { \
	friend std::ostream& operator<<(std::ostream& o, ActualTypeTag<type_name>) { \
		return o << type_string; \
	}\
 }
#define TYPE_TAG(type_name) TYPE_TAG_(type_name, #type_name)
	TYPE_TAG(short);
	TYPE_TAG(int);
	TYPE_TAG(long);
	TYPE_TAG(long long);
	TYPE_TAG(array);
	TYPE_TAG_(std::string, "string");

	struct Value {
		value const& ref;
		inline friend std::ostream& operator<<(std::ostream& o, [[maybe_unused]] Value const& value) {
#if HAS_JSON_AS_DIRECT
			switch (value.ref.get_type()) {
			case JSON_NULL: return o << "null";
			case JSON_NUMBER: return o << value.ref.as_number();
			case JSON_STRING: return o << '"' << value.ref.as_string() << '"';
			case JSON_ARRAY:
			{
				auto& arr = value.ref.as_array();
				if (arr.empty())
					return o << "[]";

				char sep = '[';
				for (auto& item : arr) {
					o << sep << ' ' << Value{ item };
					sep = ',';
				}

				return o << " ]";
			}
			}
#endif // HAS_JSON_AS_DIRECT

			return o << "<error-value>";
		}
	};

	static inline void PrintTo(value const& val, std::ostream* o) {
		*o << Value{ val };
	}

	static inline bool Eq([[maybe_unused]] value const& lhs, [[maybe_unused]] value const& rhs) {
#if HAS_JSON_GET_TYPE
		auto const lhs_type = lhs.get_type();
		auto const rhs_type = rhs.get_type();
		if (lhs_type != rhs_type)
			return false;

#if HAS_JSON_AS_DIRECT
		switch (lhs_type) {
		case JSON_NULL: return true;
		case JSON_NUMBER: return lhs.as_number() == rhs.as_number();
		case JSON_STRING: return lhs.as_string() == rhs.as_string();
		case JSON_ARRAY:
		{
			auto const& lhs_arr = lhs.as_array();
			auto const& rhs_arr = rhs.as_array();
			if (lhs_arr.size() != rhs_arr.size())
				return false;
			auto itr = rhs_arr.begin();
			for (auto const& item_l : lhs_arr) {
				auto const& item_r = *itr++;
				if (!Eq(item_l, item_r))
					return false;
			}
			return true;
		}
		}
		return true;
#else
		return false;
#endif
#else
		return false;
#endif
	}

	static inline bool operator==([[maybe_unused]] value const& lhs, std::nullptr_t ) {
#if HAS_JSON_GET_TYPE
		return lhs.get_type() == JSON_NULL;
#else
		return false;
#endif
	}
	static inline bool operator==(std::nullptr_t lhs, value const& rhs) {
		return rhs == lhs;
	}

	struct StringRef {
		StringRef(std::string_view view) : view_{ view } {}
		StringRef(std::string const& view) : view_{ view } {}
		template <size_t Length>
		StringRef(char const (&view)[Length]) : view_{ view, Length - 1 } {}
		explicit StringRef(std::nullptr_t) {}

		friend bool operator==(std::string const& lhs, StringRef const& rhs) {
			return lhs == rhs.view_;
		}
	private:
		std::string_view view_;
	};

	static inline bool operator==([[maybe_unused]] value const& lhs, [[maybe_unused]] std::string_view rhs) {
#if HAS_JSON_AS_DIRECT
		return lhs.get_type() == JSON_STRING && std::string_view{ lhs.as_string() } == rhs;
#else
		return false;
#endif
	}
	static inline bool operator==(std::string_view lhs, value const& rhs) {
		return rhs == lhs;
	}

	static inline bool operator==([[maybe_unused]] value const& lhs, [[maybe_unused]] array const& rhs) {
#if HAS_JSON_AS_DIRECT
		if (lhs.get_type() != JSON_ARRAY)
			return false;
		auto const& lhs_arr = lhs.as_array();
		if (lhs_arr.size() != rhs.size())
			return false;
		auto itr = rhs.begin();
		for (auto const& item_l : lhs_arr) {
			auto const& item_r = *itr++;
			if (!Eq(item_l, item_r))
				return false;
		}
		return true;
#else
		return false;
#endif
	}
	static inline bool operator==(array const& lhs, value const& rhs) {
		return rhs == lhs;
	}

	template <typename Value, typename Int>
	class has_as_int
	{
		using yes = char;
		struct no { char dummy[2]; };

		template <typename C, typename I> static yes test(decltype(&C::template as<I>));
		template <typename C, typename I> static no test(...);

	public:
		constexpr static bool value = sizeof(test<Value, Int>(0)) == sizeof(yes);

		template <typename V> static Int from(V const& val) { return val.template as<Int>(); }
	};

	template <typename Value>
	class has_as_number
	{
		using yes = char;
		struct no { char dummy[2]; };

		template <typename C> static yes test(decltype(&C::as_number));
		template <typename C> static no test(...);

	public:
		constexpr static bool value = sizeof(test<Value>(0)) == sizeof(yes);
		template <typename Int, typename V> static Int from(V const& val) { return static_cast<Int>(val.as_number()); }
	};

	template <typename Int>
	static inline bool equals_int([[maybe_unused]] value const& lhs, [[maybe_unused]] Int rhs) {
#if HAS_JSON_GET_TYPE
		if (lhs.get_type() != JSON_NUMBER)
			return false;
		if constexpr (has_as_int<value, Int>::value)
			return has_as_int<value, Int>::from(lhs) == rhs;
		else if constexpr (has_as_number<value>::value)
			return has_as_number<value>::from<Int>(lhs) == rhs;
		else
			return false;
#else
		return false;
#endif
	}

#define EQUALS_INT(IntType)\
	static inline bool operator==(value const& lhs, IntType rhs) { return equals_int(lhs, rhs); } \
	static inline bool operator==(IntType lhs, value const& rhs) { return equals_int(rhs, lhs); }

	EQUALS_INT(short)
	EQUALS_INT(int)
	EQUALS_INT(long)
	EQUALS_INT(long long)
#undef EQUALS_INT
}
#endif
