#pragma once
#include "UI.h"

BEGIN(Engine)
class CTexture;
END


BEGIN(Client)

class CUI_Player_Skills : public CUI
{
public:
	enum PLAYER_CHARACTER {PLAYER_CUSTOM, PLAYER_NARUTO, PLAYER_END};

	struct UI_Player_CoolTime_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float*				pCurrentCoolTime[4];
		_float*				pCoolTime[4];
		PLAYER_CHARACTER	eMyCharacter;
	};

	struct UI_Player_Skills_DESC
	{
		_float2 vPos;
		_float2 vSize;
	};

private:
	CUI_Player_Skills(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Player_Skills(const CUI& rhs);
	virtual ~CUI_Player_Skills() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureBase = { nullptr };

	CTexture* m_pTextureIcon_1 = { nullptr };
	CTexture* m_pTextureIcon_2 = { nullptr };
	CTexture* m_pTextureIcon_3 = { nullptr };
	CTexture* m_pTextureIcon_4 = { nullptr };

private:
	vector<UI_Player_Skills_DESC>	m_UI_Descs;
	vector<CTexture*>				m_Textures;
	_float*							m_pCurrentCoolTime[4];
	_float*							m_pCoolTime[4];
	PLAYER_CHARACTER				m_eMyCharacter = { PLAYER_END };

private:
	HRESULT Add_Component();

public:
	static CUI_Player_Skills* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END