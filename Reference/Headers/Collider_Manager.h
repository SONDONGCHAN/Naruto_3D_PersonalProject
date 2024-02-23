#pragma once
#include "Base.h"
#include "Collider.h"


BEGIN(Engine)

class CCollider_Manager : public CBase
{
private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);

public:
	HRESULT		Add_Collider(_uint iLevelIndex, const wstring& strLayerTag, CCollider* pCollider, void* pArg);
	void		Check_Collision_For_MyEvent(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strRaegetColliderLayerTag);
	void		Check_Collision_For_TargetEvent(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strTargetColliderLayerTag, const wstring& strMyColliderLayerTag);

	_bool		Is_Collision(_uint iLevelIndex, CCollider* MyColliderCom, const wstring& strColliderLayerTag);

	void		Kill_Dead_Collider(_uint iLevelIndex, CCollider* MyColliderCom);

	void		Collider_Clear(_uint iLevelIndex);



private:
	_uint				m_iNumLevels = { 0 };
	map<const wstring, class CCollider_Layer*>*		m_pCollider_Layers = { nullptr };
	typedef map<const wstring, class CCollider_Layer*>	COLLIDER_LAYERS;

private:
	class CCollider_Layer* Find_Collider_Layer(_uint iLevelIndex, const wstring& strLayerTag);

public:
	static CCollider_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END	