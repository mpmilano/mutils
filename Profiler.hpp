#pragma once
#include <string>
#include <mutex>
#include <memory>
#include <cassert>
#include <thread>
#include "gperftools/profiler.h"
#include "FunctionalMap.hpp"

namespace mutils{


class Profiler {
	
public: 

	struct ProfilerPauseScopeGoverner;
	
	using ProfilerPaused = std::shared_ptr<ProfilerPauseScopeGoverner>;
	
	struct ProfilerScopeGoverner {
	private:

		using paused_wp = std::weak_ptr<ProfilerPauseScopeGoverner>;

		using pause_map_ns = map<std::thread::id,std::pair<bool, paused_wp> >;
		using pause_map = typename pause_map_ns::mapnode;
		
		pause_map thread_pausing;
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
	using ProfilerActive = std::shared_ptr<ProfilerScopeGoverner>;

	struct ProfilerPauseScopeGoverner{

	private:
		ProfilerActive active;
		ProfilerPauseScopeGoverner();
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

	static ProfilerActive ensureProfiling(bool assertActive = false);
};

}
