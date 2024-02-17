#pragma once
#include "UI.h"

BEGIN(Engine)
class CTexture;
END


BEGIN(Client)

class CUI_Player_Custom : public CUI
{

public:
	enum CUSTOM_TILTE { PARTS_HEADGEAR, PARTS_FACE, PARTS_LOWER, PARTS_UPPER, PARTS_END };

	struct UI_Player_Custom_DESC
	{
		_uint*	pTitle_Cursor;
		_int	iNum_Parts[PARTS_END];
		_int*	pParts_Cursor[PARTS_END];
	};
	
private:
	struct UI_Player_Custom_UI_DESC
	{
		_float2 vPos;
		_float2 vSize;
	};

private:
	CUI_Player_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Player_Custom(const CUI& rhs);
	virtual ~CUI_Player_Custom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureBase			= { nullptr };
	CTexture* m_pTextureTitle			= { nullptr };
	CTexture* m_pTextureParts			= { nullptr };
	CTexture* m_pTextureParts_Selected	= { nullptr };


private:
	vector<UI_Player_Custom_UI_DESC>	m_UI_Descs;
	vector<CTexture*>				m_Textures;

	_uint*	m_pTitle_Cursor = { nullptr };
	_int	m_iNum_Parts[PARTS_END];
	_int*	m_pParts_Cursor[PARTS_END];

private:
	HRESULT Add_Component();

public:
	static CUI_Player_Custom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END