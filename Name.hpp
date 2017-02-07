#pragma once
#include <type_traits>
#include <sstream>
#include <cxxabi.h>
#include <unistd.h>
#include "type_utils.hpp"
#include "SerializationSupport.hpp"
#include "macro_utils.hpp"

namespace mutils{

#define MUTILS_NAME(str) ::mutils::name::name_from_macro<MACRO_GET_STR(str)>
	
	template<char... str> struct Name {
		constexpr Name() = default;
		static const constexpr char name [] = {str...,0};
		static const constexpr decltype(sizeof...(str)) name_length = sizeof...(str);
		static constexpr Name const * const p{nullptr};
	};
	template<char... str>
	const char Name<str...>::name[];
	template<char... str>
	const decltype(sizeof...(str)) Name<str...>::name_length;
	template<char... str>
	constexpr Name<str...> const * const Name<str...>::p;
	
	template<typename> struct is_name : public std::false_type{};
	template<char... str> struct is_name<Name<str...> > : public std::true_type{};
	
	namespace name{
		template<char... str>
		struct char_pack{
			template<char str2>
			static constexpr char_pack<str2,str...>* prepend(){
				return nullptr;
			}
		};
		
		constexpr char_pack<>* strip_nulls(char_pack<> const * const, void*){
			return nullptr;
		}
		
		template<char str1, char... str>
		constexpr auto strip_nulls(char_pack<str1, str...> const * const, std::enable_if_t<str1 != 0>*){
			char_pack<str...> *rec_pack{nullptr};
			auto pack1 = strip_nulls(rec_pack,nullptr);
			using cp1_t = std::decay_t<decltype(*pack1)>;
			return cp1_t:: template prepend<str1>();
		}

		template<char str1, char... str>
		constexpr auto strip_nulls(char_pack<str1, str...> const * const, std::enable_if_t<str1 == 0>*){
			char_pack<str...> *rec_pack{nullptr};
			return strip_nulls(rec_pack,nullptr);
		}

		template<char... str>
		constexpr Name<str...>* name_from_stripped(char_pack<str...> const * const){
			return nullptr;
		}

		template<char... str>
		constexpr auto name_from_macro_f(){
			char_pack<str...> *rec_pack{nullptr};
			auto ptr = strip_nulls(rec_pack,nullptr);
			return name_from_stripped(ptr);
		}

		template<char... str>
		using name_from_macro = std::decay_t<decltype(*name_from_macro_f<str...>())>;
	}
}
