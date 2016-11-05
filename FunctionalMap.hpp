#pragma once
#include <sstream>
#include <cassert>
#include <type_traits>
#include <memory>
#include "args-finder.hpp"

namespace mutils{

	struct FunctionalMapError{};

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
		std::shared_ptr<const _mapnode_super<key,value> > this_p;
	public:

		_mapnode(const _mapnode &l,const key &k,value v,const _mapnode &r, const int height);
		_mapnode(const std::shared_ptr<const _mapnode_empty<key,value> > &);
		_mapnode(const std::shared_ptr<const _mapnode_nonempty<key,value> > &);
		_mapnode();
		_mapnode(const _mapnode& mn) = default;

		/*
		template<typename Ret>
		Ret match(const std::function<Ret (std::shared_ptr<const _mapnode_empty<key,value> >)> &f,
		const std::function<Ret (std::shared_ptr<const _mapnode_nonempty<key,value> >)> &g) const;//*/

		std::shared_ptr<const _mapnode_empty<key,value> > as_empty() const;
		std::shared_ptr<const _mapnode_nonempty<key,value> > as_nonempty() const;
		

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
		value v;
		const _mapnode<key,value> r;
		_mapnode_nonempty(const decltype(l)& l, const decltype(k) &k, const decltype(v) &v, const decltype(r) &r, const int height)
			:_mapnode_super<key,value>(height),l(l),k(k),v(v),r(r){}

		bool operator<(const _mapnode_super<key,value>& _other) const {
			if (_other.height == 0) return false;
			else {
				const auto &other = *((_mapnode_nonempty const * const )&_other);
				if (k < other.k) return true;
				else if (k == other.k){
					using fakev_t = std::array<char,sizeof(v)/sizeof(char)>;
					fakev_t *fake_v = (fakev_t*) &v;
					fakev_t *fake_vother = (fakev_t*) &other.v;
					if(fake_v < fake_vother) return true;
					else if (fake_v == fake_vother){
						if (l < other.l) return true;
						else if (l == other.l && r < other.r) return true;
					}
				}
			}
			return false;
		}
	};

	template<typename key, typename value>
	std::shared_ptr<const _mapnode_empty<key,value> > _mapnode<key,value>::as_empty() const {
		if (height() == 0)
			return std::static_pointer_cast<const _mapnode_empty<key,value> >(this_p);
		else return std::shared_ptr<_mapnode_empty<key,value> >{nullptr};
	}

	template<typename key, typename value>
	std::shared_ptr<const _mapnode_nonempty<key,value> > _mapnode<key,value>::as_nonempty() const {
		if (height() > 0)
			return std::static_pointer_cast<const _mapnode_nonempty<key,value> >(this_p);
		else return std::shared_ptr<_mapnode_nonempty<key,value> >{nullptr};
	}	

	template<typename key, typename value>
	_mapnode<key,value>::_mapnode(const std::shared_ptr<const _mapnode_empty<key,value> > &p)
		:this_p{p} {}

	template<typename key, typename value>
	_mapnode<key,value>::_mapnode(const std::shared_ptr<const _mapnode_nonempty<key,value> > &p)
		:this_p{p} {}


	template<typename key, typename value>
	_mapnode<key,value>::_mapnode()
		:this_p{std::make_shared<_mapnode_empty<key,value> >()} {}

	template<typename key, typename value>
	_mapnode<key,value>::_mapnode(const _mapnode<key,value> &l,const key& k,value v,const _mapnode<key,value> &r, const int height)
		:this_p(std::make_shared<_mapnode_nonempty<key,value> >(l,k,v,r,height)){}

	template<typename key, typename value>
	std::ostream& operator<<(std::ostream& os, const _mapnode<key,value>& _mn){
		if (auto mn = _mn.as_nonempty()){
			return os << "{" << mn->l << ";" << mn->k << ";" << mn->v << ";" << mn->r << "}";
		} else return os << "empty";
	}
	
	template<typename Key, typename Value>
	struct functional_map{
		using key = Key;
		using value = Value;
		using mapnode = _mapnode<Key,Value>;
		using empty = _mapnode_empty<Key,Value>;
		using nonempty = _mapnode_nonempty<Key,Value>;
		
		static auto create(const mapnode& l, const key& x, value d, const mapnode& r){
			int hl = l.height();
			int hr = r.height();
			return mapnode{l,x,d,r, hl >= hr ? hl + 1 : hr + 1};
		}

		static mapnode singleton(const key &x, const value& d){
			return mapnode{mapnode{},x,d,mapnode{},1};
		}

		static auto balance(const mapnode &l, const key &x, value d, const mapnode &r){
			
			int hl = l.height();
			int hr = r.height();
			if (hl > hr + 2){
				if (l.as_empty()) {
					assert(false && "invalid arg!");
					throw FunctionalMapError{};
				}
				else if (auto l2 = l.as_nonempty()){
					if (l2->l.height() >= l2->r.height()){
						return create(l2->l,l2->k,l2->v,create(l2->r,x,d,r));
					}
					else {
						if (l2->r.as_empty()){
							assert(false && "invalid arg!");
							throw FunctionalMapError{};
						}
						else if (auto lr = l2->r.as_nonempty()){
							return create (create (l2->l, l2->k, l2->v, lr->l),lr->k,lr->v,create (lr->r, x, d, r));
						}
					}
				}
			}
			else if (hr > hl + 2){
				auto r2 = r;
				{
					if (r2.as_empty()) {
						assert(false && "invalid arg!");
						throw FunctionalMapError{};
					}
					else if (auto r = r2.as_nonempty()){
						if (r->r.height() >= r->l.height()) 
							return create (create (l, x, d, r->l),
										   r->k, r->v,r->r);
						else {
							if (r->l.as_empty()) {
								assert(false && "invalid arg!");
								throw FunctionalMapError{};
							}
							else if (auto rl = r->l.as_nonempty()) {
								return create (create (l, x, d, rl->l), rl->k, rl->v, create(rl->r, r->k, r->v, r->r));
							}
						}
					}
				}
			}
			else {
				return mapnode{l,x,d,r, (hl >= hr ? hl + 1 : hr + 1)};
			}
			assert(false);
			throw FunctionalMapError{};
		}

		static bool is_empty(const mapnode& mn){
			return mn.as_empty().get();
		}

		static int size(const mapnode& mn){
			if (auto _mn = mn.as_nonempty()){
				return size(_mn->l) + 1 + size(_mn->r);
			} else return 0;
		}

		static constexpr int compare(const key& l, const key& r){
			return (l < r ? -1 : (l == r ? 0 : 1));
		}

		static mapnode mk_empty(){
			static const mapnode ret{};
			return ret;
		}
		
		static mapnode add(
						   const key& x, value data, const mapnode& __mn){
			if (auto m = __mn.as_nonempty()){
								//std::cerr << std::endl;
					const int c = compare(x,m->k);
					if (c == 0) {
						//std::cerr << "replacing " << __mn << "with " << x << ":" << data;
						return mapnode{m->l,x,data,m->r,m->height};
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
					else {
						assert(false && "fire!!!");
						struct dead_code{};
						throw dead_code{};
					}
			} else return mapnode{__mn,x,data,__mn,1};
		}
		
		static value find (const key&x, const mapnode& __mn){
			if (auto m = __mn.as_nonempty()) {
				const auto c = compare(x,m->k);
				return (c == 0 ? m->v : find(x, (c < 0 ? m->l : m->r)));
			} else {
				assert(false && "not found!");
				throw FunctionalMapError{};
			}
		}

		static bool mem(const key&x, const mapnode &__mn){
			if (is_empty(__mn)) return false;
			else if (auto m = __mn.as_nonempty()){
				const auto c = compare(x,m->k);
				return ( c == 0 || mem(x,(c < 0 ? m->l : m->r )));
			}
			else {
				assert(false);
				struct dead_code{};
				throw dead_code{};
			}
		}
	};
}
