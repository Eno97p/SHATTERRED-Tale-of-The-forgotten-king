#pragma once
#include"CProfiler.h"

//#ifdef _DEBUG
//#define PROFILE_FUNCTION() ProfileScope profiler##__LINE__(__FUNCTION__)
//#define PROFILE_SCOPE(name) ProfileScope profiler##__LINE__(name)
//#define PROFILE_CALL(name, func) CProfiler::GetInstance().ProfileFunction(name, [&]() { func; })
//#define PROFILE_RESET() CProfiler::GetInstance().Reset()
//#else
//#define PROFILE_FUNCTION()
//#define PROFILE_SCOPE(name)
//#define PROFILE_CALL(name, func) func
//#define PROFILE_RESET()
//#endif




BEGIN(Engine)
class ProfileScope
{
public:
    ProfileScope(const char* name) : m_name(name) {
        CProfiler::GetInstance().StartProfile(m_name);
    }
    ~ProfileScope() {
        CProfiler::GetInstance().EndProfile(m_name);
    }
private:
    const char* m_name;
};

END