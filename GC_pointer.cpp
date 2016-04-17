#include "GC_pointer.hpp"
#include <thread>
#include <gc.h>
#include "GC_pointer.hpp"
#define GC_THREADS

namespace mutils{

	void* gc_malloc(int nbytes){
		return GC_MALLOC(nbytes);
	}

	GC_manager::GC_manager(){
		GC_INIT();
	}

	const GC_manager& GC_manager::inst(){
		static GC_manager gcm;
		return gcm;
	}
}
