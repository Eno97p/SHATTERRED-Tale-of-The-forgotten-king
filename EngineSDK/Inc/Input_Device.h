#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_byte	Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte	Get_DIPreKeyState(_ubyte byKeyID) { return m_byPreKeyState[byKeyID]; }

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return m_tMouseState.rgbButtons[eMouse];
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}
	MOUSEWHEELSTATE CheckMouseWheel(void);

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);
	void	Update_MouseState(void);


	//지우지 마셈 PSW
	eKEY_STATE GetKeyState(_ubyte byKeyID);
	eKEY_STATE GetMouseState(MOUSEKEYSTATE eMouseState);

	bool Key_Pressing(_ubyte byKeyID);

	bool Key_Down(_ubyte byKeyID);

	bool Key_Up(_ubyte byKeyID);

	bool Mouse_Pressing(MOUSEKEYSTATE eMouse);
	bool Mouse_Down(MOUSEKEYSTATE eMouse);
	bool Mouse_Up(MOUSEKEYSTATE eMouse);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte					m_byPreKeyState[256];
	DIMOUSESTATE			m_tMouseState;
	DIMOUSESTATE			 m_tPreMouseState;


	vector<eKEY_STATE> m_vecMouseState;
	vector< tKeyInfo>		m_vecKey;
public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END
#endif // InputDev_h__
