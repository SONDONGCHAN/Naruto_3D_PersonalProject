#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevel);
	HRESULT Loading_Selector();
	_bool isFinished() const { return m_isFinished;}	

#ifdef _DEBUG
public:
	void Output_LoadingText();
#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	wstring					m_strLoadingText = {};
	_bool					m_isFinished =  false;

private:
	HANDLE					m_hThread = 0;
	CRITICAL_SECTION		m_CriticalSection = {};
	LEVEL					m_eNextLevel = { LEVEL_END };
	CGameInstance*			m_pGameInstance = { nullptr };

	vector<string>			m_vecAnimModelPath[ANIM_END];
	vector<string>			m_vecNonAnimModelPath[NONANIM_END];

private:
	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_CustomRoomLevel();
	HRESULT Loading_For_GamePlayLevel();
	HRESULT Loading_For_BossStageLevel();



public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END