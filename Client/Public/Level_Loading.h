#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevel);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Loading();


private:
	class CLoader*			 m_pLoader = { nullptr };
	LEVEL					m_eNextLevel = { LEVEL_END };

	random_device		m_RandomDevice;
	mt19937_64			m_RandomNumber;

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END