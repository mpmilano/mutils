#pragma once
#include <cstddef>

namespace mutils {
namespace cstring {
template <std::size_t s> using fixed_str = char[s];
template <std::size_t s> using fixed_cstr = char const[s];
template <std::size_t s, std::size_t t>
constexpr void split_outside_parens(char split, const fixed_cstr<s> &in,
                                    fixed_str<s> (&out)[t]) {
  std::size_t out_index = 0;
  std::size_t sub_index = 0;
  for (char c : in) {
    if (c == split) {
      out_index++;
      sub_index = 0;
    } else {
      out[out_index][sub_index] = c;
      sub_index++;
    }
  }
}
constexpr std::size_t str_len(const char *str) {
  std::size_t index = 0;
  while (str[index] != 0)
    ++index;
  return index;
}
} // namespace cstring
} // namespace mutils
