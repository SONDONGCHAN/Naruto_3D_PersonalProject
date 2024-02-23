#pragma once
#include "UI.h"

BEGIN(Engine)
class CTexture;
END


BEGIN(Client)

class CUI_System : public CUI
{

public:

	enum UI_SYSTEM_OPTION {UI_DEFAULT, UI_ };

	struct UI_System_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float2 vPos;
		_float2 vSize;
		wstring	strPrototypeTag;
		_float	fRenderingTime;
		_float	fAppearTime;
	};

private:
	CUI_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_System(const CUI& rhs);
	virtual ~CUI_System() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		m_pTexture = { nullptr };
	UI_System_DESC	m_UI_Desc = {};
	_float			m_fRenderingTime = { 0.f };
	_float			m_fAppearTime = { 0.f };
	_float			m_CurrentAlpha = 0.f;
	_float2			m_CurrentSize = {0.f, 0.f};
	_bool			m_isAlphaEND = { false };
	_bool			m_RendEND = { false };


private:
	HRESULT Add_Component(const wstring& _strPrototypeTag);

public:
	static CUI_System* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END