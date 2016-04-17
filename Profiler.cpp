#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Profiler.hpp"

namespace mutils{

	Profiler::ProfilerPauseScopeGoverner::ProfilerPauseScopeGoverner():active(ensureProfiling()){
		assert(active);
		active->thread_pausing->at(std::this_thread::get_id()).first = true;
	}
	
	Profiler::ProfilerPauseScopeGoverner::~ProfilerPauseScopeGoverner(){
		active->thread_pausing->at(std::this_thread::get_id()).first = false;
	}
	
	Profiler::ProfilerActive Profiler::ensureProfiling(bool assertActive) {
		static std::mutex m;
		std::unique_lock<std::mutex> lock{m};
		static std::weak_ptr<ProfilerScopeGoverner> _current;
		ProfilerActive candidate = _current.lock();
		assert(assertActive ? bool{candidate.get() != nullptr} : true);
		if (candidate) return candidate;
		else {
			ProfilerActive ret{new ProfilerScopeGoverner{}}; //calls ProfilerStart
			_current = ret;
			return ret;
		}
	}

	Profiler::ProfilerScopeGoverner::ProfilerScopeGoverner():
		thread_pausing{pause_map_ns::mk_empty()},
		thread_locked(false),
		profopts{
			[](void* v) -> int {
				return ((ProfilerScopeGoverner*)v)->paused();
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
					std::cerr << "this thread is new! We've already seen " << pausing_p->size() << std::endl;
					}
				else {
				}
				assert(!thread_locked);
				std::unique_lock<std::mutex> lock{m};
				const auto curr_size = pause_map_ns::size(thread_pausing);
				thread_pausing = pause_map_ns::add(tid,?,thread_pausing);
				assert(pause_map_ns::size(thread_pausing) == curr_size + 1);
			}
		}
		ProfilerPaused candidate = pause_map_ns::find(tid,thread_pausing).second.lock();
		if (candidate) return candidate;
		else {
			ProfilerPaused ret{new ProfilerPauseScopeGoverner{}};
			pause_map_ns::find(tid,thread_pausing).second = ret;
			return ret;
		}
	}

	bool Profiler::ProfilerScopeGoverner::paused() const {
		auto tid = std::this_thread::get_id();
		return (pause_map_ns::mem(tid,thread_pausing) ? 
				!pause_map_ns::find(tid,thread_pausing).first :
				true);
	}
	
	Profiler::ProfilerScopeGoverner::~ProfilerScopeGoverner(){
		ProfilerStop();
	}
	
}
