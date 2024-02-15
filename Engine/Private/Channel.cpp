#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(CModel* pModel)
{
	m_LastFrame.vPosition = { 0.f,0.f,0.f };
	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(_double TrackPosition, _uint* pCurrentKeyFrame, const vector<class CBone*>& Bones)
{
	KEYFRAME		LastKeyFrame = m_vecKeyFrames.back();

	_matrix			TransformationMatrix;

	_vector			vScale, vRotation, vPosition;


	if (0.f == TrackPosition)
		*pCurrentKeyFrame = 0;

	//마지막 동작에서 그 동작을 유지 하는 타이밍
	if (TrackPosition>= LastKeyFrame.Time)
	{
		vScale		= XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation	= XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition	= XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
		Bones[m_iBoneIndex]->Set_FramePos({ 0.f, 0.f, 0.f });
	}

	else
	{
		while (TrackPosition >= m_vecKeyFrames[*pCurrentKeyFrame + 1].Time)
			++*pCurrentKeyFrame;

		//키프레임 사이의 값들을 선형보간
		_double		Ratio = (TrackPosition - m_vecKeyFrames[*pCurrentKeyFrame].Time) /
							(m_vecKeyFrames[*pCurrentKeyFrame + 1].Time - m_vecKeyFrames[*pCurrentKeyFrame].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrames[*pCurrentKeyFrame].vScale), XMLoadFloat3(&m_vecKeyFrames[*pCurrentKeyFrame + 1].vScale), (_float)Ratio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_vecKeyFrames[*pCurrentKeyFrame].vRotation), XMLoadFloat4(&m_vecKeyFrames[*pCurrentKeyFrame + 1].vRotation), (_float)Ratio);
		vPosition = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vecKeyFrames[*pCurrentKeyFrame].vPosition), XMLoadFloat3(&m_vecKeyFrames[*pCurrentKeyFrame + 1].vPosition), (_float)Ratio), 1.f);

		XMStoreFloat3(&m_FramePos, vPosition - XMLoadFloat3(&m_LastFrame.vPosition));
	}

	XMStoreFloat3(&m_LastFrame.vScale, vScale);
	XMStoreFloat4(&m_LastFrame.vRotation, vRotation);
	XMStoreFloat3(&m_LastFrame.vPosition, vPosition);
	m_LastFrame.Time = 0.f;

	//스자이 벡터와 회전 기준점을 넣으면 변환 행렬로 반환 해줌
	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_TranslationMatrix(TransformationMatrix);
	Bones[m_iBoneIndex]->Set_FramePos(m_FramePos);
}

void CChannel::Change_Invalidate_TransformationMatrix(_double ChangingPosition, _float changingDelay, const vector<class CBone*>& Bones, KEYFRAME Frame)
{
	_matrix			TransformationMatrix;

	_vector			vScale, vRotation, vPosition;


	//서로다른 애니매이션 사이를 선형 보간
	_double		Ratio = ChangingPosition / changingDelay;

		vScale = XMVectorLerp(XMLoadFloat3(&Frame.vScale), XMLoadFloat3(&m_vecKeyFrames[0].vScale), (_float)Ratio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&Frame.vRotation), XMLoadFloat4(&m_vecKeyFrames[0].vRotation), (_float)Ratio);
		vPosition = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&Frame.vPosition), XMLoadFloat3(&m_vecKeyFrames[0].vPosition), (_float)Ratio), 1.f);
	
	//스자이 벡터와 회전 기준점을 넣으면 변환 행렬로 반환 해줌
	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_TranslationMatrix(TransformationMatrix);
}


void CChannel::Set_VecKeyFrames(vector<KEYFRAME> vecKeyFrames)
{
	for (_uint i = 0; i < m_iNumKeyFrames; i++)
	{
		m_vecKeyFrames.push_back(vecKeyFrames[i]);
	}
}

CChannel:: KEYFRAME CChannel::Get_LastKeyFrame()
{
	if (2 == m_iBoneIndex)
	{
		m_LastFrame.vPosition = {0.f,0.f,0.f};
		return m_LastFrame;
	}
	return m_LastFrame;
}



CChannel* CChannel::Create()
{
	CChannel* pInstance = new CChannel();

	//if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	//{
	//	MSG_BOX("Failed to Created : CChannel");
	//	Safe_Release(pInstance);
	//}

	return pInstance;
}

void CChannel::Free()
{
}
