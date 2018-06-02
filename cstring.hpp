#pragma once
#include <cstddef>

namespace mutils {
namespace cstring {
template <std::size_t s> using fixed_str = char[s];
template <std::size_t s> using fixed_cstr = char const[s];
template <std::size_t s, std::size_t t>
constexpr std::size_t split_outside_parens(char split, const fixed_cstr<s> &in,
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
  return out_index + 1;
}
constexpr std::size_t str_len(const char *str) {
  std::size_t index = 0;
  while (str[index] != 0)
    ++index;
  return index;
}
template<std::size_t size_dst, std::size_t size_src>
constexpr std::size_t str_cpy(fixed_str<size_dst> &dst,const fixed_cstr<size_src> &src){
  static_assert(size_dst >= size_src, "Error: ouput string too small for input string size (str_cpy)");
  for (auto i = 0u; i < size_dst; ++i){
    if (i < size_src){
      dst[i] = src[i];
    }
    else dst[i] = 0;
  }
  return size_src;
}
} // namespace cstring
} // namespace mutils
