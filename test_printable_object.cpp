#include "PrintableObject.hpp"
#include "SerializationSupport.hpp"
#include <stdio.h>
#include <unistd.h>
#include <cassert>
#include <iostream>

using namespace mutils;
using namespace printable_object;

	using Foo = Name<'f','o','o'>;
	constexpr Foo const * const foo{nullptr};
	using IntFoo = Field<Foo,int>;

	using MyObj = Name<'M','y','O','b','j'>;

	using Bar = Name<'b','a','r'>;
	constexpr Bar const*const bar{nullptr};
	using DoubleBar = Field<Bar,double>;

constexpr auto _is_it_constexpr(){
	
	PrintableObject<MyObj,IntFoo,DoubleBar> o;

	return o.getField(bar) = 3;
}

constexpr bool is_it_constexpr(){
	return _is_it_constexpr() == 3;
}

int main(){
	static_assert(is_it_constexpr(),"");
	PrintableObject<MyObj,IntFoo,DoubleBar> o;
	o.getField(foo) = 4;
	o.getField(bar) = 12.4;
	std::cout << o << std::endl;
	auto fd = fopen("/tmp/test","w+");
	assert(fd);
	struct Fun {
		static void write(decltype(fopen("/tmp/test","w+")) fd, void const * const bytes, std::size_t size){
			fwrite(bytes,size,1,fd);
		}};
	post_object(Fun::write,o,fd);
	std::stringstream ss;
	ss << o;
	std::string str = ss.str();
	auto o2 = from_string<decltype(o)>(nullptr,str.c_str(),str.length());
	std::cout << *o2 << std::endl;
}
