#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel : public CBase
{
public:
	struct KEYFRAME
	{
		_float3		vScale;
		_float3 	vPosition;
		_float4		vRotation;
		_double		Time;
	};

private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(class CModel* pModel);
	void Invalidate_TransformationMatrix(_double TrackPosition, _uint* pCurrentKeyFrame, const vector<class CBone*>& Bones);
	void Change_Invalidate_TransformationMatrix(_double ChangingPosition, _float changingDelay, const vector<class CBone*>& Bones, KEYFRAME Frame);


public:
	void	Set_ChannelName(_char* szName) { strcpy_s(m_szName, szName); }
	void	Set_ChannelBoneIndex(_uint channelBoneIndex) { m_iBoneIndex = channelBoneIndex;  }
	void	Set_NumKeyFrames(_uint numkeyFrames) { m_iNumKeyFrames = numkeyFrames; }
	void	Set_VecKeyFrames(vector<KEYFRAME> vecKeyFrames);

	_uint		Get_BoneIndex() { return m_iBoneIndex; }
	KEYFRAME	Get_LastKeyFrame();

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_vecKeyFrames;

	KEYFRAME			m_LastFrame = {};
	_float3 			m_FramePos = { 0.f, 0.f, 0.f };
	_uint				m_iBoneIndex = { 0 };

public:
	static CChannel* Create();
	virtual void Free() override;
};

END