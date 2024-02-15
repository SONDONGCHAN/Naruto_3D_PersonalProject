#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone final: public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	HRESULT	Initialize(_fmatrix PivotMatrix);
	void	Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones);


public:
	const _char*	Get_BoneName() const { return m_szName; }
	_float4x4		Get_CombindTransformationMatrix() const { return m_CombinedTransformationMatrix; }
	_float4x4*		Get_CombindTransformationMatrixPtr() { return &m_CombinedTransformationMatrix; }
	_float3			Get_FramePos() const { return m_FramePos; }


	void			Set_TranslationMatrix(_fmatrix TranslationMatrix); 
	void			Set_BoneName(_char* szName) { strcpy_s(m_szName, szName); }
	void			Set_ParentBoneIndex(_int Parentboneindex) { m_iParentBoneIndex = Parentboneindex; }
	void			Set_TransformationMatrix(_float4x4 transformationMatrix) { m_TransformationMatrix = transformationMatrix; }
	void			Set_FramePos(_float3 Pos) { m_FramePos = Pos;}
	void			Set_OffRootAnim(_bool isOff) { m_bOffRootAnim = isOff;}


private:
	_char			m_szName[MAX_PATH] = "";
	_float4x4		m_TransformationMatrix;			/* �θ� �������� ǥ���� ������ ���� ���. */
	_float4x4		m_CombinedTransformationMatrix; /* ���� TransformationMatrix * �θ�`s m_CombinedTransformationMatrix*/

	_float3			m_FramePos = { 0.f, 0.f, 0.f };		//��Ʈ Bone�� �������ӿ� ������ ��
	_bool			m_bOffRootAnim = false;
	_int			m_iParentBoneIndex = { 0 };

public:
	static CBone* Create();
	CBone* Clone();
	virtual void Free() override;



};

END