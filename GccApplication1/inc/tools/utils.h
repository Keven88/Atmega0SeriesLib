#pragma once
#include <limits.h>
#ifndef __AVR__
#include<type_traits>
#include <limits>
#include <cmath>
#include <chrono>
#include <ratio>
#endif
#ifdef A4963
#include "../../src/CustomDataTypes/Volt.h"
#include "../../src/CustomDataTypes/Hertz.h"
#include "../../src/CustomDataTypes/Percentage.h"
#endif

#define NoConstructors(x) x() = delete; x(const x&) = delete; x(x&&) = delete

namespace utils {

#ifdef __AVR__
    template<typename T>
    const char* toString(T value){
        static char str[sizeof(T)*8]{};

        int n = 0;
        T tmp = value;
        do {
            n++;
            tmp/=10;
        } while(tmp != 0);

        str[n--] = '\0';

        while(n >= 0){
            str[n--] = (value % 10) + 48;
            value /=10;
        }

        return str;
    }
#endif

    static constexpr auto strlen(const char* str){
        auto n = 0;
        while(str[n++] != '\0');
        return n-1;
    }
	
	template<typename T1, typename T2>
	struct tuple {
		using t1 = T1;
		using t2 = T2;
	};
	

	struct false_type {
		static constexpr bool value = false;
	};

	struct true_type {
		static constexpr bool value = true;
	};

	template<bool B, class T = void>
	struct enable_if {};

	template<class T>
	struct enable_if<true, T> { typedef T type; };

	template< bool B, class T = void >
	using enable_if_t = typename enable_if<B, T>::type;

	template<bool condition, typename T1, typename T2>
	struct conditional;

	template<typename T, T X>
	struct integralConstant;
	
	template<auto X>
	struct autoConstant;

	template<typename T, typename O>
	struct isEqual;

	template<auto x>
	struct	minRequired;

	template<auto x>
	struct minRequiredUnsigned;
	

	template<typename T>
	struct isUnsigned;

	namespace details {
		template<bool condition, typename T1, typename T2>
		struct conditional
		{
			using type = T2;
		};

		template<typename T1, typename T2>
		struct conditional<true, T1, T2>
		{
			using type = T1;
		};


		template<typename T, typename O>
		struct isEqual
		{
			static constexpr bool value = false;
		};

		template<typename T>
		struct isEqual<T, T>
		{
			static constexpr bool value = true;
		};


		template<long long x>
		struct	minRequired {
			using min =
			typename conditional<x <= SCHAR_MAX && x >= SCHAR_MIN , char,
			typename conditional<x <= SHRT_MAX && x >= SHRT_MIN, short,
			typename conditional<x <= INT_MAX && x >= INT_MIN, int,
			typename conditional<x <= LONG_MAX && x >= LONG_MIN, long, long long
			>::type
			>::type
			>::type
			>::type;
		};

		template<unsigned long long x>
		struct	minRequiredUnsigned {
			using min =
			typename conditional<x <= UCHAR_MAX, unsigned char,
			typename conditional<x <= USHRT_MAX, unsigned short,
			typename conditional<x <= UINT_MAX, unsigned int,
			typename conditional<x <= ULONG_MAX, unsigned long, unsigned long long
			>::type
			>::type
			>::type
			>::type;
		};

		template<typename T>
		struct isUnsigned {
			static constexpr bool value = false;
		};

		template<>
		struct isUnsigned<unsigned char>
		{
			static constexpr bool value = true;
		};

		template<>
		struct isUnsigned<unsigned short>
		{
			static constexpr bool value = true;
		};

		template<>
		struct isUnsigned<unsigned int>
		{
			static constexpr bool value = true;
		};

		template<>
		struct isUnsigned<unsigned long>
		{
			static constexpr bool value = true;
		};

		template<>
		struct isUnsigned<unsigned long long>
		{
			static constexpr bool value = true;
		};

		template<typename integral, integral val>
		struct isPositive {
			static constexpr bool value = val > 0;
		};
	}

	template<bool condition, typename T1, typename T2>
	struct conditional {
		using type = typename details::conditional<condition, T1, T2>::type;
	};

	template<typename T, T X>
	struct integralConstant {
		static constexpr T value = X;
		using value_type = T;
		using type = integralConstant;
	};

	template<typename T, typename O>
	struct isEqual {
		static constexpr bool value = details::isEqual<T, O>::value;
	};

	//#define minReqS(value) typename utils::minRequired<decltype(value),value>::type
	template<auto x>
	struct minRequired {
		using type = typename conditional<x <= INT_MAX,typename details::minRequired<x>::min,void>::type;
		static_assert(!isEqual<type, void>::value, "signed value is too big to fit to an unsigned type");
	};

	//#define minReqU(value) typename utils::minRequiredUnsigned<decltype(value),value>::type
	template<auto x>
	struct minRequiredUnsigned {
		static_assert(x >= 0, " tried to use unsigned type for negative value");
		using type = typename details::minRequiredUnsigned<x>::min;
	};

	template<typename T>
	struct isUnsigned {
		static constexpr bool value = details::isUnsigned<T>::value;
	};


	template<typename ...T>
	struct list {
		static constexpr auto size = sizeof...(T)+1;
	};
	//to test if this change will cause bugs or other problems
	template<typename first, typename ...T>
	struct list<first, T...> {
		static constexpr auto size = sizeof...(T)+1;
	};

	template<typename... Pack>
	struct front{
		using type = void;
	};
	
	template<typename P,typename... Pack>
	struct front<P,Pack...>{
		using type = P;
	};
	

	template<typename Push, typename L>
	struct push_front {};

	template<typename Push,template<typename...> typename List, typename... T>
	struct push_front<Push,List<T...>>
	{
		using type = List<Push,T...>;
	};

	template<typename T>
	struct pop_front { using type = list<T>; };

	template<typename F, typename ...T>
	struct pop_front<list<F,T...>> {
		using type = list<F,T...>;
	};

	template<unsigned long long F, typename T, typename... P>
	struct getType;

	template<typename T, typename... Ts>
	struct getType<0, T, Ts...>;

	template<unsigned long long F, template<typename, typename...> typename List, typename T, typename... P>
	struct getType<F, List<T, P...>>;


	template<unsigned long long F, typename T, typename... P>
	struct getType
	{
		static_assert(F >= 0, "no negative values allowed");
		static_assert(F < (sizeof...(P)+1), "index out of bounds");
		using type = typename getType<F - 1, P...>::type;
	};

	template<unsigned long long F, template<typename,typename...> typename List,typename T, typename... P>
	struct getType<F,List<T,P...>>
	{
		static_assert(F >= 0, "no negative values allowed");
		using type =
		typename getType<F, T, P...>::type;
	};

	//necessary for tl variant (ambiguous instantiation)
	template<template<typename, typename...> typename List, typename T, typename... P>
	struct getType<0, List<T, P...>>
	{
		using type = T;
	};

	template<typename T, typename... Ts>
	struct getType<0,T,Ts...>
	{
		using type = T;
	};

	template<typename first,typename ...T>
	[[nodiscard]] constexpr bool sameTypes() noexcept {
		if constexpr(sizeof...(T) == 0)
		return true;
		else {
			if constexpr (!isEqual<first, typename front<T...>::type>::value)
			return false;
			else
			return sameTypes<T...>();
		}
		
	}
	
	template<typename t1, typename t2, typename T, typename E>
	struct condEqual {
		using type = typename conditional<isEqual<t1,t2>::value,T,E>::type;
	};
	
	template<class...> struct disjunction : false_type { };
	template<class B1> struct disjunction<B1> : B1 { };
	template<class B1, class... Bn>
	struct disjunction<B1, Bn...>
	: conditional<bool(B1::value), B1, disjunction<Bn...>>::type  { };
	
	template<typename T, typename... list>
	struct contains {
		static constexpr bool value = disjunction<isEqual<T,list>...>::value;
	};
	
	template<typename T, auto val>
	struct Pair {
		static inline constexpr auto value = val;
		using type = T;
	};
	
	template<auto val>
	struct autoConstant{
		using type = decltype(val);
		static constexpr auto value = val;
	};
	
	/*
	template<typename searched, typename first, typename ... pack>
	struct contains {
	static inline constexpr bool value = isEqual<searched,first>::value ? true : contains<searched,pack...>::value;
	};

	//last in recursion
	template<typename searched, typename first>
	struct contains<searched,first> {
	static inline constexpr bool value = isEqual<searched,first>::value;
	};*/

	#ifndef __AVR__
	template<typename T>
	[[nodiscard]] constexpr typename std::make_unsigned<T>::type getFirstSetBitPos(T n) noexcept {
		static_assert(std::numeric_limits<T>::is_integer);
		return std::log2(n&-n);
	}
	template<class T>
	struct is_duration : std::false_type {};

	template<class Rep, class Period>
	struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

	template<class T>
	struct is_hertz : std::false_type {};

	template<class Rep, class Period>
	struct is_hertz<CustomDataTypes::Frequency::Hertz<Rep,Period>> : std::true_type {};

	template<class T>
	struct is_volt : std::false_type {};

	template<class Rep, class Period>
	struct is_volt<CustomDataTypes::Electricity::Volt<Rep, Period>> : std::true_type {};


	template<typename T>
	struct is_periodic : std::false_type {};

	template<template<typename , typename> typename T , typename Rep, intmax_t Nom, intmax_t Denom>
	struct is_periodic<T<Rep, std::ratio<Nom,Denom>>> : std::true_type {};

	template<typename T>
	struct periodic_info;

	template<template<typename , typename> typename T , typename Rep, intmax_t Nom, intmax_t Denom>
	struct periodic_info<T<Rep, std::ratio<Nom,Denom>>> {
		using rep = Rep;
		using period = std::ratio<Nom,Denom>;
	};

	template <typename T, typename U>
	struct is_template_same : std::false_type {};

	template <template<typename, typename> typename T, typename Rep, typename Period, typename oRep, typename oPeriod>
	struct is_template_same<T<Rep, Period>, T<oRep, oPeriod>> : std::true_type {};

	template <typename T, typename U>
	constexpr bool is_template_same_v = is_template_same<T, U>::value;

	template<typename T>
	struct periodic_printable;

	template<template<typename , typename> typename T , typename Rep, intmax_t Nom, intmax_t Denom>
	struct periodic_printable<T<Rep, std::ratio<Nom,Denom>>> {
		static constexpr std::string_view name = T<Rep, std::ratio<Nom,Denom>>::abr_value;
	};

	template<typename Rep, intmax_t Nom, intmax_t Denom>
	struct periodic_printable<std::chrono::duration<Rep, std::ratio<Nom,Denom>>> {
		static constexpr std::string_view name = "s";
	};

	template< class T >
	struct remove_cvref {
		typedef std::remove_cv_t<std::remove_reference_t<T>> type;
	};

	[[nodiscard]] constexpr auto approximately_same(long a, long b) noexcept
	{
		return a == b;
	}

	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	[[nodiscard]] constexpr bool approximately_same(T a, T b) noexcept
	{
		return std::fabs(a - b) <= 1E-13;
	}

	template<template <typename, typename> typename T, typename Rep, typename U, typename V>
	[[nodiscard]] constexpr auto approximately_same(T<Rep, U> a, T<Rep, V> b) noexcept {
		if(std::ratio_less_v<U, V>) {
			auto tmp_b = static_cast<T<Rep, U>>(b);
			return approximately_same(a.count(), tmp_b.count());
			} else {
			auto tmp_a = static_cast<T<Rep, V>>(a);
			return approximately_same(tmp_a.count(), b.count());
		}
	}

	template<template <typename> typename T, typename Rep>
	[[nodiscard]] constexpr auto approximately_same(T<Rep> a, T<Rep> b) {
		return a == b;
	}

	template<typename Rep, typename U, typename V>
	[[nodiscard]] constexpr auto approximately_same(std::chrono::duration<Rep, U> a, std::chrono::duration<Rep, V> b) noexcept {
		if(std::ratio_less_v<U, V>) {
			auto tmp_b = std::chrono::duration_cast<std::chrono::duration<Rep, U>>(b);
			return approximately_same(a.count(), tmp_b.count());
			} else {
			auto tmp_a = std::chrono::duration_cast<std::chrono::duration<Rep, V>>(a);
			return approximately_same(tmp_a.count(), b.count());
		}
	}

	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	[[nodiscard]] constexpr auto approximately_greater_or_equal(T a, T b) noexcept
	{
		return (a > b || approximately_same(a, b));
	}

	template<typename Rep>
	[[nodiscard]] constexpr auto approximately_greater_or_equal(CustomDataTypes::Percentage<Rep> a, CustomDataTypes::Percentage<Rep>  b) noexcept
	{
		return (a.getPercent() > b.getPercent() || approximately_same(a, b));
	}

	template<template <typename, typename> typename T, typename Rep, typename U, typename V>
	[[nodiscard]] constexpr auto approximately_greater_or_equal(T<Rep, U> a, T<Rep, V> b) noexcept
	{
		return (a > b || approximately_same(a, b));
	}

	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	[[nodiscard]] constexpr auto approximately_less_or_equal(T a, T b) noexcept
	{
		return (a < b || approximately_same(a, b));
	}

	template<template <typename, typename> typename T, typename Rep, typename U, typename V>
	[[nodiscard]] constexpr auto approximately_less_or_equal(T<Rep, U> a, T<Rep, V> b) noexcept
	{
		return (a < b || approximately_same(a, b));
	}

	template<typename Rep>
	[[nodiscard]] constexpr auto approximately_less_or_equal(CustomDataTypes::Percentage<Rep> a, CustomDataTypes::Percentage<Rep>  b) noexcept
	{
		return (a.getPercent() < b.getPercent() || approximately_same(a, b));
	}

	namespace printable {
		template<typename Rep, typename p>
		std::ostream& operator<< (std::ostream& stream, const std::chrono::duration<Rep, p>& duration) {
			using duration_type =  std::chrono::duration<Rep, p>;
			stream << duration.count()
			<< utils::ratio_lookup<typename utils::periodic_info<duration_type>::period>::abr_value
			<< utils::periodic_printable<duration_type>::name;
			return stream;
		}
	}
	
	#endif
}