#include "..\Public\Model.h"
#include "Bone.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"


CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_vecMaterials(rhs.m_vecMaterials)
	, m_iNumAnimations(rhs.m_iNumAnimations)

{
	for (auto& pPrototypeMesh : rhs.m_vecMeshes)
		m_vecMeshes.push_back(dynamic_cast<CMesh*>(pPrototypeMesh->Clone(nullptr)));

	for (auto& pPrototypeBone : rhs.m_vecBones)
		m_vecBones.push_back(pPrototypeBone->Clone());

	for (auto& pPrototypeAnimation : rhs.m_vecAnimations)
		m_vecAnimations.push_back(pPrototypeAnimation->Clone());

	for (auto pMaterial : m_vecMaterials)
	{
		for (auto& pTexture : pMaterial.pMtrlTextures)
			Safe_AddRef(pTexture);
	}
		
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const wstring& pModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eModelType;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	HANDLE		hFile = CreateFile(pModelFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	if (TYPE_ANIM == m_eModelType)
	{
		if (FAILED(Ready_Bones(hFile)))
			return E_FAIL;

		if (FAILED(Ready_AnimMeshes(hFile)))
			return E_FAIL;

		if (FAILED(Ready_Materials(hFile)))
			return E_FAIL;

		if (FAILED(Ready_Animations(hFile)))
			return E_FAIL;
	}

	else if (TYPE_NONANIM == m_eModelType)
	{
		if (FAILED(Ready_NonAnimMeshes(hFile)))
			return E_FAIL;

		if (FAILED(Ready_Materials(hFile)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_vecMeshes[iMeshIndex]->Bind_Buffers();
	m_vecMeshes[iMeshIndex]->Render();

	return S_OK;
}

_int CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_int		iBoneIndex = { -1 };

	auto	iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](class CBone* pBone) {

		iBoneIndex++;

		if (false == strcmp(pBoneName, pBone->Get_BoneName()))
			return true;
		return false;
		});

	if (iter == m_vecBones.end())
		return -1;

	return iBoneIndex;
}

_float3 CModel::Get_FramePos()
{
	return m_vecBones[2]->Get_FramePos();
}

_float4x4* CModel::Get_CombinedBoneMatrixPtr(const _char* pBoneName)
{
	auto	iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](CBone* pBone)
		{
			if (false == strcmp(pBone->Get_BoneName(), pBoneName))
				return true;

			return false;
		});

	return (*iter)->Get_CombindTransformationMatrixPtr();
}

void CModel::Set_Animation(_uint iAnimIndex, _float ChangingDelay, _bool bLoop, _float fMag, _bool bCut, _bool bConsecutive, _uint iNextAnimIndex)
{

	if (iAnimIndex >= m_iNumAnimations)
		return;
	if (iAnimIndex < 0)
		return;
	
	if (bConsecutive && m_vecAnimations[iAnimIndex]->Get_CanStop())
	{
		m_bConsecutive = false;
		m_vecAnimations[m_iCurrentAnimation]->Set_CanStop();
		m_vecAnimations[m_iCurrentAnimation]->Set_Finished();
		m_TimeAfterChange = 0.f;

		m_vecAnimations[m_iCurrentAnimation]->Reset_TrackPosition();
		m_iOldAnimation = m_iCurrentAnimation; // 1
		m_iCurrentAnimation = iNextAnimIndex; // 2
		Compare_ChannelIndex(); // 3
		m_fMag = fMag;
		m_ChangingDelay = ChangingDelay;
		m_vecAnimations[m_iCurrentAnimation]->Set_Loop(bLoop);
	}

	if (iAnimIndex == m_iCurrentAnimation)
	{
		m_fMag = fMag;
		return;
	}
	if (bCut)
	{
		m_vecAnimations[m_iCurrentAnimation]->Set_CanStop();
		m_vecAnimations[m_iCurrentAnimation]->Set_Finished();
		m_TimeAfterChange = 0.f;
		m_vecAnimations[m_iCurrentAnimation]->Reset_TrackPosition();
		m_iOldAnimation = m_iCurrentAnimation; // 1
		m_iCurrentAnimation = iAnimIndex; // 2
		Compare_ChannelIndex(); // 3
		m_fMag = fMag;
		m_ChangingDelay = ChangingDelay;
		m_vecAnimations[m_iCurrentAnimation]->Set_Loop(bLoop);
		m_bConsecutive = bConsecutive;
		m_bAnimFinished = true;

		return;
	}

	if (bConsecutive && (iNextAnimIndex == m_iCurrentAnimation))
		return;

	else if (m_vecAnimations[m_iCurrentAnimation]->Get_Loop() || m_vecAnimations[m_iCurrentAnimation]->Get_CanStop())
	{
		m_vecAnimations[m_iCurrentAnimation]->Set_CanStop();
		m_vecAnimations[m_iCurrentAnimation]->Set_Finished();
		m_TimeAfterChange = 0.f;
		m_vecAnimations[m_iCurrentAnimation]->Reset_TrackPosition();
		m_iOldAnimation = m_iCurrentAnimation; // 1
		m_iCurrentAnimation = iAnimIndex; // 2
		Compare_ChannelIndex(); // 3
		m_fMag = fMag;
		m_ChangingDelay = ChangingDelay;
		m_vecAnimations[m_iCurrentAnimation]->Set_Loop(bLoop);
		m_bConsecutive = bConsecutive;
		m_bAnimFinished = true;
	}

}

void CModel::Reset_TrackPosition()
{
	m_vecAnimations[m_iCurrentAnimation]->Reset_TrackPosition();
}

HRESULT CModel::Bind_Material_ShaderResource(CShader* pShader, _uint iMeshIndex, _uint MaterialTextureType, const _char* pConstantName)
{
	_uint		iMaterialIndex = m_vecMeshes[iMeshIndex]->Get_MaterialIndex();

	if (nullptr == m_vecMaterials[iMaterialIndex].pMtrlTextures[MaterialTextureType])
	{
		return S_OK;
	}

	return m_vecMaterials[iMaterialIndex].pMtrlTextures[MaterialTextureType]->Bind_ShaderResource(pShader, pConstantName, 0);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];

	m_vecMeshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_vecBones);

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);
}

void CModel::Play_Animation(_float fTimeDelta, _float fAnimationPlayRatio)
{
	m_bAnimFinished = false;

	m_TimeAfterChange += fTimeDelta;

	if (m_TimeAfterChange < m_ChangingDelay)
	{
		m_vecAnimations[m_iCurrentAnimation]->Change_Invalidate_TransformationMatrix(m_TimeAfterChange, m_ChangingDelay, m_vecBones, vecCommonChannelIndex, vecCommonChannelKeyFrame );
	}
	else
	{
		/* 현재 애니메이션 상태에 맞게 뼈들의 TransformationMatrix행렬을 갱신한다. */
		m_vecAnimations[m_iCurrentAnimation]->Invalidate_TransformationMatrix(fTimeDelta*m_fMag, m_vecBones, fAnimationPlayRatio);
	}

	/* 모든 뼈들의 CombinedTransformationMatrix를 구해준다. */

	for (auto& pBone : m_vecBones)
		pBone->Invalidate_CombinedTransformationMatrix(m_vecBones);
}
 
void CModel::Compare_ChannelIndex()
{
	_uint OldAnimNumChannels = m_vecAnimations[m_iOldAnimation]->Get_NumChannels();	
	vector<class CChannel*>	OldChannels = m_vecAnimations[m_iOldAnimation]->Get_VecChannels();

	_uint CurrentAnimNumChannels = m_vecAnimations[m_iCurrentAnimation]->Get_NumChannels();
	vector<class CChannel*>	CurrentChannels = m_vecAnimations[m_iCurrentAnimation]->Get_VecChannels();

	vecCommonChannelIndex.clear();
	vecCommonChannelKeyFrame.clear();
	
	for (_uint i = 0; i < OldAnimNumChannels ; i++)
	{
		for (_uint j = 0; j < CurrentAnimNumChannels; j++)
		{
			if (OldChannels[i]->Get_BoneIndex() == CurrentChannels[j]->Get_BoneIndex())
			{
				vecCommonChannelIndex.push_back(OldChannels[i]->Get_BoneIndex());
				vecCommonChannelKeyFrame.push_back(OldChannels[i]->Get_LastKeyFrame());

				break;

			}
		}	
	}
	int i = 0;
	
}

void CModel::Set_OffRootAnim(_bool isOff)
{
	for ( _uint i = 0; i < m_iNumBones; i++)
	{
		m_vecBones[i]->Set_OffRootAnim(isOff);
	}
}


HRESULT CModel::Ready_NonAnimMeshes(HANDLE	hFile)
{
	_ulong		dwByte = { 0 };

	//  Model의 Mesh 갯수 불러와서 Model의 멤버변수로 저장
	if (!ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, this, XMLoadFloat4x4(&m_PivotMatrix));
		if (pMesh == nullptr)
			return E_FAIL;

		// Mesh에 해당하는 Material Index를 불러오고 Mesh의 멤버변수로 저장
		_uint MaterialIndex;
		if (!ReadFile(hFile, &MaterialIndex, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		pMesh->Set_MaterialIndex(MaterialIndex);

		// Mesh의 vertex 개수 불러와서 mesh의 멤버변수로 저장
		_uint NumVertices;
		if(!ReadFile(hFile, &NumVertices, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		pMesh->Set_NumVertices(NumVertices);

		// Mesh의 vertex 개수만큼 vertex 정보 불러와서 저장
		for (_uint j = 0; j < NumVertices; j++)
		{
			VTXMESH Vertex;
			if(!ReadFile(hFile, &Vertex, sizeof(VTXMESH), &dwByte, nullptr))
				return E_FAIL;

			pMesh->Set_Vertices(Vertex, j);
		}

		// Mesh의 Index 개수 불러와서 mesh의 멤버변수로 저장
		_uint NumIndices;
		if(!ReadFile(hFile, &NumIndices, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		pMesh->Set_NumIndices(NumIndices);

		// Mesh의 Index 개수만큼 Index 번호 불러와서 저장
		for (_uint j = 0; j < NumIndices; j++)
		{
			_uint Index;
			if(!ReadFile(hFile, &Index, sizeof(_uint), &dwByte, nullptr))
				return E_FAIL;

			pMesh->Set_Indices(Index, j);
		}

		if (FAILED(pMesh->Initialize_Prototype(m_eModelType, this, XMLoadFloat4x4(&m_PivotMatrix))))
		{
			MSG_BOX("Failed to Created : CMesh");
			Safe_Release(pMesh);
		}

		m_vecMeshes.push_back(pMesh);

	}
	return S_OK;
}

HRESULT CModel::Ready_AnimMeshes(HANDLE hFile)
{
	_ulong		dwByte = { 0 };

	//  Model의 Mesh 갯수 불러와서 Model의 멤버변수로 저장
	if (!ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, this, XMLoadFloat4x4(&m_PivotMatrix));
		if (pMesh == nullptr)
			return E_FAIL;

		// Mesh에 해당하는 Material Index를 불러오고 Mesh의 멤버변수로 저장
		_uint MaterialIndex;
		if (!ReadFile(hFile, &MaterialIndex, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		pMesh->Set_MaterialIndex(MaterialIndex);

		// Mesh의 vertex 개수 불러와서 mesh의 멤버변수로 저장
		_uint NumVertices;
		if (!ReadFile(hFile, &NumVertices, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		pMesh->Set_NumVertices(NumVertices);

		// Mesh의 vertex 개수만큼 vertex 정보 불러와서 저장
		for (_uint j = 0; j < NumVertices; j++)
		{
			VTXANIMMESH AnimVertex;
			if (!ReadFile(hFile, &AnimVertex, sizeof(VTXANIMMESH), &dwByte, nullptr))
				return E_FAIL;

			pMesh->Set_AnimVertices(AnimVertex, j);
		}

		// Mesh의 Index 개수 불러와서 mesh의 멤버변수로 저장
		_uint NumIndices;
		if (!ReadFile(hFile, &NumIndices, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		pMesh->Set_NumIndices(NumIndices);

		// Mesh의 Index 개수만큼 Index 번호 불러와서 저장
		for (_uint j = 0; j < NumIndices; j++)
		{
			_uint Index;
			if (!ReadFile(hFile, &Index, sizeof(_uint), &dwByte, nullptr))
				return E_FAIL;

			pMesh->Set_Indices(Index, j);
		}


		// Mesh의 Bone 개수 불러와서 mesh의 멤버변수로 저장
		_uint NumBones;
		if (!ReadFile(hFile, &NumBones, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		pMesh->Set_NumBones(NumBones);

		// Mesh의 Bone 개수만큼
		vector<_uint>		vecBoneIndices;
		vecBoneIndices.resize(NumBones);
		vector<_float4x4>	vecOffsetMatrices;
		vecOffsetMatrices.resize(NumBones);

		for (_uint j = 0; j < NumBones; j++)
		{
			//Mesh의 Bone의 Model전체 bone 기준 인덱스 불러오기
			if (!ReadFile(hFile, &vecBoneIndices[j], sizeof(_uint), &dwByte, nullptr))
				return E_FAIL;
			
			//Mesh의 Bone의 m_vecOffsetMatrices 불러오기
			if (!ReadFile(hFile, &vecOffsetMatrices[j], sizeof(_float4x4), &dwByte, nullptr))
				return E_FAIL;
		}

		pMesh->Set_BoneIndices(vecBoneIndices);
		pMesh->Set_OffsetMatrices(vecOffsetMatrices);

		// 현재 Mesh의 이름을 불러와서 Mesh의 멤버변수로 저장한다.
		_char	MeshName[MAX_PATH];

		if (!ReadFile(hFile, &MeshName, MAX_PATH * sizeof(_char), &dwByte, nullptr))
			return E_FAIL;

		pMesh->Set_MeshName(MeshName);

		if (FAILED(pMesh->Initialize_Prototype(m_eModelType, this, XMLoadFloat4x4(&m_PivotMatrix))))
		{
			MSG_BOX("Failed to Created : CMesh");
			Safe_Release(pMesh);
		}

		m_vecMeshes.push_back(pMesh);

	}
	return S_OK;
}

HRESULT CModel::Ready_Materials(HANDLE	hFile)
{
	_ulong		dwByte = { 0 };

	//  Model의 Material 갯수 불러와서 Model의 멤버변수로 저장
	if (!ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;


	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL_DESC			MaterialDesc = {};

		for (_int j = 0; j < 21; j++)
		{
			_tchar		szDirectory[MAX_PATH];
			_tchar		A[MAX_PATH] = L"";

			if (!ReadFile(hFile, &szDirectory, MAX_PATH * sizeof(_tchar), &dwByte, nullptr))
			 	return E_FAIL;

			if (szDirectory[0] == A[0])
				continue;

			MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, szDirectory, 1);

		}

		m_vecMaterials.push_back(MaterialDesc);

	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(HANDLE  hFile)
{
	_ulong		dwByte = { 0 };

	// 모델이 가지는 Bone의 갯수를 가져온다.
	if (!ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	// 모델이 가지는 Bone의 갯수만큼
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		CBone* pBone = CBone::Create();
		if (nullptr == pBone)
			return E_FAIL;

		// 현재 Bone의 이름을 불러와서 Bone의 멤버변수로 저장한다.
		_char	BoneName[MAX_PATH];

		if (!ReadFile(hFile, &BoneName, MAX_PATH * sizeof(_char), &dwByte, nullptr))
			return E_FAIL;

		pBone->Set_BoneName(BoneName);


		// 현재 Bone의 부모의 Index를 불러와서 멤버변수로 저장한다.
		_int	ParentBoneIndex;

		if (!ReadFile(hFile, &ParentBoneIndex,  sizeof(_int), &dwByte, nullptr))
			return E_FAIL;

		pBone->Set_ParentBoneIndex(ParentBoneIndex);


		// 현재 Bone의 TransformationMatrix를 불러와서 멤버변수로 저장한다.
		_float4x4	TransformationMatrix;

		if (!ReadFile(hFile, &TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr))
			return E_FAIL;

		 pBone->Set_TransformationMatrix(TransformationMatrix);


		if (FAILED(pBone->Initialize(XMLoadFloat4x4(&m_PivotMatrix))))
		{
			MSG_BOX("Failed to Created : CBone");
			Safe_Release(pBone);
		}

		m_vecBones.push_back(pBone);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations(HANDLE hFile)
{
	_ulong		dwByte = { 0 };

	// 모델이 가지는 Animation의 갯수를 가져온다.
	if (!ReadFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;
	
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create();
		if (nullptr == pAnimation)
			return E_FAIL;

		// Animation의 이름을 저장 
		_char AnimationName[MAX_PATH];

		if (!ReadFile(hFile, &AnimationName, MAX_PATH * sizeof(_char), &dwByte, nullptr))
			return E_FAIL;

		pAnimation->Set_AnimationName(AnimationName);


		// Animation의 Duration을 저장 
		_double Duration;

		if (!ReadFile(hFile, &Duration, sizeof(_double), &dwByte, nullptr))
			return E_FAIL;

		pAnimation->Set_Duration(Duration);


		// Animation의 TickPerSecond를 저장 
		_double TickPerSecond;

		if (!ReadFile(hFile, &TickPerSecond, sizeof(_double), &dwByte, nullptr))
			return E_FAIL;

		pAnimation->Set_TickPerSecond(TickPerSecond);


		// Animation의 channel갯수를 저장 
		_uint NumChannels;

		if (!ReadFile(hFile, &NumChannels, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		pAnimation->Set_NumChannels(NumChannels);

		vector<class CChannel*> vecChannels;

		// Animation의 Channel의 갯수 만큼
		for (_uint j = 0; j < NumChannels; j++)
		{
			CChannel* pChannel = CChannel::Create();
			if (nullptr == pChannel)
				return E_FAIL;

		
			// Channel의 이름을 저장 
			_char ChannelName[MAX_PATH];

			if (!ReadFile(hFile, &ChannelName, MAX_PATH * sizeof(_char), &dwByte, nullptr))
				return E_FAIL;

			pChannel->Set_ChannelName(ChannelName);


			// Channel에 해당하는 BoneIndex를 저장
			_uint ChannelBoneIndex;

			if (!ReadFile(hFile, &ChannelBoneIndex, sizeof(_uint), &dwByte, nullptr))
				return E_FAIL;

			pChannel->Set_ChannelBoneIndex(ChannelBoneIndex);


			// Channel의 keyFrame갯수 저장
			_uint NumKeyFrame;

			if (!ReadFile(hFile, &NumKeyFrame, sizeof(_uint), &dwByte, nullptr))
				return E_FAIL;

			pChannel->Set_NumKeyFrames(NumKeyFrame);


			// Channel의 keyFrame갯수 만큼 keyFrame정보를 저장
			vector<CChannel::KEYFRAME> vecKeyFrame;

			for (_uint k = 0; k < NumKeyFrame; k++)
			{
				CChannel::KEYFRAME KeyFrame;

				if (!ReadFile(hFile, &KeyFrame, sizeof(CChannel::KEYFRAME), &dwByte, nullptr))
					return E_FAIL;

				vecKeyFrame.push_back(KeyFrame);
			}

			pChannel->Set_VecKeyFrames(vecKeyFrame);


			if (FAILED(pChannel->Initialize(this)))
			{
				MSG_BOX("Failed to Created : CChannel");
				Safe_Release(pChannel);
			}

			vecChannels.push_back(pChannel);
		}

		pAnimation->Set_VecChannels(vecChannels);


		if (FAILED(pAnimation->Initialize(this)))
		{
			MSG_BOX("Failed to Created : CAnimation");
			Safe_Release(pAnimation);
		}

		m_vecAnimations.push_back(pAnimation);
	}
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const wstring& pModelFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto pMesh : m_vecMeshes)
		Safe_Release(pMesh);

	for (auto pBone : m_vecBones)
		Safe_Release(pBone);
	m_vecBones.clear();

	for (auto& pAnimation : m_vecAnimations)
		Safe_Release(pAnimation);
	m_vecAnimations.clear();


	for (auto pMaterial : m_vecMaterials)
	{
		for (auto& pTexture : pMaterial.pMtrlTextures)
			Safe_Release(pTexture);
	}
}
