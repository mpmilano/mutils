#pragma once
#ifndef NOTEST
#include "cstring.hpp"
namespace mutils {
namespace cstring {
#ifndef NDEBUG
constexpr bool copy_within_parens_test1() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = copy_within_parens<40>(out, "(copy only this)");
  (void)ret;
  str_cpy(out2, "copy only this");
  assert(str_len(out) == ret);
  return streq(out, out2);
}
constexpr bool copy_within_parens_test2() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = copy_within_parens(out, "ad (copy only this)");
  (void)ret;
  str_cpy(out2, "copy only this");
  assert(str_len(out) == ret);
  return streq(out, out2);
}
constexpr bool copy_within_parens_test3() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = copy_within_parens(out, "(copy only this) asfae ");
  (void)ret;
  str_cpy(out2, "copy only this");
  assert(str_len(out) == ret);
  return streq(out, out2);
}

constexpr bool copy_within_parens_test4() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = copy_within_parens(out, "(copy only this) (asfae) ");
  (void)ret;
  str_cpy(out2, "copy only this");
  assert(str_len(out) == ret);
  return streq(out, out2);
}

constexpr bool copy_within_parens_test5() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = copy_within_parens(out, "(copy only this (and this)) (asfae) ");
  (void)ret;
  str_cpy(out2, "copy only this (and this)");
  assert(str_len(out) == ret);
  return streq(out, out2);
}

constexpr bool next_paren_group_test1() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = next_paren_group<40>(out, "(copy only this)");
  str_cpy(out2, "copy only this");
  assert(streq(out, out2));
  return ret == 16;
}
constexpr bool next_paren_group_test2() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = next_paren_group(out, "ad (copy only this)");
  (void)ret;
  str_cpy(out2, "copy only this");
  assert(streq(out, out2));
  return ret == 19;
}
constexpr bool next_paren_group_test3() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = next_paren_group(out, "(copy only this) asfae ");
  (void)ret;
  str_cpy(out2, "copy only this");
  assert(streq(out, out2));
  return ret == 16;
}

constexpr bool next_paren_group_test4() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = next_paren_group(out, "(copy only this) (asfae) ");
  (void)ret;
  str_cpy(out2, "copy only this");
  assert(streq(out, out2));
  return ret == 16;
}

constexpr bool next_paren_group_test5() {
  char out[40] = {0};
  char out2[40] = {0};
  auto ret = next_paren_group(out, "(copy only this (and this)) (asfae) ");
  (void)ret;
  str_cpy(out2, "copy only this (and this)");
  assert(streq(out, out2));
  return ret == 27;
}

constexpr bool last_split_test() {
  using outstr = char[128];
  outstr outarr[2] = {{0}};
  outstr in = {0};
  str_cpy(in, "testing last split TOK wee TOK last split! TOK !!!");
  auto ret = last_split("TOK", in, outarr);
  auto correct_len = str_len("testing last split TOK wee TOK last split! ");
  assert(ret == correct_len);
  assert(streq(outarr[0], "testing last split TOK wee TOK last split! "));
  assert(streq(outarr[1], " !!!"));
  return true;
}
static_assert(last_split_test(), "testing last split");
static_assert(copy_within_parens_test1(), "testing copy_within_parens1");
static_assert(copy_within_parens_test2(), "testing copy_within_parens2");
static_assert(copy_within_parens_test3(), "testing copy_within_parens3");
static_assert(copy_within_parens_test4(), "testing copy_within_parens4");
static_assert(copy_within_parens_test5(), "testing copy_within_parens5");
static_assert(next_paren_group_test1(), "testing next_paren_group1");
static_assert(next_paren_group_test2(), "testing next_paren_group2");
static_assert(next_paren_group_test3(), "testing next_paren_group3");
static_assert(next_paren_group_test4(), "testing next_paren_group4");
static_assert(next_paren_group_test5(), "testing copy_within_parens5");
static_assert(first_word_is("the", " the woods are wary"),
              "Testing first_word_is");
static_assert(first_word_is("", " the woods are wary"),
              "Testing first_word_is");

constexpr bool str_cpy_test() {
  char out[40] = {0};
  const char *in = "this is the input string";
  auto ret = str_cpy(out, in);
  assert(ret == str_len(in));
  return streq(out, in);
}
static_assert(str_cpy_test(), "str_cpy_test");

constexpr bool remove_first_word_test() {
  char out[40] = {0};
  auto ret = remove_first_word(out, " this has many words");
  assert(ret == str_len(" has many words"));
  return streq(out, " has many words");
}

constexpr bool test_contains_outside_parens() { return true; }

static_assert(contains_outside_parens("-", "hndl2->a - 1"));
static_assert(contains_outside_parens("- ", "hndl2->a - 1"));

static_assert(num_words("  one  ") == 1);
static_assert(num_words("   ") == 0);
static_assert(num_words("  just  two  ") == 2);
static_assert(num_words("  just  quite a   few  ") == 4);

constexpr bool trim_test() {
  {
    char out[40] = {0};
    trim(out, "this ");
    assert(!contains(' ', out));
    assert(streq(out, "this"));
  }
  {
    char out[40] = {0};
    trim(out, " this");
    assert(!contains(' ', out));
    assert(streq(out, "this"));
  }
  {
    char out[40] = {0};
    trim(out, "  more complicated   ");
    assert(contains(' ', out));
    assert(streq(out, "more complicated"));
  }
  {
    char out[40] = {0};
    trim(out, " ");
    assert(!contains(' ', out));
    assert(streq(out, ""));
  }
  return true;
}

static_assert(trim_test(), "testing trim");
static_assert(remove_first_word_test(), "testing remove_first_word");

static_assert(contains_paren("(g->value)"));

#endif
} // namespace cstring
} // namespace mutils
#endif
