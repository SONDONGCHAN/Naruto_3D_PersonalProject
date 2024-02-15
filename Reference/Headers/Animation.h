#pragma once
#include "Base.h"
#include "Channel.h"


BEGIN(Engine)
class CAnimation : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel);
	void Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _float fAnimationPlayRatio);
	void Change_Invalidate_TransformationMatrix(_float TimeAfterChange, _float changingDelay, const vector<class CBone*>& Bones, vector<_uint> CommonIndex, vector<CChannel::KEYFRAME> KeyFrames);


public:
	void	Set_AnimationName(_char* szName) { strcpy_s(m_szName, szName); }
	void	Set_Duration(_double duration) { m_Duration = duration;  }
	void	Set_TickPerSecond(_double tickpersecond) { m_TickPerSecond = tickpersecond; }
	void	Set_NumChannels(_uint numChannels) { m_iNumChannels = numChannels; }
	void	Set_VecChannels(vector<class CChannel*> vecChannels) { m_vecChannels = vecChannels; }

	void	Reset_TrackPosition();

	void	Set_CanStop() { m_CanStop = false; }
	void	Set_Finished() { m_bFinished = false; }
	void	Set_Loop(_bool bLoop)	{ m_Loop = bLoop; }		// 루프를 돌릴지
	void	Set_TrackPosition() { m_TrackPosition = 0.f; }




	_uint					Get_NumChannels()	{ return m_iNumChannels; }
	vector<class CChannel*>	Get_VecChannels()	{ return m_vecChannels; }
	_bool					Get_CanStop()		{ return m_CanStop; }
	_bool					Get_Loop()			{ return m_Loop; }
	_bool					Get_Finished()		{ return m_bFinished; }


private:

	_char					m_szName[MAX_PATH] = "";
	_double					m_Duration = { 0.0 };		/* 내 애니메이션의 전체 길이. */
	_double					m_TickPerSecond = { 0.0 };	/* 초당 재생 속도 .*/
	_double					m_TrackPosition = { 0.0 };	/* 현재 내 애니메이션의 재생 위치 */


	_uint					m_iNumChannels = { 0 };		/* 이 애니메이션을 구동하는데 필요한 뼈의 갯수. */
	vector<class CChannel*>	m_vecChannels = {};			/* 이 애니메이션을 구동하는데 필요한 뼈들 */

	vector<_uint>			m_iCurrentKeyFrames;

	_bool					m_bFinished = { false };		// 애니메이션이 끝났는지
	_bool					m_CanStop	= { false };		// 애니메이션을 끊어도 되는지
	_bool					m_Loop		= { true };			// 루프를 돌릴지


public:
	static CAnimation* Create();
	CAnimation* Clone();
	virtual void Free() override;

};

END