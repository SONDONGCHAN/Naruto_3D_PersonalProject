#include "Key_Manager.h"
#include "Input_Device.h"

CKey_Manager::CKey_Manager()
{
}

HRESULT CKey_Manager::Initialize(HINSTANCE hInst, HWND hWnd)
{
    m_pInput_Device = CInput_Device::Create(hInst, hWnd);
    if (nullptr == m_pInput_Device)
        return E_FAIL;

    return S_OK;
}

void CKey_Manager::Tick()
{
    m_pInput_Device->Tick();
}

_bool CKey_Manager::Key_Pressing(_ubyte byKeyID, _float fTimeDelta, _float* pPressingTime)
{
    if ((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000)
    {
        m_fPressingTime[byKeyID] += fTimeDelta;
        if(pPressingTime!= nullptr)
            *pPressingTime = m_fPressingTime[byKeyID];
        return true;
    }
    return false;
}

_bool CKey_Manager::Key_Down(_ubyte byKeyID)
{
    if ( !(m_bKeyState_Down[byKeyID]) && ((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000) )
    {
        m_bKeyState_Down[byKeyID] = !m_bKeyState_Down[byKeyID];
        m_fPressingTime[byKeyID] = 0.f;
        return true;
    }
    
    if ((m_bKeyState_Down[byKeyID]) && !((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000))
    {
        m_bKeyState_Down[byKeyID] = !m_bKeyState_Down[byKeyID];
        return false;
    }

    return false;
}

_bool CKey_Manager::Key_Up(_ubyte byKeyID)
{   

    if (!m_bKeyState_Up[byKeyID] && ((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000))
    {
        m_bKeyState_Up[byKeyID] = !m_bKeyState_Up[byKeyID];
        return false;
    }
    if (m_bKeyState_Up[byKeyID] && !((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000))
    {
        m_bKeyState_Up[byKeyID] = !m_bKeyState_Up[byKeyID];
        return true;
    }
    return false;

}

_bool CKey_Manager::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
    if ((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000)
        return true;

    return false;
}

_bool CKey_Manager::Mouse_Down(MOUSEKEYSTATE eMouse)
{
    if (!(m_bMouseState_Down[eMouse]) && ((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000))
    {
        m_bMouseState_Down[eMouse] = !m_bMouseState_Down[eMouse];
        return true;
    }

    if ((m_bMouseState_Down[eMouse]) && !((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000))
    {
        m_bMouseState_Down[eMouse] = !m_bMouseState_Down[eMouse];
        return false;
    }

    return false;
}

_bool CKey_Manager::Mouse_Up(MOUSEKEYSTATE eMouse)
{
    if (!(m_bMouseState_Up[eMouse]) && ((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000))
    {
        m_bMouseState_Up[eMouse] = !m_bMouseState_Up[eMouse];
        return false;
    }

    if ((m_bMouseState_Up[eMouse]) && !((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000))
    {
        m_bMouseState_Up[eMouse] = !m_bMouseState_Up[eMouse];
        return true;
    }

    return false;
}

_long CKey_Manager::Mouse_Move(MOUSEMOVESTATE eMouseState)
{
    return m_pInput_Device ->Get_DIMouseMove(eMouseState);
}




CKey_Manager* CKey_Manager::Create(HINSTANCE hInst,HWND hWnd)
{
    CKey_Manager* pInstance = new CKey_Manager();

    if (FAILED(pInstance->Initialize(hInst, hWnd)))
    {
        MSG_BOX("Failed to Created : CInput_Device");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKey_Manager::Free()
{
    Safe_Release(m_pInput_Device);
}
