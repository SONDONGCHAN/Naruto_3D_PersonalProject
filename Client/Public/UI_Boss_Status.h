#pragma once
#include "UI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Boss_Status : public CUI
{
public:
	enum BOSS_CHARACTER { BOSS_NARUTO, BOSS_FINAL, BOSS_END};

	struct UI_Boss_Hp_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float*			pCurrentHp;
		_float*			pMaxHp;
		BOSS_CHARACTER	eMyCharacter;

	};
private:
	struct UI_Boss_UI_DESC
	{
		_float2 vPos;
		_float2 vSize;
	};

private:
	CUI_Boss_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Boss_Status(const CUI& rhs);
	virtual ~CUI_Boss_Status() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureBase	= { nullptr };
	CTexture* m_pTextureIcon	= { nullptr };
	CTexture* m_pTextureHp		= { nullptr };

private:
	vector<UI_Boss_UI_DESC>		m_UI_Descs;
	vector<CTexture*>			m_Textures;
	_float*						m_pCurrent_Hp;
	_float*						m_pMax_Hp;
	BOSS_CHARACTER				m_eMyCharacter = { BOSS_END };


private:
	HRESULT Add_Component();

public:
	static CUI_Boss_Status* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END