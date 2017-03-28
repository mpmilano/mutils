#pragma once
#include <type_traits>
#include "17_type_utils.hpp"
//associate varargs with an index number in some template

namespace mutils{
namespace argswrapper_ns{
	
template<template<typename> class Client, std::size_t indx, typename T>
struct wrapper{
protected:
	constexpr wrapper() = default;
public:
	static constexpr std::size_t index(){ return indx;}
	using type = T;
protected:
	Client<T> t;
	
	template<std::size_t i>
	static constexpr mismatch get(std::enable_if_t<i != indx>* = nullptr){
		return mismatch{};
	}

	template<std::size_t i>
	static constexpr wrapper* get(std::enable_if_t<i == indx>* = nullptr){
		return nullptr;
	}
	~wrapper() = default;
};

template<template<typename> class Client, template<typename... > class out, std::size_t indx, typename...>
struct wrapping_str;

template<template<typename> class Client,
				 template<typename... > class out, std::size_t indx, typename arg1, typename... args>
struct wrapping_str<Client, out,indx,arg1,args...> {
	template<typename... rst>
	using newout = out<wrapper<Client,indx,arg1>, rst... >;

	using type = typename wrapping_str<Client,newout, indx+1, args...>::type;
};


template<template<typename> class Client, template<typename... > class out, std::size_t indx>
struct wrapping_str<Client,out,indx> {
	using type = out<>;
};

}

template<template<typename> class Client, template<typename...> class out, typename... args>
using argswrapper = typename argswrapper_ns::wrapping_str<Client,out,0,args...>::type;
}
