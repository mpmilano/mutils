#pragma once
#include <string>
#include <sstream>

namespace mutils{
	
constexpr bool
isDigit(char c)
{
  switch (c) {
    case '0':
      return true;
    case '1':
      return true;
    case '2':
      return true;
    case '3':
      return true;
    case '4':
      return true;
    case '5':
      return true;
    case '6':
      return true;
    case '7':
      return true;
    case '8':
      return true;
    case '9':
      return true;
    default:
      return false;
  };
}

constexpr unsigned char
toInt(char c)
{
  struct bad_parse
  {
  };
  switch (c) {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    default:
      throw bad_parse{};
  };
}

	constexpr char intToChar(int c)
{
  struct bad_arg
  {
  };
  switch (c) {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    default:
      throw bad_arg{};
  };
}

constexpr int
exp(int base, int exponent)
{
  return (exponent == 0 ? 1 : base * exp(base, exponent - 1));
}

template <typename T>
struct string_of
{
  const std::string value;
  string_of()
    : value([] {
      std::stringstream ss;
      ss << T{};
      return ss.str();
    }())
  {
  }
};

	template<typename> struct remove_ptr_str;
	template<typename T> struct remove_ptr_str<T*>{using type = typename remove_ptr_str<T>::type;};
	template<typename T> struct remove_ptr_str{using type = T;};
	template<typename T> using remove_ptr = typename remove_ptr_str<T>::type;
	
}
