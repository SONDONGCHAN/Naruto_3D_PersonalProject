#pragma once
#include "Client_Defines.h"
#include "Level.h"
#include "GameObject.h"
#include "LandObject.h"

BEGIN(Client)

class CLevel_BossStage : public CLevel
{
private:
	CLevel_BossStage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_BossStage() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_Map(const wstring& strLayerTag);
	HRESULT Ready_LandObjects();
	HRESULT Ready_Layer_Player(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC& GameObjDesc);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC& GameObjDesc);
	HRESULT Ready_Layer_Item(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const wstring& strLayerTag);
	HRESULT Ready_Layer_EventTrigger(const wstring& strLayerTag);


	void	Add_Run_Time_Object();
	_bool	Check_Can_NextLevel();

private:
	_float	m_fRun_Time = 0.f;
	_float	m_fEnd_Time = 0.f;

	_bool	m_bCreated[2] = { false, };

	vector<CGameObject*>  pMonsters;
	_uint	m_iNumEnemy = { 0 };

public:
	static CLevel_BossStage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END