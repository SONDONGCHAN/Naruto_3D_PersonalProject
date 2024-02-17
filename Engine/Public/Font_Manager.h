#pragma once
#include "Base.h"

BEGIN(Engine)

class CFont_Manager : public CBase
{
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor = Colors::White, FONT_ORIGIN_OPTION _option = ORIGIN_CENTER, _float fRotation = 0.f, _float fScale = 1.f);
	//HRESULT Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor = Colors::White, _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);


private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	map<const wstring, class CCustomFont*>			m_Fonts;

private:
	class CCustomFont* Find_Font(const wstring& strFontTag);

public:
	static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END