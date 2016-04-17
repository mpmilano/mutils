#pragma once
#include <memory>
#include <sstream>
#include "args-finder.hpp"

namespace mutils{

	template<typename key, typename value>
	struct _mapnode_super {
		const int height;
		_mapnode_super(const _mapnode_super&) = delete;
		_mapnode_super(int height):height(height){}
		virtual bool operator<(const _mapnode_super&) const = 0;
		
		bool operator==(const _mapnode_super& other) const {
			return !((*this) < other || other < (*this));
		}
	};
	
	template<typename key, typename value>
	struct _mapnode_empty : public _mapnode_super<key,value> {
		_mapnode_empty():_mapnode_super<key,value>{0}{}
		bool operator<(const _mapnode_super<key,value>& other) const {
			return (other.height > 0 ? true : false);
		}
	};
	
	template<typename key, typename value>
	struct _mapnode_nonempty;
	
	template<typename key, typename value>
	struct _mapnode {
	private:
		const std::shared_ptr<const _mapnode_super<key,value> > this_p;
	public:

		_mapnode(const _mapnode &l,const key &k,const value &v,const _mapnode &r, const int height);
		_mapnode(std::shared_ptr<const _mapnode_empty<key,value> >);
		_mapnode(std::shared_ptr<const _mapnode_nonempty<key,value> >);
		_mapnode();
		_mapnode(const _mapnode&) = default;

		template<typename Ret>
		Ret match(const std::function<Ret (std::shared_ptr<const _mapnode_empty<key,value> >)> &f,
				  const std::function<Ret (std::shared_ptr<const _mapnode_nonempty<key,value> >)> &g) const;

		bool operator==(const _mapnode &other) const {
			return (*this_p) == (*other.this_p);
		}

		bool operator<(const _mapnode &other) const {
			return (*this_p) < (*other.this_p);
		}

		bool operator>(const _mapnode &other) const {
			return other < (*this);
		}

		int height() const {
			return this_p->height;
		}
	};
	
	template<typename key, typename value>
	struct _mapnode_nonempty : public _mapnode_super<key,value> {
		const _mapnode<key,value> l;
		const key k;
		const value v;
		const _mapnode<key,value> r;
		_mapnode_nonempty(const decltype(l)& l, const decltype(k) &k, const decltype(v) &v, const decltype(r) &r, const int height)
			:_mapnode_super<key,value>(height),l(l),k(k),v(v),r(r){}

		bool operator<(const _mapnode_super<key,value>& _other) const {
			if (_other.height == 0) return false;
			else {
				const auto &other = *((_mapnode_nonempty const * const )&_other);
				if (k < other.k) return true;
				else if (k == other.k){
					if(v < other.v) return true;
					else if (v == other.v){
						if (l < other.l) return true;
						else if (l == other.l && r < other.r) return true;
					}
				}
			}
			return false;
		}
	};

	template<typename key, typename value>
	template<typename Ret>
	Ret _mapnode<key,value>::match(const std::function<Ret (std::shared_ptr<const _mapnode_empty<key,value> >)> &f,
									  const std::function<Ret (std::shared_ptr<const _mapnode_nonempty<key,value> >)> &g) const {
		if (auto ptr = std::dynamic_pointer_cast<const _mapnode_empty<key,value> >(this_p))
			return f(ptr); 
		else if (auto ptr =
				 std::dynamic_pointer_cast<const _mapnode_nonempty<key,value> >(this_p))
			return g(ptr); 
		else assert(false && "fell through");
	}

	template<typename key, typename value>
	_mapnode<key,value>::_mapnode(std::shared_ptr<const _mapnode_empty<key,value> > p)
		:this_p{p} {}

	template<typename key, typename value>
	_mapnode<key,value>::_mapnode(std::shared_ptr<const _mapnode_nonempty<key,value> > p)
		:this_p{p} {}


	template<typename key, typename value>
	_mapnode<key,value>::_mapnode():this_p{new _mapnode_empty<key,value>()} {}

	template<typename key, typename value>
	_mapnode<key,value>::_mapnode(const _mapnode<key,value> &l,const key& k,const value &v,const _mapnode<key,value> &r, const int height)
		:this_p(new _mapnode_nonempty<key,value>(l,k,v,r,height)){}

	template<typename key, typename value>
	std::ostream& operator<<(std::ostream& os, const _mapnode<key,value>& mn){
		return os << mn.template match<std::string>(
			[](auto){return "empty";},
			[](auto mn){
				std::stringstream ss;
				ss << "{" << mn->l << ";" << mn->k << ";" << mn->v << ";" << mn->r << "}";
				return ss.str();}
			);
	}
	
	template<typename Key, typename Value>
	struct map{
		using key = Key;
		using value = Value;
		using mapnode = _mapnode<Key,Value>;
		using empty = _mapnode_empty<Key,Value>;
		using nonempty = _mapnode_nonempty<Key,Value>;
		
		static auto create(const mapnode& l, const key& x, const value &d, const mapnode& r){
			int hl = l.height();
			int hr = r.height();
			return mapnode{l,x,d,r, hl >= hr ? hl + 1 : hr + 1};
		}

		static auto singleton(const key& x, const value &d){
			return mapnode{mapnode{},x,d,mapnode{},1};
		}

		static auto balance(const mapnode &l, const key &x, const value &d, const mapnode &r){
			static const std::function<mapnode (std::shared_ptr<const empty>)> invalid_arg =
				[](const auto&) -> mapnode {assert(false && "invalid arg!");};
			
			int hl = l.height();
			int hr = r.height();
			if (hl > hr + 2){
				return l.template match<mapnode>(
					/*empty*/ invalid_arg,
					/*nonempty*/[&](std::shared_ptr<const nonempty> l){
							if (l->l.height() >= l->r.height()){
								return create(l->l,l->k,l->v,create(l->r,x,d,r));
							}
							else {
								return l->r.template match<mapnode>(
									invalid_arg,
									[&](std::shared_ptr<const nonempty> lr){
										return create (
											create (l->l, l->k, l->v, lr->l),
											lr->k,
											lr->v,
											create (lr->r, x, d, r));
									}
									);
							}
						}
					);
			}
			else if (hr > hl + 2){
				return r.template match<mapnode>(invalid_arg,
							   [&](std::shared_ptr<const nonempty> r){
								   if (r->r.height() >= r->l.height()) 
									   return create (create (l, x, d, r->l),
													  r->k, r->v,r->r);
								   else {
									   return r->l.template match<mapnode>(invalid_arg,
														[&](std::shared_ptr<const nonempty> rl){
															return create (create (l, x, d, rl->l), rl->k, rl->v, create(rl->r, r->k, r->v, r->r));
														}
										   );
							 }
						}
					);
			}
			else {
				return mapnode{l,x,d,r, (hl >= hr ? hl + 1 : hr + 1)};
			}
		}

		static bool is_empty(const mapnode& mn){
			return mn.template match<bool>([](std::shared_ptr<const empty>){return true;},
							[](std::shared_ptr<const nonempty>){return false;}
				);
		}

		static int size(const mapnode& mn){
			return mn.template match<int>(
				[](std::shared_ptr<const empty>)-> int{return 0;},
				[](std::shared_ptr<const nonempty> mn) -> int{return size(mn->l) + 1 + size(mn->r);});
		}

		static constexpr int compare(const key& l, const key& r){
			if (l < r) return -1;
			else if (l == r) return 0;
			else if (l > r) return 1;
			else assert(false && "was not comparable!");
		}

		static mapnode mk_empty(){
			static const mapnode ret;
			return ret;
		}
		
		static mapnode add(const key& x, const value& data, const mapnode& __mn){
			auto cand = __mn.template match<mapnode> (
				[&](std::shared_ptr<const empty> e){return mapnode{e,x,data,e,1};},
				[&](std::shared_ptr<const nonempty> m){
					//std::cerr << std::endl;
					const int c = compare(x,m->k);
					if (c == 0) {
						//std::cerr << "replacing " << __mn << "with " << x << ":" << data;
						return (m->v == data ? __mn : mapnode{m->l,x,data,m->r,m->height});
					}
					else if (c < 0) {
						assert(x < m->k);
						const auto ll = add (x,data,m->l);
						if  (m->l == ll) {
							//std::cerr << m->l << " and " << ll << " are the same" << std::endl;
							return __mn;
						}
						else {
							auto bal = balance(ll, m->k, m->v, m->r);/*
							std::cerr << "left: " << ll <<  "this: " << "(" << m->k << ":" << m->v << ")" << "right: " << m->r <<std::endl;
							std::cerr << "pre-balance: " << create(ll,m->k,m->v,m->r) << std::endl;
							std::cerr << "post-balance: " << bal << std::endl; //*/
							return bal;
						}
					}
					else if (c > 0){
						assert(compare(x,m->k) == 1);
						assert(x > m->k);
						const auto rr = add (x, data, m->r);
						if (m->r == rr) {
							//std::cerr << m->r << " and " << rr << " are the same" << std::endl;
							return __mn;
						}
						else {
							auto bal = balance(m->l,m->k,m->v,rr);/*
							std::cerr << "left: " << m->l <<  "this: " << "(" << m->k << ":" << m->v << ")" << "right: " << rr << std::endl;
							std::cerr << "pre-balance: " << create(m->l,m->k,m->v,rr) << std::endl;
							std::cerr << "post-balance: " << bal << std::endl; //*/
							return bal;
						}
					}
					else assert(false && "fire!!!");
				});
			assert(size(cand) > size(__mn));
			return cand;
		}

		static const value& find (const key&x, const mapnode& __mn){
			return __mn.template match<const value&>(
				[&] (std::shared_ptr<const empty>)-> const value& {assert(false && "not found!");},
				[&] (std::shared_ptr<const nonempty> m)-> const value& {
					const auto c = compare(x,m->k);
					return (c == 0 ? m->v : find(x, (c < 0 ? m->l : m->r)));
				}
				);
		}

		static bool mem(const key&x, const mapnode &__mn){
			return __mn.template match<bool>(
				[](std::shared_ptr<const empty>){return false;},
				[&](std::shared_ptr<const nonempty> m){
					const auto c = compare(x,m->k);
					return ( c == 0 || mem(x,(c < 0 ? m->l : m->r )));
				}
				);
		}
	};

}
