#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(_fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones)
{	
	if (-1 == m_iParentBoneIndex)
		m_CombinedTransformationMatrix = m_TransformationMatrix;

	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

void CBone::Set_TranslationMatrix(_fmatrix TranslationMatrix)
{
	XMStoreFloat4x4(&m_TransformationMatrix, TranslationMatrix);

	if (1 == m_iParentBoneIndex)
	{
		if (m_bOffRootAnim)
		{
			m_TransformationMatrix._41 = 0.f;
			m_TransformationMatrix._42 = 0.f;
			m_TransformationMatrix._43 = 0.f;
		}
	}
	
}

CBone* CBone::Create()
{
	CBone* pInstance = new CBone();

	//if (FAILED(pInstance->Initialize(PivotMatrix)))
	//{
	//	MSG_BOX("Failed to Created : CBone");
	//	Safe_Release(pInstance);
	//}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}


void CBone::Free()
{
}
