#pragma once
#include <cstddef>
#include <cassert>

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
constexpr std::size_t str_cpy(fixed_str<size_dst> &dst, const char* src) {
  std::size_t size_src = str_len(src);
  assert(size_dst >= size_src);
  auto i = 0u;
  for (; i < size_dst; ++i) {
    dst[i] = (i < size_src ? src[i] : 0);
  }
  return size_src;
}

constexpr bool streq(const char* a, const char* b){
  for (auto i = 0u; ; ++i){
    if (a[i] != b[i]) return false;
    else if (a[i] == 0) return b[i] == 0;
    else if (b[i] == 0) return a[i] == 0;
    else if (a[i] != b[i]) return false;
  }
}

static_assert(streq("thisi a dssaoierfowaj iao f","thisi a dssaoierfowaj iao f"),"");
static_assert(!streq("thisi a dsaerfwaj io f","thisi a dssaoierfowaj iao f"),"");
static_assert(!streq("thisi a dssaoierfowaj iao f","thisi a diafowaj iao f"),"");

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

constexpr void track_paren_level(std::size_t &paren_level, char c) {
  paren_t cp{c};
  if (cp.is_paren()) {
    if (cp.is_open()){
      paren_level++;
    }
    else {
      assert(paren_level > 0);
      assert(cp.is_closed());
      --paren_level;
    }
  }
}

struct space_t{
  enum class t{
    space, tab, ret, none
  };
  static constexpr t space_type(char c){
    switch(c){
    case ' ': return t::space;
    case '\r': return t::ret;
    case '\t': return t::tab;
    default: return t::none;
    }
  }
  const t type{t::none};
  constexpr space_t(char c):type(space_type(c)){}
  constexpr space_t(){}
  constexpr bool is_space() const {
    return type != t::none;
  }
};

constexpr bool is_space(char c){return space_t{c}.is_space();}

template <std::size_t s, std::size_t t>
constexpr std::size_t split_outside_parens(char split, const fixed_cstr<s> &in,
                                           fixed_str<s> (&out)[t]) {
  std::size_t out_index = 0;
  std::size_t sub_index = 0;
  std::size_t paren_level{0};
  for (char c : in) {
    track_paren_level(paren_level,c);
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

template <std::size_t size>
constexpr bool contains_outside_parens(char target, const fixed_cstr<size> &src) {
  std::size_t paren_level = 0;

  for (char c : src) {
    track_paren_level(paren_level, c);
    if (paren_level > 0 ) continue;
    else if (c == target) return true;
  }
  return false;
}

template <std::size_t size1>
constexpr bool first_word_is(const fixed_cstr<size1> &target, const char* src) {
    std::size_t target_index{0};
    for (auto i = 0u; ;++i){
      char c = src[i];
      if (c == 0) return target[target_index] == 0;
      if (c == ' ') continue;
      else if (target[target_index] == 0) return true;
      else if (c != target[target_index]) return false;
      else ++target_index;
    }
}

template<std::size_t size>
constexpr std::size_t remove_first_word(fixed_str<size> &out, const char* in){
  bool on_first_word{false};
  for (auto i = 0u; in[i] != 0; ++i){
    char c = in[i];
    if (on_first_word){
      if (is_space(c)) return str_cpy(out,in + i);
    }
    else if (is_space(c)) continue;
    else {
      on_first_word = true;
    }
  }
  return 0;
}

template <std::size_t size1, std::size_t size2>
constexpr bool contains_outside_parens(const fixed_cstr<size1> &target, const fixed_cstr<size2> &src) {
  std::size_t paren_level = 0;

  std::size_t src_offset{0};
  for (char c : src) {
    track_paren_level(paren_level, c);
    if (paren_level == 0 ) {
      if (first_word_is(target, src + src_offset)) return true;
    }
    ++src_offset;
  }
  return false;
}

template<std::size_t dst_size>
constexpr std::size_t copy_within_parens(fixed_str<dst_size> &dst, const char* src){
  std::size_t paren_level{0};
  std::size_t dst_index{0};
  bool in_paren_group{false};
  for (auto i = 0u; src[i] != 0; ++i){
    char c = src[i];
    track_paren_level(paren_level, c);
    if (paren_level > 0) {
      if (!in_paren_group) {
        in_paren_group = true;
        continue; //skip first paren
      }
      dst[dst_index] = c;
      ++dst_index;
    }
    else if (in_paren_group) return dst_index; //only want first paren group.
  }
  return dst_index;
}

template<std::size_t dst_size>
constexpr std::size_t next_paren_group(fixed_str<dst_size> &dst, const char* src){
  if (is_space(src[0])) return 1 + next_paren_group(dst,src + 1);
  assert(is_paren(src[0]));
  return 2 + copy_within_parens(dst,src); //include the parens in the returned size.
}

} // namespace cstring
} // namespace mutils

#include "cstring_tests.hpp"