#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point;
END

BEGIN(Client)

class CTrail_Line : public CGameObject
{

public:
	enum PLAYER_CHARACTER { PLAYER_CUSTOM, PLAYER_NARUTO, MONSTER_NARUTO, BOSS_KURAMA, PLAYER_END };

	struct Trail_Line_DESC 
	{
		_float4x4*	pSocketMatrix;
		CTransform* pParentTransform;
		PLAYER_CHARACTER eMyCharacter;
	};
private:
	CTrail_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Line(const CTrail_Line& rhs);
	virtual ~CTrail_Line() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom		= { nullptr };
	CVIBuffer_Point*	m_pVIBufferCom		= { nullptr };
	CTransform*			m_pParentTransform	= { nullptr };

	_float4x4*			m_pSocketMatrix = { nullptr };
	_float4x4			m_WorldMatrix_Current;
	_float4x4			m_WorldMatrix_Old[60];
	_matrix  			m_OriginalMat = {};
	PLAYER_CHARACTER	m_eMyCharacter = { PLAYER_END };
	_vector				m_vColor;
	_float2				m_vThick;
	_float				m_vThickness;



private:
	HRESULT Add_Component(); 


public:
	static CTrail_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END