#pragma once
#include <string>
#include <mutex>
#include <memory>
#include <cassert>
#include "gperftools/profiler.h"

template<char...>
class Profiler {
	
public: 

	struct ProfilerPauseScopeGoverner;
	
	using ProfilerPaused = std::shared_ptr<ProfilerPauseScopeGoverner>;
	
	struct ProfilerScopeGoverner {
	private:
		std::weak_ptr<ProfilerPauseScopeGoverner> *paused_ctx;
		std::mutex m;
		bool enabled{true};
		ProfilerOptions profopts{
			[](void* v) -> int {
				return *((bool*) v);
			},&enabled
				  };
		
		ProfilerScopeGoverner();
		
	public:
		template<char... c>
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
		ProfilerPauseScopeGoverner():active(ensureProfiling()){
			assert(active);
			active->enabled = false;
		}
	public:

		virtual ~ProfilerPauseScopeGoverner(){
			active->enabled = true;
		}
		ProfilerPauseScopeGoverner(const ProfilerPauseScopeGoverner&) = delete;
		template<char... c>
		friend class Profiler;
	};
	friend class ProfilerPauseScopeGoverner;
	
	friend struct ProfilerScopeGoverner;

	static ProfilerActive ensureProfiling() {
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
	};
};

template<char... name>
Profiler<name...>::ProfilerScopeGoverner::ProfilerScopeGoverner():paused_ctx{new std::weak_ptr<ProfilerPauseScopeGoverner>{}}{
	static constexpr char fname[] = {name...};
	ProfilerStartWithOptions(fname,&profopts);
}

template<char... name>
typename Profiler<name...>::ProfilerPaused Profiler<name...>::ProfilerScopeGoverner::pause(){
	std::unique_lock<std::mutex> lock{m};
	ProfilerPaused candidate = paused_ctx->lock();
	if (candidate) return candidate;
	else {
		ProfilerPaused ret{new ProfilerPauseScopeGoverner{}};
		*paused_ctx = ret;
		return ret;
	}
}

template<char... name>
Profiler<name...>::ProfilerScopeGoverner::~ProfilerScopeGoverner(){
	ProfilerStop();
	delete paused_ctx;
}
