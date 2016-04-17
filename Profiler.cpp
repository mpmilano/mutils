#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Profiler.hpp"
#include "GC_pointer.hpp"

namespace mutils{

	Profiler::ProfilerPauseScopeGoverner::ProfilerPauseScopeGoverner(Profiler::ProfilerActive pa):active(pa){
		assert(active);
		const auto tid = std::this_thread::get_id();
		pause_map_ns::find(tid,active->thread_pausing)->first = true;
		assert(pause_map_ns::find(tid,active->thread_pausing)->first == true);
		assert(pause_map_ns::mem(tid,active->thread_pausing));
		assert(active->paused());
	}
	
	Profiler::ProfilerPauseScopeGoverner::~ProfilerPauseScopeGoverner(){
		const auto tid = std::this_thread::get_id();
		pause_map_ns::find(tid,active->thread_pausing)->first = false;
		assert(pause_map_ns::find(tid,active->thread_pausing)->first == false);
		assert(pause_map_ns::mem(tid,active->thread_pausing));
		assert(!active->paused());
	}

	namespace {
		Profiler::ProfilerActive& staticmem(){
			static Profiler::ProfilerActive ret{nullptr};
			return ret;
		}

		std::mutex& staticmutex(){
			static std::mutex m;
			return m;
		}
	}
	
	Profiler::ProfilerActive Profiler::startProfiling(bool assertActive) {
		auto &ret = staticmem();
		assert(!assertActive || bool{ret});
		if (!ret) {
			std::unique_lock<std::mutex> lock{staticmutex()};
			if (!ret) staticmem() = new ProfilerScopeGoverner();
		}
		return ret;
	}

	Profiler::ProfilerActive Profiler::stopProfiling(bool assertActive){
		assert(!assertActive || !staticmem());
		auto &ret = staticmem();
		if (ret){
			std::unique_lock<std::mutex> lock{staticmutex()};
			if (ret) {
				delete staticmem(); staticmem() = nullptr;
			}
		}
		return staticmem();
	}

	Profiler::ProfilerActive Profiler::profiling(){
		return staticmem();
	}
	
	Profiler::ProfilerPaused Profiler::pauseIfActive(){
		if (auto *prof = profiling())
			return prof->pause();
		else return ProfilerPaused{nullptr};
	}

	bool Profiler::pausedOrInactive(){
		if (auto *prof = profiling())
			return prof->paused();
		else return true;
	}

	Profiler::ProfilerScopeGoverner::ProfilerScopeGoverner():
		thread_pausing{pause_map_ns::mk_empty(GC_manager::inst())},
		thread_locked(false),
		profopts{
			[](void* v) -> int {
				return !((ProfilerScopeGoverner*)v)->paused();
			},this}{
			static char fname[500];
			static bool first_run = true;
			if (first_run) {
				std::strcpy(fname,[](){
						static constexpr char _default[] = "/tmp/out.prof";
						auto env_var = std::getenv("profilerPath");
						return env_var ? env_var : _default;}());
				first_run = false;
			}
			ProfilerStartWithOptions(fname,&profopts);
		}

	void Profiler::ProfilerScopeGoverner::threadLock(){
		thread_locked = true;
	}
	
	typename Profiler::ProfilerPaused Profiler::ProfilerScopeGoverner::pause(){
		const auto tid = std::this_thread::get_id();
		{
			//take a candidate from the map, creating a new entry in the map
			//for this thread if none exists
			if (!pause_map_ns::mem(tid,thread_pausing)){
				if (thread_locked){
					std::cerr << "this thread is new! We've already seen " << pause_map_ns::size(thread_pausing) << std::endl;
					}
				else {
				}
				assert(!thread_locked);
				std::unique_lock<std::mutex> lock{m};
				const auto curr_size = pause_map_ns::size(thread_pausing);
				auto np = std::make_unique<std::pair<bool,paused_wp> >();
				thread_pausing = pause_map_ns::add(GC_manager::inst(),tid,np.get(),thread_pausing);
				memory.push_back(std::move(np));
				assert(pause_map_ns::size(thread_pausing) == curr_size + 1);
			}
		}
		thread_local auto* pause_tracker = pause_map_ns::find(tid,thread_pausing);
		assert(pause_map_ns::find(tid,thread_pausing) == pause_tracker);
		ProfilerPaused candidate = pause_tracker->second.lock();
		if (candidate) {
			assert(paused());
			return candidate;
		}
		else {
			ProfilerPaused ret{new ProfilerPauseScopeGoverner{this}};
			pause_map_ns::find(tid,thread_pausing)->second = ret;
			assert(paused());
			return ret;
		}
	}

	bool Profiler::ProfilerScopeGoverner::paused() const {
		auto tid = std::this_thread::get_id();
		return (pause_map_ns::mem(tid,thread_pausing) ? 
				pause_map_ns::find(tid,thread_pausing)->first :
				false);
	}
	
	Profiler::ProfilerScopeGoverner::~ProfilerScopeGoverner(){
		ProfilerStop();
	}
	
}
