#pragma once
#include"Base.h"

BEGIN(Engine)
class ENGINE_DLL CProfiler final
{
private:
	struct ProfileData {
		string name;
		chrono::high_resolution_clock::time_point startTime;
		double lastFrameTime = 0.0;
		double totalTime = 0.0;
		int callCount = 0;
	};
	


private:
	CProfiler();
	CProfiler(const CProfiler& rhs) = delete;
	CProfiler& operator=(const CProfiler&) = delete;
	~CProfiler() = default;
public:
	void StartProfile(const string& name);
	
	void EndProfile(const string& name);

	void Reset();

	double GetLastFrameTime(const string& name);

	vector<pair<string, double>> GetProfileResults();


	template<typename T>
	void ProfileFunction(const string&  name ,T&& func)
	{
		StartProfile(name);
		func();
		EndProfile(name);
	}

private:
	static CProfiler* m_pInstance;

private:
	unordered_map<string, ProfileData> m_profiles;
	chrono::high_resolution_clock::time_point m_globalStartTime;
	vector<std::string> m_orderedNames;
	mutex m_Mutex;
public:
	static CProfiler& GetInstance();
	static void DestroyInstance();
};

END

