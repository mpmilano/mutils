#pragma once
#include <string>
#include <mutex>
#include <list>
#include <memory>
#include <cassert>
#include <thread>
#include "gperftools/profiler.h"
#include "FunctionalMap.hpp"

namespace mutils{


class Profiler {
	
public:

	struct ProfilerPauseScopeGoverner;
	
	using paused_wp = std::weak_ptr<ProfilerPauseScopeGoverner>;
	using pause_map_ns = functional_map<std::thread::id,std::pair<bool, paused_wp>* >;
	
	using ProfilerPaused = std::shared_ptr<ProfilerPauseScopeGoverner>;
	
	struct ProfilerScopeGoverner {
	private:

		using pause_map = typename pause_map_ns::mapnode;
		
		pause_map thread_pausing;
		std::list<std::unique_ptr<std::pair<bool,paused_wp> > > memory;
		bool thread_locked;
		
		std::mutex m;
		const ProfilerOptions profopts;
		
		ProfilerScopeGoverner();
		
	public:
		friend class Profiler;
		
		friend struct ProfilerPauseScopeGoverner;

		ProfilerScopeGoverner(const ProfilerScopeGoverner&) = delete;

		ProfilerPaused pause();

		bool paused() const ;

		void threadLock();
		
		virtual ~ProfilerScopeGoverner();
	};
	using ProfilerActive = ProfilerScopeGoverner*;

	struct ProfilerPauseScopeGoverner{

	private:
		ProfilerActive active;
		ProfilerPauseScopeGoverner(ProfilerActive);
	public:

		virtual ~ProfilerPauseScopeGoverner();
		ProfilerPauseScopeGoverner(const ProfilerPauseScopeGoverner&) = delete;
		friend class Profiler;
	};
	friend struct ProfilerPauseScopeGoverner;
	
	friend struct ProfilerScopeGoverner;

	Profiler() = delete;
	Profiler(const Profiler&) = delete;
	~Profiler() = delete;

	static ProfilerActive startProfiling(bool assertActive = false);
	static ProfilerActive stopProfiling(bool assertActive = false);
	static ProfilerActive profiling();
	static ProfilerPaused pauseIfActive();
	static bool pausedOrInactive();
};

}
