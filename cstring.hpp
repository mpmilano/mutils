#pragma once
#include "17utils.hpp"
#include "CTString.hpp"
#include <cassert>
#include <cstddef>

namespace mutils {
namespace cstring {
template <std::size_t s> using fixed_str = char[s];
template <std::size_t s> using fixed_cstr = char const[s];

constexpr std::size_t str_len(const char *str) {
  std::size_t index = 0;
  while (str[index] != 0)
    ++index;
  return index;
}
template <std::size_t size_dst>
constexpr std::size_t str_cpy(fixed_str<size_dst> &dst, const char *src,
                              std::size_t offset = 0) {
  std::size_t size_src = str_len(src);
  assert(size_dst + offset >= size_src);
  auto i = 0u;
  for (; i < (size_dst - offset); ++i) {
    dst[i + offset] = (i < size_src ? src[i] : 0);
  }
  return size_src;
}

constexpr bool streq(const char *a, const char *b) {
  for (auto i = 0u;; ++i) {
    if (a[i] != b[i])
      return false;
    else if (a[i] == 0)
      return b[i] == 0;
    else if (b[i] == 0)
      return a[i] == 0;
    else if (a[i] != b[i])
      return false;
  }
}

static_assert(streq("thisi a dssaoierfowaj iao f",
                    "thisi a dssaoierfowaj iao f"),
              "");
static_assert(!streq("thisi a dsaerfwaj io f", "thisi a dssaoierfowaj iao f"),
              "");
static_assert(!streq("thisi a dssaoierfowaj iao f", "thisi a diafowaj iao f"),
              "");

struct paren_t {
  enum class type_t { brace, bracket, paren, none };
  enum class state_t { open, closed, none };

  constexpr static type_t parentype(char c) {
    switch (c) {
    case '{':
    case '}':
      return type_t::brace;
    case '[':
    case ']':
      return type_t::bracket;
    case '(':
    case ')':
      return type_t::paren;
    default:
      return type_t::none;
    }
  }

  constexpr static state_t parenstate(char c) {
    switch (c) {
    case '{':
    case '[':
    case '(':
      return state_t::open;
    case ')':
    case '}':
    case ']':
      return state_t::closed;
    default:
      return state_t::none;
    }
  }

  const type_t type{type_t::none};
  const state_t state{state_t::none};
  constexpr paren_t(){};
  constexpr paren_t(char c) : type(parentype(c)), state(parenstate(c)) {}

  constexpr bool is_paren() const { return type != type_t::none; }
  constexpr bool is_open() const { return state == state_t::open; }
  constexpr bool is_closed() const { return state == state_t::closed; }
};

constexpr bool is_paren(char c) { return paren_t{c}.is_paren(); }

constexpr void track_paren_level(std::size_t &paren_level, char c,
                                 bool reverse = false) {
  paren_t cp{c};
  if (cp.is_paren()) {
    if ((reverse ? cp.is_closed() : cp.is_open())) {
      paren_level++;
    } else {
      assert(paren_level > 0);
      assert((reverse ? cp.is_open() : cp.is_closed()));
      --paren_level;
    }
  }
}

struct space_t {
  enum class t { space, tab, ret, none };
  static constexpr t space_type(char c) {
    switch (c) {
    case ' ':
      return t::space;
    case '\r':
      return t::ret;
    case '\t':
      return t::tab;
    default:
      return t::none;
    }
  }
  const t type{t::none};
  constexpr space_t(char c) : type(space_type(c)) {}
  constexpr space_t() {}
  constexpr bool is_space() const { return type != t::none; }
};

constexpr bool is_space(char c) { return space_t{c}.is_space(); }

template <std::size_t s, std::size_t s2, std::size_t t>
constexpr std::size_t split_outside_parens(char split, const fixed_cstr<s> &in,
                                           fixed_str<s2> (&out)[t]) {
  static_assert(s2 >= s); // there must be enough space to hold everything!
  std::size_t out_index = 0;
  std::size_t sub_index = 0;
  std::size_t paren_level{0};
  for (char c : in) {
    assert(sub_index < s);
    assert(out_index < t);
    track_paren_level(paren_level, c);
    if (c == split && paren_level == 0) {
      out_index++;
      sub_index = 0;
    } else {
      out[out_index][sub_index] = c;
      sub_index++;
    }
  }
  return out_index + 1;
}

constexpr bool prefix_equal(const char *smaller, const char *larger) {
  auto smaller_size = str_len(smaller);
  assert(str_len(larger) >= smaller_size);
  for (auto i = 0u; i < smaller_size && smaller[i] != 0; ++i) {
    if (smaller[i] != larger[i])
      return false;
  }
  return true;
}

template <std::size_t s, std::size_t s2>
constexpr std::size_t first_split(const char *split, const fixed_cstr<s> &in,
                                  fixed_str<s2> (&out)[2]) {
  static_assert(s2 >= s, "Error: out buf might be too small");
  std::size_t out_index = 0;
  std::size_t sub_index = 0;
  std::size_t paren_level{0};
  const auto prefix_len = str_len(split);
  for (auto i = 0u; i < s; ++i) {
    const char c = in[i];
    assert(sub_index < s);
    track_paren_level(paren_level, c);
    if (prefix_len <= str_len(i + in) && prefix_equal(split, i + in) &&
        paren_level == 0 && out_index == 0) {
      out_index++;
      sub_index = 0;
      i += prefix_len - 1; // it'll also autoincrement
    } else {
      out[out_index][sub_index] = c;
      sub_index++;
    }
  }
  return out_index + 1;
}

template <std::size_t s, std::size_t s2>
constexpr std::size_t first_split(char split, const fixed_cstr<s> &in,
                                  fixed_str<s2> (&out)[2]) {
  static_assert(s2 >= s, "Error: out buf might be too small");
  char splitstr[2] = {split, 0};
  return first_split(splitstr, in, out);
}

template <std::size_t s>
constexpr std::size_t last_split(const char *split, const fixed_cstr<s> &in,
                                 fixed_str<s> (&out)[2]) {
  fixed_str<s> tmp_buf = {0};
  auto split_point = 0u;
  str_cpy(tmp_buf, in);
  std::size_t paren_level{0};
  auto prefix_len = str_len(split);
  const auto in_len = str_len(in);
  for (auto i = in_len; i <= in_len; --i) {
    char c = in[i];
    track_paren_level(paren_level, c, true);
    if (prefix_len <= str_len(i + in) && prefix_equal(split, i + in) &&
        paren_level == 0) {
      split_point = i;
      tmp_buf[i] = 0;
      break;
    }
  }
  str_cpy(out[0], tmp_buf);
  assert((tmp_buf + split_point)[0] == 0);
  str_cpy(out[1], tmp_buf + split_point + prefix_len);
  return split_point;
}

template <std::size_t s>
constexpr std::size_t last_split(char split, const fixed_cstr<s> &in,
                                 fixed_str<s> (&out)[2]) {
  char splitstr[2] = {split, 0};
  return last_split(splitstr, in, out);
}

template <std::size_t s>
constexpr std::size_t combine_strings(fixed_str<s> &out, const char *in1,
                                      const char *in2) {
  assert(s > (str_len(in1) + str_len(in2)));
  auto partial = str_cpy(out, in1);
  return partial + str_cpy(out, in2, partial);
}

constexpr bool test_combine_strings() {
  fixed_str<256> out = {0};
  combine_strings(out, "this is ", "a string");
  return streq(out, "this is a string");
}

static_assert(test_combine_strings);

template <std::size_t size>
constexpr bool contains_outside_parens(char target,
                                       const fixed_cstr<size> &src) {
  std::size_t paren_level = 0;

  for (char c : src) {
    track_paren_level(paren_level, c);
    if (paren_level > 0)
      continue;
    else if (c == target)
      return true;
  }
  return false;
}

template <std::size_t size>
constexpr std::size_t pre_paren(fixed_str<size> &out, const char *in) {
  const auto len = str_len(in);
  auto i = 0u;
  for (; i < len && !is_paren(in[i]); ++i) {
    out[i] = in[i];
  }
  return i;
}

template <std::size_t size>
constexpr bool contains(char target, const fixed_cstr<size> &src) {
  for (char c : src) {
    if (c == target)
      return true;
  }
  return false;
}

constexpr bool contains_paren(const char *src) {
  const auto len = str_len(src);
  for (auto i = 0u; i < len; ++i) {
    if (is_paren(src[i]))
      return true;
  }
  return false;
}

constexpr bool contains_space(const char *src) {
  const auto len = str_len(src);
  for (auto i = 0u; i < len; ++i) {
    if (is_space(src[i]))
      return true;
  }
  return false;
}

template <std::size_t size1>
constexpr bool first_word_is(const fixed_cstr<size1> &target, const char *src) {
  std::size_t target_index{0};
  bool skipped_spaces = false;
  for (auto i = 0u;; ++i) {
    char c = src[i];
    if (c == 0)
      return target[target_index] == 0;
    if (c == ' ' && !skipped_spaces) {
      skipped_spaces = true;
      continue;
    } else if (target[target_index] == 0)
      return true;
    else if (c != target[target_index])
      return false;
    else
      ++target_index;
  }
}

template <std::size_t size>
constexpr std::size_t remove_first_word(fixed_str<size> &out, const char *in) {
  bool on_first_word{false};
  for (auto i = 0u; in[i] != 0; ++i) {
    char c = in[i];
    if (on_first_word) {
      if (is_space(c))
        return str_cpy(out, in + i);
    } else if (is_space(c))
      continue;
    else {
      on_first_word = true;
    }
  }
  return 0;
}

template <std::size_t size1, std::size_t size2>
constexpr bool contains_outside_parens(const fixed_cstr<size1> &target,
                                       const fixed_cstr<size2> &src) {
  std::size_t paren_level = 0;

  std::size_t src_offset{0};
  for (char c : src) {
    track_paren_level(paren_level, c);
    if (paren_level == 0) {
      if (first_word_is(target, src + src_offset))
        return true;
    }
    ++src_offset;
  }
  return false;
}

template <std::size_t dst_size>
constexpr std::size_t copy_within_parens(fixed_str<dst_size> &dst,
                                         const char *src) {
  std::size_t paren_level{0};
  std::size_t dst_index{0};
  bool in_paren_group{false};
  for (auto i = 0u; src[i] != 0; ++i) {
    char c = src[i];
    track_paren_level(paren_level, c);
    if (paren_level > 0) {
      if (!in_paren_group) {
        in_paren_group = true;
        continue; // skip first paren
      }
      dst[dst_index] = c;
      ++dst_index;
    } else if (in_paren_group)
      return dst_index; // only want first paren group.
  }
  return dst_index;
}

template <std::size_t dst_size>
constexpr std::size_t next_paren_group(fixed_str<dst_size> &dst,
                                       const char *src) {
  assert(str_len(src) > 1);
  if (!paren_t{src[0]}.is_open())
    return 1 + next_paren_group(dst, src + 1);
  assert(is_paren(src[0]));
  return 2 + copy_within_parens(
                 dst, src); // include the parens in the returned size.
}

template <std::size_t dst_size>
constexpr std::size_t trim(fixed_str<dst_size> &dst, const char *src) {
  const auto len = str_len(src);
  auto new_size = len;
  if (len == 0)
    return str_cpy(dst, src);
  assert(len > 0);
  for (auto i = (len - 1); i < (len) && is_space(src[i]); --i) {
    new_size = i;
  }
  auto final_size = 0u;
  bool skipped_spaces = false;
  for (auto i = 0u; i < new_size; ++i) {
    if (!is_space(src[i]) || skipped_spaces) {
      skipped_spaces = true;
      dst[final_size] = src[i];
      ++final_size;
    }
  }
  return final_size;
}

constexpr int parse_int(const char *src) {
  const auto string_length = str_len(src);
  int multiplier = exp(10, string_length - 1);
  int accum = 0;
  for (unsigned int i = 0; i < string_length; ++i) {
    accum += toInt(src[i]) * multiplier;
    multiplier /= 10;
  }
  return accum;
}

template <typename str_holder> constexpr auto build_type_string() {
  constexpr str_holder _str{};
  constexpr const char *src = _str();
  constexpr std::size_t len = str_len(src);
  if constexpr (len == 0)
    return String<>{};
  else if constexpr (len >= 10) {
    struct rest_str {
      constexpr const char *operator()() const { return str_holder{}() + 10; }
      constexpr rest_str() = default;
    };
    return String<src[0], src[1], src[2], src[3], src[4], src[5], src[6],
                  src[7], src[8],
                  src[9]>::append(build_type_string<rest_str>());
  } else if constexpr (len >= 5) {
    struct rest_str {
      constexpr const char *operator()() const { return str_holder{}() + 5; }
      constexpr rest_str() = default;
    };
    return String<src[0], src[1], src[2], src[3], src[4]>::append(
        build_type_string<rest_str>());
  } else {
    struct rest_str {
      constexpr const char *operator()() const { return str_holder{}() + 1; }
      constexpr rest_str() = default;
    };
    return String<src[0]>::append(build_type_string<rest_str>());
  }
}

struct build_type_string_test {
  constexpr build_type_string_test() = default;
  constexpr const char *operator()() const {
    return "This is a string I am converting for you";
  }
};
static_assert(streq(build_type_string<build_type_string_test>().string,
                    build_type_string_test{}()),
              "Build type string failed");

} // namespace cstring
} // namespace mutils

#include "cstring_tests.hpp"
