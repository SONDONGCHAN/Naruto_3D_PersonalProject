#pragma once
#include "UI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Player_Status : public CUI
{
public:
	enum PLAYER_CHARACTER { PLAYER_CUSTOM, PLAYER_NARUTO, PLAYER_END };

	struct UI_Player_Hp_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float*				pCurrentHp;
		_float*				pMaxHp;
		_float*				pCurrentCoolTime;
		_float*				pCoolTime;
		PLAYER_CHARACTER	eMyCharacter;

	};
private:
	struct UI_Player_Status_DESC 
	{
		_float2 vPos;
		_float2 vSize;
	};

private:
	CUI_Player_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Player_Status(const CUI& rhs);
	virtual ~CUI_Player_Status() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*			m_pTextureBase = { nullptr };
	CTexture*			m_pTextureIcon = { nullptr };
	CTexture*			m_pTextureHp = { nullptr };

private:
	vector<UI_Player_Status_DESC>	m_UI_Descs;
	vector<CTexture*>				m_Textures;
	_float*							m_pCurrent_Hp;
	_float*							m_pMax_Hp;
	_float*							m_pCurrentCoolTime;
	_float*							m_pCoolTime;
	PLAYER_CHARACTER				m_eMyCharacter = { PLAYER_END };


private:
	HRESULT Add_Component();

public:
	static CUI_Player_Status* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END