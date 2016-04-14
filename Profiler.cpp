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

	Profiler::ProfilerActive Profiler::ensureProfiling() {
		static std::mutex m;
		std::unique_lock<std::mutex> lock{m};
		static std::weak_ptr<ProfilerScopeGoverner> _current;
		ProfilerActive candidate = _current.lock();
		if (candidate) return candidate;
		else {
			ProfilerActive ret{new ProfilerScopeGoverner{}}; //calls ProfilerStart
			_current = ret;
			return ret;
		}
	}

	Profiler::ProfilerScopeGoverner::ProfilerScopeGoverner():
		thread_pausing{new pause_map{}},
		profopts{
			[](void* v) -> int {
				auto pause_lock = *((decltype(thread_pausing)*) v); //well this probably is *not* safe
				const pause_map& thread_pausing = *pause_lock;
				auto tid = std::this_thread::get_id();
				return (thread_pausing.count(tid) > 0 ? 
						!thread_pausing.at(tid).first :
						true);
			},&thread_pausing}{
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
	
	typename Profiler::ProfilerPaused Profiler::ProfilerScopeGoverner::pause(){
		auto tid = std::this_thread::get_id();
		{
			//take a candidate from the map, creating a new entry in the map
			//for this thread if none exists
			auto pausing_p = thread_pausing;
			if (pausing_p->count(tid) == 0){
				std::unique_lock<std::mutex> lock{m};
				//copy the old map to a new map
				auto newmap = std::make_shared<pause_map>(*pausing_p);
				//add a new entry to the new map
				(*newmap)[tid].first = false;
				thread_pausing = newmap;
			}
			
		}
		ProfilerPaused candidate = thread_pausing->at(tid).second.lock();
		if (candidate) return candidate;
		else {
			ProfilerPaused ret{new ProfilerPauseScopeGoverner{}};
			thread_pausing->at(tid).second = ret;
			return ret;
		}
	}
	
	Profiler::ProfilerScopeGoverner::~ProfilerScopeGoverner(){
		ProfilerStop();
	}
	
}
