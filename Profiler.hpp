#pragma once
#include <string>
#include <mutex>
#include <memory>
#include <cassert>
#include <map>
#include <thread>
#include "gperftools/profiler.h"

namespace mutils{


class Profiler {
	
public: 

	struct ProfilerPauseScopeGoverner;
	
	using ProfilerPaused = std::shared_ptr<ProfilerPauseScopeGoverner>;
	
	struct ProfilerScopeGoverner {
	private:

		using paused_wp = std::weak_ptr<ProfilerPauseScopeGoverner>;

		using pause_map = std::map<std::thread::id,std::pair<bool, paused_wp> >;
		
		std::shared_ptr<pause_map> thread_pausing;
		
		std::mutex m;
		const ProfilerOptions profopts;
		
		ProfilerScopeGoverner();
		
	public:
		friend class Profiler;
		
		friend struct ProfilerPauseScopeGoverner;

		ProfilerScopeGoverner(const ProfilerScopeGoverner&) = delete;

		ProfilerPaused pause();
		
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
	friend class ProfilerPauseScopeGoverner;
	
	friend struct ProfilerScopeGoverner;

	static ProfilerActive ensureProfiling();
};

}
