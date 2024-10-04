#include "CProfiler.h"



CProfiler* CProfiler::m_pInstance = nullptr;

CProfiler::CProfiler()
{
   m_globalStartTime = std::chrono::high_resolution_clock::now();
}

void CProfiler::StartProfile(const string& name)
{
    lock_guard<mutex> lock(m_Mutex);
    auto& profile = m_profiles[name];
    if (profile.name.empty()) 
    {
        profile.name = name;
        m_orderedNames.push_back(name);
    }
    profile.startTime = chrono::high_resolution_clock::now();
    profile.callCount++;
}

void CProfiler::EndProfile(const string& name)
{
    
        lock_guard<std::mutex> lock(m_Mutex);
        auto& profile = m_profiles[name];
        auto endTime = chrono::high_resolution_clock::now();
        profile.lastFrameTime = chrono::duration<double, milli>(endTime - profile.startTime).count();
        profile.totalTime += profile.lastFrameTime;
   
}

void CProfiler::Reset()
{
    lock_guard<std::mutex> lock(m_Mutex);
    for (auto& pair : m_profiles)
    {
        pair.second.lastFrameTime = 0.0;
        pair.second.callCount = 0;
    }
}

double CProfiler::GetLastFrameTime(const string& name)
{
    
        lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_profiles.find(name);
        if (it != m_profiles.end())
        {
            return it->second.lastFrameTime;
        }
        return 0.0;
}

vector<pair<string, double>> CProfiler::GetProfileResults()
{

    lock_guard<mutex> lock(m_Mutex);
    vector<pair<string, double>> results;
    //auto EndTime = std::chrono::high_resolution_clock::now();

    for (const auto& name : m_orderedNames)
    {
        auto it = m_profiles.find(name);
        if (it != m_profiles.end()) 
        {
            results.emplace_back(it->second.name, it->second.lastFrameTime);
        }
    }
    return results;




    //double totalTime = std::chrono::duration<double, std::milli>(EndTime - m_globalStartTime).count();
    //
    //for (const auto& pair : m_profiles)
    //{
    //    const auto& profile = pair.second;
    //    double averageTime = profile.callCount > 0 ? profile.totalTime / profile.callCount : 0.0;
    //    results.emplace_back(profile.name, averageTime);
    //}
    //
    //sort(results.begin(), results.end(),
    //    [](const auto& a , const auto& b ){ return a.second > b.second; });
    //
    //return results;
}

CProfiler& CProfiler::GetInstance()
{
    static std::once_flag flag;
    std::call_once(flag, []() {
        m_pInstance = new CProfiler();
        std::atexit([]() { delete m_pInstance; });
        });
    return *m_pInstance;


    
}

void CProfiler::DestroyInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
