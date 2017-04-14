#pragma once
#include "restrict.hpp"
#include "args-finder.hpp"
#include <type_traits>
#include <cassert>
#include <tuple>
#include <set>
#include <vector>
#include <map>
#include <mutex>
#include <dirent.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "extras"
#include "macro_utils.hpp"
#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <list>
#include <cstdlib>
#include "AtScopeEnd.hpp"

namespace mutils{

	/*
	  //Disabled: untested, unused
	template<typename T, std::size_t size1, std::size_t size2>
	auto prefix_array(const std::array<T,size1>& t,
					  const std::array<T,size2> &arr,
					  const T& remove){
		assert(remove == t[0]);
		assert(remove == arr[0]);
		std::array<T, size1 + size2 - 1 > ret;
		std::size_t i = 0;
		for (; i < size1; ++i){
			ret[i] = t[i];
		}
		std::size_t j = 1;
		for (; i < size1 + size2 -1 ; ++i){
			ret[i] = arr[j];
			++j;
		}
		return ret;
		}//*/

	template<typename T>
	constexpr T mke(){
		return *((typename std::decay<T>::type*) nullptr);
	}

	template<typename> struct is_shared_ptr_str : std::false_type{};
	template<typename T> struct is_shared_ptr_str<std::shared_ptr<T> > : std::true_type{};
	template<typename T> using is_shared_ptr = is_shared_ptr_str<T>;

	template<typename> struct is_unique_ptr_str : std::false_type{};
	template<typename T> struct is_unique_ptr_str<std::unique_ptr<T> > : std::true_type{};
	template<typename T> using is_unique_ptr = is_unique_ptr_str<T>;

	template<typename>
	struct is_set : std::false_type {};

	template<typename T>
	struct is_set<std::set<T> > : std::true_type {};

	template<typename T>
	struct extract_type_if_set_str {
		static_assert(!is_set<T>::value,"internal error");
		using type = T;
	};	

	template<typename T>
	struct extract_type_if_set_str<std::set<T> > {
		using type = T;
	};

	template<typename T>
	using extract_type_if_set = typename extract_type_if_set_str<T>::type;

	template<typename T>
	constexpr typename std::enable_if<!std::is_pointer<T>::value,typename std::decay<T>::type*>::type
	mke_p(){
		static_assert(!is_shared_ptr<std::decay_t<T> >::value,"Error: mke_p on shared pointer");
		static_assert(!is_unique_ptr<std::decay_t<T> >::value,"Error: mke_p on unique pointer");
		return (typename std::decay<T>::type*) nullptr;
	}

	template<typename T, restrict(std::is_pointer<T>::value)>
	constexpr T mke_p(){
		return (T) nullptr;
	}

	template<typename T>
	T cpy(const T& t){
		return T(t);
	}

	template<int ...>
	struct seq { };

	template<int N, int ...S>
	struct gens : gens<N-1, N-1, S...> { };

	template<int ...S>
	struct gens<0, S...> {
		typedef seq<S...> type;
		static type build(){ return type();}
	};

	template<typename Obj, typename Tuple, int ...S>
	auto __make_unique_tupleargs(const Tuple &t, seq<S...>) {
		return std::make_unique<Obj>(std::get<S>(t)...);
	}


	template<typename Obj, typename Tuple>
	auto make_unique_tupleargs(const Tuple &t) {
		return __make_unique_tupleargs<Obj>(t,gens<std::tuple_size<Tuple>::value >::build() );
	}

	
	template<typename F, typename Tuple, int ...S>
	auto __callFunc(const F& f, const Tuple &t, seq<S...>) {
		return f(std::get<S>(t)...);
	}


	template<typename F, typename Tuple, restrict(!std::is_function<F>::value)>
	auto callFunc(const F &f, const Tuple &t) {
		return __callFunc(f,t,gens<std::tuple_size<Tuple>::value >::build() );
	}

	template<typename Ret, typename Tuple, typename... Args>
	auto callFunc(Ret (*f) (Args...), const Tuple &t) {
		return __callFunc(f,t,gens<std::tuple_size<Tuple>::value >::build() );
	}

	template<typename Ret, typename Tuple, typename Pack, typename... Args>
	Ret callFunc(Ret (*f) (Args...), const Tuple &t, Pack p) {
		return __callFunc(convert(f),t,p);
	}

	template<typename F, typename Tuple, int ...S>
	auto __callFunc(const F& f, Tuple &t, seq<S...>) {
		return f(std::get<S>(t)...);
	}


	template<typename F, typename Tuple, restrict(!std::is_function<F>::value)>
	auto callFunc(const F &f, Tuple &t) {
		return __callFunc(f,t,gens<std::tuple_size<Tuple>::value >::build() );
	}

	template<typename Ret, typename Tuple, typename... Args>
	auto callFunc(Ret (*f) (Args...), Tuple &t) {
		return __callFunc(f,t,gens<std::tuple_size<Tuple>::value >::build() );
	}

	template<typename Ret, typename Tuple, typename Pack, typename... Args>
	Ret callFunc(Ret (*f) (Args...), Tuple &t, Pack p) {
		return __callFunc(convert(f),t,p);
	}

	template<typename Ret, typename Tuple, int ...S>
	Ret callConstructor(const Tuple &t, seq<S...>) {
		return Ret(std::get<S>(t)...);
	}

	template<typename Arg>
	constexpr Arg last_of_f(const std::tuple<Arg>*){
		return mke<Arg>();
	}

	template<typename Arg1, typename... Args>
	constexpr decltype(last_of_f(mke_p<std::tuple<Args...> >()))
		last_of_f(const std::tuple<Arg1, Args...>*){
		return last_of_f(mke_p<std::tuple<Args...> >());
	}

	template<typename... T>
	struct last_of {
		static_assert(sizeof...(T) > 0, "Error: cannot call last_of on empty packs");
		using type = decltype(last_of_f(mke_p<std::tuple<typename std::decay<T>::type...> >()));
	};

	template<typename T>
	std::unique_ptr<T> heap_copy(const T& t){
		return std::make_unique<T>(t);
	}

	template<typename T>
	std::shared_ptr<T> shared_copy(const T& t){
		return std::make_shared<T>(t);
	}

	template<const int i,restrict(i <= 0)>
	constexpr unsigned long long unique_id(const char*){
		return 0;
	}
	

		template<const int i>
		constexpr typename std::enable_if<(i > 0), unsigned long long>::type
		unique_id(const char str[i]){
			return (i == 0 ? 0 : (str[0] << sizeof(char)*i ) + unique_id<i-1>(str+1));
		}
	
	template <class T>
	std::string
	type_name()
	{
		typedef typename std::remove_reference<T>::type TR;
		std::unique_ptr<char, void(*)(void*)> own
			(
#ifndef _MSC_VER
				abi::__cxa_demangle(typeid(TR).name(), nullptr,
									nullptr, nullptr),
#else
				nullptr,
#endif
				std::free
				);
		std::string r = own != nullptr ? own.get() : typeid(TR).name();
		if (std::is_const<TR>::value)
			r += " const";
		if (std::is_volatile<TR>::value)
			r += " volatile";
		if (std::is_lvalue_reference<T>::value)
			r += "&";
		else if (std::is_rvalue_reference<T>::value)
			r += "&&";
		return r;
	}

	template<typename A, typename B>
	constexpr auto conditional(std::true_type*, const A a, const B &){
		return a;
	}

	template<typename A, typename B>
	constexpr auto conditional(std::false_type*, const A &, const B b2){
		return b2;
	}

	template<bool b, typename A, typename B>
	constexpr auto conditional(const A &a, const B &b2){
		typedef typename std::integral_constant<bool,b>::type ptr;
		ptr* p = nullptr;
		return conditional(p,a,b2);
	}


	template<typename> struct _Left;
	template<typename A, typename B> struct _Left<std::pair<A,B> >{
		typedef A type;
	};
	template<typename T>
	using Left = typename _Left<T>::type;

	template<typename> struct _Right;
	template<typename A, typename B> struct _Right<std::pair<A,B> >{
		typedef B type;
	};
	template<typename T>
	using Right = typename _Right<T>::type;



	unsigned int gensym();

	int get_thread_id();

	std::vector<std::string> read_dir(const std::string &name);

	template<typename T>
	std::unique_ptr<T> make_unique(T *t){
		return std::unique_ptr<T>(t);
	}

	template<typename T, restrict(!(std::is_same<T CMA std::nullptr_t>::value))>
	std::shared_ptr<const T> make_cnst_shared(std::shared_ptr<T> t){
		return std::static_pointer_cast<const T, T>(t);
	}

	template<typename>
	std::shared_ptr<const std::nullptr_t> make_cnst_shared(std::nullptr_t){
		return std::shared_ptr<const std::nullptr_t>();
	}

	template<typename T>
	const T& constify(const T& t){
		return t;
	}



	template<typename T>
	std::string to_string(const T &t){
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	template<typename T>
	std::enable_if_t<!std::is_same<T, std::nullptr_t>::value, T>
	choose_non_np(std::nullptr_t, const T& t){
		return t;
	}

	template<typename T>
	std::enable_if_t<!std::is_same<T, std::nullptr_t>::value, T>
	choose_non_np(const T& t, std::nullptr_t){
		return t;
	}

	std::nullptr_t choose_non_np(std::nullptr_t, std::nullptr_t);

	template<typename T>
	std::enable_if_t<!std::is_same<T, std::nullptr_t>::value, T>
	choose_non_np(const T& t1, const T& t2){
		if (t1) return t1; else return t2;
	}

	template<typename A, typename B, typename C, typename... D>
	auto choose_non_np(const A& a, const B &b, const C &c, const D & ... d){
		return choose_non_np(choose_non_np(a,b),c,d...);
	}

	std::nullptr_t dref_np(std::nullptr_t*);

	template<typename T, restrict(!(std::is_same<T,std::nullptr_t>::value))>
	T dref_np(const T& t){ return t;}

	/*
	  template<typename T, typename U>
	  typename std::enable_if<!(std::is_same<T,std::nullptr_t>::value || std::is_same<T,std::nullptr_t>::value), T>::type
	  pick_useful(const U& u, const T& t){
	  return t;
	  }
	//*/

	template<typename... T>
	constexpr void discard(const T & ...) {}

	void break_here();

	template<typename T, typename Ret>
	auto map(std::vector<T> &v, Ret (*f) (T&) ){
		std::vector<Ret> out;
		for (auto &e : v){
			out.emplace_back(f(e));
		}
		return out;
	}

	template<typename T, typename Ret>
	auto map(const std::vector<T> &v, Ret (*f) (const T&) ){
		std::vector<Ret> out;
		for (auto &e : v){
			out.emplace_back(f(e));
		}
		return out;
	}


	template <typename T>
	using is_vector = std::is_same<T, std::vector< typename T::value_type,
												   typename T::allocator_type > >;

	bool init_rand(int seed);

	template<typename Lock, typename T>
	auto trylock(Lock &l, const std::function<T ()> &then, const std::function<T ()> &els){
		if (std::try_lock(l) == -1) {
			AtScopeEnd ase{[&](){l.unlock();}};
			return then();
		}
		else return els();
	}
	
	double better_rand();

	long int long_rand();

	unsigned int int_rand();

	constexpr unsigned char int_from_ascii(char c){
		switch(c){
		case '0' : return 0;
		case '1' : return 1;
		case '2' : return 2;
		case '3' : return 3;
		case '4' : return 4;
		case '5' : return 5;
		case '6' : return 6;
		case '7' : return 7;
		case '8' : return 8;
		case '9' : return 9;
		default: return -1;
		}
	}

	constexpr unsigned int decode_ip(char const * const v){
		unsigned int ret{0};
		unsigned int accum{0};
		unsigned int multiplier = 1000;
		for (auto i = 0; ; ++i){
			if (v[i] == '.' || v[i] == 0){
				accum /= multiplier;
				accum <<= 8*3;
				multiplier = 1000;
				ret >>= 8;
				ret += accum;
				accum = 0;
				if (v[i] == '.') continue;
				else if (v[i] == 0) break;
			}
			multiplier /= 10;
			accum += multiplier * int_from_ascii(v[i]);
		}
		return ret;
	}

	static_assert(decode_ip("0.0.0.0") == 0,"");
	static_assert(decode_ip("1...") == 1,"");
	static_assert(decode_ip("12.0.0.0") == 12,"");
	
	std::string string_of_ip(unsigned int i);
	
	using type_id = int;
	type_id type_id_counter(bool increment);
	
	template<typename T>
	type_id get_type_id(){
		static auto id_for_T = type_id_counter(true);
		return id_for_T;
	}

	template<typename T>
	struct ReassignableReference{
	private:
		T *t;
	public:
		operator T&(){return *t;}
		T& get(){return *t;}
		const T& get() const {return *t;}
		void reset(T& _t){this->t = & _t;}
		
		ReassignableReference(T& t):t(&t){}
	};

	std::list<std::string> split(const std::string &s, char c, std::size_t pos = 0);

	void copy_into(std::size_t how_many,std::size_t const * const sizes,void ** bufs, char* src);

	constexpr auto total_size(std::size_t how_many, std::size_t const * const sizes){
		std::size_t accum{0};
		for (unsigned int i = 0; i < how_many; ++i){
			accum += sizes[i];
		}
		return accum;
	}

	struct comma_is_space : std::ctype<char> {
		//from http://stackoverflow.com/questions/7302996/changing-the-delimiter-for-cin-c
		comma_is_space();
		static typename std::ctype_base::mask const* get_table();
	};
	
	struct comma_space {
		constexpr comma_space() = default;
	};
	std::ostream& operator<<(std::ostream& o, const mutils::comma_space&);

	std::istream& operator>>(std::istream& o, const mutils::comma_space&);
}


namespace std{
		template<std::size_t size, typename T>
	std::ostream & operator<<(std::ostream &os, const std::array<T,size>& so){
		os << "[";
		for (auto &e : so){
			os << e << ",";
		}
		return os << "]";
	}

	std::ostream & operator<<(std::ostream &os, const std::vector<char>& so);
	std::ostream & operator<<(std::ostream &os, const std::vector<unsigned char>& so);
	
	template<typename T>
	std::ostream & operator<<(std::ostream &os, const std::vector<T>& so){
		os << "<[";
		for (auto &e : so){
			os << e << ",";
		}
		return os << "]>";
	}
}
