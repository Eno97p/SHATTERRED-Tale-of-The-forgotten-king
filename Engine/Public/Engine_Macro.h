#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;



#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif


#define NO_COPY(CLASSNAME)								\
		private:										\
		CLASSNAME(const CLASSNAME&) = delete;					\
		CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static unsigned int DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {						\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned int CLASSNAME::DestroyInstance( void ) {		\
			unsigned int dwRefCnt = { 0 };						\
			if(nullptr != m_pInstance)	{						\
				dwRefCnt = m_pInstance->Release();				\
				if (0 == dwRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return dwRefCnt;									\
	}




#define KEY_CHECK( key , state) 	CGameInstance::GetInstance()->GetKeyState(key) == state
#define KEY_HOLD(key) KEY_CHECK(key,eKEY_STATE::HOLD)
#define KEY_TAP(key) KEY_CHECK(key, eKEY_STATE::TAP)
#define KEY_AWAY(key) KEY_CHECK(key,eKEY_STATE::AWAY )
#define KEY_NONE(key) KEY_CHECK(key,eKEY_STATE::NONE )


#ifdef _DEBUG
#define PROFILE_FUNCTION() ProfileScope profiler##__LINE__(__FUNCTION__)
#define PROFILE_SCOPE(name) ProfileScope profiler##__LINE__(name)
#define PROFILE_CALL(name, func) CProfiler::GetInstance().ProfileFunction(name, [&]() { func; })
#define PROFILE_RESET() CProfiler::GetInstance().Reset()
#else
#define PROFILE_FUNCTION()
#define PROFILE_SCOPE(name)
#define PROFILE_CALL(name, func) func
#define PROFILE_RESET()
#endif


//For_PhysX
#define PVD_HOST "127.0.0.1" //Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.

//For _Nividia_HBAO_Plus
//#define __GFSDK_DX11__



#endif // Engine_Macro_h__
