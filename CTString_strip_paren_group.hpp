#pragma once

namespace mutils {
namespace CTString {

template <char lparen, char rparen>
struct strip_paren_group_struct
{
  // error case
  template<typename l, typename r> struct pair{
    constexpr pair() = default;
    using first = l;
    using second = r;
  };
  template <typename StrippedSoFar, typename n>
  static constexpr auto strip_paren_group(n, String<>)
  {
    assert(lparen == rparen && "Error: expression ended before matching paren found");
    return pair<String<>,String<>>{};
  }

  // final case
  template <typename StrippedSoFar, char... str2>
  static constexpr auto strip_paren_group(zero, String<rparen, str2...>)
  {
    return pair<std::decay_t<decltype(StrippedSoFar::append(String<rparen>{}))>, String<str2...>>{};
  }

  // increase indent
  template <typename StrippedSoFar, typename n, char... str2>
  static constexpr auto strip_paren_group(n, String<lparen, str2...>)
  {
    using next = std::decay_t<decltype(StrippedSoFar::append(String<lparen>{}))>;
    return strip_paren_group<next>(succ<n>{}, String<str2...>{});
  }

  // decrease indent
  template <typename StrippedSoFar, typename n, char... str2>
  static constexpr auto strip_paren_group(succ<n>, String<rparen, str2...>)
  {
    using next = std::decay_t<decltype(StrippedSoFar::append(String<rparen>{}))>;
    return strip_paren_group<next>(n{}, String<str2...>{});
  }

  // normal case
  template <typename StrippedSoFar, typename n, char char1, char... str2>
  static constexpr auto strip_paren_group(n, String<char1, str2...>)
  {
    static_assert(char1 != lparen && char1 != rparen);
    using next = std::decay_t<decltype(StrippedSoFar::append(String<char1>{}))>;
    return strip_paren_group<next>(n{}, String<str2...>{});
  }

  // getting started

  // take first group
  template <char... str2>
  static constexpr auto strip_paren_group_start(zero, String<lparen, str2...>)
  {
    using pair = std::decay_t<decltype(strip_paren_group<String<lparen>>(zero{}, String<str2...>{}))>;
    return typename pair::first{};
  }

  // take remainder group
  template <char... str2>
  static constexpr auto strip_paren_group_start(succ<zero>, String<lparen, str2...>)
  {
    using pair = std::decay_t<decltype(strip_paren_group<String<lparen>>(zero{}, String<str2...>{}))>;
    return typename pair::second{};
  }

  template <typename n, char... str2>
  static constexpr auto strip_paren_group_start(n, String<' ', str2...>)
  {
    return String<str2...>::trim_ends().template strip_paren_group<lparen, rparen>(n{});
  }
};
}
}
