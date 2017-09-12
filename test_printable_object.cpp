#include "PrintableObject.hpp"
#include "../mutils-serialization/SerializationSupport.hpp"
#include <stdio.h>
#include <unistd.h>
#include <cassert>
#include <iostream>

using namespace mutils;
using namespace printable_object;

using Foo = MUTILS_NAME("foo");
using IntFoo = Field<Foo,int>;
using MyObj = MUTILS_NAME("MyObj");
using Bar = MUTILS_NAME("Bar");
using DoubleBar = Field<Bar,double>;
using Baz = MUTILS_NAME("Baz");
using StringBaz = Field<Baz,std::string>;

constexpr auto _is_it_constexpr(){
	
	PrintableObject<MyObj,IntFoo,DoubleBar> o;

	return o.getField(Bar::p) = 3;
}

constexpr bool is_it_constexpr(){
	return _is_it_constexpr() == 3;
}

int main(){
	static_assert(is_it_constexpr(),"");
	PrintableObject<MyObj,IntFoo,DoubleBar,StringBaz> o;
	o.getField(Foo::p) = 4;
	o.getField(Bar::p) = 12.4;
	o.getField(Baz::p) = "hi";
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
