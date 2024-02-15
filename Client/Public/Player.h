#pragma once
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Client)

class CPlayer : public CLandObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	CLandObject* Get_CurrentCharacter() { return m_pCurrentCharacter; }

private:
	void	Key_Input(_float fTimeDelta);
	void	Check_Tag();


private:
	map<const wstring, class CLandObject*>		m_Charaters;
	_uint	m_iCurrent_Character_Index = { 0 };
	_bool	m_bTagging = false;

	CLandObject* m_pCurrentCharacter = { nullptr };
	CTransform*  m_pMapTransform = { nullptr };

private:
	HRESULT Add_Character();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END