#pragma once
#include "mutils.hpp"
#include <type_traits>

#define DECT(x...) std::decay_t<decltype(x)>

namespace mutils{

	template<typename A, typename B>
	using TSeq = B;
	
	template<typename , typename, bool b>
	constexpr bool failOn(){
		static_assert(b,"Static assert failed");
		return true;
	}

#define sassert2(x,y,z) (mutils::failOn<x,y,z>())
	
#define tassert(x) decltype(sassert)
	
	template<typename T>
	using decay = typename std::decay<T>::type;
	
	template<template<typename> class Pred, typename Arg>
	using type_check = std::enable_if_t<Pred<Arg>::value, Arg>;
	
	template<template<typename> class Pred, typename Arg>
	struct neg_error_helper : std::integral_constant<bool, !Pred<Arg>::value >{
		static_assert(!Pred<Arg>::value,"Static assert error");
	};
	
	template<template<typename> class Pred, typename Arg>
	struct error_helper : std::integral_constant<bool, Pred<Arg>::value >{
		static_assert(Pred<Arg>::value,"Static assert error");
	};
	
	template<typename T> struct argument_type;
	template<typename T, typename U> struct argument_type<T(U)> { typedef U type; };

	template<int index, typename EnumType, typename fold_fun, typename Accum>
	std::enable_if_t<(index == static_cast<int>(EnumType::MAX)),Accum> enum_fold_helper(const fold_fun& , const Accum &accum){
		return accum;
	}

	template<int index, typename EnumType, typename fold_fun, typename Accum>
	std::enable_if_t<(index < static_cast<int>(EnumType::MAX)),Accum> enum_fold_helper(const fold_fun& ff, const Accum &accum){
		return enum_fold_helper<index + 1, EnumType,fold_fun,Accum>(ff,ff.template fun<static_cast<EnumType>(index)>(accum));
	}
	
	template<typename EnumType, typename fold_fun, typename Accum>
	auto enum_fold(const fold_fun& ff, const Accum& accum){
		using ret_t = decltype(ff.template fun<static_cast<EnumType>(0)>(std::declval<Accum>()) );
		return enum_fold_helper<0,EnumType,fold_fun,ret_t>(ff, accum);
	}

	
template <bool... b>
constexpr static bool
forall()
{
  return (true && ... && b);
}

template <bool... b>
constexpr static bool
exists()
{
  return (false || ... || b);
}

template <typename Cand, typename... List>
constexpr bool
contained()
{
  return exists<std::is_same<Cand, List>::value...>();
}

struct mismatch;

template <typename S>
constexpr auto*
find_subtype_f()
{
  constexpr mismatch* np{ nullptr };
  return np;
}

template <typename S, typename T1, typename... T>
constexpr auto* find_subtype_f(std::enable_if_t<!std::is_base_of<S, T1>::value>* = nullptr);

template <typename S, typename T1, typename... T>
constexpr auto*
find_subtype_f(std::enable_if_t<std::is_base_of<S, T1>::value>* = nullptr)
{
  constexpr T1* np{ nullptr };
  return np;
}

template <typename S, typename T1, typename... T>
constexpr auto*
find_subtype_f(std::enable_if_t<!std::is_base_of<S, T1>::value>*)
{
  return find_subtype_f<S, T...>();
}

template <typename S, typename... T>
using find_subtype = DECT(*find_subtype_f<S, T...>());

template <typename S>
constexpr bool
contains_subtype()
{
  return false;
}

template <typename S, typename T1, typename... T>
constexpr bool
contains_subtype()
{
  return std::is_base_of<S, T1>::value || contains_subtype<S, T...>();
}

	template <bool... b>
constexpr std::size_t
count_matches()
{
  return (0 + ... + b);
}

struct mismatch
{
  constexpr mismatch() = default;
};

template <typename... T>
constexpr bool
contains_match()
{
  return !forall<std::is_same<T, mismatch>::value...>();
}

template <typename... T>
constexpr bool
contains_single_match()
{
  return count_matches<std::is_same<T, mismatch>::value...>() == (sizeof...(T)-1);
}

template <typename K, typename... T>
constexpr K*
_find_match(std::enable_if_t<!std::is_same<K, mismatch>::value>* = nullptr)
{
  constexpr K* ret{ nullptr };
  return ret;
}

template <typename K, typename... T>
constexpr auto _find_match(std::enable_if_t<std::is_same<K, mismatch>::value>* = nullptr)
{
  return _find_match<T...>();
}

template <typename Fst, typename... T>
constexpr auto find_match()
{
  static_assert(contains_single_match<Fst, T...>());
  return _find_match<Fst, T...>();
}


}


