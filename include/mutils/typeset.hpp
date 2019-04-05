#pragma once
#include "typelist.hpp"
namespace mutils{
template <typename... t1>
struct typeset;

template <typename... T2>
constexpr auto to_typeset(typelist<T2...>);

namespace typelist_ns {

template <typename typeset2, typename... t>
constexpr auto subtract(typeset<t...>, typeset2);
}

template <typename... t1>
struct typeset
{
  constexpr typeset() = default;

  template <typename t>
  static constexpr auto add(std::enable_if_t<contained<t, t1...>()>* = nullptr)
  {
    return typeset{};
  }

  template <typename t>
  static constexpr auto add(std::enable_if_t<!contained<t, t1...>()>* = nullptr)
  {
    return typeset<t1..., t>{};
  }

  template <typename... t>
  static constexpr auto add(typelist<t...>* = nullptr, std::enable_if_t<sizeof...(t) == 0>* = nullptr)
  {
    return typeset{};
  }

  template <typename a, typename b, typename... rst>
  static constexpr auto add()
  {
    return add<a>().template add<b>().template add<rst...>();
  }

  template <typename... t2>
  static constexpr auto combine(typeset<t2...>)
  {
    return typeset::template add<t2...>();
  }

  template <typename t>
  static constexpr bool contains()
  {
    return typelist<t1...>::template contains<t>();
  }

  template <typename ap1, typename ap2, typename... rest>
  static constexpr auto combine(ap1, ap2, rest...)
  {
    return combine(ap1{}).combine(ap2{}).combine(rest{}...);
  }

  static constexpr auto combine() { return typeset{}; }

  template <template <typename, typename> class less_than>
  static constexpr auto as_sorted_list()
  {
    return typelist<t1...>::template sort<less_than>();
  }

  template <typename... t2>
  static constexpr auto intersect(typeset<t2...>)
  {
    return to_typeset(typelist<t1...>::intersect(typelist<t2...>{}));
  }

  template <typename... t2>
  static constexpr auto subtract(typeset<t2...>)
  {
    return typelist_ns::subtract(typeset{}, typeset<t2...>{});
  }
};

namespace typelist_ns {

constexpr auto combine()
{
  return typeset<>{};
}
template <typename T, typename... U>
constexpr auto combine(T, U... u)
{
  return T::combine(u...);
}

template <typename typeset2, typename accum>
constexpr auto _subtract(accum a, typeset<>, typeset2)
{
  return a;
}

template <typename typeset2, typename accum, typename t1, typename... t>
constexpr auto _subtract(accum, typeset<t1, t...>, typeset2 b, std::enable_if_t<!typeset2::template contains<t1>()>* = nullptr);

template <typename typeset2, typename accum, typename t1, typename... t>
constexpr auto _subtract(accum, typeset<t1, t...>, typeset2 b, std::enable_if_t<typeset2::template contains<t1>()>* = nullptr)
{
  return _subtract(accum{}, typeset<t...>{}, b);
}

template <typename typeset2, typename accum, typename t1, typename... t>
constexpr auto _subtract(accum, typeset<t1, t...>, typeset2 b, std::enable_if_t<!typeset2::template contains<t1>()>*)
{
  return _subtract(accum::template add<t1>(), typeset<t...>{}, b);
}

template <typename typeset2, typename... t>
constexpr auto subtract(typeset<t...> a, typeset2 b)
{
  return _subtract(typeset<>{}, a, b);
}

constexpr typeset<>
intersect()
{
  return typeset<>{};
}

template <typename T>
constexpr T intersect(T)
{
  return T{};
}

template <typename T1, typename T2, typename... rest>
constexpr auto intersect(T1, T2, rest...)
{
  return T1::intersect(intersect(T2{}, rest{}...));
}
}

template <typename... t>
std::ostream& operator<<(std::ostream& o, typeset<t...>)
{
  static const auto print = [](std::ostream& o, const auto& e) {
    o << string_of<DECT(*e)>{}.value << ", ";
    return nullptr;
  };
  o << "[";
  (print(o,(t*)nullptr),...);
  (void) print;
  return o << "]";
}

template <typename... T2>
constexpr auto to_typeset(typelist<T2...>)
{
  return typeset<>::add<T2...>();
}
/*
        template<typename T>
        constexpr bool print_obj(T = T{}){
                static_assert(T{} != T{});
                return true;
        }//*/

}
