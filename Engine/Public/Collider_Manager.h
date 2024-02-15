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
	HRESULT Initialize();

public:
	HRESULT		Add_Collider(const wstring& strLayerTag, CCollider* pCollider, void* pArg);
	void		Check_Collision_For_MyEvent(CCollider* MyColliderCom, const wstring& strRaegetColliderLayerTag);
	void		Check_Collision_For_TargetEvent(CCollider* MyColliderCom, const wstring& strTargetColliderLayerTag, const wstring& strMyColliderLayerTag);

	
	_bool		Is_Collision(CCollider* MyColliderCom, const wstring& strColliderLayerTag);


private:
	map<const wstring, class CCollider_Layer* >		m_pCollider_Layer = {  };
	typedef map<const wstring, class CCollider_Layer*>	COLLIDER_LAYERS;

private:
	class CCollider_Layer* Find_Collider_Layer(const wstring& strLayerTag);

public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};

END	