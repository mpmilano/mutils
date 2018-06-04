#pragma once
#ifndef NOTEST
#include "cstring.hpp"
namespace mutils { namespace cstring {

constexpr bool copy_within_parens_test1(){
  char out[40] = {0};
  char out2[40] = {0};
  copy_within_parens(out,"(copy only this)");
  str_cpy(out2,"copy only this");
  return streq(out,out2);
}
constexpr bool copy_within_parens_test2(){
  char out[40] = {0};
  char out2[40] = {0};
  copy_within_parens(out,"ad (copy only this)");
  str_cpy(out2,"copy only this");
  return streq(out,out2);
}
constexpr bool copy_within_parens_test3(){
  char out[40] = {0};
  char out2[40] = {0};
  copy_within_parens(out,"(copy only this) asfae ");
  str_cpy(out2,"copy only this");
  return streq(out,out2);
}

constexpr bool copy_within_parens_test4(){
  char out[40] = {0};
  char out2[40] = {0};
  copy_within_parens(out,"(copy only this) (asfae) ");
  str_cpy(out2,"copy only this");
  return streq(out,out2);
}

static_assert(copy_within_parens_test1(), "testing copy_within_parens");
static_assert(copy_within_parens_test2(), "testing copy_within_parens");
static_assert(copy_within_parens_test3(), "testing copy_within_parens");
static_assert(copy_within_parens_test4(), "testing copy_within_parens");
static_assert(first_word_is("the", " the woods are wary"),"Testing first_word_is");
static_assert(first_word_is("", " the woods are wary"),"Testing first_word_is");

constexpr bool str_cpy_test(){
  char out[40] = {0};
  const char* in = "this is the input string";
  auto ret = str_cpy(out,in);
  assert(ret == str_len(in));
  return streq(out,in);
}
static_assert(str_cpy_test(),"str_cpy_test");

constexpr bool remove_first_word_test(){
  char out[40] = {0};
  auto ret = remove_first_word(out," this has many words");
  assert(ret == str_len(" has many words"));
  return streq(out," has many words");
}
static_assert(remove_first_word_test(), "testing remove_first_word");

}}
#endif