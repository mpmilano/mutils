#pragma once
#include "mutils.hpp"
#include "type_utils.hpp"
#include <type_traits>

namespace mutils{
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
