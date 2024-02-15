#pragma once

#include "Component.h"
#include "Channel.h"
#include "Animation.h"



BEGIN(Engine)

class CMesh;

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	HRESULT Initialize_Prototype(TYPE eModelType, const wstring& pModelFilePath, _fmatrix PivotMatrix);
	HRESULT Initialize(void* pArg);
	HRESULT Render(_uint iMeshIndex);

public:
	_uint			Get_NumMeshes() const { return m_iNumMeshes; }
	vector<CMesh*>	Get_vecMeshes() const { return m_vecMeshes; }
	_float4x4		Get_PivotMat() { return m_PivotMatrix; }
	_int			Get_BoneIndex(const _char* pBoneName) const;
	_uint			Get_NumAnimation() const { return m_iNumAnimations; }
	_float3			Get_FramePos();

	_float4x4*		Get_CombinedBoneMatrixPtr(const _char* pBoneName);
	CAnimation*		Get_Current_Animation() { return m_vecAnimations[m_iCurrentAnimation]; }
	_uint			Get_Current_Animation_Index() { return m_iCurrentAnimation; }
	_bool			Get_Consecutive() { return m_bConsecutive; }

	//
	_bool			Get_AnimFinished() {return m_bAnimFinished; }


	void			Set_Animation(_uint iAnimIndex, _float ChangingDelay ,_bool bLoop , _float fMag ,_bool bCut, _bool bConsecutive , _uint iNextAnimIndex = 0);
	void			Reset_TrackPosition();



	HRESULT Bind_Material_ShaderResource(class CShader* pShader, _uint iMeshIndex, _uint MaterialTextureType, const _char* pConstantName);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	void	Play_Animation(_float fTimeDelta, _float fAnimationPlayRatio);

	void	Compare_ChannelIndex();
	void	Set_OffRootAnim(_bool isOff);


private:
	_float						m_TimeAfterChange = { 0.1f };
	_float						m_ChangingDelay = { 0.1f };
	_float4x4					m_PivotMatrix;
	vector<_uint>				vecCommonChannelIndex;
	vector<CChannel::KEYFRAME>	vecCommonChannelKeyFrame;

	//메시
	_uint					m_iNumMeshes = { 0 };	
	vector<class CMesh*>	m_vecMeshes;			

	//텍스쳐
	_uint									m_iNumMaterials = { 0 }; 
	vector<MESH_MATERIAL_DESC>				m_vecMaterials;

	//뼈
	TYPE					m_eModelType;
	vector<class CBone*>	m_vecBones;
	_uint					m_iNumBones = { 0 };

	//애니메이션
	_uint						m_iNumAnimations	= { 0 };
	_uint						m_iCurrentAnimation = { 0 };
	_uint						m_iOldAnimation		= { 0 };
	_uint						m_iNextAnimation	= { 0 };


	vector<class CAnimation*>	m_vecAnimations;


	// 제어 관련
	_float						m_fMag = { 1.f };					//애니메이션 재생 속도 제어
	_bool						m_bConsecutive = { false };			//연속재생 해야되는 애니메이션 중인지
	_bool						m_bAnimFinished = { false };		//연속재생 해야되는 애니메이션 중인지


private:
	HRESULT Ready_Bones(HANDLE hFile);
	HRESULT Ready_NonAnimMeshes(HANDLE hFile);
	HRESULT Ready_AnimMeshes(HANDLE hFile);
	HRESULT Ready_Materials(HANDLE hFile);
	HRESULT Ready_Animations(HANDLE hFile);


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const wstring& pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END