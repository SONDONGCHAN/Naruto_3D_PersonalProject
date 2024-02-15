#pragma once
#include "Base.h"

BEGIN(Engine)

class CKey_Manager : public CBase
{
private:
	CKey_Manager();
	virtual ~CKey_Manager() = default;

public:
	HRESULT Initialize(HINSTANCE hInst,  HWND hWnd);

	void Tick();

public:
	_bool		Key_Pressing(_ubyte byKeyID, _float fTimeDelta, _float* pPressingTime);
	_bool		Key_Down(_ubyte byKeyID);
	_bool		Key_Up(_ubyte byKeyID);

	_bool		Mouse_Pressing(MOUSEKEYSTATE eMouse);
	_bool		Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool		Mouse_Up(MOUSEKEYSTATE eMouse);

	_long		Mouse_Move(MOUSEMOVESTATE eMouseState);


private:
	class CInput_Device* m_pInput_Device = { nullptr };

private:
	_bool			m_bKeyState_Down[0xff]		= { false, };
	_float			m_fPressingTime[0xff]		= { 0.f, };
	_bool			m_bKeyState_Up[0xff]		= { false, };


	_bool			m_bMouseState_Down[DIM_END] = { false, };
	_bool			m_bMouseState_Up[DIM_END] = { false, };

public:
	static CKey_Manager* Create(HINSTANCE hInst, HWND hWnd);
	virtual void Free() override;

};

END