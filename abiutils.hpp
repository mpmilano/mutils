#pragma once
#include <cxxabi.h>
#include "private_access.hpp"

namespace mutils{
	
	struct raw_exn_accessor {typedef void* (std::exception_ptr::*type);};
	template struct stow_private<raw_exn_accessor,&std::exception_ptr::__ptr_>;

	inline void* access_raw_exn(std::exception_ptr p){
		return p.*stowed<raw_exn_accessor>::value;
	}

	inline
	__cxxabiv1::__cxa_exception*
	cxa_exception_from_thrown_object(void* thrown_object)
	{
		return static_cast<__cxxabiv1::__cxa_exception*>(thrown_object) - 1;
	}
	
	inline __cxxabiv1::__cxa_exception* access_cxa_exn(std::exception_ptr p){
		return cxa_exception_from_thrown_object(access_raw_exn(p));
	}

	std::type_info* exn_type(std::exception_ptr p){
		return access_cxa_exn(p)->exceptionType;
	}

}
