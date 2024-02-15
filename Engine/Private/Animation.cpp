#include "Animation.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration(rhs.m_Duration)
	, m_TickPerSecond(rhs.m_TickPerSecond)
	, m_TrackPosition(rhs.m_TrackPosition)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_vecChannels(rhs.m_vecChannels)
	, m_iCurrentKeyFrames(rhs.m_iCurrentKeyFrames)
	, m_CanStop(rhs.m_CanStop)
{
	for (auto pChannel : m_vecChannels)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CAnimation::Initialize( CModel* pModel)
{
	m_iCurrentKeyFrames.resize(m_iNumChannels);


	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _float fAnimationPlayRatio)
{
	m_TrackPosition += m_TickPerSecond * fTimeDelta;
	
	if (m_TrackPosition >= m_Duration * fAnimationPlayRatio)
	{
		m_CanStop = true;
	}
	if (m_TrackPosition >= m_Duration)
	{
		m_bFinished = true;

		if (false == m_Loop)
		{
			return;
		}
		else
		{
			m_TrackPosition = 0;
		}
	}


	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		m_vecChannels[i]->Invalidate_TransformationMatrix(m_TrackPosition, &m_iCurrentKeyFrames[i], Bones);
	}


}

void CAnimation::Change_Invalidate_TransformationMatrix(_float TimeAfterChange, _float changingDelay, const vector<class CBone*>& Bones, vector<_uint> CommonIndex, vector<CChannel::KEYFRAME> KeyFrames)
{		
	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		int count = 0;
		for (auto index : CommonIndex)
		{
			if (index == m_vecChannels[i]->Get_BoneIndex())
			{
				m_vecChannels[i]->Change_Invalidate_TransformationMatrix(TimeAfterChange, changingDelay, Bones, KeyFrames[count]);
				break;
			}
			count++;

		}
	}
}

void CAnimation::Reset_TrackPosition()
{
	m_TrackPosition = 0.0f;

	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		m_iCurrentKeyFrames[i] = 0;
	}
}


CAnimation* CAnimation::Create()
{
	CAnimation* pInstance = new CAnimation();

	//if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	//{
	//	MSG_BOX("Failed to Created : CAnimation");
	//	Safe_Release(pInstance);
	//}

	return pInstance; 
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_vecChannels)
		Safe_Release(pChannel);

	m_vecChannels.clear();
}
