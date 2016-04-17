#include <type_traits>
#include <utility>
#pragma once

namespace mutils{

	void* gc_malloc(int nbytes);

	struct GC_manager;

	struct gc_unique_type;
	
	template<typename T>
	class GC_ptr{
	private:
		T* t;
	public:
		
		T const * operator->() const {
			return t;
		}
		
		T* operator->() {
			return t;
		}

		T* get() {
			return t;
		}

		T& operator*(){
			return *t;
		}

		const T& operator*() const {
			return *t;
		}

		operator bool() const {
			return t;
		}
		
	private:

		static_assert(std::is_trivially_destructible<T>::value,
					  "Error: GC will not call destructor!");
		
		template<typename... Args>
		GC_ptr(gc_unique_type*, Args &&... args):
			t(new (gc_malloc(sizeof(T))) T(std::forward<Args>(args)...) ){}
		
		GC_ptr(T* t):t(t){}
	public:


		GC_ptr():t(nullptr){}
		
		GC_ptr(const GC_ptr&) = default;

		template<typename T2>
		GC_ptr(const GC_ptr<T2> &g):t(g.t){
			static_assert(std::is_base_of<T,T2>::value,"Error: cannot convert");
		}

		template<typename T2>
		GC_ptr<T2> downCast() const {
			static_assert(std::is_base_of<T,T2>::value,"Error: cannot convert");
			return GC_ptr<T2>((T2*)t);
		}

		template<typename T2>
		friend class GC_ptr;
		friend struct GC_manager;
		
	};

	struct GC_manager{
	private:
		GC_manager();
	public:
		GC_manager(const GC_manager&) = delete;

		static const GC_manager& inst();

		template<typename T, typename... Args>
		GC_ptr<T> make(Args && ... args) const {
			gc_unique_type* np{nullptr};
			return GC_ptr<T>{np,std::forward<Args>(args)...};
		}
	};

}
