#pragma once
#include "17_type_utils.hpp"
#include "17utils.hpp"
#include "CTString_decl.hpp"
#include "type_utils.hpp"

namespace mutils{
	template <typename...>
struct typelist;

namespace typelist_ns {

template <template <typename, typename> class less_than, typename... list>
constexpr auto min();

template <template <typename, typename> class less_than, typename... rst>
constexpr auto min_helper();

template <template <typename, typename> class less_than, typename fst>
constexpr auto _min_helper()
{
  constexpr fst* np{ nullptr };
  return np;
}

template <template <typename, typename> class less_than, typename candidate, typename fst, typename... list>
constexpr auto _min_helper(std::enable_if_t<!less_than<fst, candidate>::value>* = nullptr)
{
  return min_helper<less_than, candidate, list...>();
}

template <template <typename, typename> class less_than, typename candidate, typename fst, typename... list>
constexpr auto _min_helper(std::enable_if_t<less_than<fst, candidate>::value>* = nullptr)
{
  return min_helper<less_than, fst, list...>();
}

template <template <typename, typename> class less_than, typename... rst>
constexpr auto min_helper()
{
  return _min_helper<less_than, rst...>();
}

template <template <typename, typename> class less_than, typename fst, typename... list>
constexpr auto _min()
{
  return min_helper<less_than, fst, list...>();
}

template <typename cand>
constexpr auto remove_first(const typelist<>& a)
{
  return a;
}

template <typename cand, typename fst, typename... rst>
constexpr auto remove_first(typelist<fst, rst...>, std::enable_if_t<!std::is_same<cand, fst>::value>* = nullptr);

template <typename cand, typename fst, typename... rst>
constexpr auto remove_first(typelist<fst, rst...>, std::enable_if_t<std::is_same<cand, fst>::value>* = nullptr)
{
  return typelist<rst...>{};
}

template <typename cand, typename fst, typename... rst>
constexpr auto remove_first(typelist<fst, rst...>, std::enable_if_t<!std::is_same<cand, fst>::value>*)
{
  return typelist<fst>::append(remove_first<cand>(typelist<rst...>{}));
}
template <template <typename, typename> class less_than, typename... list>
constexpr auto min()
{
  return _min<less_than, list...>();
}

template <typename c1, typename... str2>
constexpr auto reverse_helper()
{
  return typelist<str2...>::reverse().append(typelist<c1>{});
}
}

template <>
struct typelist<>
{
  constexpr typelist() = default;
  template <typename... t2>
  static constexpr auto append(typelist<t2...> a)
  {
    return a;
  }

  template <typename ap1, typename ap2, typename... rest>
  static constexpr auto append(ap1, ap2, rest...)
  {
    return append(ap1{}).append(ap2{}).append(rest{}...);
  }

  static constexpr auto append() { return typelist{}; }

  template <typename T>
  static constexpr bool contains()
  {
    return false;
  }

  template <template <typename, typename> class less_than>
  static constexpr auto sort()
  {
    return typelist{};
  }

  static constexpr auto reverse() { return typelist{}; }

	template<template<typename> class>
	static constexpr auto filter() {return typelist{};}
	
	template<template<typename> class>
	static constexpr auto map() {return typelist{};}

  template <typename... T>
  static constexpr auto intersect(T...)
  {
    return typelist{};
  }

  template <typename>
  using find_subtype = mismatch;

  template <typename>
  static constexpr bool contains_subtype()
  {
    return false;
  }
};

namespace typelist_ns {
constexpr auto append()
{
  return typelist<>{};
}
template <typename T, typename... U>
constexpr auto append(T, U... u)
{
  return T::append(u...);
}

	template<typename T1, typename... _rest>
	struct first_struct {
		using type = T1;
		using rest = typelist<_rest...>;
	};

}

template <typename... t1>
struct typelist
{

	using first = typename typelist_ns::first_struct<t1...>::type;
	using rest = typename typelist_ns::first_struct<t1...>::rest;
	
  constexpr typelist() = default;
  template <typename... t2>
  static constexpr auto append(typelist<t2...>)
  {
    return typelist<t1..., t2...>{};
  }

	template <typename... t2>
  static constexpr auto prepend(typelist<t2...>)
  {
    return typelist<t2..., t1...>{};
  }

  template <typename ap1, typename ap2, typename... rest>
  static constexpr auto append(ap1, ap2, rest...)
  {
    return append(ap1{}).append(ap2{}).append(rest{}...);
  }

  static constexpr auto append() { return typelist{}; }

  template <typename T>
  static constexpr bool contains()
  {
    return contained<T, t1...>();
  }

  template <template <typename, typename> class less_than>
  static constexpr auto sort()
  {
    using fst = DECT(*typelist_ns::min<less_than, t1...>());
    return typelist<fst>::append(typelist_ns::remove_first<fst>(typelist{}).template sort<less_than>());
  }

  static constexpr auto reverse() { return typelist_ns::reverse_helper<t1...>(); }

private:
  template <typename sofar>
  static constexpr auto intersect_helper(sofar a, typelist<>)
  {
    return a;
  }

  template <typename sofar, typename t, typename... t2>
  static constexpr auto intersect_helper(sofar, typelist<t, t2...>)
  {
    return typelist::intersect_helper(std::conditional_t<contains<t>(), DECT(sofar::append(typelist<t>{})), sofar>{}, typelist<t2...>{});
  }

public:
  template <typename... t2>
  static constexpr auto intersect(typelist<t2...> a)
  {
    return typelist::intersect_helper<typelist<>, t2...>(typelist<>{}, a);
  }

  template <typename S>
  using find_subtype = ::mutils::find_subtype<S, t1...>;

  template <typename S>
  static constexpr bool contains_subtype()
  {
    return ::mutils::contains_subtype<S, t1...>();
  }

	template<template<typename> class C>
	struct high_order_helper{
		template<typename fst, typename... rst>
		static constexpr auto filter(fst* f,rst*... r){
			constexpr std::integral_constant<bool, C<fst>::value> *choice{nullptr};
			return high_order_helper::_filter<fst,rst...>(f,r...,choice);
		}
		template<typename...> static constexpr auto filter(){
			return typelist<>{};
		}
		
		template<typename fst, typename... rst>
		static constexpr auto _filter(fst*,rst*... r,std::true_type*){
			return typelist<fst>::append(filter<rst...>(r...));
		}

		template<typename fst, typename... rst>
		static constexpr auto _filter(fst*,rst*... r,std::false_type*){
			return filter<rst...>(r...);
		}
	};

	template<template<typename> class C>
	static constexpr auto filter() {
		return high_order_helper<C>::template filter<t1...>((t1*)nullptr...);
	}
	
	template<template<typename> class C>
	static constexpr auto map() {
		return typelist<C<t1>...>{};
	}
	
};
template <typename... t>
std::ostream& operator<<(std::ostream& o, typelist<t...>)
{
  static const auto print = [](auto& o, const auto& e) {
    o << string_of<DECT(*e)>{}.value << ", ";
    return nullptr;
  };
  o << "[";
  auto ignore = { nullptr, nullptr, print(o,(t*)nullptr)... };
	(void)ignore;
  return o << "]";
}
}
