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

	template<typename T> struct identity_struct {using type = T; };
	template<template<typename> class T> struct identity_struct1 {template<typename U> using type = T<U>; };

}
