#pragma once
#include <type_traits>
#include <map>
#include <cassert>

template<typename A, typename...> struct variant_impl;
template<typename...> struct variant;

template<typename A> struct variant_impl<A>{
public:
	A *a;
	using type = A;
	
	template<std::size_t i>
	std::enable_if_t<i == 0, A&> get(std::size_t index){
		assert(index == i);
		(void)index;
		return *a;
	}
};


template<typename A,typename B, typename... C> struct variant_impl<A,B,C...>{
public:
	using rst = variant_impl<C...>;
	using type = A;
	
	union {
		A *a;
		B *b;
		rst c;
	};
	
	template<std::size_t i>
	std::enable_if_t<i == 0, A&> get(int used_entry){
		assert(used_entry == i);
		(void)used_entry
		return *a;
	}
	
	template<std::size_t i>
	std::enable_if_t<i == 1, B&> get(int used_entry){
		assert(used_entry == i);
		(void)used_entry;
		return *b;
	}

	template<std::size_t i>
	std::enable_if_t<(i > 1), decltype(c.template get<i-2>(0))>
	get(int used_entry){
		assert(used_entry == i);
		return c.template get<i-2>(used_entry - 2);
	}
};

//matches first
template<std::size_t pack_size, typename,typename A, typename... B>
std::enable_if_t<pack_size == sizeof...(B) + 1,
	std::pair<std::size_t, variant_impl<A,B...> > > populate_variant_impl(std::size_t index, A &a){
	variant_impl<A,B...> ret;
	ret.a = &a;
	return std::make_pair(index,ret);
}

//matches second
template<std::size_t pack_size, typename,typename A, typename B, typename... C>
std::enable_if_t<pack_size == sizeof...(C) + 2,
	std::pair<std::size_t, variant_impl<A,B,C...> > > populate_variant_impl(std::size_t index, B &b){
	
	variant_impl<A,B,C...> ret;
	ret.b = &b;
	return std::make_pair(index+1,ret);
}

//matches third or later (or not at all)
template<std::size_t, typename A, typename B, typename C, typename... D>
std::enable_if_t<
	!std::is_same<A,B>::value && !std::is_same<A,C>::value
	,std::pair<std::size_t, variant_impl<B,C,D...> > > populate_variant_impl(std::size_t index, A &a){
	
	variant_impl<B,C,D...> ret;
	auto ret2 = populate_variant_impl<sizeof...(D),A,D...>(index+2,a);
	ret.c = ret2.second;
	return std::make_pair(ret2.first,ret);
}


template<typename A, typename... B>
std::pair<std::size_t, variant_impl<B...> > populate_variant(A &a){
	return populate_variant_impl<sizeof...(B),A,B...>(0,a);
}

template<typename... C> struct variant{
private:
	variant_impl<C...> impl;
public:
	const std::size_t used_entry;

	variant(const std::pair<std::size_t, variant_impl<C...> >& p)
		:impl(p.second),used_entry(p.first){}
	
	template<typename D>
	variant(D &d):variant(populate_variant<D,C...>(d)){}

	template<std::size_t index>
	inline auto get(){
		return impl.template get<index>(used_entry);
	}
};
